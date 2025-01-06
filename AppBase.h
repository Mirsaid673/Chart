#pragma once

class WindowContext;
class RenderContext;

#include <memory>
#include <string>

class AppBase {
private:
    int32_t initialize();
    void terminate();
    std::shared_ptr<RenderContext> render_context;
    std::shared_ptr<WindowContext> window_context;

protected:
    std::string window_title = "Chart";
    uint32_t window_width = 1200;
    uint32_t window_height = 800;

    virtual void start() {};
    virtual void update() {};
    virtual void draw() {};
    virtual void clean() {};
    virtual void onResize(uint32_t new_width, uint32_t new_height) {};

    bool running = true;
public:
    int32_t run();

    AppBase();
    virtual ~AppBase() = default;
};



