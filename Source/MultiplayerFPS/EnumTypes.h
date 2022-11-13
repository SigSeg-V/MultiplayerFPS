#pragma once

UENUM()
enum class EWeaponType : uint8
{
	Pistol UMETA(Display Name = "Glock 18"),
	Railgun UMETA(Display Name = "Signal .50"),
	MachineGun UMETA(Display Name = "M249 SAW"),
	MAX,
};

UENUM()
enum class ERoundType : uint8
{
	NineMM UMETA(Display Name = "9mm Parabellum"),
	FiftyCal UMETA(Display Name = ".50 AP Caliber"),
	FiveFiveSix UMETA(Display Name = "5.56 NATO"),
	MAX,
};

UENUM()
enum class EFireMode : uint8
{
	Single UMETA(Display Name = "Single Fire"),
	Burst UMETA(Display Name = "Burst Fire"),
	Automatic UMETA(Display Name = "Automatic Fire"),
	MAX,
};
