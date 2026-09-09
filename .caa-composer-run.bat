@echo off
cd /d "%~dp0"
cls
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\tck_init.bat"
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\TCK\command\tck_profile.bat" V5R21_B21
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\mkGetPreq.bat" -p "C:\Program Files\Dassault Systemes\B21"
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\mkmk.bat" -au
call "C:\Program Files\Dassault Systemes\B21_RADE\intel_a\code\command\mkrtv.bat"
