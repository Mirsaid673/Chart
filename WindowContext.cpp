#include "WindowContext.h"

#include <ostream>
#include <unordered_map>

#include <backends/imgui_impl_win32.h>

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int32_t WindowContext::init() {
    ImGui_ImplWin32_EnableDpiAwareness();
    wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, "ImGui Example", nullptr };
    RegisterClassExA(&wc);
    return 0;
}

static std::unordered_map<HWND, WindowContext*> g_handler_windows;
int32_t WindowContext::createWindow(uint32_t width, uint32_t height, const std::string& title) {
    hwnd = CreateWindowA(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, width, height, nullptr, nullptr, wc.hInstance, nullptr);
    if (hwnd == nullptr)
        return -1;

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    g_handler_windows[hwnd] = this;

    ImGui_ImplWin32_Init(hwnd);
    return 0;
}

void WindowContext::destroy() const {
    ImGui_ImplWin32_Shutdown();
    DestroyWindow(hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
    g_handler_windows.erase(hwnd);
}

void WindowContext::update() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            should_close = true;
    }

    ImGui_ImplWin32_NewFrame();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE: {
            auto it = g_handler_windows.find(hWnd);
            if (it == g_handler_windows.end())
                return 0;
            auto context = it->second;
            context->onResize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
        break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
        return 0;
        default: break;
    }
    return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}