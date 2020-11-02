// Copyright 2020 Ashish Jagadish, Inc. All Rights Reserved.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


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

	// FRotator OpenDoor {0.f, 90.f, 0.f};
	// GetOwner()->SetActorRotation(OpenDoor);

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s actor has the OpenDoor component on it, but no pressure plate set!"), *GetOwner()->GetName())
	}

	ActorThatOpensDoor = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogTemp, Warning, TEXT("Current actor rotation is: %s"), *GetOwner()->GetActorRotation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Current yaw of the actor rotation is: %f"), CurrentYaw);

	// checking if PressurePlate is true helps to avoid unreal from crashing due to null pointer.
	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpensDoor))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (DoorLastOpened + 0.75 < GetWorld()->GetTimeSeconds())
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(const float& RefDeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, TargetYaw, RefDeltaTime * 1.f);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

void UOpenDoor::CloseDoor(const float& RefDeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, RefDeltaTime * .75f);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}