# Nightmare Night
SKSE plugin for UI related logic of [Nightmare Night](https://github.com/Scrabx3/Nightmare-Night)

## Requirements
* [xmake](https://xmake.io/#/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/alandtse/CommonLibVR/tree/ng)
	* You need to build from the alandtse/ng branch
* Create Environment Variables:
  * `XSE_TES5_MODS_PATH`: Path to your MO2/Vortex `mods` folder

## Building
```
git clone https://github.com/Scrabx3/NightmareNight-SKSE.git
cd NightmareNight-SKSE
git submodule update --init --recursive
xmake f -m release [
	--copy_to_papyrus=(y/n)		# copy build to the "Nightmare Night" papyrus mod instance
]
xmake
```
