#include "Chart.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <grpcpp/grpcpp.h>
#include "api/chart_api.grpc.pb.h"

std::string target_str = "164.92.247.24:50051";
void Chart::start() {
    auto channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());
    std::unique_ptr<chart_api::ChartAPI::Stub> stub = chart_api::ChartAPI::NewStub(channel);

    chart_api::GetAlgosRequest request;
    chart_api::GetAlgosResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub->GetAlgos(&context, request, &response);
    if (not status.ok())
    {
        return;
    }
    std::cout << response.algos_size() << std::endl;
    auto& algos = response.algos();
    for (auto& alg : algos)
    {
        std::cout << alg << std::endl;
    }
}

void Chart::draw() {
    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);

    ImGui::Begin("Another Window", nullptr);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::End();

    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Side Bar", nullptr);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::End();
}
