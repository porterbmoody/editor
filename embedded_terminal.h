#pragma once
#include <windows.h>
#include <string>
#include <vector>

class embedded_terminal
{
public:
    embedded_terminal(HWND parent, int x, int y, int width, int height);
    ~embedded_terminal();

    void execute_command(const std::string& exePath);
    void append_output(const std::string& text);
    void run_log(int depth);
    std::vector<HKEY> get_hkeys();
    std::string hkey_to_string(HKEY hkey);
    void log_subkeys_recursive(FILE* log_file, HKEY hkey, std::string path, int depth, int max_depth);

private:
    HWND hTerminal;
    HANDLE hReadPipe;
    HANDLE hWritePipe;
    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0,0,0));

    void createPipes();
    void read_output(HANDLE hProcess);
};