// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	FORCEINLINE void AddKill() { ++Kills; }
	FORCEINLINE int32 GetKills() const { return Kills; }
	
	FORCEINLINE void AddDeath() { ++Deaths; }
	FORCEINLINE int32 GetDeaths() const { return Deaths; }

	
	
protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Player State")
	int32 Kills;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Player State")
	int32 Deaths;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
