// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ENUM_TO_I32(Enum) static_cast<int32>(Enum)


/**
 * @brief 
 * @param Index Current index into the array/enum
 * @param Max Size of the array/enum
 * @param Step Number of positions to go up or down. Use a negative integer to go backwards. 
 * @return 
 */
FORCEINLINE int32 GetSafeWrappedIndex(int32 Index, int32 Max, int32 Step)
{
	return (Index + Max + Step) % Max;
}