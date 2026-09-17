// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "BuildingPart.generated.h"

UCLASS()
class SURVIVALGAME_API ABuildingPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingPart();

	// Mesh used to display the building piece.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* Mesh;

	// Arrow used as the pivot and placement reference for the building piece.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	UArrowComponent* PivotArrow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
