// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiplayerFPSGameModeBase.h"
#include "FPSPlayerState.h"
#include "FPSGameState.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"

AMultiplayerFPSGameModeBase::AMultiplayerFPSGameModeBase()
{
	DefaultPawnClass = AFPSCharacter::StaticClass();
	PlayerControllerClass =	AFPSPlayerController::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

bool AMultiplayerFPSGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void AMultiplayerFPSGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	AFPSGameState* FPSGameState =
	Cast<AFPSGameState>(GameState);

	// set end game condition
	if (FPSGameState != nullptr)
	{
		FPSGameState->SetKillLimit(KillLimit);
	}
}


void AMultiplayerFPSGameModeBase::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	TArray<AActor*> PlayerControllers;

	// get our players
	UGameplayStatics::GetAllActorsOfClass(this, AFPSPlayerController::StaticClass(), PlayerControllers);

	for (AActor* PlayerController : PlayerControllers)
	{
		AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(PlayerController);

		if (FPSPlayerController == nullptr)
		{
			continue;
		}

		APawn* Pawn = FPSPlayerController->GetPawn();

		// get rid of all players once game finishes
		if (Pawn != nullptr)
		{
			Pawn->Destroy();
		}

		// display the final scores
		FPSPlayerController->ClientShowScoreboard();
	}

	// replays
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMultiplayerFPSGameModeBase::RestartMap, 5.0f);
}

bool AMultiplayerFPSGameModeBase::ReadyToEndMatch_Implementation()
{
	// end game cond
	return HasWinner();
}

bool AMultiplayerFPSGameModeBase::HasWinner() const
{
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		const AFPSPlayerState* FPSPlayerState = Cast<AFPSPlayerState>(PlayerState);

		// end game once our end conditions are reached
		if (FPSPlayerState != nullptr && FPSPlayerState->GetKills() >= KillLimit)
		{
			return true;
		}
	}

	return false;
}

void AMultiplayerFPSGameModeBase::OnKill(AController* KillerController, AController* VictimController)
{
	if (!IsMatchInProgress())
	{
		return;
	}

	if (KillerController != nullptr && KillerController != VictimController)
	{
		AFPSPlayerState* KillerPlayerState = Cast<AFPSPlayerState>(KillerController->PlayerState);

		// add kill stat to killer
		if (KillerPlayerState != nullptr)
		{
			KillerPlayerState->AddKill();
		}

		AFPSPlayerController* KillerFPSController = Cast<AFPSPlayerController>(KillerController);

		// tell client to play kill stuff
		if (KillerFPSController != nullptr && VictimController != nullptr && VictimController->PlayerState != nullptr)
		{
			KillerFPSController->ClientNotifyKill(VictimController->PlayerState->GetPlayerName());
		}
	}

	if (VictimController != nullptr)
	{
		AFPSPlayerState* VictimPlayerState = Cast<AFPSPlayerState>(VictimController->PlayerState);

		// add to death stat
		if (VictimPlayerState != nullptr)
		{
			VictimPlayerState->AddDeath();
		}

		APawn* Pawn = VictimController->GetPawn();

		// get rid of dead character
		if (Pawn != nullptr)
		{
			Pawn->Destroy();
		}

		// respan
		if (!HasWinner())
		{
			RestartPlayer(VictimController);
		}
	}
}

void AMultiplayerFPSGameModeBase::RestartMap() const
{
	// replay after win
	GetWorld()->ServerTravel(GetWorld()->GetName(), false, false);
}