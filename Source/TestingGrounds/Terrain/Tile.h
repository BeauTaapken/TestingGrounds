// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT()
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	FVector Location; 
	float Rotation;
	float Scale;
};

USTRUCT(BlueprintType)
struct FSpawnObjects
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	UStaticMeshComponent* floor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	int MinSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	int MaxSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	float Radius = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	float MinScale = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
	float MaxScale = 1;
};

class UActorPool;

UCLASS()
class TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceActors(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<AActor>> ToSpawn);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceAIPawns(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<APawn>> ToSpawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category="Navigation")
	FVector NavigationBoundsOffset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* InPool);

	UFUNCTION(BlueprintCallable, Category = "Score")
	void TileConquered();

private:
	static FBox FixFloorFBox(FBox Box);

	bool CanSpawnAtLocation(FVector Location, float Radius);

	TArray<FSpawnPosition> RandomSpawnPositions(const FSpawnObjects& SpawnObjects);

	bool FindEmptyLocation(UStaticMeshComponent* floor, FVector& OutLocation, float Radius);
	
	void PositionNavMeshBoundsVolume();
	
	template<class T>
	void RandomlyPlaceActors(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<T>> ToSpawn);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition);

	UActorPool* Pool;

	AActor* NavMeshBoundsVolume;

	bool IsTileConquered = false;
};
