// Copyright RaxaStudios 2018

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	PawnOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

}

void UOpenDoor::OpenDoor()
{
	FRotator open = FRotator(0.0f, OpenAngle, 0.0f);
	Owner->SetActorRotation(open);
}


void UOpenDoor::CloseDoor()
{
	FRotator close = FRotator(0.0f, 0.0f, 0.0f);
	Owner->SetActorRotation(close);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll trigger volume, if actor opens, open door
	if (PressurePlate->IsOverlappingActor(PawnOpens))
	{
		OpenDoor();
		//UE_LOG(LogTemp, Warning, TEXT("TESTING PRESSUREPLATE HIT"));
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	// Check for time to close door
	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorCloseDelay) 
	{
		CloseDoor();
	}
}

