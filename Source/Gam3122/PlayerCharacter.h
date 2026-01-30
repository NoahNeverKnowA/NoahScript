// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
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

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCamComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Hunger = 100.0f;

	// Optional individual resource counts (kept for convenience)
	UPROPERTY(EditAnywhere, Category = "Resources")
	int Wood = 0;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Stone = 0;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Berry = 0;

	// Consolidated resource storage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<int32> ResourcesArray;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TArray<FString> ResourcesNameArray;

	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float amount);

	UFUNCTION(BlueprintCallable)
	void SetStamina(float amount);

	UFUNCTION(BlueprintCallable)
	void SetHunger(float amount);
	
	UFUNCTION()
	void DecreaseStats();

	// Inventory / resources API
	UFUNCTION()
	void GiveResource(int32 amount, const FString& resourceType);
};
