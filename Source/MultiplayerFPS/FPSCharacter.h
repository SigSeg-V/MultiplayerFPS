// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnumTypes.h"
#include "MultiplayerFPS.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapon.h"
#include "../../Plugins/Developer/RiderLink/Source/RD/thirdparty/clsocket/src/ActiveSocket.h"

#include "FPSCharacter.generated.h"


UCLASS()
class MULTIPLAYERFPS_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

#pragma region Character Constants

protected:

	float CurrentSpeedMultiplier = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS Character|Movement")
	float WalkSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "FPS Character|Movement")
	float WalkSpeedMultiplier = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS Character|Movement")
	float SprintSpeed = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "FPS Character|Movement")
	float SprintSpeedMultiplier = 1.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS Character|Movement")
	float JumpVelocity = 600.f;

#pragma endregion

#pragma region Inputs

protected:
	// Props
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Jump;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Fire;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_NextWeapon;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_PrevWeapon;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Pistol;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_MachineGun;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Railgun;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Scoreboard;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Sprint;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool SwapYAxis = false;
	
	// Fns
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayerInputSprintStart(const FInputActionValue &Value);
	void PlayerInputSprintStop(const FInputActionValue &Value);
	
	// For enhanced input movement
	void PlayerInputMove(const FInputActionValue &Value);

	// For enhanced input movement
	void PlayerInputLook(const FInputActionValue &Value);
	
	//Called on first mouse down
	void StartFire(const FInputActionValue& Value);

	//Called on mouse up
	void StopFire(const FInputActionValue& Value);

	// Switch to pistol weapon
	void Pistol(const FInputActionValue& Value);

	// Switch to primary weapon
	void MachineGun(const FInputActionValue& Value);

	// Switch to railgun
	void Railgun(const FInputActionValue& Value);

	// Switch to the next weapon on the list
	void NextWeapon(const FInputActionValue& Value);

	// Switch to the prev weapon on the list
	void PrevWeapon(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerCycleWeapons(int32 Step);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(EWeaponType WeaponType);

#pragma endregion

#pragma region Camera

protected:
	// Props
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS Character")
	class UCameraComponent* Camera;
	
	//Fns
public:
	FVector GetCameraLocation() const;
	FVector GetCameraDirection() const;

#pragma endregion

#pragma region Health

protected:
	// Props
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "FPS Character")
	int32 Health = 0;
	
	// Fns
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	int32 MaxHealth = 100;
public:
	FORCEINLINE void AddHealth(const int32 Amount) { SetHealth(Health + Amount); }

	FORCEINLINE void RemoveHealth(const int32 Amount) { SetHealth(Health - Amount); }

	FORCEINLINE void SetHealth(const int32 NewHealth) { Health = FMath::Clamp(NewHealth, 0, MaxHealth); }
	
	void ApplyDamage(int Damage, AFPSCharacter* OtherCharacter);

	FORCEINLINE bool IsDead() const { return !Health; }

#pragma endregion

#pragma region Armor

protected:
	// Props
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "FPS Character")
	int32 Armor = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	int32 MaxArmor = 100;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	float ArmorAbsorption = 0.15f;
	
	// Fns
public:
	FORCEINLINE void AddArmor(const int32 Amount) { SetArmor(Armor + Amount); }

	FORCEINLINE void RemoveArmor(const int32 Amount) { SetArmor(Armor - Amount); }

	FORCEINLINE void SetArmor(const int32 NewArmor) { Armor = FMath::Clamp(NewArmor, 0, MaxArmor); }

	FORCEINLINE bool HasArmor() const { return !Armor; }

	void ArmorAbsorbDamage(int32& Damage);

#pragma endregion 

#pragma region Sounds

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* SpawnSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* WeaponChangeSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* PainSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* LandSound;


#pragma endregion

#pragma region Weapon

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Character")
	TArray<int32> Ammo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	TArray<AWeapon*> Weapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	TArray<TSubclassOf<AWeapon>> WeaponClasses;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Character")
	AWeapon* Weapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "FPS Character")
	AWeapon* FirstPersonWeapon;

public:
	void AddWeapon(EWeaponType WeaponType);

protected:
	int32 WeaponIndex = INDEX_NONE;

	bool EquipWeapon(EWeaponType WeaponType, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category = "FPS Character")
	int32 GetWeaponAmmo() const;

public:
	FORCEINLINE void AddAmmo(ERoundType AmmoType, int32 Amount)
	{
		SetAmmo(AmmoType, GetAmmo(AmmoType) + Amount);
	}
	
	FORCEINLINE void ConsumeAmmo(ERoundType AmmoType, int32 Amount)
	{
		SetAmmo(AmmoType, GetAmmo(AmmoType) - Amount);
	}

	FORCEINLINE int32 GetAmmo(ERoundType AmmoType) const
	{ return
	Ammo[ENUM_TO_I32(AmmoType)];
	}
	
	FORCEINLINE void SetAmmo(ERoundType AmmoType, int32 Amount)
	{
		Ammo[ENUM_TO_I32(AmmoType)] = FMath::Max(0, Amount);
	}

#pragma endregion

#pragma region RPC

public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAnimMontage(UAnimMontage* AnimMontage);

	UFUNCTION(Client, Unreliable)
	void ClientPlaySound2D(USoundBase* Sound);

#pragma endregion

#pragma region Overrides
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma endregion

#pragma region First Person

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS Character|First Person")
	class USkeletalMeshComponent* FirstPersonMesh;
public:
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE class USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
		FORCEINLINE class USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

#pragma endregion
	
protected:
	UPROPERTY()
	class AMultiplayerFPSGameModeBase* GameMode;


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void FellOutOfWorld(const UDamageType& DmgType) override;
	virtual void Landed(const FHitResult& Hit) override;
	void Scoreboard(const FInputActionValue& Value);
};
