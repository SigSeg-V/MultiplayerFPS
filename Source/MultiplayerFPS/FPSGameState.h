// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AFPSGameState : public AGameState
{
	GENERATED_BODY()


public:

	FORCEINLINE void SetKillLimit (const int32 NewKillLimit) { KillLimit = NewKillLimit; }
	FORCEINLINE int32 GetKillLimit (const int32 NewKillLimit) const {return  KillLimit; }
	
protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Player State")
	int32 KillLimit;

	UFUNCTION(BlueprintCallable)
	TArray<class AFPSPlayerState*> GetPlayerStatesOrderedByKills() const;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
