#pragma once
#include "CommandDispatcher.h"
#include "CommandHandler.h"

class FProceduralLandscapeHandler : public FCommandHandler
{
public:
	void RegisterCommands();

private:
	FExecStatus GenerateLandscape(const TArray<FString>& Args);
	FExecStatus SegmentationMask(const TArray<FString>& Args);
	FExecStatus IsMapGenerationCompleted(const TArray<FString>& Args);

};