#include "ChartApp.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <implot.h>

static ImGuiWindowClass base_window_class;
void ChartApp::start() {
    setStyle();

    auto channel = grpc::CreateChannel(m_target, grpc::InsecureChannelCredentials());
    m_stub = chart_api::ChartAPI::NewStub(channel);

    grpc::ClientContext algo_context;
    chart_api::GetAlgosRequest algo_request;
    chart_api::GetAlgosResponse algo_response;

    const auto algos_status = m_stub->GetAlgos(&algo_context, algo_request, &algo_response);
    if (not algos_status.ok()) {
        std::cerr << algos_status.error_code() << " " << algos_status.error_message() << std::endl;
        return;
    }
    m_algos.reserve(algo_response.algos_size());
    for (const auto& alg : algo_response.algos())
        m_algos.emplace_back(alg);

    // get streams
    for (const auto& algo : m_algos) {
        grpc::ClientContext streams_context;
        chart_api::GetStreamsByAlgoRequest streams_request;
        chart_api::GetStreamsByAlgoResponse streams_response;
        streams_request.set_algo(algo);
        const auto streams_status = m_stub->GetStreamsByAlgo(&streams_context, streams_request, &streams_response);
        if (not streams_status.ok()) {
            std::cerr << streams_status.error_code() << " " << streams_status.error_message() << std::endl;
            return;
        }
        for (auto& stream : streams_response.streams()) {
            auto id = stream.id();
            m_streams[id] = stream;
        }
    }
    if (not m_streams.empty()) {
        auto front = m_streams.begin();
        m_current_stream = front->first;
        auto it = std::ranges::find(m_algos, front->second.algo());
        m_current_algo = it - m_algos.begin();
    }

    m_current_stream_data = std::move(getStreamData());
    m_fields_layout = std::move(configureLayout(getCurrentStream()));

    base_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
}

std::vector<chart_api::DataPoint> ChartApp::getStreamData(int32_t stream_id) {
    grpc::ClientContext context;
    chart_api::GetDataRequest request;
    request.set_id(stream_id);
    request.set_limit(100000);
    chart_api::GetDataResponse response;
    const auto status = m_stub->GetData(&context, request, &response);

    if (not status.ok()) {
        std::cerr << status.error_code() << " " << status.error_message() << std::endl;
        return {};
    }

    std::vector<chart_api::DataPoint> result;
    result.reserve(response.data().size());
    for (auto& data : response.data())
        result.emplace_back(data);
    return std::move(result);
}

void ChartApp::onStreamChange(int32_t new_stream_id) {
    m_current_stream_data = std::move(getStreamData(new_stream_id));
    m_fields_layout = std::move(configureLayout(getStream(new_stream_id)));
}

void ChartApp::draw() {
    // setup docking
    const auto dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    static auto first_time = true;
    if (first_time)
    {
        first_time = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
        auto dockspace_main_id = dockspace_id;
        auto right = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_main_id);

        ImGui::DockBuilderDockWindow(m_content_title.c_str(), dockspace_main_id);
        ImGui::DockBuilderDockWindow(m_side_bar_title.c_str(), right);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    contentWindow();
    sideBar();
}

void ChartApp::contentWindow() {
    ImGui::SetNextWindowClass(&base_window_class);

    ImGui::Begin(m_content_title.c_str(), nullptr);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
    const auto& curr_stream = getCurrentStream();
    const auto& symbol = getSymbol(curr_stream);
    const auto region = ImGui::GetContentRegionAvail();
    auto rows = std::ranges::count_if(m_fields_layout, [](const auto& layout) {return not layout.empty();});
    const auto height = region.y / static_cast<float>(rows);
    for (const auto& charts : m_fields_layout) {
        if (charts.empty())
            continue;
        ImGui::PushID(&charts);
        ImGui::BeginChild("##chart", {-1, height});
        if (ImPlot::BeginPlot(symbol.c_str(), {-1,-1}, ImPlotFlags_NoTitle | ImPlotFlags_NoMouseText)) {
            ImPlot::SetupAxes(nullptr,nullptr,0,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.2f");
            for (const auto& chart : charts) {
                const auto& to_pass = chart.type == m_base_chart.type ? Chart(m_base_chart.type, symbol) : chart;
                plotChart(to_pass, m_current_stream_data);
            }
            ImPlot::EndPlot();
        }
        ImGui::EndChild();
        ImGui::PopID();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void ChartApp::sideBar() {
    ImGui::SetNextWindowClass(&base_window_class);
    ImGui::Begin(m_side_bar_title.c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::BeginTabBar(m_side_bar_title.c_str());

    algosTab();
    streamsTab();
    fieldsTab();

    ImGui::EndTabBar();
    ImGui::End();
}

void ChartApp::algosTab() {
    if (ImGui::BeginTabItem(m_algos_title.c_str())) {
        for (auto i = 0; i < m_algos.size(); i++) {
            auto& algo = m_algos[i];
            bool selected = i == m_current_algo;
            if (ImGui::Selectable(algo.c_str(), &selected))
                m_current_algo = i;
        }
        ImGui::EndTabItem();
    }
}

void ChartApp::streamsTab() {
    if (ImGui::BeginTabItem(m_streams_title.c_str())) {
        for (const auto&[id, stream] : m_streams) {
            if (stream.algo() != m_algos[m_current_algo])
                continue;
            bool selected = id == m_current_stream;
            if (ImGui::Selectable(getSymbol(stream).c_str(), &selected)) {
                if (m_current_stream != id)
                    onStreamChange(id);
                m_current_stream = id;
            }
        }
        ImGui::EndTabItem();
    }
}

void ChartApp::fieldsTab() {
    if (ImGui::BeginTabItem(m_fields_title.c_str())) {
        auto& curr_stream = getCurrentStream();
        for (const auto& indicator : curr_stream.indicator_param()) {
            auto params = indicator.param();
            const auto& name = params["name"];
            const auto& type = indicator.type();
            Chart curr_chart(type, name);

            auto found = -1;
            for (auto i = 0; i < m_fields_layout.size(); i++) {
                const auto& field = m_fields_layout[i];
                if (auto it = std::ranges::find(field, curr_chart); it != field.end()) {
                    found = i;
                    break;
                }
            }
            std::string input;
            if (found != -1)
                input = std::to_string(found);

            ImGui::PushID(&curr_chart);
            ImGui::SetNextItemWidth(24);
            if (ImGui::InputText(name.c_str(), &input, ImGuiInputTextFlags_CharsDigit | ImGuiInputTextFlags_CharsNoBlank))
            {
                if (input.empty() and found != -1) {
                    auto to_erase = std::ranges::find(m_fields_layout[found], curr_chart);
                    m_fields_layout[found].erase(to_erase);
                }else {
                    auto to_push = std::stoi(input);
                    if (to_push < m_fields_layout.size())
                        m_fields_layout[to_push].push_back(curr_chart);
                }
            }

            ImGui::PopID();
        }
        ImGui::EndTabItem();
    }
}

std::vector<std::vector<Chart>> ChartApp::configureLayout(const chart_api::StreamData &stream) {
    std::vector<std::vector<Chart>> result;
    result.resize(stream.indicator_param_size() + 1);
    // for (auto& i : stream.indicator_param()) {
    //         std::cout << i.type() << std::endl << std::endl;
    //     for (auto& p : i.param()) {
    //         std::cout << p.first << " " << p.second << std::endl;
    //     }
    // }
    result.front() = {1, m_base_chart};
    return std::move(result);
}

void ChartApp::onResize(uint32_t new_width, uint32_t new_height) {
    std::cout << new_width << ", " << new_height << std::endl;
}