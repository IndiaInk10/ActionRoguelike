// Fill out your copyright notice in the Description page of Project Settings.


#include "KCharacter.h"

#include "KInteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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


// This include is added at the top of your KCharacter.cpp file
#include "DrawDebugHelpers.h"
// This is entirely optional, it draws two arrows to visualize rotations of the player
void AKCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// 거리 계산 후 거리에 따른 처리
	MyLocation = GetActorLocation();
	CameraLocation = CameraComp->GetComponentLocation();
	float distance = FVector::Distance(MyLocation, CameraLocation);
	if(distance <= 100.f)
	{
		distance /= 100.f;
		// 메터리얼의 파라미터값을 변경해주는 부분
		MyMesh->SetScalarParameterValueOnMaterials("FadeOut", distance);
	}

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

