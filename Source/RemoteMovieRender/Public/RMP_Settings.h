// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RMP_Settings.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, config=Editor, defaultconfig, MinimalAPI)
class URMP_Settings : public UObject
{
	GENERATED_UCLASS_BODY()


	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Test")
	FSoftObjectPath TestSequence;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Test")
	FSoftObjectPath TestSequenceLevel;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "MovieRenderPipeline")
	FSoftObjectPath MoviePipelinePrimaryConfig;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite)
	FDirectoryPath MovieOutputPath;
};
