@echo on
echo compiling editor.cpp

taskkill /f /im editor.exe
C:/msys64/mingw64/bin/windres.exe resources.rc resources.o
C:/msys64/mingw64/bin/c++.exe editor.cpp embedded_terminal.cpp resources.o -o editor.exe -mwindows
editor.exe

echo running editor.exe