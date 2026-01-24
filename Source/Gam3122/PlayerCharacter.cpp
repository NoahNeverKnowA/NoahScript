// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player initialized"));
	}
	
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));

	PlayerCamComp->SetupAttachment(GetMesh(), "head");

	PlayerCamComp->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	
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

	CreatePlayerInputComponent()->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	CreatePlayerInputComponent()->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	CreatePlayerInputComponent()->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
	CreatePlayerInputComponent()->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	CreatePlayerInputComponent()->BindAction("JumpEvent", IE_Pressed, this, &APlayerCharacter::StartJump);
	CreatePlayerInputComponent()->BindAction("JumpEvent", IE_Released, this, &APlayerCharacter::StopJump);

}

void APlayerCharacter::MoveForward(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);
}

void APlayerCharacter::MoveRight(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);
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

