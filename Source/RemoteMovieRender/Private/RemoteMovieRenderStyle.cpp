// Copyright Epic Games, Inc. All Rights Reserved.

#include "RemoteMovieRenderStyle.h"
#include "RemoteMovieRender.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FRemoteMovieRenderStyle::StyleInstance = nullptr;

void FRemoteMovieRenderStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FRemoteMovieRenderStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FRemoteMovieRenderStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("RemoteMovieRenderStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FRemoteMovieRenderStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("RemoteMovieRenderStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("RemoteMovieRender")->GetBaseDir() / TEXT("Resources"));

	Style->Set("RemoteMovieRender.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FRemoteMovieRenderStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FRemoteMovieRenderStyle::Get()
{
	return *StyleInstance;
}
