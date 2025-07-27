// SeaExplorer.cpp
// Фінальний GUI-проєкт WinAPI з синім фоном, списком файлів та вбудованим cmd.exe

#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

const wchar_t CLASS_NAME[] = L"SeaExplorerWindow";
HWND hListBox;
HWND hCmdWindow = nullptr;

// Отримує список файлів з C:\Users
std::vector<std::wstring> GetUserDirectoryFiles() {
    std::vector<std::wstring> files;
    std::wstring path = L"C:\\Users";

    for (const auto& entry : fs::directory_iterator(path)) {
        files.push_back(entry.path().filename().wstring());
    }
    return files;
}

// Заповнює ListBox
void PopulateListBox(HWND hListBox) {
    std::vector<std::wstring> files = GetUserDirectoryFiles();
    for (const auto& file : files) {
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)file.c_str());
    }
}

// Вбудовує консоль cmd.exe у вікно
void EmbedCMD(HWND hwndParent) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    wchar_t cmdLine[] = L"cmd.exe /k cd C:\\Users";

    CreateProcessW(nullptr, cmdLine, nullptr, nullptr, FALSE,
                   CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);

    Sleep(500); // дати вікну час з'явитись

    HWND hwndCmd = FindWindowW(L"ConsoleWindowClass", nullptr);
    if (hwndCmd) {
        SetParent(hwndCmd, hwndParent);
        SetWindowLong(hwndCmd, GWL_STYLE, WS_VISIBLE | WS_CHILD);
        MoveWindow(hwndCmd, 20, 320, 300, 200, TRUE);
        hCmdWindow = hwndCmd;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"You can see all like in a sea ;)",
                      WS_VISIBLE | WS_CHILD,
                      20, 10, 300, 20, hwnd, nullptr, nullptr, nullptr);

        hListBox = CreateWindowW(L"LISTBOX", nullptr,
                                 WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                                 20, 40, 300, 250, hwnd, nullptr, nullptr, nullptr);

        PopulateListBox(hListBox);
        EmbedCMD(hwnd);
        break;

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        return 1;
    }

    case WM_DESTROY:
        if (hCmdWindow) PostMessage(hCmdWindow, WM_CLOSE, 0, 0);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Sea Explorer",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 360, 560,
                               nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
