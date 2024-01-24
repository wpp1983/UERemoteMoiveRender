// Copyright Epic Games, Inc. All Rights Reserved.

#include "RemoteMovieRenderCommands.h"

#define LOCTEXT_NAMESPACE "FRemoteMovieRenderModule"

void FRemoteMovieRenderCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "RemoteMovieRender", "Execute RemoteMovieRender action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
