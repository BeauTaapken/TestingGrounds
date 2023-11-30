// Fill out your copyright notice in the Description page of Project Settings.


#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) {
    UBlackboardComponent* BlackBoardComponent = OwnerComp.GetBlackboardComponent();
    int Index = BlackBoardComponent->GetValueAsInt(IndexKey.SelectedKeyName);
    UE_LOG(LogTemp, Warning, TEXT("Waypoint index: %i"), Index);
    return EBTNodeResult::Succeeded;
}
