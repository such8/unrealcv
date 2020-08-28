#include "ProceduralLandscapeHandler.h"

#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#pragma optimize("", off)

void FProceduralLandscapeHandler::RegisterCommands()
{
	CommandDispatcher->BindCommand(
		"vset /simulator/generate",
		FDispatcherDelegate::CreateRaw(this, &FProceduralLandscapeHandler::GenerateLandscape),
		"Generate a new landscape (replacing the old one)"
	);

	CommandDispatcher->BindCommand(
		"vset /simulator/segmentation_mask [str]",
		FDispatcherDelegate::CreateRaw(this, &FProceduralLandscapeHandler::SegmentationMask),
		"Turns segmentation mask on or off"
	);

	CommandDispatcher->BindCommand(
		"vget /simulator/is_generation_completed",
		FDispatcherDelegate::CreateRaw(this, &FProceduralLandscapeHandler::IsMapGenerationCompleted),
		"Queries whether map generation has finished."
	);

}

FExecStatus FProceduralLandscapeHandler::GenerateLandscape(const TArray<FString>& Args)
{

	UWorld* world = this->GetWorld();
	AGameMode* gameMode = world->GetAuthGameMode<AGameMode>();
	if (gameMode != nullptr) {
		gameMode->RestartGame();
		return FExecStatus::OK("Generating new landscape...");
	}

	return FExecStatus::Error("Failed casting the current game mode to AGameMode. Maybe your gamemode's parent class is AGameModeBase?");
}

FExecStatus FProceduralLandscapeHandler::SegmentationMask(const TArray<FString>& Args) {
	FString state = Args[0];

	APostProcessVolume* ISeg_PPVol = nullptr;
	APostProcessVolume* _postProcessVolume;
	UWorld* world = GetWorld();
	for (TActorIterator<APostProcessVolume> ActorItr(world); ActorItr; ++ActorItr)
	{
		_postProcessVolume = Cast<APostProcessVolume>(*ActorItr);
		if (IsValid(_postProcessVolume) && _postProcessVolume->GetName().Equals("ImageSegmentation_PPVolume")) {
			ISeg_PPVol = _postProcessVolume;
			break;
		}
	}
	
	if (IsValid(ISeg_PPVol)) {
		if (state.Equals("on")) {
			ISeg_PPVol->BlendWeight = 1.0f;
		}
		else {
			if (state.Equals("off")) {
				ISeg_PPVol->BlendWeight = 0.0f;
			}
			else {
				return FExecStatus::Error("Expected argument \"on\" or \"off\"");
			}
		}

		return FExecStatus::OK();
	}

	return FExecStatus::Error("Couldn't find Post Processing Volume");

	
}

FExecStatus FProceduralLandscapeHandler::IsMapGenerationCompleted(const TArray<FString>& Args)
{
	TArray<AActor*> genReadyActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GenerationReady"), genReadyActors);

	if (genReadyActors.Num() == 0) {
		return FExecStatus::Error("Generation is still in progress");
	} 

	return FExecStatus::OK();
}

#pragma optimize("", on)