@echo off
echo --------- Convert Space Invaders ---------
echo Space Invaders Unpack roms
python ./unpack.py invaders.zip
if errorlevel 1 goto :error

echo Converting Space Invaders
cd invaders
python invaders_rom_convert.py
cd ..
if errorlevel 1 goto :error

echo --- Success ---
goto end

:error
echo --- Error #%errorlevel%.
pause

:end