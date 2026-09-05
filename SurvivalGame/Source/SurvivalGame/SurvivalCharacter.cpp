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

}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	// Interaction line trace will be implemented in a later step.
}

void ASurvivalCharacter::StartSprint()
{
	// Increase the character's movement speed while sprinting.
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}
void ASurvivalCharacter::StopSprint()
{
	// Return the character to normal walking speed.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
