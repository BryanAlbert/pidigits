@echo off

rem Instead of launching Visual Studio Code file from a Visual Studio 2017 Command Prompt, we're launching 
rem a standard Command Prompt and running the contents of the VsDevCmd.bat batch to set up the envrionment 
rem of a Developer Command Prompt so that Visual Studio code can run in that environment (and access the 
rem C++ compiler and so on).

setlocal

echo.
echo Calling VsDevDmd.bat to configure Visual Studio Command Prompt
echo.

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

where /q cl >nul
if %errorlevel% neq 0 ( 
	echo fxc not found.
	goto WRONG_COMMAND_PROMPT
)

if "%WindowsSdkDir%" == "" ( 
	echo WindowsSdkDir enviroment variable not set.
	goto WRONG_COMMAND_PROMPT
)

set INCLUDEPATH="%WindowsSdkDir%\Include\%WindowsSDKVersion%\um"

if not exist %INCLUDEPATH%\d2d1effecthelpers.hlsli ( 
	echo d2d1effecthelpers.hlsli not found in include path %INCLUDEPATH%.
	goto WRONG_COMMAND_PROMPT
)

echo Calling code to launch Visual Studio Code...
code
