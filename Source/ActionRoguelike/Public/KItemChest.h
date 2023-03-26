// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "KItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AKItemChest : public AActor, public IKGameplayInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float TargetPitch;
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	AKItemChest();
};
