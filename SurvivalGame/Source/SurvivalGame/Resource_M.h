// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Resource_M.generated.h"

UCLASS()
class SURVIVALGAME_API AResource_M : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResource_M();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Name of this resource type, such as Wood, Stone, or Berry.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString ResourceName = "Wood";

	// Amount of resource given to the player with each interaction.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 ResourceAmount = 5;

	// Total amount available before this resource is depleted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 TotalResource = 100;

	// Temporary text used to display the resource name for testing.
	FText TempText;

	// Displays the resource name above the object for testing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	UTextRenderComponent* ResourceNameText;

	// Mesh used to represent the resource in the game world.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	UStaticMeshComponent* Mesh;
};
