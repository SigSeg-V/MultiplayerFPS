// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include <filesystem>

#include "FPSCharacter.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup the root mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	Mesh->SetCollisionProfileName("OverlapAll");
	RootComponent = Mesh;

	// Set rotation
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating Movement"));
	RotatingMovement->RotationRate = FRotator(0,90,0);

	// Replication enabled
	bReplicates = true;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);
}

void APickup::OnPickedUp(AFPSCharacter* Character)
{
	if (PickupSound)
	{
		Character->ClientPlaySound2D(PickupSound);
	}
}

void APickup::SetIsEnabled(bool NewbIsEnabled)
{
	bIsEnabled = NewbIsEnabled;
	SetActorHiddenInGame(!bIsEnabled);
	SetActorEnableCollision(bIsEnabled);
}

void APickup::Respawn()
{
	SetIsEnabled(true);
}

void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	AFPSCharacter* Character =Cast<AFPSCharacter>(OtherActor);

	if (!Character || Character->IsDead() || !HasAuthority())
	{
		return;
	}

	OnPickedUp(Character);
	SetIsEnabled(false);

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &APickup::Respawn, RespawnTime);
}

