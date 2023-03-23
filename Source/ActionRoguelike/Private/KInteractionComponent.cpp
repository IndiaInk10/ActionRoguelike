// Fill out your copyright notice in the Description page of Project Settings.


#include "KInteractionComponent.h"

#include "KGameplayInterface.h"


// Sets default values for this component's properties
UKInteractionComponent::UKInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UKInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UKInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UKInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); // 참조연산자를 통한 값 대입

	FVector End = EyeLocation + (EyeRotation.Vector() * 1000);
	
	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	float Radius = 30.0f;
	Shape.SetSphere(Radius);
	
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	for(FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if(HitActor)
		{
			if(HitActor->Implements<UKGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
			
				IKGameplayInterface::Execute_Interact(HitActor, MyPawn);
				
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
				break;
			}
		}
	}

	DrawDebugLine(GetWorld(), EyeLocation, End, FColor::Green, false, 2.0f, 0, 2.0f);
}