#include "AppBase.h"

#include "RenderContext.h"
#include "WindowContext.h"

#include <imgui.h>
#include <implot.h>

AppBase::AppBase(): render_context(std::make_shared<RenderContext>()), window_context(std::make_shared<WindowContext>()) {}

int32_t AppBase::initialize() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    ImGui::StyleColorsDark();

    if (auto init_status = window_context->init(); init_status != 0)
        return init_status;

    auto create_window_status = window_context->createWindow(window_width, window_height, window_title);

    if ( create_window_status != 0)
        return create_window_status;

    if (auto init_status = render_context->init(window_context.get()); init_status != 0)
        return init_status;

    window_context->setResizeCallback([&](uint32_t width, uint32_t height) {
        render_context->resize(width, height);
        onResize(width, height);
    });

    return 0;
}

void AppBase::terminate() {
    render_context->destroy();
    window_context->destroy();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

int32_t AppBase::run() {
    auto init_status = initialize();
    if (init_status != 0) {
        terminate();
        return init_status;
    }
    start();
    while (not window_context->shouldClose() and running) {
        window_context->update();
        render_context->update();
        update();

        ImGui::NewFrame();
        draw();
        ImGui::EndFrame();
        render_context->render();
    }
    clean();
    terminate();
    return 0;
}
