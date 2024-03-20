#include "FloatingIslandOnionGenerator.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "VoxelUtilities/VoxelSDFUtilities.h"

class FFloatingIslandOnionGeneratorInstance : public TVoxelGeneratorInstanceHelper<FFloatingIslandOnionGeneratorInstance, UFloatingIslandOnionGenerator>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FFloatingIslandOnionGeneratorInstance, UFloatingIslandOnionGenerator>;

	explicit FFloatingIslandOnionGeneratorInstance(const UFloatingIslandOnionGenerator& MyGenerator)
		: Super(&MyGenerator)
		, Seed(MyGenerator.Seed)
		, Height(MyGenerator.Height)
		, TopNoiseFrequency(MyGenerator.TopNoiseFrequency)
		, TopNoiseStrength(MyGenerator.TopNoiseStrength)
		, PerturbFrequency(MyGenerator.PerturbFrequency)
		, PerturbAmplitude(MyGenerator.PerturbAmplitude)
	{
	}

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override
	{
		TopNoise.SetSeed(Seed);
		TopNoise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
		TopNoise.SetFractalOctavesAndGain(15, 0.5);
		TopNoise.SetFractalLacunarity(2.0);
		TopNoise.SetFractalType(EVoxelNoiseFractalType::FBM);
		TopNoise.SetMatrixFromRotation_2D(40);
		
		ShapeNoise.SetSeed(FVoxelUtilities::MurmurHash32(Seed));
		ShapeNoise.SetInterpolation(EVoxelNoiseInterpolation::Quintic);
		ShapeNoise.SetFractalOctavesAndGain(7, 0.5);
		ShapeNoise.SetFractalLacunarity(2.0);
		ShapeNoise.SetFractalType(EVoxelNoiseFractalType::FBM);
	}

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		v_flt topNoise = Z - (TopNoise.IQNoise_2D(X,Y,TopNoiseFrequency, 15) * TopNoiseStrength);

		v_flt PerturbX = X;
		v_flt PerturbY = Y;
		ShapeNoise.GradientPerturbFractal_2D(PerturbX,PerturbY, PerturbFrequency,7, PerturbAmplitude);

		FVector elongated = Elongate(FVector(PerturbX,PerturbY,0), FVector(1,100,1));
		
		TVoxelRange<v_flt> cone = FVoxelSDFUtilities::sdRoundCone(
			FVector(elongated.X,elongated.Y,Z),
			FVector::Zero(),
			FVector(0,0,-Height),
			150,
			50);

		v_flt firstLayer = Shell(cone.GetSingleValue(), 150);
		v_flt secondLayer = Shell(firstLayer, 75);
		v_flt thirdLayer = Shell(secondLayer, 37);

		TVoxelRange<v_flt> smooth = FVoxelSDFUtilities::opSmoothIntersection(topNoise,thirdLayer,20);
		return smooth.GetSingleValue() * 0.2;
	}
	
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
	{
		FVoxelMaterialBuilder Builder;
		Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
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
	FVoxelFastNoise TopNoise;
	FVoxelFastNoise ShapeNoise;

	const int32 Seed;
	const float Height;
	const float TopNoiseFrequency;
	const float TopNoiseStrength;
	const float PerturbFrequency;
	const float PerturbAmplitude;

	v_flt Shell(v_flt DistanceField, v_flt Thickness) const
	{
		return FMath::Abs(DistanceField) - Thickness;
	}

	FVector Elongate(const FVector& Position, const FVector& Size) const
	{
		FVector Elongated;
		Elongated.X = FMath::Clamp(Position.X, -Size.X, Size.X);
		Elongated.Y = FMath::Clamp(Position.Y, -Size.Y, Size.Y);
		Elongated.Z = FMath::Clamp(Position.Z, -Size.Z, Size.Z);
		return Position - Elongated;
	}
};

///////////////////////////////////////////////////////////////////////////////

TVoxelSharedRef<FVoxelGeneratorInstance> UFloatingIslandOnionGenerator::GetInstance()
{
	return MakeVoxelShared<FFloatingIslandOnionGeneratorInstance>(*this);
}
