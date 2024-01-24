// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "RemoteMovieRenderStyle.h"

class FRemoteMovieRenderCommands : public TCommands<FRemoteMovieRenderCommands>
{
public:

	FRemoteMovieRenderCommands()
		: TCommands<FRemoteMovieRenderCommands>(TEXT("RemoteMovieRender"), NSLOCTEXT("Contexts", "RemoteMovieRender", "RemoteMovieRender Plugin"), NAME_None, FRemoteMovieRenderStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
