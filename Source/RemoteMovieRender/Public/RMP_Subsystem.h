// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "MovieRenderPipelineDataTypes.h"
#include "RMP_Subsystem.generated.h"

class URMP_Job;
DECLARE_LOG_CATEGORY_EXTERN(LogRMP, Log, All);

class ULevelSequence;
class UMoviePipelineExecutorJob;

USTRUCT(BlueprintType)
struct FCreateNewSequence_Param
{
	GENERATED_BODY()
	FString SequenceName;
	
};

DECLARE_DELEGATE_RetVal_OneParam(bool, FCreateSequenceDelegate, FCreateNewSequence_Param);
DECLARE_DELEGATE_RetVal_OneParam(bool, FCancelCreateSequenceDelegate, FCreateNewSequence_Param);

/**
 *
 */
UCLASS(BlueprintType)
class REMOTEMOVIERENDER_API URMP_Subsystem : public UEngineSubsystem 
{
	GENERATED_BODY()

public:
	friend class URMP_Job;
	/**
	 *  Start the service, listen to the http request from PAT.
	 */
	void StartService();
	/**
	 *  Stop the service 
	 */
	void StopService();

	/**
	 *  Create a new job to render a sequence to a mp4 file.
	 */
	URMP_Job* NewJob(FString URL_Name);

	////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *  USubsystem Interface
	 */
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	////////////////////////////////////////////////////////////////////////////////////////


	
	////////////////////////////////////////////////////////////////////////////////////////
	/**
	 * Interface To Sequence Creator 
	 */
	
	FCreateSequenceDelegate OnCreateNewSequenceImpl;
	FCancelCreateSequenceDelegate OnCancelNewSequenceImpl;
	void OnCreateNewSequenceFinishImpl(ULevelSequence* InSequence,const FSoftObjectPath& InLevel);
	////////////////////////////////////////////////////////////////////////////////////////

protected:
	void CreateNewSequence(const FCreateNewSequence_Param& InParam) const;
	void CancelNewSequence(const FCreateNewSequence_Param& InParam) const;

private:
	bool Tick(float DeltaTime);
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<URMP_Job>> CachedJobs;

	FTSTicker::FDelegateHandle TickerHandle;
};
