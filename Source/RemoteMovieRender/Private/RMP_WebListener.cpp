// Fill out your copyright notice in the Description page of Project Settings.


#include "RMP_WebListener.h"

#include "RMP_Subsystem.h"


// Sets default values
ARMP_WebListener::ARMP_WebListener()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARMP_WebListener::OnNewMovieRequest(const FString& InMoiveURL)
{
	UE_LOG(LogRMP, Log, TEXT("ARMP_WebListener::OnNewMovieRequest:%s"), *InMoiveURL);
	GEngine->GetEngineSubsystem<URMP_Subsystem>()->NewJob(InMoiveURL);
	
}

// Called when the game starts or when spawned
void ARMP_WebListener::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARMP_WebListener::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

