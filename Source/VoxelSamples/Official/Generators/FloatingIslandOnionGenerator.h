#pragma once

#include "CoreMinimal.h"
#include "VoxelGenerators/VoxelGenerator.h"
#include "FloatingIslandOnionGenerator.generated.h"

UCLASS(Blueprintable)
class VOXELSAMPLES_API UFloatingIslandOnionGenerator : public UVoxelGenerator
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float Height = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float TopNoiseFrequency = 0.002;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float TopNoiseStrength = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float PerturbFrequency = 0.01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float PerturbAmplitude = 50;
	
	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};