#include "ChartApp.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <implot.h>
#include <implot_internal.h>


const Chart ChartApp::m_base_chart = Chart("base_candles", "");

double to_time(const std::string& str) {
    return std::stod(str);
}

void plotCandles(const std::string &label, const std::vector<chart_api::DataPoint> &candles);
void plotVols(const std::string &label, const std::vector<chart_api::DataPoint> &vols, const Chart &chart);

void ChartApp::plotChart(const Chart &chart, const std::vector<chart_api::DataPoint> &data) {
    const auto& type = chart.type;

    if (chart.type == m_base_chart.type)
        plotCandles(chart.name, data);
    else if(type == "vol")
        plotVols(chart.name, data, chart);
}

void plotVols(const std::string &label, const std::vector<chart_api::DataPoint> &data, const Chart &chart) {
    static const ImVec4 color = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    ImU32 ucolor      = ImGui::GetColorU32(color);

    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    if (ImPlot::BeginItem(label.c_str())) {
        ImPlot::GetCurrentItem()->Color = ucolor;
        // fit data if requested
        // if (ImPlot::FitThisFrame()) {
        //     for (const auto& candle : data) {
        //         const auto time = to_time(candle.timestamp());
        //         ImPlot::FitPoint(ImPlotPoint(time, candle.low()));
        //         ImPlot::FitPoint(ImPlotPoint(time, candle.high()));
        //     }
        // }

        auto need_fit = ImPlot::FitThisFrame();
        for (const auto& point : data) {
            auto it = std::ranges::find_if(point.indicators(), [&](const chart_api::Indicator &ind) { return ind.type() == chart.type and ind.name() == chart.name; });
            if (it == point.indicators().end())
                continue;
            auto& params = it->param();
            // const auto time = to_time(vol.timestamp());
            // auto open_pos  = ImPlot::PlotToPixels( time - half_width, vol.open());
            // auto close_pos = ImPlot::PlotToPixels(time + half_width, vol.close());
            // auto low_pos   = ImPlot::PlotToPixels(time, vol.low());
            // auto high_pos  = ImPlot::PlotToPixels(time, vol.high());
            // draw_list->AddLine(low_pos, high_pos, ucolor);
            // draw_list->AddRectFilled(open_pos, close_pos, color);
        }
        ImPlot::EndItem();
    }
}

//timeframe in seconds
double roundTime(time_t t, uint32_t timeframe) {
    t += timeframe / 2;
    return t - (t % timeframe);
}

void plotCandles(const std::string &label, const std::vector<chart_api::DataPoint> &candles) {
    static constexpr float candle_width = 0.75f;
    static const ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
    static const ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);

    const auto count = candles.size();
    if (count < 2) {
        if (ImPlot::BeginItem("No data loaded")) {
            ImPlot::EndItem();
        }
        return;
    }

    auto first_candle = to_time(candles.front().timestamp());
    auto time_frame = to_time(candles[1].timestamp()) - first_candle;
    auto min_z = time_frame * 10;
    auto max_z = time_frame * 1000;
    const auto half_width = time_frame * candle_width / 2;
    ImPlot::SetupAxisZoomConstraints(ImAxis_X1, min_z, max_z);
    //ImPlot::SetupAxisLimits(ImAxis_X1, first_candle, last_candle + time_frame * 10,  ImPlotCond_Always);
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    if (ImPlot::BeginItem(label.c_str())) {
        // ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        auto need_fit = ImPlot::FitThisFrame();

        for (const auto& candle : candles) {
            const auto time = to_time(candle.timestamp());
            if (need_fit) {
                ImPlot::FitPoint(ImPlotPoint(time, candle.low()));
                ImPlot::FitPoint(ImPlotPoint(time, candle.high()));
            }

            auto open_pos  = ImPlot::PlotToPixels( time - half_width, candle.open());
            auto close_pos = ImPlot::PlotToPixels(time + half_width, candle.close());
            auto low_pos   = ImPlot::PlotToPixels(time, candle.low());
            auto high_pos  = ImPlot::PlotToPixels(time, candle.high());
            ImU32 color = ImGui::GetColorU32(candle.open() > candle.close() ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }
        ImPlot::EndItem();
    }

    auto cursor = ImGui::GetMouseCursor();
    if (ImPlot::IsPlotHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        auto color = IM_COL32_WHITE;
        const auto limits = ImPlot::GetPlotLimits();
        ImPlotPoint plot_mouse = ImPlot::GetPlotMousePos();
        plot_mouse.x = roundTime(plot_mouse.x, time_frame);
        auto mouse = ImPlot::PlotToPixels(plot_mouse);
        auto min = ImPlot::PlotToPixels(limits.X.Min, limits.Y.Min);
        auto max = ImPlot::PlotToPixels(limits.X.Max, limits.Y.Max);
        ImPlot::PushPlotClipRect();
        draw_list->AddLine({mouse.x, min.y}, {mouse.x, max.y}, color);
        draw_list->AddLine({min.x, mouse.y}, {max.x, mouse.y}, color);

        // ImGuiTextBuffer& builder = ImPlot::GetCurrentContext()->MousePosStringBuilder;
        // builder.Buf.shrink(0);
        // auto x_pos = std::move(std::to_string(plot_mouse.x));
        // auto y_pos = std::move("$" + std::to_string(plot_mouse.y));
        // builder.append((x_pos + ", " + y_pos).c_str());
        // if (!builder.empty()) {
        //     auto plot = ImPlot::GetCurrentPlot();
        //     auto context = ImPlot::GetCurrentContext();
        //     const ImVec2 size = ImGui::CalcTextSize(builder.c_str());
        //     const ImVec2 pos = ImPlot::GetLocationPos(plot->PlotRect, size, plot->MouseTextLocation, context->Style.MousePosPadding);
        //     draw_list->AddText(pos, ImPlot::GetStyleColorU32(ImPlotCol_InlayText), builder.c_str());
        // }
        ImPlot::PopPlotClipRect();
    } else {
        ImGui::SetMouseCursor(cursor);
    }
}
