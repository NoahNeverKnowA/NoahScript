// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "Resources_M.h"
#include "Engine/Engine.h" // for GEngine and logging
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCam"));

    PlayerCamComp->SetupAttachment(GetMesh(), TEXT("head"));

    PlayerCamComp->bUsePawnControlRotation = true;

    // Initialize ResourcesArray for collecting materials
    ResourcesArray.SetNum(3);
    ResourcesNameArray.Add(TEXT("Wood"));
    ResourcesNameArray.Add(TEXT("Stone"));
    ResourcesNameArray.Add(TEXT("Berry"));
    
    // Initialize BuildingArray for craftable buildings (separate from resources)
    BuildingArray.SetNum(3);
    BuildingNameArray.Add(TEXT("Wall"));
    BuildingNameArray.Add(TEXT("Floor"));
    BuildingNameArray.Add(TEXT("Ceiling"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle StatsTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerCharacter::DecreaseStats, 2.0f, true);

    // Start stamina regeneration timer: every 2 seconds, +10 stamina
    GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &APlayerCharacter::RegenerateStamina, 2.0f, true);

    if(objWidget)
        {
        objWidget->UpdatematOBJ(0.0f);
        objWidget->UpdatebuildOBJ(0.0f);
	}
}

void APlayerCharacter::RegenerateStamina()
{
    // Regenerate only when below max; you can add more conditions (e.g., not sprinting)
    if (Stamina < 100.0f)
    {
        // Add 10 stamina every 2 seconds
        SetStamina(10.0f);
    }
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	PlayerUI->UpdateBars(Health, Hunger, Stamina);

    if (isBuilding)
    {
        if (spawnedPart)
        {
            FVector StartLocation = PlayerCamComp->GetComponentLocation();
            FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
            FVector EndLocation = StartLocation + Direction;
            spawnedPart->SetActorLocation(EndLocation);
        }
    }
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
        PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
        PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
        PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
        PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerCharacter::StartJump);
        PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerCharacter::StopJump);

        PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::FindObject);
        PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &APlayerCharacter::RotateBuilding);
    }
}

void APlayerCharacter::MoveForward(float axisValue)
{
    if (Controller)
    {
        FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, axisValue);
    }
}

void APlayerCharacter::MoveRight(float axisValue)
{
    if (Controller)
    {
        FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
        AddMovementInput(Direction, axisValue);
    }
}

void APlayerCharacter::StartJump()
{
    bPressedJump = true;
}

void APlayerCharacter::StopJump()
{
    bPressedJump = false;
}

void APlayerCharacter::FindObject()
{
    if (Stamina > 5.0f)
    {
        SetStamina(-5.0f);
    }
    else
    {
        // Not enough stamina to interact
        return;
    }

    // Ensure we have a camera (fall back to controller rotation if necessary)
    if (!PlayerCamComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("FindObject: PlayerCamComp is null"));
        if (!Controller)
        {
            UE_LOG(LogTemp, Warning, TEXT("FindObject: Controller is null - cannot form trace"));
            return;
        }
    }

    FHitResult HitResult;
    FVector StartLocation;
    FVector ForwardVector;

    if (PlayerCamComp)
    {
        StartLocation = PlayerCamComp->GetComponentLocation();
        ForwardVector = PlayerCamComp->GetForwardVector();
    }
    else
    {
        StartLocation = GetActorLocation();
        ForwardVector = Controller->GetControlRotation().Vector();
    }

    FVector EndLocation = StartLocation + ForwardVector * 800.0f;

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindObject), true);
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = false;

    if (!isBuilding)
    {
        const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);

        if (!bHit)
        {
            // Nothing hit
            return;
        }

        AActor* HitActor = HitResult.GetActor();
        if (!HitActor)
        {
            return;
        }

        AResources_M* HitResource = Cast<AResources_M>(HitActor);
        if (!HitResource)
        {
            // Hit something but not a resource actor
            return;
        }

        // Collect resource from the resource actor
        const FString HitName = HitResource->resourceName;
        const int ResourceValue = HitResource->resourceAmount;

        HitResource->totalResource -= ResourceValue;

        if (HitResource->totalResource > 0)
        {
            GiveResource(ResourceValue, HitName);

            matsCollected += ResourceValue;
            
            objWidget->UpdatematOBJ(matsCollected); 

            // use correct GameplayStatics call and the header's member 'hitDecal'
            UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.ImpactPoint, FRotator(-90, 0, 0), 2.0f);
        }
        else
        {
            HitResource->Destroy();
        }
    }
    else
    {
        // If we're in building mode, toggle it off when interact is used
        isBuilding = false;
        objectsBuilt += 1.0f;

        objWidget->UpdatebuildOBJ(objectsBuilt);
    }
}

void APlayerCharacter::SetHealth(float amount)
{
    Health = FMath::Clamp(Health + amount, 0.0f, 100.0f);
}

void APlayerCharacter::SetStamina(float amount)
{
    // Clamp stamina into [0,100]
    Stamina = FMath::Clamp(Stamina + amount, 0.0f, 100.0f);
}

void APlayerCharacter::SetHunger(float amount)
{
    Hunger = FMath::Clamp(Hunger + amount, 0.0f, 100.0f);
}

void APlayerCharacter::DecreaseStats()
{
    // Periodic hunger and health effects only. Stamina is not drained here;
    // stamina only changes when actions (e.g. FindObject) explicitly modify it.

    if (Hunger > 0.0f)
    {
        SetHunger(-1.0f);
    }

    if (Hunger <= 0.0f)
    {
        SetHealth(-3.0f);
    }
}

void APlayerCharacter::GiveResource(int32 amount, const FString& resourceType)
{
    // Find the resource index in ResourcesNameArray (NOT BuildingNameArray)
    int32 Index = ResourcesNameArray.IndexOfByKey(resourceType);
    
    if (Index != INDEX_NONE && ResourcesArray.IsValidIndex(Index))
    {
        // Add to ResourcesArray only
        ResourcesArray[Index] += amount;
        
        // Sync individual variables
        if (Index == 0)
        {
            Wood = ResourcesArray[0];
        }
        else if (Index == 1)
        {
            Stone = ResourcesArray[1];
        }
        else if (Index == 2)
        {
            Berry = ResourcesArray[2];
        }
    }
}

bool APlayerCharacter::ConsumeResource(int32 amount, const FString& resourceType)
{
    // Find the resource index in ResourcesNameArray
    int32 Index = ResourcesNameArray.IndexOfByKey(resourceType);
    
    if (Index != INDEX_NONE && ResourcesArray.IsValidIndex(Index))
    {
        if (ResourcesArray[Index] >= amount)
        {
            // Deduct from ResourcesArray
            ResourcesArray[Index] -= amount;
            
            // Sync individual variables
            if (Index == 0) Wood = ResourcesArray[0];
            else if (Index == 1) Stone = ResourcesArray[1];
            else if (Index == 2) Berry = ResourcesArray[2];
            
            return true;
        }
    }
    return false;
}

void APlayerCharacter::UpdateResources(float woodAmount, float stoneAmount, const FString& buildingObject)
{
    // Ensure arrays are properly sized
    if (ResourcesArray.Num() < 3)
    {
        ResourcesArray.SetNum(3);
    }

    // Convert float inputs to integers (rounding).
    const int32 WoodDelta = FMath::RoundToInt(woodAmount);
    const int32 StoneDelta = FMath::RoundToInt(stoneAmount);

    // Update ONLY ResourcesArray (deduct resources when crafting)
    ResourcesArray[0] = FMath::Max(0, ResourcesArray[0] + WoodDelta);
    ResourcesArray[1] = FMath::Max(0, ResourcesArray[1] + StoneDelta);

    // Sync individual convenience fields
    Wood = ResourcesArray[0];
    Stone = ResourcesArray[1];

    // If a building name was passed, add to BuildingArray (NOT ResourcesArray)
    if (!buildingObject.IsEmpty())
    {
        int32 Index = BuildingNameArray.IndexOfByKey(buildingObject);
        if (Index == INDEX_NONE)
        {
            // Add new building type
            Index = BuildingNameArray.Add(buildingObject);
            // Ensure BuildingArray has same length
            if (BuildingArray.Num() < BuildingNameArray.Num())
            {
                BuildingArray.SetNum(BuildingNameArray.Num());
            }
        }

        // Increment building count (separate from resources)
        BuildingArray[Index] = FMath::Max(0, BuildingArray[Index] + 1);
    }
}

void APlayerCharacter::SpawnBuilding(int32 buildingID, bool& bSuccess)
{
    // Initialize output parameter to false
    bSuccess = false;

    // If already in building mode, can't spawn another part.
    if (isBuilding)
    {
        return;
    }

    // Validate buildingID and availability in BuildingArray
    if (!BuildingArray.IsValidIndex(buildingID) || BuildingArray[buildingID] < 1)
    {
        return;
    }

    // Prepare spawn parameters and location (fall back if camera is null)
    FActorSpawnParameters SpawnParams;
    FVector StartLocation = PlayerCamComp ? PlayerCamComp->GetComponentLocation() : GetActorLocation();
    FVector Direction = PlayerCamComp ? PlayerCamComp->GetForwardVector() * 400.0f : GetActorForwardVector() * 400.0f;
    FVector EndLocation = StartLocation + Direction;
    FRotator myRot(0.0f, 0.0f, 0.0f);

    // Deduct one from BuildingArray only (NOT resources)
    BuildingArray[buildingID] = FMath::Max(0, BuildingArray[buildingID] - 1);

    // Spawn and set building state to reflect actual spawn result
    spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);
    isBuilding = (spawnedPart != nullptr);

    // Set output parameter
    bSuccess = isBuilding;
}

void APlayerCharacter::RotateBuilding()
{
    if (isBuilding && spawnedPart)
    {
        spawnedPart->AddActorLocalRotation(FRotator(0, 90, 0));
    }
}

// Helper function to get player character safely from any widget
APlayerCharacter* APlayerCharacter::GetPlayerCharacterSafe(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        return nullptr;
    }

    APawn* Pawn = PC->GetPawn();
    return Cast<APlayerCharacter>(Pawn);
}

// Sync helper for Blueprint when it modifies ResourcesArray directly
void APlayerCharacter::SyncArraysFromResourcesArray()
{
    // Just sync individual variables from ResourcesArray
    Wood = (ResourcesArray.IsValidIndex(0)) ? ResourcesArray[0] : 0;
    Stone = (ResourcesArray.IsValidIndex(1)) ? ResourcesArray[1] : 0;
    Berry = (ResourcesArray.IsValidIndex(2)) ? ResourcesArray[2] : 0;
}

#if WITH_EDITOR
void APlayerCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    // If ResourcesArray was modified, sync individual variables
    if (PropertyChangedEvent.Property && 
        PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(APlayerCharacter, ResourcesArray))
    {
        Wood = (ResourcesArray.IsValidIndex(0)) ? ResourcesArray[0] : 0;
        Stone = (ResourcesArray.IsValidIndex(1)) ? ResourcesArray[1] : 0;
        Berry = (ResourcesArray.IsValidIndex(2)) ? ResourcesArray[2] : 0;   
    }
}
#endif