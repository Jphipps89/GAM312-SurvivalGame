// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the first person camera and attach it to the character.
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));

	// Allow the camera to rotate with the player's view.
	FirstPersonCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Set up the resource arrays so each index matches a resource type.
	ResourcesArray.SetNum(3);
	ResourceNames.SetNum(3);

	ResourceNames[0] = "Wood";
	ResourceNames[1] = "Stone";
	ResourceNames[2] = "Berry";
}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Update survival stats every two seconds.
	GetWorldTimerManager().SetTimer(
		StatsTimerHandle,
		this,
		&ASurvivalCharacter::DecreaseStats,
		2.0f,
		true
	);
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			0.0f,
			FColor::Green,
			FString::Printf(TEXT("Health: %.0f"), Health)
		);

		GEngine->AddOnScreenDebugMessage(
			2,
			0.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Hunger: %.0f"), Hunger)
		);

		GEngine->AddOnScreenDebugMessage(
			3,
			0.0f,
			FColor::Cyan,
			FString::Printf(TEXT("Stamina: %.0f"), Stamina)
		);
	}

	// Drain stamina continuously while sprinting.
	if (bIsSprinting && Stamina > 0.0f)
	{
		Stamina = FMath::Clamp(
			Stamina - (SprintStaminaDrainRate * DeltaTime),
			0.0f,
			100.0f
		);

		// Automatically stop sprinting when stamina is depleted.
		if (Stamina <= 0.0f)
		{
			StopSprint();
		}
	}
}

// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement axis inputs.
	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);

	// Bind mouse look directly to Unreal's controller rotation functions.
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	// Bind jump controls.
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &ASurvivalCharacter::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &ASurvivalCharacter::StopJump);

	// Bind interaction input.
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalCharacter::FindObject);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASurvivalCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASurvivalCharacter::StopSprint);

}

void ASurvivalCharacter::MoveForward(float AxisValue)
{
	// Get the direction the controller is facing and move forward or backward.
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, AxisValue);
}

void ASurvivalCharacter::MoveRight(float AxisValue)
{
	// Get the direction perpendicular to the controller and move left or right.
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, AxisValue);
}

void ASurvivalCharacter::StartJump()
{
	bPressedJump = true;
}

void ASurvivalCharacter::StopJump()
{
	bPressedJump = false;
}

void ASurvivalCharacter::FindObject()
{
	// Store information about anything hit by the line trace.
	FHitResult HitResult;

	// Start the trace at the first person camera.
	const FVector StartLocation = FirstPersonCamera->GetComponentLocation();

	// Trace forward in the direction the camera is facing.
	const FVector Direction = FirstPersonCamera->GetForwardVector() * 800.0f;
	const FVector EndLocation = StartLocation + Direction;

	// Ignore the player character when checking for collisions.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	// perform the line trace using the Visibility channel.
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams))
	{
		// Check whether the object hit is one of the resource actors.
		AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

		if (HitResource)
		{
			const FString HitName = HitResource->ResourceName;
			const int32 ResourceValue = HitResource->ResourceAmount;

			// Require enough stamina to gather a resource.
			if (Stamina <= 5.0f)
			{
				return;
			}

			// Gathering consumes stamina.
			SetStamina(-5.0f);

			// Only collect from a resource that still has something remaining.
			if (HitResource->TotalResource > 0)
			{
				// Give the resource to the player's inventory.
				GiveResource(ResourceValue, HitName);

				// Reduce the amount remaining in the resource actor.
				HitResource->TotalResource -= ResourceValue;

				// Check whether this interaction depleted the resource.
				if (HitResource->TotalResource <= 0)
				{
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							2.0f,
							FColor::Red,
							TEXT("Resource Depleted")
						);
					}

					HitResource->Destroy();
				}
				else
				{
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							2.0f,
							FColor::Green,
							TEXT("Resource Collected")
						);
					}
				}
			}
		}
	}
}

void ASurvivalCharacter::StartSprint()
{
	// Only allow sprinting while the player has stamina.
	if (Stamina > 0.0f)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASurvivalCharacter::StopSprint()
{
	bIsSprinting = false;

	// Return the character to normal walking speed.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASurvivalCharacter::SetHealth(float Amount)
{
	// Adjust health while preventing it from exceeding the maximum value.
	if (Health + Amount < 100.0f)
	{
		Health += Amount;
	}
}

void ASurvivalCharacter::SetHunger(float Amount)
{
	// Adjust health while preventing it from exceeding the maximum value.
	if (Hunger + Amount < 100.0f)
	{
		Hunger += Amount;
	}
}

void ASurvivalCharacter::SetStamina(float Amount)
{
	// Adjust stamina while preventing it from exceeding the maximum value.
	if (Stamina + Amount < 100.0f)
	{
		Stamina += Amount;
	}
}

void ASurvivalCharacter::DecreaseStats()
{
	// Hunger decreases over time while the player still has hunger remaining.
	if (Hunger > 0.0f)
	{
		SetHunger(-1.0f);
	}

	// Regenerate stamina over time.
	if (Stamina < 100.0f)
	{
		SetStamina(1.0f);
	}

	// When hunger reaches zero, begin reducing health.
	if (Hunger <= 0.0f)
	{
		SetHealth(-1.0f);
	}
}

void ASurvivalCharacter::GiveResource(int32 Amount, FString ResourceType)
{
	// Add the collected amount to the correct resource inventory slot.
	if (ResourceType == "Wood")
	{
		ResourcesArray[0] += Amount;
		Wood = ResourcesArray[0];
	}
	else if (ResourceType == "Stone")
	{
		ResourcesArray[1] += Amount;
		Stone = ResourcesArray[1];
	}
	else if (ResourceType == "Berry")
	{
		ResourcesArray[2] += Amount;
		Berry = ResourcesArray[2];
	}
}