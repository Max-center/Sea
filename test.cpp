#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

const wchar_t CLASS_NAME[] = L"MyWindowClass";
HWND hListBox;

std::vector<std::wstring> GetUserDirectoryFiles() {
    std::vector<std::wstring> files;
    std::wstring path = L"C:\\Users";

    for (const auto& entry : fs::directory_iterator(path)) {
        files.push_back(entry.path().filename().wstring());
    }

    return files;
}

void PopulateListBox(HWND hListBox) {
    std::vector<std::wstring> files = GetUserDirectoryFiles();
    for (const auto& file : files) {
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)file.c_str());
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC", L"You can see all like in a sea ;)", WS_VISIBLE | WS_CHILD,
                      20, 10, 300, 20, hwnd, nullptr, nullptr, nullptr);

        hListBox = CreateWindowW(L"LISTBOX", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                                 20, 40, 300, 250, hwnd, nullptr, nullptr, nullptr);

        // одразу заповнити список
        PopulateListBox(hListBox);
        break;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX: {
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, RGB(0, 0, 255)); // синій фон
        SetTextColor(hdcStatic, RGB(255, 255, 255)); // білий текст
        static HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
        return (INT_PTR)hBrush;
    }

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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // змінюється вручну в WM_ERASEBKGND

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Sea Explorer", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 360, 360,
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
