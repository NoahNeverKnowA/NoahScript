// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingPart.h"
#include "PlayerWidget.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GAM3122_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Timer handle for stamina regeneration
	FTimerHandle StaminaRegenTimerHandle;

	// Regenerate stamina over time
	UFUNCTION()
	void RegenerateStamina();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float axisValue);

	UFUNCTION()
	void MoveRight(float axisValue);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();
	
	UFUNCTION()
	void FindObject();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* PlayerCamComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Hunger = 100.0f;

	// Optional individual resource counts (kept for convenience)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Wood = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Stone = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int32 Berry = 0;

	// Consolidated resource storage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<int32> ResourcesArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FString> ResourcesNameArray;

	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	// Building supplies counts - Unified array for ALL resources and buildings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
	TArray<int32> BuildingArray;

	// Names for building types (kept in sync with BuildingArray)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
	TArray<FString> BuildingNameArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	bool isBuilding = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building")
	TSubclassOf<ABuildingPart> BuildPartClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	ABuildingPart* spawnedPart = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerWidget* PlayerUI;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void SetHealth(float amount);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void SetStamina(float amount);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void SetHunger(float amount);
	
	UFUNCTION()
	void DecreaseStats();

	// Inventory / resources API
	UFUNCTION(BlueprintCallable, Category = "Resources")
	void GiveResource(int32 amount, const FString& resourceType);

	// Consume resources (returns true if successful)
	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool ConsumeResource(int32 amount, const FString& resourceType);

	// Update wood, stone, and a building-type entry (buildingObject is an FString name).
	// If buildingObject is provided the matching building count will be incremented by 1.
	UFUNCTION(BlueprintCallable, Category = "Resources")
	void UpdateResources(float woodAmount, float stoneAmount, const FString& buildingObject);
	
	// Exposed to Blueprints with output parameter for success status
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SpawnBuilding(int32 buildingID, bool& bSuccess);

	UFUNCTION()
	void RotateBuilding();

	// Helper function to get player character safely from any widget
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player", meta = (WorldContext = "WorldContextObject"))
	static APlayerCharacter* GetPlayerCharacterSafe(const UObject* WorldContextObject);

	// Call this after manually modifying ResourcesArray in Blueprint to sync everything
	UFUNCTION(BlueprintCallable, Category = "Resources")
	void SyncArraysFromResourcesArray();

#if WITH_EDITOR
	// Called when a property is changed in the editor or Blueprint
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
