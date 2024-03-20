#include "MultiIndexGenerator.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"
#include "VoxelRender/MaterialCollections/VoxelMaterialCollectionBase.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "VoxelSamples/VoxelUtils.h"

class FMultiIndexGeneratorInstance : public TVoxelGeneratorInstanceHelper<FMultiIndexGeneratorInstance, UMultiIndexGenerator>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FMultiIndexGeneratorInstance, UMultiIndexGenerator>;

	explicit FMultiIndexGeneratorInstance(const UMultiIndexGenerator& MyGenerator)
		: Super(&MyGenerator)
	{
		if(IsValid(MyGenerator.Layer0))
		{
			Layer0Name = FName(MyGenerator.Layer0->GetName());
		}

		if(IsValid(MyGenerator.Layer1))
		{
			Layer1Name = FName(MyGenerator.Layer1->GetName());
		}

		if(IsValid(MyGenerator.Layer2))
		{
			Layer2Name = FName(MyGenerator.Layer2->GetName());
		}

		if(IsValid(MyGenerator.Layer3))
		{
			Layer3Name = FName(MyGenerator.Layer3->GetName());
		}
	}

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override
	{
		/*
		 *	The "MakeSeed" Node takes the Input Seed and hashes the value
		 *	The Next Output hashes the previous one
		 */
		
		auto seed = FVoxelUtilities::MurmurHash32(1337);
		Noise.SetSeed(seed);
		Noise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
		Noise.SetFractalOctavesAndGain(7, 0.5);
		Noise.SetFractalLacunarity(2.0);
		Noise.SetFractalType(EVoxelNoiseFractalType::FBM);
		
		MaterialNoise.SetSeed(FVoxelUtilities::MurmurHash32(seed));
		MaterialNoise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
		MaterialNoise.SetFractalOctavesAndGain(7, 0.5);
		MaterialNoise.SetFractalLacunarity(2.0);
		MaterialNoise.SetFractalType(EVoxelNoiseFractalType::FBM);
		
		if(IsValid(InitStruct.MaterialCollection) && InitStruct.MaterialConfig == EVoxelMaterialConfig::MultiIndex)
		{
			Layer0Index = InitStruct.MaterialCollection->GetMaterialIndex(Layer0Name);
			Layer1Index = InitStruct.MaterialCollection->GetMaterialIndex(Layer1Name);
			Layer2Index = InitStruct.MaterialCollection->GetMaterialIndex(Layer2Name);
			Layer3Index = InitStruct.MaterialCollection->GetMaterialIndex(Layer3Name);
		}
		
	}

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		auto noise = Noise.GetPerlinFractal_2D(X,Y,0.002,7) * 300;
		return (Z - noise) * 0.2;
	}
	
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		FVoxelMaterialBuilder Builder;
		Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);

		auto Height = Z + (MaterialNoise.GetPerlinFractal_2D(X,Y,0.02,7) * 25);
		
		TArray<v_flt> InputsArray =
		{
			// Height 0
			-75.0,
			// Falloff 0
			5.0,
			
			// Height 1
			-20.0,
			// Falloff 1
			5.0,
			
			// Height 2
			10.0,
			// Falloff 2
			5.0
		};
		v_flt OutputsArray[4];
		HeightSplit(Height,InputsArray,OutputsArray);

		Builder.AddMultiIndex(Layer0Index,OutputsArray[0]);
		Builder.AddMultiIndex(Layer1Index,OutputsArray[1]);
		Builder.AddMultiIndex(Layer2Index,OutputsArray[2]);
		Builder.AddMultiIndex(Layer3Index,OutputsArray[3]);
		return Builder.Build();
	}

	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
	{
		return TVoxelRange<v_flt>::Infinite();
	}

	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final
	{
		return FVector::UpVector;
	}
	//~ End FVoxelGeneratorInstance Interface

private:
	FVoxelFastNoise Noise;
	FVoxelFastNoise MaterialNoise;
	FName Layer0Name;
	FName Layer1Name;
	FName Layer2Name;
	FName Layer3Name;
	
	int Layer0Index = -1;
	int Layer1Index = -1;
	int Layer2Index = -1;
	int Layer3Index = -1;
};

///////////////////////////////////////////////////////////////////////////////

TVoxelSharedRef<FVoxelGeneratorInstance> UMultiIndexGenerator::GetInstance()
{
	return MakeVoxelShared<FMultiIndexGeneratorInstance>(*this);
}
