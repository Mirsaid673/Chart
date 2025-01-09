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
    const std::string m_side_bar_title = "Side Bar";
    const std::string m_algos_title = "Algos";
    const std::string m_streams_title = "Streams";
    const std::string m_fields_title = "Fields";
\
    const std::string m_base_chart = "charts";

    std::unique_ptr<chart_api::ChartAPI::Stub> m_stub;
    std::vector<std::string> m_algos;
    std::unordered_map<int32_t, chart_api::StreamData> m_streams;
    std::vector<chart_api::DataPoint> m_current_stream_data;
    std::vector<std::vector<std::string>> m_fields_layout;

    uint32_t m_current_algo = 0;
    int32_t m_current_stream = 0;

    const chart_api::StreamData& getCurrentStream()  {return m_streams[m_current_stream];}
    const chart_api::StreamData& getStream(int32_t id)  {return m_streams[id];}
    std::vector<chart_api::DataPoint> getStreamData() { return std::move(getStreamData(m_current_stream)); }
    std::vector<chart_api::DataPoint> getStreamData(int32_t stream_id);

    void onStreamChange(int32_t new_stream_id);
    void plotCandles(const std::string& label, const std::vector<chart_api::DataPoint>& candles);

    void contentWindow();
    void sideBar();
    void algosTab();
    void streamsTab();
    void fieldsTab();

    std::vector<std::vector<std::string>> configureLayout(const chart_api::StreamData& stream);
    std::string getSymbol(const chart_api::StreamData& stream) {
        return stream.coin() + "/" + stream.coin_second();
    }
    void setStyle();
};
