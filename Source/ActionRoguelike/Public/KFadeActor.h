// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KGameplayInterface.h"
#include "Components/TimelineComponent.h"
#include "KFadeActor.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AKFadeActor : public AActor, public IKGameplayInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	UMaterialInterface* DynamicMaterial;

	FTimeline FadeTimeline;

	UPROPERTY(EditAnywhere)
	float EndingFadeAmount;

public:	
	// Sets default values for this actor's properties
	AKFadeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void TimelineProgress(float Value);

	void InitFadeTimeline();

public:
	void StartFade();
	void EndFade();
};
