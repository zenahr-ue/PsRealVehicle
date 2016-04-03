// Copyright 2016 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PrvVehicleMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FSuspensionInfo
{
	GENERATED_USTRUCT_BODY()

	/** If yes, suspension location and rotation will be extracted from the bone transofrm */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bInheritWheelBoneTransform;

	/** Bone name to get the wheel transform */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Suspension, meta = (EditCondition = "bInheritWheelBoneTransform"))
	FName BoneName;

	/** Suspension location in Actor space */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bInheritWheelBoneTransform"))
	FVector Location;

	/** Suspension rotation in Actor space */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bInheritWheelBoneTransform"))
	FRotator Rotation;

	/** Is wheel belongs to the right track or the left one? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Suspension)
	bool bRightTrack;

	/** Maximum suspension length */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Suspension)
	float Length;

	/** Wheel [collision] radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Suspension)
	float CollisionRadius;

	/** How strong wheel reacts to compression */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Suspension)
	float Stiffness;

	/** How fast wheel becomes stable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Suspension)
	float Damping;

	/** Defaults */
	FSuspensionInfo()
	{
		bInheritWheelBoneTransform = true;

		Length = 25.f;
		CollisionRadius = 36.f;
		Stiffness = 4000000.f;
		Damping = 4000.f;
	}
};

USTRUCT(BlueprintType)
struct FSuspensionState
{
	GENERATED_USTRUCT_BODY()

	/** Wheel initial suspension config */
	UPROPERTY(BlueprintReadWrite)
	FSuspensionInfo SuspensionInfo;

	/** Suspension length on last tick */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PreviousLength;

	/** Force that was generated by suspension compression */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector SuspensionForce;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector WheelCollisionLocation;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector WheelCollisionNormal;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelLoad;

	/** Is wheel engaged into physics simulation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool WheelTouchedGround;

	/**   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	/** Defaults */
	FSuspensionState()
	{
		WheelTouchedGround = false;
	}
};

USTRUCT(BlueprintType)
struct FGearInfo
{
	GENERATED_USTRUCT_BODY()

	/** Determines the amount of torque multiplication */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Ratio;

	/** Value of engineRevs/maxEngineRevs that is low enough to gear down */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float DownRatio;

	/** Value of engineRevs/maxEngineRevs that is high enough to gear up */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float UpRatio;

	/** Defaults */
	FGearInfo()
	{
		DownRatio = 0.15f;
		UpRatio = 0.9f;
	}
};

USTRUCT(BlueprintType)
struct FTrackInfo
{
	GENERATED_USTRUCT_BODY()

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Input;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TorqueTransfer;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float LinearVelocity;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AngularVelocity;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DriveTorque;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FrictionTorque;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RollingFrictionTorque;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BrakeRatio;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector DriveForce;
};


/**
 * Component that uses Torque and Force to move tracked vehicles
 */
UCLASS()
class PSREALVEHICLEPLUGIN_API UPrvVehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_UCLASS_BODY()


	//////////////////////////////////////////////////////////////////////////
	// Initialization
	
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


	//////////////////////////////////////////////////////////////////////////
	// Physics initialization

	void CalculateMOI();
	void InitSuspension();
	void InitGears();


	//////////////////////////////////////////////////////////////////////////
	// Physics simulation

	void UpdateThrottle(float DeltaTime);
	void UpdateGearBox();
	void UpdateBrake();

	void UpdateTracksVelocity(float DeltaTime);
	void UpdateHullVelocity(float DeltaTime);
	void UpdateEngine();
	void UpdateDriveForce();

	void UpdateSuspension(float DeltaTime);
	void UpdateFriction(float DeltaTime);

	float ApplyBrake(float DeltaTime, float AngularVelocity, float BrakeRatio);
	float CalculateFrictionCoefficient(FVector DirectionVelocity, FVector ForwardVector, FVector2D FrictionEllipse);

	/** Shift gear up or down 
	 * Attn.! It doesn't think about why it happend, so it should be done externally!) */
	void ShiftGear(bool bShiftUp);


	/////////////////////////////////////////////////////////////////////////
	// Vehicle setup

	/** Kg */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	float SprocketMass;

	/** Cm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	float SprocketRadius;

	/** Kg */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	float TrackMass;


	/////////////////////////////////////////////////////////////////////////
	// Suspension setup

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Suspension)
	TArray<FSuspensionInfo> SuspensionSetup;

	/** Global factor that applied to all wheels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Suspension)
	float DampingFactor;

	/** Global factor that applied to all wheels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Suspension)
	float StiffnessFactor;


	/////////////////////////////////////////////////////////////////////////
	// Gear Box setup

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GearBox)
	TArray<FGearInfo> GearSetup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GearBox)
	bool bAutoGear;

	/** Minimum time it takes the automatic transmission to initiate a gear change (seconds)*/
	UPROPERTY(EditAnywhere, Category = GearBox, meta = (editcondition = "bAutoGear", ClampMin = "0.0", UIMin = "0.0"))
	float GearAutoBoxLatency;


	/////////////////////////////////////////////////////////////////////////
	// Brake system

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem)
	bool bAutoBrake;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem)
	float BrakeForce;

	/** @TODO Should be done with curve depending on vehicle speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem)
	float SteeringBrakeFactor;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem)
	float SteeringBrakeTransfer;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem)
	bool bSteeringStabilizer;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BrakeSystem, meta = (EditCondition = "bSteeringStabilizer"))
	float AutoBrakeStableTransfer;


	/////////////////////////////////////////////////////////////////////////
	// Engine setup

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	float ThrottleUpRatio;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	float ThrottleDownRatio;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	float DifferentialRatio;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	float TransmissionEfficiency;

	/** Torque (Nm) at a given RPM */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	FRuntimeFloatCurve EngineTorqueCurve;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EngineSetup)
	float EngineExtraPowerRatio;


	/////////////////////////////////////////////////////////////////////////
	// Friction

	/** Ellipse to calculate static friction coefficient
	 * http://hyperphysics.phy-astr.gsu.edu/hbase/frict2.html */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsSimulation)
	FVector2D StaticFrictionCoefficientEllipse;

	/** Ellipse to calculate kinetic friction coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsSimulation)
	FVector2D KineticFrictionCoefficientEllipse;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsSimulation)
	float FrictionTorqueCoefficient;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsSimulation)
	float RollingFrictionCoefficient;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsSimulation)
	float RollingVelocityCoefficient;



	/////////////////////////////////////////////////////////////////////////
	// Movement cache

	float FinalMOI;

	TArray<FSuspensionState> SuspensionData;

	int32 NeutralGear;
	int32 CurrentGear;
	bool bReverseGear;

	float LastAutoGearShiftTime;
	float LastAutoGearHullVelocity;

	FTrackInfo LeftTrack;
	FTrackInfo RightTrack;
	
	float RightTrackTorque;
	float LeftTrackTorque;

	float HullAngularVelocity;

	// Cached RPM limits
	float MinEngineRPM;
	float MaxEngineRPM;

	float EngineRPM;
	float EngineTorque;
	float DriveTorque;

	/** How many wheels are touched the ground */
	int32 ActiveFrictionPoints;


	//////////////////////////////////////////////////////////////////////////
	// Vehicle control

public:
	/** Set the user input for the vehicle throttle */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	void SetThrottleInput(float Throttle);
	
	/** Set the user input for the vehicle steering */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	void SetSteeringInput(float Steering);

	/** Set the user input for handbrake */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	void SetHandbrakeInput(bool bNewHandbrake);


	//////////////////////////////////////////////////////////////////////////
	// Vehicle stats

public:
	/** How fast the vehicle is moving forward */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetForwardSpeed() const;

	/** Get current real throttle value */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetThrottle() const;

	/** Get current engine's rotation speed */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetEngineRotationSpeed() const;

	/** Get current engine's max rotation speed */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetEngineMaxRotationSpeed() const;

	/** Get current engine torque */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetEngineTorque() const;

	/** Get left track drive torque */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetDriveTorqueLeft() const;

	/** Get right track drive torque */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetDriveTorqueRight() const;

	/** Get left track angular velocity */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetAngularVelocityLeft() const;

	/** Get right track angular velocity */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetAngularVelocityRight() const;

	/** Get left track brake ratio */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetBrakeRatioLeft() const;

	/** Get left track brake ratio */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	float GetBrakeRatioRight() const;
	

	//////////////////////////////////////////////////////////////////////////
	// Tracks info

public:
	/** Get left track internal data */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	void GetTrackInfoLeft(FTrackInfo& OutTrack) const;

	/** Get right track internal data */
	UFUNCTION(BlueprintCallable, Category="PsRealVehicle|Components|VehicleMovement")
	void GetTrackInfoRight(FTrackInfo& OutTrack) const;


	//////////////////////////////////////////////////////////////////////////
	// Data access

public:
	/** Get current gear */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	int32 GetCurrentGear() const;

	/** Get neutral gear */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	int32 GetNeutralGear() const;

	/** Get gear reverse state */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	bool IsCurrentGearReverse() const;

	/** Get gearbox config */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	FGearInfo GetGearInfo(int32 GearNum) const;

	/** Get current gearbox config */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	FGearInfo GetCurrentGearInfo() const;

protected:
	/** Get the mesh this vehicle is tied to */
	class USkinnedMeshComponent* GetMesh();


	//////////////////////////////////////////////////////////////////////////
	// Debug

public:
	/** Draw debug text for the wheels and suspension */
	virtual void DrawDebug(UCanvas* Canvas, float& YL, float& YPos);

	/** Draw debug primitives for the wheels and suspension */
	virtual void DrawDebugLines();

	/** */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	void ShowDebug(bool bEnableDebug) { bShowDebug = bEnableDebug; }

	/** */
	UFUNCTION(BlueprintCallable, Category = "PsRealVehicle|Components|VehicleMovement")
	bool IsDebug() { return bShowDebug; }

protected:
	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	bool bShowDebug;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	bool bDebugAutoGearBox;


	//////////////////////////////////////////////////////////////////////////
	// Internal data

protected:
	// What the player has the steering set to. Range -1...1
	UPROPERTY(Transient)
	float RawSteeringInput;

	// What the player has the accelerator set to. Range -1...1
	UPROPERTY(Transient)
	float RawThrottleInput;

	// True if the player is holding the handbrake
	UPROPERTY(Transient)
	uint32 bRawHandbrakeInput : 1;

	UPROPERTY(Transient)
	float ThrottleInput;

	UPROPERTY(Transient)
	float SteeringInput;

	UPROPERTY(Transient)
	float BrakeInput;

};
