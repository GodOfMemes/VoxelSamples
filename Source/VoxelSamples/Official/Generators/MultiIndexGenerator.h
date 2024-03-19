#pragma once

#include "CoreMinimal.h"
#include "VoxelGenerators/VoxelGenerator.h"
#include "MultiIndexGenerator.generated.h"

UCLASS(Blueprintable)
class VOXELSAMPLES_API UMultiIndexGenerator : public UVoxelGenerator
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UMaterialInterface> Layer0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UMaterialInterface> Layer1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UMaterialInterface> Layer2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TObjectPtr<UMaterialInterface> Layer3;
	
	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};