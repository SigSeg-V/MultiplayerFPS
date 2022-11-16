#pragma once

UENUM()
enum class EWeaponType : uint8
{
	Pistol ,//UMETA(DisplayName = "Glock 18"),
	Railgun ,//UMETA(DisplayName = "Signal .50"),
	MachineGun ,//UMETA(DisplayName = "M249 SAW"),
	MAX,
};

UENUM()
enum class ERoundType : uint8
{
	NineMM ,//UMETA(DisplayName = "9mm Parabellum"),
	FiftyCal ,//UMETA(DisplayName = ".50 AP Caliber"),
	FiveFiveSix ,//UMETA(DisplayName = "5.56 NATO"),
	MAX,
};

UENUM()
enum class EFireMode : uint8
{
	Single ,//UMETA(DisplayName = "Single Fire"),
	Burst ,//UMETA(DisplayName = "Burst Fire"),
	Automatic ,//UMETA(DisplayName = "Automatic Fire"),
	MAX,
};
