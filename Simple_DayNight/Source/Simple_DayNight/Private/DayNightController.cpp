//MIT License
//
//Copyright(c) 2024 David Lejeune
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "DayNightController.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Misc/ConfigCacheIni.h"
#include "Misc/ConfigUtilities.h"


static TAutoConsoleVariable<int32> CVarSmoothTime(
	TEXT("sdn.SmoothTime"),
	1,
	TEXT("Sets whether to use a smooth or stepped sun update")
);

static TAutoConsoleVariable<float> CVarDayLength(
	TEXT("sdn.DayLength"),
	10.0f,
	TEXT("Sets how many real-time minutes equal 24 in-game hours")
);

static TAutoConsoleVariable<float> CVarSeasonLength(
	TEXT("sdn.SeasonLength"),
	4.0f,
	TEXT("Number of days for each season")
);

static TAutoConsoleVariable<float> CVarStepRate(
	TEXT("sdn.TimeStep"),
	1.0f,
	TEXT("Number of seconds between updates when not using Smoothed sun update")
);


// Sets default values
ADayNightController::ADayNightController(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject< USceneComponent > (TEXT("Root"));
	RootComponent = Root;

	Axis = CreateDefaultSubobject<USceneComponent>(TEXT("Axis"));
	Axis->SetupAttachment(Root);

	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(Axis);

	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	SkySphere->SetupAttachment(Root);
	UStaticMesh* Sky = LoadObject<UStaticMesh>(nullptr, TEXT("/Simple_DayNight/Skysphere_Full.SkySphere_Full"));
	
	SkySphere->SetStaticMesh(Sky);
	SkySphere->SetWorldScale3D(FVector(100000, 100000, 100000));

	Atmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("Atmosphere"));
	Atmosphere->SetupAttachment(Root);

	//Set the properties that are linked to CVars to the CVar values (aka what's in DefaultSimpleDayNight.ini)
	SmoothTime = (bool)CVarSmoothTime.GetValueOnAnyThread();
	SeasonLength = CVarSeasonLength.GetValueOnAnyThread();
	LengthOfDay = CVarDayLength.GetValueOnAnyThread();
	SteppedTimeRate = CVarStepRate.GetValueOnAnyThread();

	//Load the config to overwrite any properties that aren't defaults
	LoadConfig();

	//Update Sun and Star positions for the loaded properties.
	UpdateSunPosition();
	UpdateStarPosition();
}

//If we're running in editor we want to save any changes to the properties to the DefaultSimpleDayNight.ini
#if WITH_EDITOR
void ADayNightController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.GetPropertyName());

	UE_LOG(LogTemp, Log, TEXT("%s changed"), *PropertyName.ToString());

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ADayNightController, Latitude) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayNightController, TimeOfDay) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayNightController, AxialTilt))
	{
		UpdateSunPosition();
		UpdateStarPosition();
	}
	
	if (PropertyChangedEvent.Property)
	{
		SaveConfig(CPF_Config, *GetDefaultConfigFilename());
	}
}
#endif

// Called when the game starts or when spawned
void ADayNightController::BeginPlay()
{
	Super::BeginPlay();

	//Update CVars to match locally overridden values.
	//For some reason in a build this will throw an exception and crash if done in PostInitProperties()
	CVarSmoothTime.AsVariable()->Set(SmoothTime, ECVF_SetByProjectSetting);
	CVarSeasonLength.AsVariable()->Set(SeasonLength, ECVF_SetByProjectSetting);
	CVarDayLength.AsVariable()->Set(LengthOfDay, ECVF_SetByProjectSetting);
	CVarStepRate.AsVariable()->Set(SteppedTimeRate, ECVF_SetByProjectSetting);

	FAutoConsoleVariableSink TimeSink(FConsoleCommandDelegate::CreateUObject(this, &ADayNightController::SmoothSinkFunction));

	if (!SmoothTime)
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ADayNightController::UpdateSunPositionTimer);
		GetWorldTimerManager().SetTimer(SteppedTimerHandle, TimerDelegate, SteppedTimeRate, true);
	}
	
}

// Called every frame
void ADayNightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	TimeOfDay += DeltaTime * (24 / LengthOfDay / 60);

	//Always update star position because it has no effect on performance
	UpdateStarPosition();

	//Only update Sun position on tick if SmoothTime is enabled.
	if (SmoothTime)
	{
		UpdateSunPosition();
	}
}

void ADayNightController::PostInitProperties()
{
	Super::PostInitProperties();

	

}

void ADayNightController::UpdateSunPosition()
{
	//For some reason I just couldn't get rotating around the component's up vector to work. This math does work.
	FVector North = FVector(0, -1, 0).RotateAngleAxis(-Latitude, FVector(1, 0, 0));

	FVector East = FVector(1, 0, 0).RotateAngleAxis(TimeOfDay * 15 + 180, North);

	Axis->SetWorldRotation(FRotationMatrix::MakeFromZY(North, East).ToQuat());

	//Wobble the Sun to fake seasons. Multiply by Pi for more predictable result. Multiply SeasonLength by 2 so SeasonLength=number of days between Solstices and Equinoxes
	//Clock starts at Northern Winter Solstice.
	//(I'm treating the orbit as a perfect circle because that's good enough for video games.)
	Sun->SetRelativeRotation(FRotator(AxialTilt * FMath::Cos(((TimeOfDay-12)/(24*(SeasonLength*2)))*PI), 0, 0));

}

void ADayNightController::UpdateStarPosition()
{
	FVector North = FVector(0, -1, 0).RotateAngleAxis(-Latitude, FVector(1, 0, 0));
	
	//Fake seasonal procession of the stars by doing some math with the calculated seasonal offset.
	FVector East = FVector(1, 0, 0).RotateAngleAxis((TimeOfDay * 15) +(fmod((TimeOfDay - 12) / (24 * (SeasonLength) * 2), 2)*180) - 90, North); 
	
	SkySphere->SetWorldRotation(FRotationMatrix::MakeFromZY(North, East).ToQuat());
}

void ADayNightController::UpdateSunPositionTimer()
{
	UpdateSunPosition();
}

void ADayNightController::SmoothSinkFunction()
{		
	bool useSmooth;

	//Only update the class variables if the CVars were last changed by something other than the Project Settings.
	if ((EConsoleVariableFlags)((uint32)CVarSmoothTime.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting)
	{		
		useSmooth = (bool)CVarSmoothTime.GetValueOnAnyThread();

		if (useSmooth != SmoothTime)
		{
			//If SmoothTime CVar has been changed to True, stop the stepped update timer and invalidate the timer handle.
			if (useSmooth)
			{
				SmoothTime = true;
				if (SteppedTimerHandle.IsValid())
				{
					GetWorldTimerManager().ClearTimer(SteppedTimerHandle);
					SteppedTimerHandle.Invalidate();
				}
			}
			//If SmoothTime CVar has been changed to False, start the timer for stepped update of the sun position.
			if (!useSmooth)
			{
				SmoothTime = false;
				FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ADayNightController::UpdateSunPositionTimer);
				GetWorldTimerManager().SetTimer(SteppedTimerHandle, TimerDelegate, SteppedTimeRate, true);
			}
		}
	}	
	
	if ((EConsoleVariableFlags)((uint32)CVarStepRate.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting)
	{
		SteppedTimeRate = CVarStepRate.GetValueOnAnyThread();
	}	

	if ((EConsoleVariableFlags)((uint32)CVarDayLength.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting)
	{
		LengthOfDay = CVarDayLength.GetValueOnAnyThread();
	}

	if ((EConsoleVariableFlags)((uint32)CVarSeasonLength.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting)
	{
		SeasonLength = CVarSeasonLength.GetValueOnAnyThread();
	}

	//If we're running editor save changes to DefaultSimpleDayNight.ini
#if WITH_EDITOR
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
#else
	//If we're running a build, save any changes that aren't from ProjectSetting to a local SimpleDayNight.ini
	if ((EConsoleVariableFlags)((uint32)CVarSeasonLength.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting ||
		(EConsoleVariableFlags)((uint32)CVarDayLength.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting ||
		(EConsoleVariableFlags)((uint32)CVarStepRate.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting ||
		(EConsoleVariableFlags)((uint32)CVarSmoothTime.AsVariable()->GetFlags() & ECVF_SetByMask) != ECVF_SetByProjectSetting)
	{
		SaveConfig();
	}
#endif
}


