// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"

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

	// Create three building inventory slots:
    // 0 = Wall, 1 = Floor, 2 = Ceiling.
	BuildingArray.SetNum(3);
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

		GEngine->AddOnScreenDebugMessage(
			4,
			0.0f,
			FColor::White,
			FString::Printf(TEXT("Wood: %d"), Wood)
		);

		GEngine->AddOnScreenDebugMessage(
			5,
			0.0f,
			FColor::White,
			FString::Printf(TEXT("Stone: %d"), Stone)
		);

		GEngine->AddOnScreenDebugMessage(
			6,
			0.0f,
			FColor::White,
			FString::Printf(TEXT("Berry: %d"), Berry)
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

	// Keep the active building piece positioned in front of the player.
	if (bIsBuilding && SpawnedPart)
	{
		FHitResult BuildHit;

		const FVector TraceStart = FirstPersonCamera->GetComponentLocation();
		const FVector TraceEnd =
			TraceStart + (FirstPersonCamera->GetForwardVector() * 800.0f);

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(SpawnedPart);

		if (GetWorld()->LineTraceSingleByChannel(
			BuildHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			TraceParams))
		{
			SpawnedPart->SetActorLocation(BuildHit.ImpactPoint);
		}
		else
		{
			SpawnedPart->SetActorLocation(TraceEnd);
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

	// Rotate the active building piece.
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &ASurvivalCharacter::RotateBuilding);

	// Open or close the crafting/building menu.
	PlayerInputComponent->BindAction("CraftMenu", IE_Pressed, this, &ASurvivalCharacter::ToggleCraftMenu);
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

	// If the player is currently positioning a building piece,
	// place it instead of performing the normal resource interaction.
	if (bIsBuilding && SpawnedPart)
	{
		UE_LOG(LogTemp, Warning, TEXT("BUILDING PLACED"));

		bIsBuilding = false;
		SpawnedPart = nullptr;
		return;
	}

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

			// Spawn a decal where the resource was hit.
			if (HitDecal)
			{
				UGameplayStatics::SpawnDecalAtLocation(
					GetWorld(),
					HitDecal,
					FVector(10.0f, 10.0f, 10.0f),
					HitResult.ImpactPoint,
					HitResult.ImpactNormal.Rotation(),
					1.0f
				);
			}

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

void ASurvivalCharacter::UpdateResources(int32 WoodAmount, int32 StoneAmount, FString BuildingObject)
{
	// Make sure the player has enough Wood and Stone to craft the item.
	if (ResourcesArray[0] >= WoodAmount && ResourcesArray[1] >= StoneAmount)
	{
		// Remove the required resources from the player's inventory.
		ResourcesArray[0] -= WoodAmount;
		ResourcesArray[1] -= StoneAmount;

		// Keep the individual inventory variables synchronized.
		Wood = ResourcesArray[0];
		Stone = ResourcesArray[1];

		// Add the crafted item to the correct building inventory slot.
		if (BuildingObject == "Wall")
		{
			BuildingArray[0] += 1;
		}
		else if (BuildingObject == "Floor")
		{
			BuildingArray[1] += 1;
		}
		else if (BuildingObject == "Ceiling")
		{
			BuildingArray[2] += 1;
		}
	}
}

void ASurvivalCharacter::SpawnBuilding(int32 BuildingID, bool& IsSuccess)
{
	// Assume the build attempt fails unless all checks pass.
	IsSuccess = false;

	// Make sure the player is not already positioning a building piece.
	if (bIsBuilding)
	{
		return;
	}

	// Make sure the requested building inventory slot is valid.
	if (!BuildingArray.IsValidIndex(BuildingID))
	{
		return;
	}

	// Make sure the player owns at least one of the selected building pieces.
	if (BuildingArray[BuildingID] <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SPAWN FAILED: No building pieces in inventory"));
		return;
	}

	// Make sure a building class has been selected.
	if (!BuildPartClass)
	{
		return;
	}

	// Spawn the selected building piece in front of the player's camera.
	const FVector SpawnLocation =
		FirstPersonCamera->GetComponentLocation() +
		(FirstPersonCamera->GetForwardVector() * 400.0f);

	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedPart = GetWorld()->SpawnActor<ABuildingPart>(
		BuildPartClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParameters
	);

	if (SpawnedPart)
	{
		// Remove one crafted building piece from inventory.
		BuildingArray[BuildingID] -= 1;

		// Enter building placement mode.
		bIsBuilding = true;

		IsSuccess = true;
	}
}

void ASurvivalCharacter::RotateBuilding()
{
	// Rotate the active building piece by 90 degrees.
	if (bIsBuilding && SpawnedPart)
	{
		FRotator NewRotation = SpawnedPart->GetActorRotation();
		NewRotation.Yaw += 90.0f;

		SpawnedPart->SetActorRotation(NewRotation);
	}
}

void ASurvivalCharacter::ToggleCraftMenu()
{
	// Get the controller so the menu can manage mouse and input behavior.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!PlayerController || !CraftingWidgetClass)
	{
		return;
	}

	// Close the crafting menu if it is already open.
	if (CraftingWidgetInstance && CraftingWidgetInstance->IsInViewport())
	{
		CraftingWidgetInstance->RemoveFromParent();

		// Return control to normal gameplay.
		PlayerController->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
	else
	{
		// Create the crafting menu the first time it is opened.
		if (!CraftingWidgetInstance)
		{
			CraftingWidgetInstance = CreateWidget<UUserWidget>(
				PlayerController,
				CraftingWidgetClass
			);
		}

		if (CraftingWidgetInstance)
		{
			CraftingWidgetInstance->AddToViewport();

			// Allow the player to interact with the crafting menu.
			PlayerController->bShowMouseCursor = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(CraftingWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputMode);
		}
	}
}
