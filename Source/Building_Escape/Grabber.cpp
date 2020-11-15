// Copyright 2020 Ashish Jagadish, Inc. All Rights Reserved.


#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"
#include "DrawDebugHelpers.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting to duty!"));

	// Checking for physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// Protection from null pointer
	if (PhysicsHandle)
	{
		// physics handle found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s"), *GetOwner()->GetName());
	}

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


void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber press"));
}


void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber released"));
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	//UE_LOG(LogTemp, Warning, TEXT("Player location is %s and player rotation is %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

	FVector LineTraceDirection = PlayerViewPointRotation.Vector();
	FVector LineTraceEnd =  PlayerViewPointLocation + LineTraceDirection * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 0, 255),
		false,
		0.f,
		0.f,
		5.f
	);

	FHitResult Hit;

	// Ray casting and return true for the first blocking hit
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner()) // ignore the owner because the cast out ray first hits the player
		);

	// see what it hits
	AActor* HitActor = Hit.GetActor();

	// avoid undefined behavior (ue4 crashing) when doing *HitActor->GetName() due to null pointer 
	if (HitActor)
	{
		UE_LOG(LogTemp, Error, TEXT("The line trace has hit %s"), *HitActor->GetName());
	}
}

