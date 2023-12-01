// Fill out your copyright notice in the Description page of Project Settings.


#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "PatrolRoute.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) {
    //Get patrol route
    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
    UPatrolRoute* PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolRoute>();

    if (!ensure(PatrolRoute)) { return EBTNodeResult::Failed; }

    TArray<AActor*> PatrolPoints = PatrolRoute->GetPatrolPoints();
    if (PatrolPoints.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("A guard does not have patrol points"));
        return EBTNodeResult::Failed;
    }
    
    //Set next waypoint
    UBlackboardComponent* BlackBoardComponent = OwnerComp.GetBlackboardComponent();
    int Index = BlackBoardComponent->GetValueAsInt(IndexKey.SelectedKeyName);
    BlackBoardComponent->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

    //Cycle the index
    int NextIndex = (Index + 1) % PatrolPoints.Num();
    BlackBoardComponent->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

    return EBTNodeResult::Succeeded;
}
