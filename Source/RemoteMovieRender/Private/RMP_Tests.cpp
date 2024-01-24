
#include "CoreMinimal.h"
#include "RMP_Job.h"
#include "LevelSequence.h"
#include "..\Public\RMP_Subsystem.h"
#include "RMP_Settings.h"

#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

struct RenderSequenceToMovieParameters
{
	TWeakObjectPtr<URMP_Job> TestJob;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FRenderSequenceToMovie, RenderSequenceToMovieParameters, InRenderSequenceToMovieParameters);

bool FRenderSequenceToMovie::Update()
{
	static double LastReportTime = FPlatformTime::Seconds();
	double TimeNow = FPlatformTime::Seconds();

	if (TimeNow - StartTime > 50.0)
	{
		UE_LOG(LogEditorAutomationTests, Warning, TEXT("TimeOut for FRenderSequenceToMovie"));
		return true;
	}

	if (LastReportTime - TimeNow > 5.0)
	{
		LastReportTime = TimeNow;

		FString LogText;
		switch (InRenderSequenceToMovieParameters.TestJob->GetJobState())
		{
		case ECreateMovieJobState::SequenceCreating:
			{
				LogText = "SequenceCreating";
				break;
			}
		case ECreateMovieJobState::MovieRendering:
			{
				LogText = "MovieRendering";
				break;
			}
		case ECreateMovieJobState::Uploading:
			{
				LogText = "Uploading";
				break;
			}
		}
		
		UE_LOG(LogEditorAutomationTests, Log, TEXT("Waiting for FRenderSequenceToMovie: %s == %s"), *LogText,
			*InRenderSequenceToMovieParameters.TestJob->GetJobName());
		
	}
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND(FCreateFakeSequence);
bool FCreateFakeSequence::Update()
{
	// wait 5 seconds, and return a fake sequence.
	const double TimeNow = FPlatformTime::Seconds();
	if (TimeNow - StartTime > 5.0)
	{
		UE_LOG(LogEditorAutomationTests, Log, TEXT("Doing for CreateFakeSequence"));
		if (GetDefault<URMP_Settings>()->TestSequence.IsNull())
		{
			UE_LOG(LogEditorAutomationTests, Error, TEXT("TestSequence is null"));
			return true;
		}
		if (GetDefault<URMP_Settings>()->TestSequenceLevel.IsNull())
		{
			UE_LOG(LogEditorAutomationTests, Error, TEXT("TestSequenceLevel is null"));
			return true;
		}
		 
		const auto TestSequence = Cast<ULevelSequence>(GetDefault<URMP_Settings>()->TestSequence.TryLoad());
		if (TestSequence == nullptr)
		{
			UE_LOG(LogEditorAutomationTests, Error, TEXT("Can not load TestSequence %s"), *GetDefault<URMP_Settings>()->TestSequence.GetAssetPathString());
			return true;
		}
		GEngine->GetEngineSubsystem<URMP_Subsystem>()->OnCreateNewSequenceFinishImpl(TestSequence, GetDefault<URMP_Settings>()->TestSequenceLevel);
		return true;
	}

	return false;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRMP_Test, "RMP.MainTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRMP_Test::RunTest(const FString& Parameters)
{
	auto Service = GEngine->GetEngineSubsystem<URMP_Subsystem>();
	// temp add SequenceMaker to the delegate
	const bool bSequenceMakerExist = false;
	if (bSequenceMakerExist == false)
	{
		Service->OnCreateNewSequenceImpl.BindLambda(
			[](const FCreateNewSequence_Param& InParam) -> bool
			{
				UE_LOG(LogEditorAutomationTests, Log, TEXT("Waiting for CreateFakeSequence"));
				ADD_LATENT_AUTOMATION_COMMAND(FCreateFakeSequence());
				return true;
			});
	}

	TWeakObjectPtr<URMP_Job> TestJob = Service->NewJob("Test");
	TestJob->StartJob();

	// check the fake file exist.
	const FString FakeFileName = TestJob->GetFileFullPath("_waiting");
	const bool bFakeFileExist = IFileManager::Get().FileExists(*FakeFileName);
	TestTrue(TEXT("Fake file should exist"), bFakeFileExist);
	

	RenderSequenceToMovieParameters Params;
	Params.TestJob = TestJob;
	ADD_LATENT_AUTOMATION_COMMAND(FRenderSequenceToMovie(Params));

	return true;
}