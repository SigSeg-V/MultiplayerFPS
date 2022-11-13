// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Pickup.generated.h"

UCLASS()
class MULTIPLAYERFPS_API APickup : public AActor
{
	GENERATED_BODY()

protected:

	// Sets default values for this actor's properties
	APickup();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	URotatingMovementComponent* RotatingMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float RespawnTime = 30.f;

	FTimerHandle RespawnTimer;
	bool bIsEnabled = true;

	virtual void OnPickedUp(class AFPSCharacter* Character);

	void SetIsEnabled(bool NewbIsEnabled);

	void Respawn();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
};
