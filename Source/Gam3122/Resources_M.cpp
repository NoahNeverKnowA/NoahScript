// Fill out your copyright notice in the Description page of Project Settings.

#include "Resources_M.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AResources_M::AResources_M()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and assign mesh as root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Create text render component and attach to root
	ResourceNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ResourceNameText"));
	ResourceNameText->SetupAttachment(RootComponent);

	// Hide editor default text; set visuals but leave text empty until runtime
	ResourceNameText->SetText(FText::GetEmpty());
	ResourceNameText->SetHorizontalAlignment(EHTA_Center);
	ResourceNameText->SetVerticalAlignment(EVRTA_TextCenter);
	ResourceNameText->SetTextRenderColor(FColor::White);
	ResourceNameText->SetWorldSize(30.0f);
	ResourceNameText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

// Called when the game starts or when spawned
void AResources_M::BeginPlay()
{
	Super::BeginPlay();

	// Remove or hide other TextRenderComponents that may have been added in the editor/blueprint
	TArray<UTextRenderComponent*> TextComps;
	GetComponents<UTextRenderComponent>(TextComps);

	for (UTextRenderComponent* Comp : TextComps)
	{
		if (!Comp)
		{
			continue;
		}

		// Keep the intended component, hide/destroy any others to avoid overlap
		if (Comp != ResourceNameText)
		{
			// If it's the default editor preview text ("Text"), destroy it.
			if (Comp->Text.ToString() == TEXT("Text"))
			{
				Comp->DestroyComponent();
			}
			else
			{
				// Otherwise hide it at runtime so it doesn't overlap with our label
				Comp->SetVisibility(false, true);
			}
		}
	}

	// Only set and show the resource name at runtime on the intended component
	if (ResourceNameText)
	{
		tempText = FText::FromString(resourceName);
		ResourceNameText->SetText(tempText);
	}
}

// Called every frame
void AResources_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}