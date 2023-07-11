// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeConst

#include "FPSCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FPSPlayerController.h"
#include "MultiplayerFPSGameModeBase.h"

#pragma region Overrides

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Setup arms
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Arms"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->bOnlyOwnerSee = true;

	// floating arms with shadows doesn't look good so let our arms not cast shadows, and our invis body cast shadow
	FirstPersonMesh->CastShadow = false;
	GetMesh()->bCastHiddenShadow = true;
	
	// Set up player camera
	Camera = CreateEditorOnlyDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	// Rotate with player's mesh
	Camera->bUsePawnControlRotation = true;
	
	// Attach Camera to the socket in skeleton
	Camera->SetupAttachment(FirstPersonMesh, "Camera");
	
	// Set movement constants
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
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

	// Init weapons and armor so we start with something
	constexpr int32 WeaponCount = ENUM_TO_I32(EWeaponType::MAX);
	Weapons.Init(nullptr, WeaponCount);

	constexpr int32 AmmoCount = ENUM_TO_I32(EWeaponType::MAX);
	Ammo.Init(50, AmmoCount);

	// I'm giving all the weapons here but might change this to start with a pistol only?
	for (int32 i = 0; i < WeaponCount; i++)
	{
		AddWeapon(static_cast<EWeaponType>(i));
	}

	EquipWeapon(EWeaponType::MachineGun, false);

	// reference game mode for kills/deaths
	GameMode = Cast<AMultiplayerFPSGameModeBase>(GetWorld()->GetAuthGameMode());
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Rep health and armor
	DOREPLIFETIME_CONDITION(AFPSCharacter, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, Armor, COND_OwnerOnly);

	// Rep weaps
	DOREPLIFETIME_CONDITION(AFPSCharacter, Weapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, Weapons, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, Ammo, COND_OwnerOnly);
}

#pragma endregion

#pragma region Inputs
// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	// See if valid bindings
	check(PlayerInputComponent)

	// get our bindings and ad them to the mapping context
	UEnhancedInputComponent* EInpComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (EInpComponent == nullptr || PlayerController == nullptr)
	{
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if(EnhancedSubsystem == nullptr)
	{
		return;
	}
	EnhancedSubsystem->AddMappingContext(IMC_Player, 1);

	// Movement inputs
	EInpComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AFPSCharacter::PlayerInputMove);
	EInpComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AFPSCharacter::PlayerInputLook);

	// Sprinting
	EInpComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AFPSCharacter::PlayerInputSprintStart);
	EInpComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AFPSCharacter::PlayerInputSprintStop);
	
	// Fire inputs
	EInpComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &AFPSCharacter::StartFire);
	EInpComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AFPSCharacter::StopFire);

	// Weapon swap inputs
	EInpComponent->BindAction(IA_Pistol, ETriggerEvent::Started, this, &AFPSCharacter::Pistol);
	EInpComponent->BindAction(IA_MachineGun, ETriggerEvent::Started, this, &AFPSCharacter::MachineGun);
	EInpComponent->BindAction(IA_Railgun, ETriggerEvent::Started, this, &AFPSCharacter::Railgun);
	EInpComponent->BindAction(IA_PrevWeapon, ETriggerEvent::Started, this, &AFPSCharacter::PrevWeapon);
	EInpComponent->BindAction(IA_NextWeapon, ETriggerEvent::Started,this, &AFPSCharacter::NextWeapon);

	// UI Inputs
	EInpComponent->BindAction(IA_Scoreboard, ETriggerEvent::Started, this, &AFPSCharacter::Scoreboard);
}

void AFPSCharacter::PlayerInputSprintStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
}

void AFPSCharacter::PlayerInputSprintStop(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier;
}

void AFPSCharacter::PlayerInputMove(const FInputActionValue& Value)
{

	// movement input from mapping context
	const FVector2D InputValue = Value.Get<FVector2D>();
	
	// Got movement horizontally
	if (InputValue.X)
	{
		// no increase in speed sideways when sprinting
		AddMovementInput(GetActorRightVector(), InputValue.X);
	}
	// Got movement forward
	if(InputValue.Y)
	{
		// y direction + sprint - if sprinting - assume direction == 1
		AddMovementInput(GetActorForwardVector(), InputValue.Y);	
		
	}
}

// applies (un)negated input for looking around 
void AFPSCharacter::PlayerInputLook(const FInputActionValue& Value)
{
	// looking input from mapping context
	const FVector2D InputValue = Value.Get<FVector2D>();

	// horizontal mouse movement
	if (InputValue.X)
	{
		AddControllerYawInput(InputValue.X);
	}

	// vertical mouse movement
	if (InputValue.Y)
	{
		// by default Y axis is inverted, so negated as per user setting
		AddControllerPitchInput((SwapYAxis? 1 : -1) * InputValue.Y);
	}
}

void AFPSCharacter::StartFire(const FInputActionValue& Value)
{
	if (Weapon)
	{
		Weapon->ServerStartFire();
	}
}

void AFPSCharacter::StopFire(const FInputActionValue& Value)
{
	if (Weapon)
{
	Weapon->ServerStopFire();
}
}

void AFPSCharacter::Pistol(const FInputActionValue& Value)
{
	ServerEquipWeapon(EWeaponType::Pistol);
}

void AFPSCharacter::MachineGun(const FInputActionValue& Value)
{
	ServerEquipWeapon(EWeaponType::MachineGun);
}

void AFPSCharacter::Railgun(const FInputActionValue& Value)
{
	ServerEquipWeapon(EWeaponType::Railgun);
}

void AFPSCharacter::NextWeapon(const FInputActionValue& Value)
{
	ServerCycleWeapons(1);
}

void AFPSCharacter::PrevWeapon(const FInputActionValue& Value)
{
	ServerCycleWeapons(-1);
}

FVector AFPSCharacter::GetCameraLocation() const
{
	return Camera->GetComponentLocation();
}

FVector AFPSCharacter::GetCameraDirection() const
{
	return GetControlRotation().Vector();
}

void AFPSCharacter::ServerEquipWeapon_Implementation(EWeaponType WeaponType)
{
	EquipWeapon(WeaponType);
}

void AFPSCharacter::ServerCycleWeapons_Implementation(int32 Step)
{
	const int32 WeaponCount = Weapons.Num();
	const int32 StartWeaponIndex =
	GetSafeWrappedIndex(WeaponIndex, WeaponCount, Step);
	for (int32 i = StartWeaponIndex; i != WeaponIndex; i = GetSafeWrappedIndex(i,WeaponCount, Step))
	{
		if (EquipWeapon(static_cast<EWeaponType>(i)))
		{
			break;
		}
	}
}

#pragma endregion

#pragma region Armor

void AFPSCharacter::ApplyDamage(int Damage, AFPSCharacter* OtherCharacter)
{
	if (IsDead())
	{
		return;
	}
	
	ArmorAbsorbDamage(Damage);
	RemoveHealth(Damage);
	
	if (HitSound && OtherCharacter)
	{
		OtherCharacter->ClientPlaySound2D(HitSound);
	}

	// notify that the play was killed
	if (IsDead())
	{
		if (GameMode)
		{
			GameMode->OnKill(OtherCharacter->GetController(), GetController());
		}
	}
}

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

#pragma region Generic RPCs

void AFPSCharacter::MulticastPlayAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage);
}

void AFPSCharacter::ClientPlaySound2D_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}

#pragma endregion

#pragma region Weapons
void AFPSCharacter::AddWeapon(EWeaponType WeaponType)
{
	const int32 NewWeaponIndex = ENUM_TO_I32(WeaponType);
	if (!WeaponClasses.IsValidIndex(NewWeaponIndex) || Weapons[NewWeaponIndex])
	{
		return;
	}

	UClass* WeaponClass = WeaponClasses[NewWeaponIndex];
	if(!WeaponClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();

	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnParameters);
	if (NewWeapon == nullptr)
	{
		return;
	}

	NewWeapon->SetActorHiddenInGame(true);
	Weapons[NewWeaponIndex] = NewWeapon;
	NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GripPoint");
}

bool AFPSCharacter::EquipWeapon(EWeaponType WeaponType, bool bPlaySound)
{
	const int32 NewWeaponIndex = ENUM_TO_I32(WeaponType);
	if(!Weapons.IsValidIndex(NewWeaponIndex))
	{
		return false;
	}

	AWeapon* NewWeapon = Weapons[NewWeaponIndex];
	
	if (NewWeapon == nullptr || Weapon == NewWeapon)
	{
		return false;
	}

	// Swapping out the weapons
	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(true);
	}

	Weapon = NewWeapon;
	Weapon->SetActorHiddenInGame(false);

	WeaponIndex = NewWeaponIndex;

	if (WeaponChangeSound && bPlaySound)
	{
		ClientPlaySound2D(WeaponChangeSound);
	}
	return true;
}

int32 AFPSCharacter::GetWeaponAmmo() const
{
	return Weapon != nullptr ? Ammo[ENUM_TO_I32(Weapon->GetAmmoType())] : 0;
}
#pragma endregion 


void AFPSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (AWeapon* WeaponToDestroy : Weapons)
	{
		WeaponToDestroy->Destroy();
	}
}

void AFPSCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
	Super::FellOutOfWorld(DmgType);

	if (GameMode)
	{
		GameMode->OnKill(nullptr, GetController());
	}
}

void AFPSCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UGameplayStatics::PlaySound2D(GetWorld(), LandSound);
}

void AFPSCharacter::Scoreboard(const FInputActionValue& Value)
{
	const AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());

	if (PlayerController != nullptr)
	{
		PlayerController->ToggleScoreboard();
	}
}
