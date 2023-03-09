// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;


UCLASS()
class ACTIONROGUELIKE_API AKExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKExplosiveBarrel();
	
protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* ForceComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHited(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
