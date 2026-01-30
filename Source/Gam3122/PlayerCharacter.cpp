// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"

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