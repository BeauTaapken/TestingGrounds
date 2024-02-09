// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestingGroundsGameMode.h"
#include "InfiniteTerrainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGGROUNDS_API AInfiniteTerrainGameMode : public ATestingGroundsGameMode
{
	GENERATED_BODY()
	
public:
	AInfiniteTerrainGameMode();

	UFUNCTION(BlueprintCallable, Category = "Bounds Pool")
	void PopulateBoundsVolumePool();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void NewTileConquered();

	UFUNCTION(BlueprintGetter, Category = "Score")
	int32 GetScore();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	class UActorPool* NavMeshBoundsVolumePool;


private:
	void AddToPool(class ANavMeshBoundsVolume *VolumeToAdd);

	int32 Score = 0;
};