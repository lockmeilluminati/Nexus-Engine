#pragma once
#include <string>
#include <vector>
#include "include/imgui/imgui.h"

struct LoggerState {
    bool showUI = true;
    std::vector<std::string> logs;

    void AddLog(const std::string& message) {
        logs.push_back(message);
    }

    void Clear() {
        logs.clear();
    }
};

inline void DrawLoggerWindow(LoggerState& logger, float screenW, float screenH) {
    if (!logger.showUI) return;

    // Permanently docked to the bottom right, taking up 30% of the screen width
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.7f, screenH - 220), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screenW * 0.3f, 220), ImGuiCond_Always);
    
    if (ImGui::Begin("Engine Logs", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        if (ImGui::Button("Clear Logs")) logger.Clear();
        ImGui::Separator();
        
        ImGui::BeginChild("LogRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& log : logger.logs) {
            if (log.find("[Error]") != std::string::npos) {
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", log.c_str());
            } else if (log.find("[Asset]") != std::string::npos || log.find("[Environment]") != std::string::npos) {
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", log.c_str());
            } else {
                ImGui::TextUnformatted(log.c_str());
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
}