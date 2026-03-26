@echo off
echo --------- Convert Gyruss ---------
echo Gyruss Unpack roms
python ./unpack.py gyruss.zip
if errorlevel 1 goto :error

echo Converting Gyruss
cd gyruss
python gyruss_rom_convert.py
cd ..
if errorlevel 1 goto :error

echo --- Success ---
goto end

:error
echo --- Error #%errorlevel%.
pause

:end