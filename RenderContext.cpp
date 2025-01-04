#include "RenderContext.h"

#include "WindowContext.h"
#include <backends/imgui_impl_dx9.h>

int32_t RenderContext::init(WindowContext* wnd_ctx) {
    if (wnd_ctx == nullptr)
        return -1;

    if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return -1;

    window_context = wnd_ctx;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    auto status = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd_ctx->hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
    if (status < 0)
        return -1;

    ImGui_ImplDX9_Init(pd3dDevice);
    return 0;
}

void RenderContext::destroy() {
    ImGui_ImplDX9_Shutdown();
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = nullptr; }
    if (pD3D) { pD3D->Release(); pD3D = nullptr; }
    window_context = nullptr;
}

void RenderContext::reset() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = pd3dDevice->Reset(&d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

void RenderContext::render() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); // TODO: make it class member

    pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
    pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (pd3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        pd3dDevice->EndScene();
    }
    HRESULT result = pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST) {
        HRESULT hr = pd3dDevice->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST) {
            ::Sleep(10);
            return;
        }
        if (hr == D3DERR_DEVICENOTRESET)
            reset();
    }
}

void RenderContext::resize(uint32_t width, uint32_t height) {
    if (width > d3dpp.BackBufferWidth || height > d3dpp.BackBufferHeight) {
        d3dpp.BackBufferWidth = width;
        d3dpp.BackBufferHeight = height;
        reset();
    }
}

void RenderContext::update() {
    ImGui_ImplDX9_NewFrame();
}
