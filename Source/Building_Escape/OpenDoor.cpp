// Copyright 2020 Ashish Jagadish, Inc. All Rights Reserved.


#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT


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

	//UE_LOG(LogTemp, Warning, TEXT("Current rotation of %s is: %s"), *GetOwner()->GetName(), *GetOwner()->GetActorRotation().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Current yaw (of rotation) of %s is: %f"), *GetOwner()->GetName(), CurrentYaw);

	// checking if PressurePlate is true helps to avoid unreal from crashing due to null pointer during the operarion PressurePlate->IsOverlappingActor(ActorThatOpensDoor)
	//if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpensDoor))
	if (TotalMassOfActors() > MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (DoorLastOpened + DoorCloseDelay < GetWorld()->GetTimeSeconds())
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(const float& RefDeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, TargetYaw, RefDeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

void UOpenDoor::CloseDoor(const float& RefDeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, RefDeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OverlappingActors);

	// Add up their masses
	for (AActor* OverlappingActor : OverlappingActors)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s is on pressure plate"), *OverlappingActor->GetName());
		TotalMass += OverlappingActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}