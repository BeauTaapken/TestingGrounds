// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "TestingGrounds/ActorPool.h"
#include "TestingGrounds/Character/Mannequin.h"
#include "TestingGrounds/InfiniteTerrainGameMode.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavigationBoundsOffset = FVector(2000, 0, 0);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	Super::EndPlay(EndPlayReason);

	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);
	for (AActor* Actor : AttachedActors) {
		//TODO this can still sometimes leave dead AI lying around, probably won't happen enough to cause a crash, but still something that we might want to try and fix
		if (!Actor->IsA(AMannequin::StaticClass()) || (Actor->IsA(APawn::StaticClass()) && Cast<APawn>(Actor)->GetController() == nullptr)) {
			Actor->Destroy();
		}
	}

	if (Pool != nullptr && NavMeshBoundsVolume != nullptr)
	{
		Pool->Return(NavMeshBoundsVolume);
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATile::SetPool(UActorPool* InPool) 
{
	Pool = InPool;

	PositionNavMeshBoundsVolume();
}

void ATile::TileConquered() 
{
	AInfiniteTerrainGameMode* GameMode = Cast<AInfiniteTerrainGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode && !IsTileConquered) {
		GameMode->NewTileConquered();
		IsTileConquered = true;
	}
}

void ATile::PositionNavMeshBoundsVolume() {
  	NavMeshBoundsVolume = Pool->Checkout();
  	if (NavMeshBoundsVolume == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Not enough actors in pool."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Got actor from pool."));
  	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	FNavigationSystem::Build(*GetWorld());
}

template<class T>
void ATile::RandomlyPlaceActors(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<T>> ToSpawn) 
{
	if (SpawnObjects.floor == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Unable to get floor from Tile, will not spawn things on top"));
		return;
	}
	
	int NumberToSpawn = FMath::RandRange(SpawnObjects.MinSpawn, SpawnObjects.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++) {
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnObjects.MinScale, SpawnObjects.MaxScale);
		if (FindEmptyLocation(SpawnObjects.floor, SpawnPosition.Location, SpawnObjects.Radius * SpawnPosition.Scale)) {
			SpawnPosition.Rotation = FMath::FRandRange(-180.0f, 180.0f);
			int itemToSpawnInArray = ToSpawn.Num() > 1 ? 
				FMath::RandRange(0, ToSpawn.Num()-1) :
				0;
			PlaceActor(ToSpawn[itemToSpawnInArray], SpawnPosition);
		}
	}
}

void ATile::PlaceActors(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<AActor>> ToSpawn) 
{
	RandomlyPlaceActors(SpawnObjects, ToSpawn);
}

void ATile::PlaceAIPawns(const FSpawnObjects& SpawnObjects, TArray<TSubclassOf<APawn>> ToSpawn) 
{
	RandomlyPlaceActors(SpawnObjects, ToSpawn);
}

FBox ATile::FixFloorFBox(FBox Box) 
{
	Box.Min.X = 0;
	Box.Min.Z = 0;
	Box.Max.X = Box.Max.X * 2;
	Box.Max.Z = 0;
	return Box;
}

TArray<FSpawnPosition> ATile::RandomSpawnPositions(const FSpawnObjects& SpawnObjects)
{
	TArray<FSpawnPosition> SpawnPositions;
	int NumberToSpawn = FMath::RandRange(SpawnObjects.MinSpawn, SpawnObjects.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++) {
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnObjects.MinScale, SpawnObjects.MaxScale);
		if (FindEmptyLocation(SpawnObjects.floor, SpawnPosition.Location, SpawnObjects.Radius * SpawnPosition.Scale)) {
			SpawnPosition.Rotation = FMath::FRandRange(-180.0f, 180.0f);
			SpawnPositions.Add(SpawnPosition);
		}			
	}
	return SpawnPositions;
}

bool ATile::FindEmptyLocation(UStaticMeshComponent* floor, FVector& OutLocation, float Radius) 
{
	FBox FloorBox = floor->GetStaticMesh()->GetBoundingBox();
	FloorBox = FixFloorFBox(FloorBox);
	const int MAX_ATTEMPTS = 100;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++) {
		FVector CandidatePoint = FMath::RandPointInBox(FloorBox);
		if (CanSpawnAtLocation(CandidatePoint, Radius)) {
			OutLocation = CandidatePoint;
			return true;
		}
	} 
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition) 
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (Spawned)
	{
		Spawned->SetActorRelativeLocation(SpawnPosition.Location);
		Spawned->SetActorRelativeRotation(FRotator(0, SpawnPosition.Rotation, 0));
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
	}
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition) 
{
	FRotator Rotation = FRotator(0, SpawnPosition.Rotation, 0);
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SpawnDefaultController();
		Spawned->Tags.Add(FName("Enemy"));
	}
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius) 
{ 
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	return !HasHit;
}
