#include "PlanetGenerator.h"
#include "Curves/CurveLinearColor.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"

class FPlanetGeneratorInstance : public TVoxelGeneratorInstanceHelper<FPlanetGeneratorInstance, UPlanetGenerator>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FPlanetGeneratorInstance, UPlanetGenerator>;

	explicit FPlanetGeneratorInstance(const UPlanetGenerator& MyGenerator)
		: Super(&MyGenerator)
		, PlanetHeight(MyGenerator.PlanetHeight)
		, PlanetColor(MyGenerator.PlanetColor)
		, Radius(MyGenerator.Radius)
		, NoiseStrength(MyGenerator.NoiseStrength)
		, NoiseFrequency(MyGenerator.NoiseFrequency)
	{
		
	}

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override
	{
		Noise.SetSeed(1443);
		Noise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
		Noise.SetFractalOctavesAndGain(15,0.6);
		Noise.SetFractalLacunarity(2);
		//Noise.SetMatrixFromRotation_3D(FRotator::MakeFromEuler({50,40,40}));
		Noise.SetMatrixFromRotation_3D(FRotator(40,45,50));
		Noise.SetFractalType(EVoxelNoiseFractalType::FBM);
	
		GradNoise.SetSeed(1337);
		GradNoise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
	}

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		float Height = Radius;
		if(IsValid(PlanetHeight))
		{
			auto noise = GenerateNoise(X,Y,Z);
			Height = Radius + (Radius * PlanetHeight->GetFloatValue(noise) * NoiseStrength);
		}
		float Value = FVector(X, Y, Z).Size() - Height;
		return Value;
	}
	
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		FVoxelMaterialBuilder Builder;
		Builder.SetMaterialConfig(EVoxelMaterialConfig::RGB);
		Builder.SetColor(FColor::Red);

		if(IsValid(PlanetColor))
		{
			auto noise = GenerateNoise(X,Y,Z);
			Builder.SetColor(PlanetColor->GetLinearColorValue(noise));
		}
	
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
	TObjectPtr<UCurveFloat> PlanetHeight;
	TObjectPtr<UCurveLinearColor> PlanetColor;
	const float Radius;
	const float NoiseStrength;
	const float NoiseFrequency;
	FVoxelFastNoise GradNoise;
	FVoxelFastNoise Noise;

	v_flt GenerateNoise(v_flt X, v_flt Y, v_flt Z) const
	{
		FVector SamplePosition = FVector(X,Y,Z).GetSafeNormal();
		GradNoise.GradientPerturb_3D(SamplePosition.X,SamplePosition.Y,SamplePosition.Z, 0.02, 0.01);
		return (Noise.IQNoise_3D(SamplePosition.X,SamplePosition.Y,SamplePosition.Z, NoiseFrequency, 15) - 0.1) / 0.5;
	}
};

///////////////////////////////////////////////////////////////////////////////

TVoxelSharedRef<FVoxelGeneratorInstance> UPlanetGenerator::GetInstance()
{
	return MakeVoxelShared<FPlanetGeneratorInstance>(*this);
}
