// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RMP_WebListener.generated.h"

UCLASS()
class REMOTEMOVIERENDER_API ARMP_WebListener : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARMP_WebListener();

	UFUNCTION(BlueprintCallable, Category = "RMP")
	void OnNewMovieRequest(const FString& InMovieName, const FString& InLLMParam);

	UFUNCTION(BlueprintCallable, Category = "RMP")
	void OnNewMovieRequestFakeSequence(const FString& InMovieName, const FString& InLLMParam);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
