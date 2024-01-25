// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* ATile::PlaceActors(UStaticMeshComponent* floor, TArray<TSubclassOf<AActor>> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale) 
{
	if (floor == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Unable to get floor from Tile, will not spawn things on top"));
		return nullptr;
	}

	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++) {
		FVector SpawnPoint;
		float RandomScale = FMath::RandRange(MinScale, MaxScale);
		if (FindEmptyLocation(floor, SpawnPoint, Radius * RandomScale)) {
			int itemToSpawnInArray = ToSpawn.Num() > 1 ? 
				FMath::RandRange(0, ToSpawn.Num()-1) :
				0;
			PlaceActor(ToSpawn[itemToSpawnInArray], SpawnPoint, RandomScale);
		}			
	}
	return floor;
}

FBox ATile::FixFloorFBox(FBox Box) 
{
	Box.Min.X = 0;
	Box.Min.Z = 0;
	Box.Max.X = Box.Max.X * 2;
	Box.Max.Z = 0;
	return Box;
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

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float RandomScale) 
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPoint);
	float Yaw = FMath::FRandRange(-180.0f, 180.0f);
	Spawned->SetActorRelativeRotation(FRotator(0, Yaw, 0));
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorScale3D(FVector(RandomScale));
	
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
