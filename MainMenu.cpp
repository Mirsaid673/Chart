#include "ChartApp.h"
#include <imgui.h>
#include <ranges>

void ChartApp::resetMode() {
    auto user_it = m_users.find(m_current_user);
    if (user_it == m_users.end())
        user_it = m_users.begin();
    m_current_user = user_it->first;

    auto& exchanges = user_it->second.exchanges();
    auto exchange_it = exchanges.find(m_current_exchange);
    if (exchange_it == exchanges.end())
        exchange_it = exchanges.begin();
    m_current_exchange = exchange_it->first;

    auto& markets = exchange_it->second.markets();
    auto markets_it = markets.find(m_current_market);
    if (markets_it == markets.end())
        markets_it = markets.begin();
    m_current_market = markets_it->first;

    auto& modes = markets_it->second.modes();
    auto modes_it = modes.find(m_current_mode);
    if (modes_it == modes.end())
        modes_it = modes.begin();
    m_current_mode = modes_it->first;

    auto& mode_time = modes_it->second.timestamps();
    auto time_it = std::ranges::find(mode_time, m_current_mode);
    if (time_it == mode_time.end())
        time_it = mode_time.begin();
    m_current_mode_time = *time_it;
}
void ChartApp::mainMenu() {
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Users")) {
        for (const auto &key: m_users | std::views::keys) {
            if (ImGui::MenuItem(key.c_str())) {
                m_current_user = key;
                resetMode();
            }
        }
        ImGui::EndMenu();
    }
    auto& exchanges = m_users[m_current_user].exchanges();
    if (ImGui::BeginMenu("Exchanges")) {
        for (const auto &key: exchanges | std::views::keys) {
            if (ImGui::MenuItem(key.c_str())) {
                m_current_exchange = key;
                resetMode();
            }
        }
        ImGui::EndMenu();
    }
    auto& markets = exchanges.at(m_current_exchange).markets();
    if (ImGui::BeginMenu("Markets")) {
        for (const auto &key: markets | std::views::keys) {
            if (ImGui::MenuItem(key.c_str())) {
                m_current_market = key;
                resetMode();
            }
        }
        ImGui::EndMenu();
    }

    auto& modes = markets.at(m_current_market).modes();
    if (ImGui::BeginMenu("Modes")) {
        for (const auto &key: modes | std::views::keys) {
            if (ImGui::MenuItem(key.c_str())) {
                m_current_mode = key;
                resetMode();
            }
        }
        ImGui::EndMenu();
    }

    auto& mode_times = modes.at(m_current_mode).timestamps();
    if (ImGui::BeginMenu("Times")) {
        for (const auto &key: mode_times) {
            if (ImGui::MenuItem(key.c_str())) {
                m_current_mode_time = key;
                resetMode();
            }
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}