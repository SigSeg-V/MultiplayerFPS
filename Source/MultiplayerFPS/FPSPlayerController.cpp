// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "PlayerMenu.h"
#include "Blueprint/UserWidget.h"

void AFPSPlayerController::ClientShowScoreboard_Implementation()
{
	if (PlayerMenu)
	{
		PlayerMenu->SetScoreboardVisibility(true);
	}
}

void AFPSPlayerController::ClientNotifyKill_Implementation(const FString& Name)
{
	if (PlayerMenu)
	{
		PlayerMenu->NotifyKill(Name);
	}
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController() && PlayerMenuClass)
	{
		PlayerMenu = CreateWidget<UPlayerMenu>(this, PlayerMenuClass);

		if (PlayerMenu)
		{
			PlayerMenu->AddToViewport(0);
		}
	}
}

void AFPSPlayerController::ToggleScoreboard() const
{
	if (PlayerMenu)
	{
		PlayerMenu->ToggleScoreboard();
	}
}