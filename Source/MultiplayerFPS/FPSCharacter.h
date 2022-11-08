// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "FPSCharacter.generated.h"


UCLASS()
class MULTIPLAYERFPS_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();


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

	// Fns
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// For enhanced input movement
	void PlayerInputMove(const FInputActionValue &Value);

	// For enhanced input movement
	void PlayerInputLook(const FInputActionValue &Value);

	// For enhanced input movement
	void PlayerInputJump(const FInputActionValue &Value);

#pragma endregion

#pragma region Camera

protected:
	// Props
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS Character")
	class UCameraComponent* Camera;
	
	//Fns
	

#pragma endregion

#pragma region Health

protected:
	// Props
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "FPS Character")
	int32 Health = 0;
	
	// Fns
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	int32 MaxHealth = 100;

	FORCEINLINE void AddHealth(const int32 Amount) { SetHealth(Health + Amount); }

	FORCEINLINE void RemoveHealth(const int32 Amount) { SetHealth(Health - Amount); }

	FORCEINLINE void SetHealth(const int32 NewHealth) { Health = FMath::Clamp(NewHealth, 0, MaxHealth); }

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

	FORCEINLINE void AddArmor(const int32 Amount) { SetHealth(Armor + Amount); }

	FORCEINLINE void RemoveArmor(const int32 Amount) { SetHealth(Armor - Amount); }

	FORCEINLINE void SetArmor(const int32 NewArmor) { Armor = FMath::Clamp(NewArmor, 0, MaxArmor); }

	FORCEINLINE bool HasArmor() const { return !Armor; }

	void ArmorAbsorbDamage(int32& Damage);

#pragma endregion 

#pragma region Sounds

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS Character")
	USoundBase* SpawnSound;

#pragma endregion

#pragma region Overrides
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion

	

};
