#pragma once

#include "CoreMinimal.h"
#include "VoxelGenerators/VoxelGenerator.h"
#include "PlanetGenerator.generated.h"

UCLASS(Blueprintable)
class VOXELSAMPLES_API UPlanetGenerator : public UVoxelGenerator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float Radius = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float NoiseFrequency = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float NoiseStrength = 0.02;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UCurveFloat> PlanetHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UCurveLinearColor> PlanetColor;
	
	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};