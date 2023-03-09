// Fill out your copyright notice in the Description page of Project Settings.


#include "KExplosiveBarrel.h"

#include "KMagicProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AKExplosiveBarrel::AKExplosiveBarrel()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	
	// 1. 경로를 통해서 StaticMesh를 읽어오고
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/ExampleContent/Meshes/SM_RedBarrel.SM_RedBarrel"));
	// // 읽어오기를 성공했다고 가정했을때 타입캐스팅(?)을 하여 사용
	// UStaticMesh* BarrelAsset = MeshAsset.Object;
	// 2. 또 다른 방법으로 LoadObject을 통해 경로에서 불러올 수 있다
	UStaticMesh* BarrelAsset = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/ExampleContent/Meshes/SM_RedBarrel.SM_RedBarrel"));
	MeshComp->SetStaticMesh(BarrelAsset);

	UMaterialInterface* BarrelMaterialAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));
	MeshComp->SetMaterial(0, BarrelMaterialAsset);

	// Editor와 다르게 SimulatePhysics를 해도 자동으로 CollisionProfile이 PhysicsActor로 변하지 않음
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName("PhysicsActor");
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->Radius = 700.0f;
	ForceComp->ImpulseStrength = 2000.0f;
	ForceComp->bImpulseVelChange = true;
	ForceComp->SetupAttachment(MeshComp);
}

void AKExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MeshComp->OnComponentHit.AddDynamic(this, &AKExplosiveBarrel::OnHited);
}


// Called when the game starts or when spawned
void AKExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AKExplosiveBarrel::OnHited(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// UClass* HitActor = OtherActor->GetClass();
	// if(HitActor != AKMagicProjectile::StaticClass())  return;

	// OtherActor->Destroy();
	Cast<URadialForceComponent>(HitComponent->GetChildComponent(0))->FireImpulse();
}