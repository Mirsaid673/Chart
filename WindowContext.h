#pragma once

#include <windows.h>
#include <cstdint>
#include <string>
#include <functional>

class WindowContext {
private:
    WNDCLASSEXA wc{};
    HWND hwnd = nullptr;
    std::function<void(uint32_t, uint32_t)> resize_callback = [](uint32_t width, uint32_t height) {};
    bool should_close = false;

public:
    int32_t init();
    int32_t createWindow(uint32_t width, uint32_t height, const std::string& title);
    void destroy() const;
    void update();

    void setResizeCallback(std::function<void(uint32_t, uint32_t)> resize_callback) {
        this->resize_callback = std::move(resize_callback);
    }

    void onResize(uint32_t width, uint32_t height) const {
        resize_callback(width, height);
    }

    bool shouldClose() const {return should_close;}
    friend class RenderContext;
};

