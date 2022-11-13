// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "EnumTypes.h"
#include "AmmoPickup.generated.h"


/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Pickup")
	ERoundType RoundType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Pickup")
	int32 AmmoCount;

	virtual void OnPickedUp(AFPSCharacter* Character) override;
};
