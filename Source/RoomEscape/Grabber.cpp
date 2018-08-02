// Copyright RaxaStudios 2018

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsComp();
	SetupInputComp();
}

void UGrabber::FindPhysicsComp()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComp()
{
	// Look for input component
	InputComp = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComp)
	{
		InputComp->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComp->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab() {
	auto HitResult = GetBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit) 
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab, 
			NAME_None, 
			ComponentToGrab->GetOwner()->GetActorLocation(), 
			true
		);
	}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));
	// release actor
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// check for attached object every frame, move as needed, else do nothing
	if (PhysicsHandle->GrabbedComponent) 
	{
		PhysicsHandle->SetTargetLocation(GetTraceEnd());
	}
}

FVector UGrabber::GetTraceStart()
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewLocation,
		OUT PlayerViewRotation
	);
	return PlayerViewLocation;

}

FVector UGrabber::GetTraceEnd() 
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewLocation,
		OUT PlayerViewRotation
	);
	FVector LineTraceEnd = PlayerViewLocation + PlayerViewRotation.Vector() * Reach;

	/*DrawDebugLine(
		GetWorld(),
		PlayerViewLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f
	);*/
	return LineTraceEnd;
}

const FHitResult UGrabber::GetBodyInReach()
{
	// Ray-cast(line-trace) out to reach distance
	FHitResult Hit;
	FCollisionQueryParams Trace(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetTraceStart(),
		GetTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		Trace
	);

	// Check for hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit: %s"), *(ActorHit->GetName()));
	}


	return Hit;
}
