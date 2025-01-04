#pragma once

#include <d3d9.h>
#include <cstdint>

class WindowContext;

class RenderContext {
private:
    LPDIRECT3D9              pD3D = nullptr;
    LPDIRECT3DDEVICE9        pd3dDevice = nullptr;
    D3DPRESENT_PARAMETERS    d3dpp = {};
    WindowContext*           window_context = nullptr;

public:
    int32_t init(WindowContext* wnd_ctx);
    void destroy();
    void reset();
    void render();
    void resize(uint32_t width, uint32_t height);
    void update();
};
