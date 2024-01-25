
#include "CoreMinimal.h"
#include "RMP_Job.h"
#include "LevelSequence.h"
#include "..\Public\RMP_Subsystem.h"
#include "RMP_Settings.h"

#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

struct RenderSequenceToMovieParameters
{
	TObjectPtr<URMP_Job> TestJob;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FRenderSequenceToMovie, RenderSequenceToMovieParameters, InRenderSequenceToMovieParameters);

bool FRenderSequenceToMovie::Update()
{
	static double LastReportTime = FPlatformTime::Seconds();
	double TimeNow = FPlatformTime::Seconds();

	if (InRenderSequenceToMovieParameters.TestJob == nullptr)
	{
		UE_LOG(LogEditorAutomationTests, Warning, TEXT("FRenderSequenceToMovieParameters.TestJob is nullptr"));
		return true;
	}
	
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

	if (InRenderSequenceToMovieParameters.TestJob->IsFinished())
	{
		UE_LOG(LogEditorAutomationTests, Log, TEXT("FRenderSequenceToMovie finished: %s"), *InRenderSequenceToMovieParameters.TestJob->GetJobName());
		return true;
	}
	return false;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRMP_Test, "RMP.TestWithFakeSequence", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRMP_Test::RunTest(const FString& Parameters)
{
	auto Service = GEngine->GetEngineSubsystem<URMP_Subsystem>();
	// temp add SequenceMaker to the delegate

	auto TestJob = Service->NewJob("Test", "Test", true);
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