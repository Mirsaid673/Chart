cmake_minimum_required(VERSION 3.28)
project(Chart)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)

add_executable(Chart
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx9.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        RenderContext.cpp
        RenderContext.h
        Chart.cpp
        Chart.h
        main.cpp
        AppBase.cpp
        AppBase.h
        WindowContext.cpp
        WindowContext.h)

target_link_libraries(Chart PRIVATE dwmapi D3D9)
target_include_directories(Chart PRIVATE ${IMGUI_DIR})