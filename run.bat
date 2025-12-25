@echo on
echo compiling environment.cpp

C:/msys64/mingw64/bin/windres.exe resources.rc resources.o
@REM C:/msys64/mingw64/bin/c++.exe environment.cpp embedded_terminal.cpp resources.o -o environment.exe -mwindows
C:/msys64/mingw64/bin/c++.exe environment.cpp embedded_terminal.cpp -o environment.exe -mwindows
environment.exe

echo running environment.exe