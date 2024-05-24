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

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SimpleDayNightSettings.generated.h"

/**
 * 
 */
UCLASS(Config = SimpleDayNight, DefaultConfig)
class SIMPLE_DAYNIGHT_API USimpleDayNightSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(Config, EditAnywhere, Category = "Optimization", meta = (ConsoleVariable = "sdn.SmoothTime", DisplayName = "Use Smooth Sun Update", ToolTip="Disable to use a stepped update for the sun position. Can increase framerate due to lower frequency of VSM rebuilds"))
	bool SmoothTime = true;

	UPROPERTY(Config, EditAnywhere, Category = "Optimization", meta= (ConsoleVariable = "sdn.TimeStep", DisplayName = "Time Step for Stepped Update",  ToolTip="Sets the frequency of sun position updates when using stepped update"))
	float SteppedTimeRate = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (ConsoleVariable="sdn.DayLength", DisplayName = "Length of Day", ToolTip="How many real-time minutes equal 24 in-game hours"))
	float LengthOfDay = 10.0f;

	UPROPERTY(Config, EditAnywhere, Category = "General", meta=(ConsoleVariable="sdn.SeasonLength", DisplayName = "Length of Seasons", ToolTip="How many in-game days a season lasts"))
	float SeasonLength = 5.0f;
	
};
