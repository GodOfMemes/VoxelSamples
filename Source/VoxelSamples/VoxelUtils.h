#pragma once

#include "CoreMinimal.h"
#include "VoxelMacros.h"

// Copied from VoxelGraph Module

template<typename TInArray, typename TOutArray>
void HeightSplit(v_flt Height, const TInArray& Inputs, TOutArray& Outputs)
{
	checkVoxelSlow(Inputs.Num() % 2 == 0);
	checkVoxelSlow(Outputs.Num() == Inputs.Num() / 2 + 1);

	const int32 NumSplits = Inputs.Num() / 2;

	const auto GetHeight = [&](int32 Split) { return Inputs[2 * Split]; };
	const auto GetFalloff = [&](int32 Split) { return Inputs[2 * Split + 1]; };

	for (int32 Layer = 0; Layer < NumSplits + 1; Layer++)
	{
		float Strength = 1;

		// Lower bound
		if (Layer != 0)
		{
			const v_flt Bound = GetHeight(Layer - 1);
			const v_flt Falloff = GetFalloff(Layer - 1);

			Strength = FMath::Min(Strength, FMath::SmoothStep(Bound - Falloff, Bound + Falloff, Height));
		}

		// Upper bound
		if (Layer != NumSplits)
		{
			const v_flt Bound = GetHeight(Layer);
			const v_flt Falloff = GetFalloff(Layer);

			Strength = FMath::Min(Strength, 1 - FMath::SmoothStep(Bound - Falloff, Bound + Falloff, Height));
		}

		ensureVoxelSlow(0 <= Strength);
		Outputs[Layer] = Strength;
	}
}