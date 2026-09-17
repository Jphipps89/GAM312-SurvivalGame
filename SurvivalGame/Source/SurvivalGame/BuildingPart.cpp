// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPart.h"

// Sets default values
ABuildingPart::ABuildingPart()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the pivot arrow and make it the root component.
	PivotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PivotArrow"));
	RootComponent = PivotArrow;

	// Create the mesh and attach it to the pivot arrow.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(PivotArrow);

}

// Called when the game starts or when spawned
void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildingPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

