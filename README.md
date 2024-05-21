# SimpleDayNightPlugin
A plugin for Unreal to implement a simple day/night cycle, complete with seasonal changes in sun position based on latitude, as well as a full star sphere with seasonal procession of constellations.
(this isn't 100% mathematically correct, but it's more than good enough for video games).

## Requirements
- Unreal 4.27 or later.
- Recommended: Enable Virtual Textures

## Usage
Copy the Simple_DayNight folder into your project's Plugins folder. Re-generate project files for your project and build.

In-editor, simply drag and drop the DayNightController into your level (or create a blueprint subclass of the DayNightController and drag and drop that into your level).

### Settings

There are a handful of settings exposed in the Actor class, and a subset of those settings is also exposed in the Simple Day Night System Plugin Settings.

The settings exposed at the Plugin level are also linked to console variables, which can be changed at runtime.

Those settings are:
- __Use Smooth Sun Update__: (CVar: sdn.SmoothTime) This toggles whether to update the sun's position on tick, or use a stepped update which will update the Sun's position at a configurable interval. This is an optimization setting, as every time the sun's position updates it forces a full rebuild of all Virtual Shadow Maps (if Virtual Shadow Maps are enabled in your project), which can be expensive. Stepped update will usually give a higher average framerate, but can cause some frame pacing issues as the VSM rebuild occurs.
- __Time Step for Stepped Update__: (CVar: sdn.TimeStep) How many seconds between updates of the Sun's position when Smooth Update is disabled. Use your own judgement on this. Depending on how long you configure the day to be, a high value for this setting can cause some pretty jarring light shifts. The default value is one second.
- __Length of Day__: (CVar: sdn.DayLength) How many real-time minutes equal an in-game 24 hour period. Does what it says, really. Higher values equal longer days.
- __Length of Seasons__: (CVar: sdn.SeasonLength) How many in-game days a season lasts. Also pretty much does what it says. A full year is this setting times four.

Other settings:
- __Latitude__: The latitude of your game world in degrees. Positive values are for the northern hemisphere, negative values for the southern hemisphere. 90 is the North Pole, 0 is the equator. This is exposed to blueprint, so you could potentially do something like change the latitude as the player moves to better simulate a large open world.
- __Axial Tilt__: The axial tilt of the planet. The default value is Earth's 23.5 degrees. If you want to do another planet you can change this value. You can also change this to -23.5 to start in Summer instead of Winter (or Winter instead of Summer if in Southern latitudes).
- __Time Of Day__: The time of day. Time is in a 24 hour clock, but doesn't reset on each loop. With all other settings at default, when Time Of Day is 0, it is midnight before the Northern Winter Solstice. If you want to start on a specific day of the year you're going to have to do some math from there.

__Latitude__ and __Axial Tilt__ are both variables that can be changed in the DefaultSimpleDayNightConfig.ini, but are not linked to console variables, so cannot be changed at runtime without additional scripting (I don't recommend changing Axial Tilt very often at runtime, it'll mix up some of the math).

## Credits/Licensing
This plugin includes the public domain 4K Celestial Coordinate Deep Star Map 2020 from the NASA Scientific Visualization Studio (https://svs.gsfc.nasa.gov/4851/#media_group_319116). Higher resolutions are available from there. 

NASA/Goddard Space Flight Center Scientific Visualization Studio. Gaia DR2: ESA/Gaia/DPAC. Constellation figures based on those developed for the IAU by Alan MacRobert of Sky and Telescope magazine (Roger Sinnott and Rick Fienberg).

Otherwise, where applicable, everything else is copyright David Lejeune, made available under an MIT Open Source License (See License)

