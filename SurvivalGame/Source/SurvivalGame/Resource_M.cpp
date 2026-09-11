// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource_M.h"

// Sets default values
AResource_M::AResource_M()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh component and make it the root.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Create the text component and attach it to the mesh.
	ResourceNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ResourceNameText"));
	ResourceNameText->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AResource_M::BeginPlay()
{
	Super::BeginPlay();
	
	// Convert the resource name to text and display it for testing.
	TempText = FText::FromString(ResourceName);
	ResourceNameText->SetText(TempText);
}

// Called every frame
void AResource_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

