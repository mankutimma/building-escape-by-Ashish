// Copyright 2020 Ashish Jagadish, Inc. All Rights Reserved.


#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

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

	// UE_LOG(LogTemp, Warning, TEXT("Grabber reporting to duty!"));

	FindPhysicsHandle();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Activate VisualizeDebugLine to see the line emerging from player's eyes
	// VisualizeDebugLine();

	// If the physics handle is attached
		// Move the object we are holding

	if (!PhysicsHandle) { return; } // Pointer usage must be protected. This check is same as if (PhysicsHandle == nullptr).
	// If the physics handle is present on the owner actor, the pointer in the above if condition will not be a null pointer and the below if codeblock will be 
	// executed if physics handle has grabbed a physics object
	if (PhysicsHandle->GrabbedComponent)
	{
		//Move the object we are holding using the physics handle
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

void UGrabber::FindPhysicsHandle()
{
	// Checking for physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// Warning against a null pointer
	if (PhysicsHandle == nullptr) // if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	PlayerInputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (PlayerInputComponent) // true even if input keys are not mentioned under access mappings in projectsettings->engine->input. wth?
	{	
		UE_LOG(LogTemp, Warning, TEXT("Player input component found on %s"), *GetOwner()->GetName());
		PlayerInputComponent->BindAction("GrabAndRelease", EInputEvent::IE_Released, this, &UGrabber::Release);
		PlayerInputComponent->BindAction("GrabAndRelease", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player input component MISSING on %s"), *GetOwner()->GetName());
	}
}

FVector UGrabber::GetPlayerWorldPosition() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Player location is %s and player rotation is %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());
	return PlayerViewPointLocation;
}

FVector UGrabber::GetPlayerReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Player location is %s and player rotation is %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

	FVector LineTraceDirection = PlayerViewPointRotation.Vector();
	FVector LineTraceEnd = PlayerViewPointLocation + LineTraceDirection * Reach;
	return LineTraceEnd;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;

	// Ray casting and return true for the first blocking hit
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerWorldPosition(),
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner()) // ignore the owner because the cast out ray first hits the player
		);

	return Hit;
}

void UGrabber::Grab()
{
	// UE_LOG(LogTemp, Warning, TEXT("Grabber press"));

	// If we (casted ray) hit something, then attach the physics handle. Attach physics handle = hold object

	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	AActor* ActorThatIsHit = HitResult.GetActor();
	// If the casted ray hit something, then, HitResult.GetActor() will not be a null pointer and the code inside if codeblock will get executed
	// Also, since HitResult.GetActor() is not a null pointer, *HitResult.GetActor()->GetName() will not lead to undefined behavior (UE4 crashing)
	if (ActorThatIsHit)
	{
		// Always check pointer before usage. This is pointer check: !PhysicsHandle. This (arrow i.e., ->) is usage: PhysicsHandle->GrabComponentAtLocation
		if (!PhysicsHandle) { return; }
		// Attach physics handle, i.e., grab the object which was hit by the ray
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
		UE_LOG(LogTemp, Warning, TEXT("You just grabbed %s"), *HitResult.GetActor()->GetName()); // *ActorThatIsHit->GetName()
	}
}

void UGrabber::Release()
{
	// UE_LOG(LogTemp, Warning, TEXT("Grabber released"));

	// remove/release the physics handle i.e., drop/release the held object
	if (PhysicsHandle)
	{
		PhysicsHandle->ReleaseComponent();
	}
	//else
	//{
	//  // Only check as above in if statement above is enough. No need to log here since the log is already done in BeginPlay when FindPhysicsHandle() is called
	//	UE_LOG(LogTemp, Error, TEXT("No physics handle found on %s to release object"), *GetOwner()->GetName()); 
	//}
}

void UGrabber::VisualizeDebugLine() const
{
	DrawDebugLine(
		GetWorld(),
		GetPlayerWorldPosition(),
		GetPlayerReach(),
		FColor(0, 0, 255),
		false,
		0.f,
		0.f,
		5.f
	);
}
