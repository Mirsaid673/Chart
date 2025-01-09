#include "Chart.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <implot.h>
#include <implot_internal.h>

double to_time(const std::string& str) {
    return std::stod(str);
}

void Chart::plotCandles(const std::string &label, const std::vector<chart_api::DataPoint> &candles) {
    static const float candle_width = 0.25f;
    static const ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
    static const ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);

    const auto count = candles.size();
    if (count < 2) {
        if (ImPlot::BeginItem("No data loaded")) {
            ImPlot::EndItem();
        }
        return;
    }

    auto min_z = to_time(candles[1].timestamp()) - to_time(candles[0].timestamp());
    auto max_z = to_time(candles.back().timestamp()) + to_time(candles.front().timestamp());
    const auto half_width = min_z * candle_width;

    ImPlot::SetupAxisZoomConstraints(ImAxis_X1, min_z * 10, min_z * 1'000);
    // if (ImPlot::IsPlotHovered()) {
    //     ImPlotPoint mouse   = ImPlot::GetPlotMousePos();
    //     mouse.x             = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day).ToDouble();
    //     float  tool_l       = ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
    //     float  tool_r       = ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
    //     float  tool_t       = ImPlot::GetPlotPos().y;
    //     float  tool_b       = tool_t + ImPlot::GetPlotSize().y;
    //     ImPlot::PushPlotClipRect();
    //     //draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128,128,128,64));
    //     ImPlot::PopPlotClipRect();
    //     // find mouse location index
    //     int idx = BinarySearch(xs, 0, count - 1, mouse.x);
    //     // render tool tip (won't be affected by plot clip rect)
    //     if (idx != -1) {
    //         ImGui::BeginTooltip();
    //         char buff[32];
    //         ImPlot::FormatDate(ImPlotTime::FromDouble(xs[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
    //         ImGui::Text("Day:   %s",  buff);
    //         ImGui::Text("Open:  $%.2f", opens[idx]);
    //         ImGui::Text("Close: $%.2f", closes[idx]);
    //         ImGui::Text("Low:   $%.2f", lows[idx]);
    //         ImGui::Text("High:  $%.2f", highs[idx]);
    //         ImGui::EndTooltip();
    //     }
    // }

    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    if (ImPlot::BeginItem(label.c_str())) {
        // override legend icon color
        // ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (const auto& candle : candles) {
                const auto time = to_time(candle.timestamp());
                ImPlot::FitPoint(ImPlotPoint(time, candle.low()));
                ImPlot::FitPoint(ImPlotPoint(time, candle.high()));
            }
        }
        for (const auto& candle : candles) {
            const auto time = to_time(candle.timestamp());
            auto open_pos  = ImPlot::PlotToPixels( time - half_width, candle.open());
            auto close_pos = ImPlot::PlotToPixels(time + half_width, candle.close());
            auto low_pos   = ImPlot::PlotToPixels(time, candle.low());
            auto high_pos  = ImPlot::PlotToPixels(time, candle.high());
            ImU32 color      = ImGui::GetColorU32(candle.open() > candle.close() ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }
        ImPlot::EndItem();
    }
}
