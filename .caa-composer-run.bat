@echo off
cd /d "%~dp0"
if not exist "C:\temp" mkdir "C:\temp"
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\tck_init.bat"
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\TCK\command\tck_profile.bat" V5R21_B21
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\mkCreateRuntimeView.bat"
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\mkrun.bat" -c "cnext"
