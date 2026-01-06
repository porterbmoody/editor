@echo on
echo compiling editor.cpp

@REM taskkill /f /im editor.exe
@REM C:/msys64/mingw64/bin/windres.exe resources.rc resources.o
C:/msys64/mingw64/bin/c++.exe quantum_os.cpp -o quantum_os.exe
quantum_os.exe

echo running quantum_os.exe