// Fill out your copyright notice in the Description page of Project Settings.


#include "RMP_Job.h"

#include "FileHelpers.h"
#include "LevelEditor.h"
#include "LevelSequence.h"
#include "MoviePipelinePIEExecutor.h"
#include "MoviePipelineQueueEngineSubsystem.h"
#include "MoviePipelineQueueSubsystem.h"
#include "MovieRenderPipelineSettings.h"
#include "RMP_Settings.h"
#include "UnrealEdGlobals.h"
#include "..\Public\RMP_Subsystem.h"
#include "Editor/UnrealEdEngine.h"
#include "Kismet/GameplayStatics.h"

UWorld* FindCurrentWorld()
{
	UWorld* World = nullptr;
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (WorldContext.WorldType == EWorldType::Game)
		{
			World = WorldContext.World();
		}
		else if (WorldContext.WorldType == EWorldType::Editor)
		{
			World = WorldContext.World();
		}
#if WITH_EDITOR
		else if (GIsEditor && WorldContext.WorldType == EWorldType::PIE)
		{
			World = WorldContext.World();
			if (World)
			{
				return World;
			}
		}
#endif
	}

	return World;
}

URMP_Job::URMP_Job()
{
	JobState = ECreateMovieJobState::None;
}

void URMP_Job::Init(const FString& InMovieName, const FString& InLLMParam, bool InNeedFakeSequence)
{
	MovieName = InMovieName;
	LLMParam = InLLMParam;
	bNeedFakeSequence = InNeedFakeSequence;
	JobState = ECreateMovieJobState::None;
}

void URMP_Job::StartJob()
{
	UE_LOG(LogRMP, Log, TEXT("StartJob: %s"), *GetJobName());

	JobState = ECreateMovieJobState::Starting;
	
	CreateFakeMp4File();
}

bool URMP_Job::IsRunning() const
{
	return JobState != ECreateMovieJobState::None
	&& JobState != ECreateMovieJobState::Finished;
}

bool URMP_Job::IsFinished() const
{
	return JobState == ECreateMovieJobState::Finished;
}

FString URMP_Job::GetJobName() const
{
	return MovieName;
}

FString URMP_Job::GetFileFullPath(FString FileExtent) const
{
	FString FileLocation = FPaths::ConvertRelativePathToFull(GetDefault<URMP_Settings>()->MovieOutputPath.Path + TEXT("/"));
	FString FullPath = FString::Printf(TEXT("%s%s%s.mp4"), *FileLocation, *MovieName, *FileExtent);
	return FullPath;
}

void URMP_Job::CreateFakeMp4File()
{
	UE_LOG(LogRMP, Log, TEXT("Start CreateFakeMp4File: %s"), *GetJobName());

	const FString FakeFileName = GetFileFullPath("_waiting");
	FArchive* FakeFile = IFileManager::Get().CreateFileWriter(*FakeFileName);
	if (FakeFile == nullptr)
	{
		UE_LOG(LogRMP, Warning, TEXT("CreateFakeMp4File Failed: %s"), *FakeFileName)	
	}

	FakeFile->Close();
	delete FakeFile;

	CreateNewSequence();
}

void URMP_Job::CreateNewSequence()
{
	UE_LOG(LogRMP, Log, TEXT("Start CreateNewSequence: %s"), *GetJobName());

	JobState = ECreateMovieJobState::SequenceCreating;
	GEngine->GetEngineSubsystem<URMP_Subsystem>()->CreateNewSequence(FCreateNewSequence_Param{GetJobName()}, bNeedFakeSequence);
}

void URMP_Job::OnCreateNewSequenceFinish(ULevelSequence* InSequence, const FSoftObjectPath& InLevel)
{
	UE_LOG(LogRMP, Log, TEXT("OnCreateNewSequenceFinish: %s"), *GetJobName());

	RenderSequenceToMp4(InSequence, InLevel);
}

void URMP_Job::RenderSequenceToMp4(ULevelSequence* InSequence, const FSoftObjectPath& InLevel)
{
	UE_LOG(LogRMP, Log, TEXT("Start RenderSequenceToMp4: %s"), *GetJobName());
	
	JobState = ECreateMovieJobState::MovieRendering;

	UMoviePipelineQueueSubsystem* Subsystem = GEditor->GetEditorSubsystem<UMoviePipelineQueueSubsystem>();
	check(Subsystem);
	check(!Subsystem->IsRendering());

	auto SequenceToMovieJob = Subsystem->GetQueue()->AllocateNewJob(UMoviePipelineExecutorJob::StaticClass());
	SequenceToMovieJob->JobName = GetJobName();
	SequenceToMovieJob->Sequence = InSequence;
	SequenceToMovieJob->Map = InLevel;
	if (UMoviePipelinePrimaryConfig* AssetAsConfig = Cast<UMoviePipelinePrimaryConfig>(GetDefault<URMP_Settings>()->MoviePipelinePrimaryConfig.TryLoad()))
	{
		SequenceToMovieJob->SetConfiguration(AssetAsConfig);
	}
	else
	{
		SequenceToMovieJob->SetConfiguration(GetMutableDefault<UMoviePipelineExecutorJob>()->GetConfiguration());
	}

	const UMovieRenderPipelineProjectSettings* ProjectSettings = GetDefault<UMovieRenderPipelineProjectSettings>();
	TSubclassOf<UMoviePipelineExecutorBase> ExecutorClass = ProjectSettings->DefaultLocalExecutor.TryLoadClass<UMoviePipelineExecutorBase>();

	check(ExecutorClass != nullptr);
	auto SequenceToMoiveExecutor = Subsystem->RenderQueueWithExecutor(ExecutorClass);
	check(SequenceToMoiveExecutor != nullptr);
	
	SequenceToMoiveExecutor->OnExecutorFinished().AddUObject(this, &URMP_Job::OnMovieRenderFinished);
	SequenceToMoiveExecutor->OnExecutorErrored().AddUObject(this, &URMP_Job::OnMovieRenderErrored);
	
}

void URMP_Job::OnMovieRenderFinished(UMoviePipelineExecutorBase* ExecutorBase, bool bSuccess)
{
	UE_LOG(LogRMP, Log, TEXT("OnMovieRenderFinished: %s  Success:%s "),
		*GetJobName(), bSuccess ? TEXT("true") : TEXT("false"));

	OnRemoveJob();
	
	if (bSuccess)
	{
		// output file to local
		OnFinish();
	}
	else
	{
		OnFailed();
	}
	
}

void URMP_Job::OnMovieRenderErrored(UMoviePipelineExecutorBase* InExecutor, UMoviePipeline* InPipelineWithError,
	bool bIsFatal, FText ErrorText)
{
	UE_LOG(LogRMP, Log, TEXT("OnMovieRenderErrored: %s  : %s "),*GetJobName(), *ErrorText.ToString());
	
	OnRemoveJob();
	
	OnFailed();
}


void URMP_Job::OnFinish()
{
	UE_LOG(LogRMP, Log, TEXT("OnFinish: %s"), *GetJobName());
	JobState = ECreateMovieJobState::Finished;
	OnDeleteWaitingFile();
}

void URMP_Job::OnDeleteWaitingFile()
{
	const FString FakeFileName = GetFileFullPath("_waiting");
	IFileManager::Get().Delete(*FakeFileName);
}

void URMP_Job::OnFailed()
{
	UE_LOG(LogRMP, Log, TEXT("OnFailed: %s"), *GetJobName());
	JobState = ECreateMovieJobState::Finished;

	OnDeleteWaitingFile();
}


void URMP_Job::OnRemoveJob()
{
	UMoviePipelineQueueSubsystem* Subsystem = GEditor->GetEditorSubsystem<UMoviePipelineQueueSubsystem>();
	check(Subsystem);
	Subsystem->GetQueue()->DeleteAllJobs();
}

