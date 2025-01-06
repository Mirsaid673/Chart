cmake_minimum_required(VERSION 3.28)
project(Chart)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMPLOT_DIR ${CMAKE_SOURCE_DIR}/implot)
set(API_DIR ${CMAKE_SOURCE_DIR}/api)

add_executable(Chart
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx9.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMPLOT_DIR}/implot.cpp
        ${IMPLOT_DIR}/implot_demo.cpp
        ${IMPLOT_DIR}/implot_items.cpp
        ${API_DIR}/chart_api.grpc.pb.cc
        ${API_DIR}/chart_api.grpc.pb.h
        ${API_DIR}/chart_api.pb.cc
        ${API_DIR}/chart_api.pb.h
        WindowContext.cpp
        WindowContext.h
        RenderContext.cpp
        RenderContext.h
        AppBase.cpp
        AppBase.h
        main.cpp
        Chart.cpp
        Chart.h
        Style.cpp)

find_package(gRPC CONFIG REQUIRED)
target_link_libraries(Chart PRIVATE
        dwmapi
        D3D9
        gRPC::grpc++)
target_include_directories(Chart PRIVATE ${IMGUI_DIR} ${IMPLOT_DIR})