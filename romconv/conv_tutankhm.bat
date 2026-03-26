@echo off
echo --------- Convert Tutancham ---------
echo Tutancham Unpack roms
python ./unpack.py tutankhm.zip
if errorlevel 1 goto :error

echo Converting Tutancham
cd tutankhm
python tutankhm_rom_convert.py
cd ..
if errorlevel 1 goto :error

echo --- Success ---
goto end

:error
echo --- Error #%errorlevel%.
pause

:end