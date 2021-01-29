// Copyright 2020 Ashish Jagadish, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float Reach = 175.f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr; // Pointer should never remain uninitialized. Either initialize it to something meaningful or to nullptr
	UInputComponent* PlayerInputComponent = nullptr;

	void SetupInputComponent();
	FVector GetPlayerWorldPosition() const;
	FVector GetPlayerReach() const;
	FHitResult GetFirstPhysicsBodyInReach() const;
	void FindPhysicsHandle();
	void Grab();
	void Release();
	void VisualizeDebugLine() const;
};
