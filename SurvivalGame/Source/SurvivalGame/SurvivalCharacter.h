// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "SurvivalCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivalCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// First-person camera attached to the player character.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void FindObject();

	UFUNCTION()
	void StartSprint();

	UFUNCTION()
	void StopSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1000.0f;

	// Player's current health.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.0f;

	// Player's current hunger level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Hunger = 100.0f;

	// Player's current stamina.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina = 100.0f;

	// Adjusts the player's health.
	UFUNCTION(BlueprintCallable)
	void SetHealth(float Amount);

	// Adjusts the player's hunger.
	UFUNCTION(BlueprintCallable)
	void SetHunger(float Amount);

	// Adjusts the player's stamina.
	UFUNCTION(BlueprintCallable)
	void SetStamina(float Amount);

	// Handles hunger decrease, stamina regeneration, and starvation damage.
	UFUNCTION(BlueprintCallable)
	void DecreaseStats();

	FTimerHandle StatsTimerHandle;

	// Amount of wood currently held by the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Wood = 0;

	// Amount of stone currently held by the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Stone = 0;

	// Amount of berries currently held by the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Berry = 0;

	// Stores the current amount of each resource by index.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<int32> ResourcesArray;

	// Stores the name associated with each resource index.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FString> ResourceNames;
};
