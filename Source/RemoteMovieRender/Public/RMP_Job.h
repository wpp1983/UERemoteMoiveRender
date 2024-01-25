// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieRenderPipelineDataTypes.h"
#include "UObject/Object.h"
#include "RMP_Job.generated.h"

class UMoviePipelineExecutorBase;
class UMoviePipelineExecutorJob;
class ULevelSequence;

UENUM()
enum ECreateMovieJobState
{
	None,
	Starting,
	SequenceCreating,
	MovieRendering,
	Uploading,
	Finished,
};

/**
 * the job will
 *    1. listen to the http request from PAT
 *		a. create a fake file with name XXX_waiting
 *    2. create a new sequence
 *		a. need the SequenceEditor register to the "CreateNewSequence" delegate
 *		b. call "OnCreateNewSequenceFinish" when the sequence created 
 *    3. render the sequence to mp4
 *    4. upload the mp4
 *		a. upload file
 *		a. rename the file XXX_waiting to XXX when upload finished
 */
UCLASS()
class URMP_Job : public UObject
{
	GENERATED_BODY()
public:
	URMP_Job();
	
	void Init(const FString& InMovieName, const FString& InLLMParam, bool InNeedFakeSequence = false);	

	void StartJob();

	bool IsRunning() const;
	bool IsFinished() const;

	FString GetJobName() const;

	FString GetFileFullPath(FString FileExtent = "") const;

	ECreateMovieJobState GetJobState() const { return JobState; }
	
	void OnCreateNewSequenceFinish(ULevelSequence* InSequence, const FSoftObjectPath& InLevel);

private:
	void CreateFakeMp4File();
	void CreateNewSequence();
	void RenderSequenceToMp4(ULevelSequence* InSequence, const FSoftObjectPath& InLevel);
	void OnMovieRenderFinished(UMoviePipelineExecutorBase* ExecutorBase, bool bSuccess);
	void OnMovieRenderErrored(UMoviePipelineExecutorBase* InExecutor, UMoviePipeline* InPipelineWithError, bool bIsFatal, FText ErrorText);
	void OnFinish();
	void OnDeleteWaitingFile();
	void OnFailed();
	void OnRemoveJob();

private:
	ECreateMovieJobState JobState;
	FString MovieName;
	FString LLMParam;
	
	bool bNeedFakeSequence;
};
