// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#pragma region Overrides

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set up player camera
	Camera = CreateEditorOnlyDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	// Rotate with player's mesh
	Camera->bUsePawnControlRotation = true;

	// Attach Camera to the socket in skeleton
	Camera->SetupAttachment(GetMesh(), "Camera");

	// Set movement constants
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// set off respawn sound
	UGameplayStatics::PlaySound2D(GetWorld(), SpawnSound);

	// Set up health and armor
	if(!HasAuthority()){ return; }

	SetHealth(MaxHealth);
	SetArmor(0);
	
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Rep health and armor
	DOREPLIFETIME_CONDITION(AFPSCharacter, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, Armor, COND_OwnerOnly);
}

#pragma endregion

#pragma region Inputs
// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	// See if valid bindings
	check(PlayerInputComponent)

	// get our bindings and ad them to the mapping context
	if (UEnhancedInputComponent* EInpComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		EInpComponent != nullptr)
	{
		if (const APlayerController* PController = Cast<APlayerController>(GetController());
			PController != nullptr)
		{
			if (UEnhancedInputLocalPlayerSubsystem* EInpSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PController->GetLocalPlayer());
			EInpSubsystem != nullptr)
			{
				EInpSubsystem->AddMappingContext(IMC_Player, 1);
			}
		}
		EInpComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AFPSCharacter::PlayerInputMove);
		EInpComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AFPSCharacter::PlayerInputLook);
		EInpComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AFPSCharacter::PlayerInputJump);
	}

}

void AFPSCharacter::PlayerInputMove(const FInputActionValue& Value)
{

	// movement from mapping context
	const FVector2D InputValue = Value.Get<FVector2D>();
	
	// Got movement horizontally
	if (InputValue.X)
	{
		// Get right perpendicular vec to what camera is pointing at
		AddMovementInput(GetActorRightVector(), InputValue.X);
	}
	// Got movement forward
	if(InputValue.Y)
	{
		AddMovementInput(GetActorForwardVector(), InputValue.Y);
	}
}

void AFPSCharacter::PlayerInputLook(const FInputActionValue& Value)
{
	// movement from mapping context
	const FVector2D InputValue = Value.Get<FVector2D>();

	// horizontal mouse movement
	if (InputValue.X)
	{
		AddControllerYawInput(InputValue.X);
	}

	// vertical mouse movement
	if (InputValue.Y)
	{
		AddControllerPitchInput(InputValue.Y);
	}
}

void AFPSCharacter::PlayerInputJump(const FInputActionValue& Value)
{
}
#pragma endregion

#pragma region Armor

void AFPSCharacter::ArmorAbsorbDamage(int32& Damage)
{
	// make sure to at least absorb 1 damage when absorbing damage
	const int32 AbsorbedDamage = FMath::Clamp(Damage * ArmorAbsorption, 1, MAX_int32);

	// Set new armor durability
	const int32 RemainingArmor = Armor - AbsorbedDamage;
	SetArmor(RemainingArmor);

	// Make sure to deal at least 1 damage here
	Damage = FMath::Clamp(Damage * (1 - ArmorAbsorption), 1, MAX_int32);
}


#pragma endregion 