// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "FPSCharacter.h"
#include "Components/SlateWrapperTypes.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = Mesh;
	bReplicates = true;
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	Character = Cast<AFPSCharacter>(NewOwner);
}

void AWeapon::StartFire()
{
	// fire delay still upo so can't fire 
	if (!bWantsFire || GetWorldTimerManager().GetTimerRemaining(FireTimer) > 0.f)
	{
		return;
	}

	// out of ammo check
	if (!Character->GetAmmo(AmmoType))
	{
		if (NoAmmoSound)
		{
			Character->ClientPlaySound2D(NoAmmoSound);
		}
		return;
	}

	// can fire, so do so
	Character->ConsumeAmmo(AmmoType,1);
	
	// get loc and dir for FireHitscan
	const FVector FireLocation = Character->GetCameraLocation();
	const FVector FireDirection = Character->GetCameraDirection();

	FireHitscan(FireLocation, FireDirection);
	
	if (FireAnimMontage)
	{
		Character->MulticastPlayAnimMontage(FireAnimMontage);
	}

	if (bWantsFire)
	{
		switch (FireMode)
		{
		case EFireMode::Automatic:
			GetWorldTimerManager().SetTimer(FireTimer, this, &AWeapon::StartFire, FireRate);
			break;
		case EFireMode::Single:
			GetWorldTimerManager().SetTimer(FireTimer, FireRate, false);
			break;
		}
	}
}

void AWeapon::FireHitscan(FVector FireLocation, FVector FireDirection)
{
	FHitResult Hit(ForceInit);
	FCollisionQueryParams TraceParams("Fire Trace", false, Character);
	const FVector Start = FireLocation;
	const FVector End = Start + FireDirection * HitscanRange;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	AFPSCharacter* HitCharacter = Cast<AFPSCharacter>(Hit.GetActor());
	if (HitCharacter != nullptr)
	{
		HitCharacter->ApplyDamage(HitscanDamage, Character);
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::ServerStopFire_Implementation()
{
	bWantsFire = false;
}

void AWeapon::ServerStartFire_Implementation()
{
	bWantsFire = true;
    StartFire();
}

