#include "Chart.h"

void Chart::draw() {
    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);

    ImGui::Begin("Another Window", nullptr, ImGuiWindowFlags_NoTitleBar);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::End();

    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Side Bar", nullptr, ImGuiWindowFlags_NoTitleBar);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::End();
}
