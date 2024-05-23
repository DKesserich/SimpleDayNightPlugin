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

#include "Simple_DayNight.h"

#include "Misc/ConfigCacheIni.h"
//#include "Misc/ConfigUtilities.h"
#include "HAL/IConsoleManager.h"

#define LOCTEXT_NAMESPACE "FSimple_DayNightModule"

void FSimple_DayNightModule::StartupModule()
{
	// Load the CVar values from DefaultSimpleDayNight.ini
	UE_LOG(LogTemp, Log, TEXT("Default Engine Ini Name: %s"), *GEngineIni);
	FString ConfigPath = FPaths::ProjectConfigDir() + "/DefaultSimpleDayNight.ini";
	ApplyCVarSettingsFromIni(TEXT("/Script/Simple_DayNight.SimpleDayNightSettings"), *ConfigPath, ECVF_SetByProjectSetting);
}

void FSimple_DayNightModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimple_DayNightModule, Simple_DayNight)