#define idm_exit 100
#define idm_save 101
#define idm_theme 102
#define idm_edit 103
#define idm_file 104
#define idm_run 105
#define idm_directory 106
#define idm_terminal 107
#define IDI_APP_ICON 108
#define idm_git_push 109
#define IDC_MY_EDIT 110
#define idm_open_file 111
#define idm_run_log 112

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <thread>
#define UNICODE
#define _UNICODE
#include <windows.h>
#include "resources.h"
#include "embedded_terminal.h"
#include <unordered_map>

HWND hEdit;
HWND hButton;
HBRUSH hBackgroundBrush;
HBRUSH hEditBrush;
HWND hConsoleChild;
HWND hTerminal;
HBRUSH hButtonBrush;

bool terminal_visible = true;
embedded_terminal* terminal = nullptr;
std::unordered_map<std::string, std::string> configurations = {
    {"toolbar_width",  "70"},
    {"button_height",  "40"},
    {"registry_depth", "3"},
    {"window_width",   "800"},
    {"window_height",  "600"},
    {"toolbar_width",  "600"},
    {"editor_title",  "C++ IDE"},
    {"cpp_executable_path", "C:/msys64/mingw64/bin/c++.exe"},
    {"output_exe", "new_file.exe"},
    {"output_source_file", "new_file.cpp"},
};
int toolbar_width = std::stoi(configurations.find("toolbar_width")->second);
int button_height = std::stoi(configurations.find("button_height")->second);
int registry_depth = std::stoi(configurations.find("registry_depth")->second);
int window_width = std::stoi(configurations.find("window_width")->second);
int window_height = std::stoi(configurations.find("window_height")->second);
int terminal_height = window_height / 2;
int editor_width = window_width - toolbar_width;
int editor_height = window_height - terminal_height;
std::string editor_title = configurations.find("editor_title")->second;
std::string cpp_executable_path = configurations.find("cpp_executable_path")->second;
std::string output_exe = configurations.find("output_exe")->second;
std::string output_source_file = configurations.find("output_source_file")->second;

struct ButtonInfo {
    const char* text;
    int id;
};
void update_terminal_position(HWND parent)
{
    if (!hConsoleChild || !terminal_visible) return;
    RECT rcClient;
    GetClientRect(parent, &rcClient);
    int terminal_height = rcClient.bottom / 2;
    int editorWidth = rcClient.right - toolbar_width;
    int editor_height = rcClient.bottom - terminal_height;
    MoveWindow(hEdit, toolbar_width, terminal_height, editorWidth, editor_height, TRUE);
    // SetWindowPos(hConsoleChild, NULL,toolbar_width, editor_height,editorWidth, terminal_height,SWP_NOZORDER);
}

bool write_text_file(const std::string path, const std::string& text)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;
    file.write(text.data(), text.size());
    return true;
}
void on_file(HWND)  { terminal->append_output("file\n"); }
void on_edit(HWND)  { terminal->append_output("edit\n");}
void on_directory(HWND)  { terminal->append_output("directory\n"); }
void on_save(HWND)
{
    int length = GetWindowTextLengthA(hEdit);
    std::string code(length, '\0');
    if (terminal) {
        terminal->append_output("length: " + std::to_string(length) + "\n");
    }
    GetWindowTextA(hEdit, code.data(), length + 1);
    std::string normalized;
    normalized.reserve(code.size());
    // terminal->append_output("saving: " + code + "\n");
    for (size_t i = 0; i < code.size(); ++i)
    {
        if (code[i] == '\r' && i + 1 < code.size() && code[i + 1] == '\n')
        {
            normalized.push_back('\n');
            ++i;
        }
        else
        {
            normalized.push_back(code[i]);
        }
    }
    write_text_file(output_source_file, normalized);
}
void on_theme(HWND) { terminal->append_output("theme"); }
void on_open_file(HWND)
{
    std::ifstream file(output_source_file);
    if (!file) {
        terminal->append_output("File not found: " + output_source_file + "\n");
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    SetWindowTextA(hEdit, content.c_str());
    terminal->append_output("Loaded file: " + output_source_file + "\n");
}
void on_run_log(HWND)
{
    // std::cout << "running log: " << output_source_file << std::endl;
    terminal->run_log(registry_depth);
}
    // std::string content((std::istreambuf_iterator<char>(file)),
    //                     std::istreambuf_iterator<char>());
    // SetWindowTextA(hEdit, content.c_str());
    // terminal->append_output("Loaded file: " + file_path + "\n");
// void on_open_file(HWND hWnd) {
//     OPENFILENAME ofn;
//     char szFile[MAX_PATH] = { 0 };x
//     ZeroMemory(&ofn, sizeof(ofn));
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hwndOwner = hWnd;
//     ofn.lpstrFilter = "C++ Files\0*.cpp\0All Files\0*.*\0";
//     ofn.lpstrFile = szFile;
//     ofn.nMaxFile = sizeof(szFile);
//     ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
//     ofn.lpstrDefExt = "cpp";
//     if (GetOpenFileName(&ofn)) {
//         current_source_file = szFile;
//         std::ifstream file(current_source_file);
//         if (file) {
//             std::string content((std::istreambuf_iterator<char>(file)),
//                                  std::istreambuf_iterator<char>());
//             SetWindowTextA(hEdit, content.c_str());
//         }
//     }
// }
void on_terminal(HWND hWnd) {
    terminal->append_output("opening terminal...\n");
}
void on_run(HWND hWnd)
{
    std::thread([hWnd]() {
        on_save(hWnd);
        std::string compile_cmd = cpp_executable_path + " " + output_source_file + " -o " + output_exe + " -std=c++17 -Wall";
        std::string cmd_kill_process = "cmd /c \"tasklist | findstr /I \\\"" + output_exe + "\\\" >nul && taskkill /IM \\\"" + output_exe + "\\\" /F\"";

        if (!output_exe.empty()) {
            terminal->execute_command(cmd_kill_process);
        }
        terminal->execute_command(compile_cmd);
        terminal->execute_command(output_exe);
    }).detach();
}
void on_git_push(HWND)  
{ 
    terminal->append_output("pushing to github\n");
    terminal->execute_command("git add .");
    terminal->execute_command("git commit -m \"auto commit from c++ ide\"");
    terminal->execute_command("git pull");
    terminal->execute_command("git push");
}
void on_exit(HWND hWnd){DestroyWindow(hWnd);}
struct CommandHandler
{
	UINT id;
	void (*handler)(HWND);
};
    CommandHandler commands[] =
    {
        { idm_file,      on_file },
        { idm_edit,      on_edit },
        { idm_theme,     on_theme },
        { idm_open_file, on_open_file },
        { idm_run_log,  on_run_log },
        { idm_run,       on_run  },
        { idm_directory, on_directory },
        { idm_save,      on_save },
        { idm_terminal,  on_terminal },
        { idm_git_push,  on_git_push },
        { idm_exit,      on_exit },
    };
HWND create_button(
    HWND parent,
    HINSTANCE hInstance,
    HFONT font,
    const wchar_t* text,
    int id,
    int index
)
{
    HWND b = CreateWindowEx(
        0,
        L"BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, // owner-draw
        0, button_height * index, toolbar_width, button_height,
        parent,
        (HMENU)(INT_PTR)id,
        hInstance,
        NULL
    );

    SendMessage(b, WM_SETFONT, (WPARAM)font, TRUE);
    return b;
}

// HWND create_button(
//     HWND parent,
//     HINSTANCE hInstance,
//     HFONT font,
//     const char* text,
//     int id,
//     int index
// )
// {
//     HWND b = CreateWindowEx(
//         0, L"BUTTON", text,
//         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
//         0, button_height * index, toolbar_width, button_height,
//         parent,
//         (HMENU)(INT_PTR)id,
//         hInstance,
//         NULL
//     );
//     SendMessage(b, WM_SETFONT, (WPARAM)font, TRUE);
//     return b;
// }
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    // auto config = terminal->read_config("config.txt");
    switch (message) 
    {
        case WM_CREATE:
        {
            // embedded_terminal* terminal = new embedded_terminal(toolbar_width, editor_height, editor_width, terminal_height);
            // terminal = std::make_unique<embedded_terminal>(hWnd, toolbar_width, editor_height, editor_width, terminal_height);
            std::cout << "WM_CREATE" << std::endl;
            HINSTANCE hInstance = GetModuleHandle(NULL);
            HFONT hToolbarFont = CreateFont(
                18, 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                L"Audiowide"
            );
            
            hEdit = CreateWindowEx(0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                toolbar_width, 0,
                editor_width, editor_height,
                hWnd, NULL, hInstance, NULL);
            hBackgroundBrush = CreateSolidBrush(RGB(30, 30, 30));
            hEditBrush = CreateSolidBrush(RGB(20, 20, 20));
            hButtonBrush = CreateSolidBrush(RGB(0, 0, 0));
            ButtonInfo buttons[] = {
                {"file",      idm_file},
                {"open file", idm_open_file},
                {"run log",   idm_run_log},
                {"edit",      idm_edit},
                {"theme",     idm_theme},
                {"directory", idm_directory},
                {"run",       idm_run},
                {"terminal",  idm_terminal},
                {"git push",  idm_git_push},
                {"save",      idm_save},
                {"exit",      idm_exit},
            };
            for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); ++i) {
                // create_button(hWnd, hInstance, hToolbarFont, buttons[i].text, buttons[i].id, i);
                std::string s = buttons[i].text;
                std::wstring ws(s.begin(), s.end());
                create_button(hWnd, hInstance, hToolbarFont, ws.c_str(), buttons[i].id, i);
            }
            break;
        }
        case WM_SIZE:
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            int terminal_height = rc.bottom / 2;
            MoveWindow(hEdit, toolbar_width, 0, rc.right - toolbar_width, rc.bottom - terminal_height, TRUE);
            update_terminal_position(hWnd);
            break;
        }
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(220, 220, 220));
            SetBkColor(hdc, RGB(20, 20, 20));
            return (LRESULT)hEditBrush;
        }
        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlType == ODT_BUTTON) {
                HBRUSH brush;
                if (dis->itemState & ODS_SELECTED) {
                    brush = CreateSolidBrush(RGB(30, 30, 30));
                } else if (dis->itemState & ODS_HOTLIGHT) {
                    brush = CreateSolidBrush(RGB(50, 50, 50));
                } else {
                    brush = CreateSolidBrush(RGB(0, 0, 0));
                }

                FillRect(dis->hDC, &dis->rcItem, brush);
                DeleteObject(brush);
                DrawEdge(dis->hDC, &dis->rcItem, EDGE_RAISED, BF_RECT);
                SetTextColor(dis->hDC, RGB(220, 220, 220));
                SetBkMode(dis->hDC, TRANSPARENT);
                wchar_t text[256];
                GetWindowText(dis->hwndItem, text, 256);
                DrawText(dis->hDC, text, -1, &dis->rcItem,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                return TRUE;
            }
            break;
        }

        // case WM_DRAWITEM:
        // {
        //     DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        //     if (dis->CtlID == id) {
        //         HBRUSH brush = CreateSolidBrush(RGB(0,0,0));
        //         FillRect(dis->hDC, &dis->rcItem, brush);
        //         DeleteObject(brush);
        //         SetTextColor(dis->hDC, RGB(220,220,220));
        //         SetBkMode(dis->hDC, TRANSPARENT);
        //         DrawText(dis->hDC, text, -1, &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        //         return TRUE;
        //     }
        //     break;
        // }

        // case WM_CTLCOLOREDIT:
        // {
        //     HDC hdc = (HDC)wParam;
        //     HWND hwndCtrl = (HWND)lParam;
        //     if (hwndCtrl == hTerminal)
        //     {
        //         SetTextColor(hdc, RGB(255, 255, 255));
        //         SetBkColor(hdc, RGB(0, 0, 0));
        //         return (LRESULT)hBlackBrush;
        //     }
        //     break;
        // }
        case WM_CTLCOLORBTN:
        {
            // std::cout <<  "WM_CTLCOLORBTN" << std::endl;
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(220, 220, 220));
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)hButtonBrush;
        }
        case WM_COMMAND:
        {
            std::cout << "WM_COMMAND" << std::endl;
            UINT id = LOWORD(wParam);
            for (auto& cmd : commands)
            {
                if (cmd.id == id)
                {
                    cmd.handler(hWnd);
                    return 0;
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        //     freopen("CONOUT$", "w", stdout);
        //     freopen("CONOUT$", "w", stderr);
        //     std::cout << "Console attached!" << std::endl;
        // }?
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    std::cout << "here1" << std::endl;
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    std::cout << "here2" << std::endl;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"TextEditorClass";
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, L"c++ editor", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height, NULL, NULL, hInstance, NULL);
    terminal = new embedded_terminal(hWnd, toolbar_width, editor_height, editor_width, terminal_height);
    // // RECT rcClient;
    // GetClientRect(hWnd, &rcClient);
        // HWND hEdit = CreateWindowEx(0, "EDIT", "",
        //     WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
        //     0, toolbar_width, 400, 500, hWnd, NULL, hInstance, NULL);
        // HFONT hToolbarFont = CreateFont(
            //     18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            //     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            //     CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Roboto");
            // HMENU h_menu = CreateMenu();
            // AppendMenu(h_menu, MF_STRING, idm_file, "file");
            // AppendMenu(h_menu, MF_STRING, idm_edit, "edit");
            // AppendMenu(h_menu, MF_STRING, idm_theme, "theme");
            // AppendMenu(h_menu, MF_STRING, idm_save, "save");
            // AppendMenu(h_menu, MF_STRING, idm_directory, "directory");
            // AppendMenu(h_menu, MF_STRING, idm_run, "run");
            // AppendMenu(h_menu, MF_STRING, idm_terminal, "terminal");
            // AppendMenu(h_menu, MF_STRING, idm_git_push, "git_push");
            // AppendMenu(h_menu, MF_STRING, idm_exit, "exit");
            // SetMenu(hWnd, h_menu);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // WNDCLASSEX wc{};
    // wc.cbSize = sizeof(WNDCLASSEX);
    // wc.lpfnWndProc = WndProc;
    // wc.hInstance = hInstance;
    // wc.lpszClassName = "TextEditorClass";
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    // RegisterClassEx(&wc);

    // HWND hWnd = CreateWindowEx(
    //     0, "TextEditorClass", "C++ Editor",
    //     WS_OVERLAPPEDWINDOW,
    //     CW_USEDEFAULT, CW_USEDEFAULT,
    //     800, 600,
    //     NULL, NULL, hInstance, NULL
    // );

    // ShowWindow(hWnd, nCmdShow);
    // UpdateWindow(hWnd);

    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0)) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }

    // return msg.wParam;
    // std::cout << "WinMain" << std::endl;
    // WNDCLASSEX wc{};
    // wc.cbSize = sizeof(WNDCLASSEX);
    // wc.lpfnWndProc = WndProc;
    // wc.hInstance = hInstance;
    // wc.lpszClassName = "TextEditorClass";
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    // wc.hIcon = (HICON)LoadImage(
    //     hInstance,
    //     MAKEINTRESOURCE(IDI_APP_ICON),
    //     IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR
    // );
    // wc.hIconSm = (HICON)LoadImage(
    //     hInstance,
    //     MAKEINTRESOURCE(IDI_APP_ICON),
    //     IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR
    // );

    // RegisterClassEx(&wc);

    // HWND hWnd = CreateWindowEx(
    //     0,
    //     "TextEditorClass",
    //     "c++ editor",
    //     WS_OVERLAPPEDWINDOW,
    //     CW_USEDEFAULT, CW_USEDEFAULT,
    //     window_width, window_height,
    //     NULL, NULL, hInstance, NULL
    // );

    // ShowWindow(hWnd, nCmdShow);
    // UpdateWindow(hWnd);

    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0)) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }
    return msg.wParam;
}
