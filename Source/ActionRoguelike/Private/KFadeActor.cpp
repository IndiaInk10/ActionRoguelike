// Fill out your copyright notice in the Description page of Project Settings.

#include "KFadeActor.h"

void AKFadeActor::InitFadeTimeline()
{
	FRichCurve* RichCurve = new FRichCurve();
	FKeyHandle Key = RichCurve->AddKey(0.f, 0.2f);
	RichCurve->AddKey(0.5f, 1.f);
	RichCurve->SetKeyInterpMode(Key, RCIM_Linear);
	UCurveFloat* FadeCurve = NewObject<UCurveFloat>();
	FadeCurve->FloatCurve = *RichCurve;

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
	FadeTimeline.AddInterpFloat(FadeCurve, TimelineProgress);
	FadeTimeline.SetLooping(false);
}
void AKFadeActor::TimelineProgress(float Value)
{
	float FadeAmount = 1 - EndingFadeAmount;
	FadeAmount *= Value;
	FadeAmount = 1 - FadeAmount;
	MeshComp->SetScalarParameterValueOnMaterials("Opacity", FadeAmount);
}
// Sets default values
AKFadeActor::AKFadeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	MeshComp->SetStaticMesh(Mesh);

	UMaterialInterface* FadeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/ActionRoguelike/MaterialLibrary/M_Fade_new.M_Fade_new'"));
	MeshComp->SetMaterial(0, FadeMaterial);

	MeshComp->SetCollisionProfileName("FadeActor");

	RootComponent = MeshComp;


	InitFadeTimeline();
	EndingFadeAmount = 0.01;
}

// Called when the game starts or when spawned
void AKFadeActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AKFadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FadeTimeline.TickTimeline(DeltaTime);
}

void AKFadeActor::StartFade()
{
	FadeTimeline.PlayFromStart();
}
void AKFadeActor::EndFade()
{
	FadeTimeline.Reverse();
}
