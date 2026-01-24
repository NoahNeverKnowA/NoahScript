// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"

AMyGameModeBase::AMyGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Game Mode Started"));
}	
