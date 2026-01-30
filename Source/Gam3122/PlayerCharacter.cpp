// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "Resources_M.h"
#include "Engine/Engine.h" // for GEngine and logging

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player initialized"));
    }

    PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCam"));

    PlayerCamComp->SetupAttachment(GetMesh(), TEXT("head"));

    PlayerCamComp->bUsePawnControlRotation = true;

    // initialize resource arrays
    ResourcesArray.SetNum(3);
    ResourcesNameArray.Add(TEXT("Wood"));
    ResourcesNameArray.Add(TEXT("Stone"));
    ResourcesNameArray.Add(TEXT("Berry"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle StatsTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerCharacter::DecreaseStats, 2.0f, true);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Input initialized"));
    }

    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
        PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
        PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
        PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
        PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerCharacter::StartJump);
        PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerCharacter::StopJump);

        // Bind an action mapping named "Interact" (map it to Left Mouse Button in Project Settings)
        PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::FindObject);
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

        check(GEngine != nullptr);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Resource Collected"));
    }
    else
    {
        HitResource->Destroy();
        check(GEngine != nullptr);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Resource Depleted"));
    }
}

void APlayerCharacter::SetHealth(float amount)
{
    if (Health + amount < 100)
    {
        Health = Health + amount;
    }
}

void APlayerCharacter::SetStamina(float amount)
{
    if (Stamina + amount < 100)
    {
        Stamina = Stamina + amount;
    }
}

void APlayerCharacter::SetHunger(float amount)
{
    if (Hunger + amount < 100)
    {
        Hunger = Hunger + amount;
    }
}

void APlayerCharacter::DecreaseStats()
{
    if (Hunger > 0.0f)
    {
        SetHunger(-1.0f);
    }

    // decrease stamina
    SetStamina(-10.0f);

    if (Hunger <= 0.0f)
    {
        SetHealth(-3.0f);
    }
}

void APlayerCharacter::GiveResource(int32 amount, const FString& resourceType)
{
    if (resourceType == TEXT("Wood"))
    {
        ResourcesArray[0] = ResourcesArray[0] + amount;
    }
    else if (resourceType == TEXT("Stone"))
    {
        ResourcesArray[1] = ResourcesArray[1] + amount;
    }
    else if (resourceType == TEXT("Berry"))
    {
        ResourcesArray[2] = ResourcesArray[2] + amount;
    }
}