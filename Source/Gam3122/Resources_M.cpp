// Fill out your copyright notice in the Description page of Project Settings.

#include "Resources_M.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AResources_M::AResources_M()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	ResourceNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ResourceNameText"));
	ResourceNameText->SetupAttachment(RootComponent);

	// Initialize the displayed text from resourceName
	ResourceNameText->SetText(FText::FromString(resourceName));
}

// Called when the game starts or when spawned
void AResources_M::BeginPlay()
{
	Super::BeginPlay();

	tempText = FText::FromString(resourceName);
	if (ResourceNameText)
	{
		ResourceNameText->SetText(tempText);
	}
}

// Called every frame
void AResources_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Example: keep the text synced (optional)
	if (ResourceNameText)
	{
		ResourceNameText->SetText(FText::FromString(resourceName));
	}
}