#pragma once

#include "AppBase.h"

#include <grpcpp/grpcpp.h>
#include "api/chart_api.grpc.pb.h"

#include <vector>

class Chart : public AppBase{
    void start() override;
    void draw() override;
    void onResize(uint32_t new_width, uint32_t new_height) override;

private:
    const std::string m_target = "164.92.247.24:50051";
    const std::string m_content_title = "Content";
    const std::string m_algos_title = "Algos";
    const std::string m_streams_title = "Streams";

    std::unique_ptr<chart_api::ChartAPI::Stub> m_stub;
    std::vector<std::string> m_algos;
    std::unordered_map<int32_t, chart_api::StreamData> m_streams;
    std::vector<chart_api::DataPoint> m_current_stream_data;

    uint32_t m_current_algo = 0;
    int32_t m_current_stream = 0;

    std::vector<chart_api::DataPoint> getStreamData() { return std::move(getStreamData(m_current_stream)); }
    std::vector<chart_api::DataPoint> getStreamData(int32_t stream_id);
    void onStreamChange();
    void plotCandles(const std::string& label, const std::vector<chart_api::DataPoint>& candles);

    void contentWindow();
    void algosWindow();
    void streamsWindow();

    void setStyle();
};
