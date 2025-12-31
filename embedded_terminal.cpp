#include "embedded_terminal.h"
#include <iostream>
#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <chrono>
#include <regex>
#include <string>
#include <sstream>
#include <string>
#include <unordered_map>

embedded_terminal::embedded_terminal(HWND parent, int x, int y, int width, int height)
{
    hTerminal = CreateWindowExA(
        WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        x, y, width, height, parent, (HMENU)200, GetModuleHandle(NULL), NULL
    );
}

void embedded_terminal::append_output(const std::string& text)
{
    int len = GetWindowTextLengthA(hTerminal);
    SendMessageA(hTerminal, EM_SETSEL, len, len);
    SendMessageA(hTerminal, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
    SendMessageA(hTerminal, EM_SCROLLCARET, 0, 0);
}

void embedded_terminal::createPipes()
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
        MessageBoxA(NULL, "Failed to create pipe", "Error", MB_OK);

    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
}

void embedded_terminal::read_output(HANDLE hProcess)
{
    char buffer[4096];
    DWORD read;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
    {
        buffer[read] = 0;
        append_output(buffer);
    }

    CloseHandle(hReadPipe);
    WaitForSingleObject(hProcess, INFINITE);
    CloseHandle(hProcess);
}

void embedded_terminal::execute_command(const std::string& exePath)
{
    createPipes();

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};
    if (!CreateProcessA(NULL, (LPSTR)exePath.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW,
                        NULL, NULL, &si, &pi))
    {
        MessageBoxA(NULL, "Failed to start process", "Error", MB_OK);
        return;
    }

    CloseHandle(hWritePipe);
    read_output(pi.hProcess);
}

std::vector<HKEY> embedded_terminal::get_hkeys() {
    // std::cout << "C:/msys64/mingw64/include/winreg.h" << std::endl;
    std::ifstream file("C:/msys64/mingw64/include/winreg.h");
    if (!file) {
        std::cerr << "Failed to open winreg.h\n";
        return {};
    }
    std::regex re(R"(#define\s+(HKEY_[A-Z_]+)\s+\(\(HKEY\)\s+\(ULONG_PTR\)\(\(LONG\)(0x[0-9A-Fa-f]+)\)\))");
    std::string line;
    std::vector<HKEY> hkeys = {};
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, re)) {
            std::string hkey_name = match[1].str();
            std::string hex_key = match[2].str();
            embedded_terminal::append_output(hkey_name + "\n");
            embedded_terminal::append_output(hex_key + "\n");
            ULONG_PTR key_num;
            std::stringstream ss;
            key_num = std::stoul(hex_key, nullptr, 16);
            HKEY hkey = (HKEY)key_num;
            hkeys.push_back(hkey);
        }
    }
    return hkeys;
}

template<typename T>
void print(const T& value) {
    std::cout << value << std::endl;
}

void embedded_terminal::log_subkeys_recursive(FILE* log_file, HKEY hkey, std::string path, int depth, int max_depth) {
    if (depth > max_depth) return;
    char name[256];
    bool has_subkeys = false;
    for (DWORD index = 0; RegEnumKey(hkey, index, name, sizeof(name)) == ERROR_SUCCESS; index++) {
        has_subkeys = true;
        HKEY subkey;
        std::string full_path = path + "," + name;

        if (RegOpenKeyEx(hkey, name, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
            embedded_terminal::log_subkeys_recursive(log_file, subkey, full_path, depth + 1, max_depth);
            RegCloseKey(subkey);
        }
    }
    if (!has_subkeys || depth == max_depth) {
        fprintf(log_file, "%s,%p\n", path.c_str(), (void*)hkey);
    }
}
std::string embedded_terminal::hkey_to_string(HKEY hkey) {
    std::ostringstream oss;
    oss << "0x" << std::hex << reinterpret_cast<ULONG_PTR>(hkey);
    return oss.str();
}
void embedded_terminal::run_log(int depth)
{
    const char* headers = "root,subkey,subsubkey,subsubsubkey\n";
    const char* log_path = "logs/log2.txt";
    embedded_terminal::append_output("log_path" + std::string(log_path) + "\n");
    // std::cout << log_path << std::endl;
    FILE* log_file = fopen(log_path, "w");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
    }
    fprintf(log_file, headers);
    auto start = std::chrono::high_resolution_clock::now();
    auto hkeys = embedded_terminal::get_hkeys();
    for (const auto& hkey : hkeys) {
        std::string path = hkey_to_string(hkey);
        embedded_terminal::append_output(path + "\n");
        // std::cout << path << std::endl;
        log_subkeys_recursive(log_file, hkey, path, 0, depth);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    embedded_terminal::append_output("Runtime: " + std::to_string(duration.count()) + " seconds\n");
    fclose(log_file);
}

// std::unordered_map<std::string, std::string> embedded_terminal::read_config(const std::string& filename)
// {
// 	std::unordered_map<std::string, std::string> config;
// 	std::ifstream file(filename);
// 	if (!file.is_open())
// 		return config;
// 	std::string line;
// 	while (std::getline(file, line)) {
// 		if (line.empty() || line[0] == '#')
// 			continue;
// 		size_t pos = line.find('=');
// 		if (pos == std::string::npos)
// 			continue;
// 		config[line.substr(0, pos)] = line.substr(pos + 1);
// 	}
// 	return config;
// }

