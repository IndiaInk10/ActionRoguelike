// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "KCharacter.h"

#include "KInteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KFadeActor.h"
#include "KGameplayInterface.h"

// Sets default values
AKCharacter::AKCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp"); // Pointer에 변수 할당
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent); // Character의 루트 component에 부착하기
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UKInteractionComponent>("InteractionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;

	// 메터리얼의 파라미터를 변경하기 위한 스켈레탈 메쉬에 대한 접근
	MyMesh = GetMesh();
}

// Called when the game starts or when spawned
void AKCharacter::BeginPlay()
{
	Super::BeginPlay();
}


// This is entirely optional, it draws two arrows to visualize rotations of the player
void AKCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckBlockingMesh();

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

void AKCharacter::CheckBlockingMesh()
{
	// FadeCharacter
	// 거리 계산 후 거리에 따른 처리
	// 
	//FVector MyLocation = GetActorLocation();
	//FVector CameraLocation = CameraComp->GetComponentLocation();
	//float distance = FVector::Distance(MyLocation, CameraLocation);
	//if (distance <= 100.f)
	//{
	//	distance /= 100.f;
	//	// 메터리얼의 파라미터값을 변경해주는 부분
	//	MyMesh->SetScalarParameterValueOnMaterials("FadeOut", distance);
	//}

	// FadeOtherActor(Mesh)
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FVector Start = GetActorLocation();
	FVector CameraLocation = CameraComp->GetComponentLocation(); 

	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;
	TArray<AActor*> HitActors;

	FCollisionShape Shape;
	float Radius = 30.0f;
	Shape.SetSphere(Radius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, Start, CameraLocation, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// 이전 Blocking Mesh 처리
	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		// 포인터에 ! 연산자 불가능
		if (HitActor)
		{
			HitActors.Add(HitActor);
			if (PreviousHitActors.Find(HitActor) == INDEX_NONE)
			{
				PreviousHitActors.Add(HitActor);
				AKFadeActor* FadeActor = Cast<AKFadeActor>(Hit.GetActor());
				if (FadeActor)  FadeActor->StartFade();
			}
			// DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 0.1f);
		}
	}
	for (AActor* Actor : PreviousHitActors)
	{
		if (HitActors.Find(Actor) == INDEX_NONE)
		{
			PreviousHitActors.Remove(Actor);
			AKFadeActor* FadeActor = Cast<AKFadeActor>(Actor);
			if (FadeActor)  FadeActor->EndFade();
		}
	}

	// DrawDebugLine(GetWorld(), Start, CameraLocation, FColor::Green, false, 2.0f, 0, 0.1f);
}


// Called to bind functionality to input
void AKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AKCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &AKCharacter::PrimaryInteract);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AKCharacter::Jump);
}

void AKCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), value);
}
void AKCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// Left Handed
	// X = Forward(Red)
	// Y = Right(Green)
	// Z = Up(Blue)
	
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, value);
}

void AKCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AKCharacter::PrimaryAttack_TimeElapsed, 0.2f);
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}
void AKCharacter::PrimaryAttack_TimeElapsed()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	// FTransform SpawnTM = FTransform(GetControlRotation(), GetActorLocation());
	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	GetWorld()->SpawnActor(ProjectileClass, &SpawnTM, SpawnParams);
}

void AKCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

