// Fill out your copyright notice in the Description page of Project Settings.


#include "RMP_Subsystem.h"

#include "LevelSequence.h"
#include "RMP_Job.h"
#include "RMP_Settings.h"

DEFINE_LOG_CATEGORY(LogRMP)


URMP_Job* URMP_Subsystem::NewJob(const FString& URL_Name, const FString& LLMParam, bool InNeedFakeSequence)
{
	URMP_Job* NewJob = NewObject<URMP_Job>();
	NewJob->Init(URL_Name,LLMParam,InNeedFakeSequence);

	CachedJobs.Add(NewJob);

	return NewJob;
}

void URMP_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &URMP_Subsystem::Tick));
}

void URMP_Subsystem::Deinitialize()
{
	Super::Deinitialize();

	FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
}

void URMP_Subsystem::CreateNewSequence(const FCreateNewSequence_Param& InParam, bool bUseFakeSequence) const
{
	if (!bUseFakeSequence)
	{
		if (OnCreateNewSequenceImpl.IsBound())
		{
			OnCreateNewSequenceImpl.Execute(InParam);
		}
		else
		{
			UE_LOG(LogRMP, Error, TEXT("OnCreateNewSequenceImpl: Is Not Bound"));
		}
	}
	else
	{
		// for test without sequenceMaker
		if (GetDefault<URMP_Settings>()->TestSequence.IsNull())
		{
			UE_LOG(LogRMP, Error, TEXT("TestSequence is null"));
			return;
		}
		if (GetDefault<URMP_Settings>()->TestSequenceLevel.IsNull())
		{
			UE_LOG(LogRMP, Error, TEXT("TestSequenceLevel is null"));
			return;
		}
		 
		const auto TestSequence = Cast<ULevelSequence>(GetDefault<URMP_Settings>()->TestSequence.TryLoad());
		if (TestSequence == nullptr)
		{
			UE_LOG(LogRMP, Error, TEXT("Can not load TestSequence %s"), *GetDefault<URMP_Settings>()->TestSequence.GetAssetPathString());
			return;
		}
		GEngine->GetEngineSubsystem<URMP_Subsystem>()->OnCreateNewSequenceFinishImpl(TestSequence, GetDefault<URMP_Settings>()->TestSequenceLevel);
	}
}

void URMP_Subsystem::CancelNewSequence(const FCreateNewSequence_Param& InParam) const
{
	if (OnCancelNewSequenceImpl.IsBound())
	{
		OnCancelNewSequenceImpl.Execute(InParam);
	}
	else
	{
		UE_LOG(LogRMP, Error, TEXT("OnCancelNewSequenceImpl: Is Not Bound"));
	}
}


void URMP_Subsystem::OnCreateNewSequenceFinishImpl(ULevelSequence* InSequence, const FSoftObjectPath& InLevel)
{
	URMP_Job* Job = CachedJobs.Num() > 0 ? CachedJobs[0] : nullptr;

	if (Job != nullptr)
	{
		Job->OnCreateNewSequenceFinish(InSequence, InLevel);
	}
	else
	{
		UE_LOG(LogRMP, Error, TEXT("OnCreateNewSequenceFinish: Job is nullptr"));
	}
}

bool URMP_Subsystem::Tick(float DeltaTime)
{
	URMP_Job* Job = CachedJobs.Num() > 0 ? CachedJobs[0] : nullptr;

	if (Job != nullptr)
	{
		if (Job->IsFinished())
		{
			CachedJobs.RemoveAt(0);
		}
		else if (!Job->IsRunning())
		{
			Job->StartJob();
		}
	}
	
	return true; // Keep ticking
}
