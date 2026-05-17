#pragma once
#include <vector>
#include <string>
#include "include/raylib.h"
#include "include/imgui/imgui.h"
#include "NexusStorage.hpp"
#include "NexusLogger.hpp"
#include "NexusCharacter.hpp"

struct SceneManagerState {
    int selectedObjectIndex = -1;
};

inline void DrawSceneManagerWindow(SceneManagerState& state, std::vector<GameObject>& sceneObjects, const std::vector<CharacterProfile>& database, int& engineState, int& currentFrame, int& endlessTick, LoggerState& logger) {
    
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 320, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 520), ImGuiCond_Always);
    
    if (ImGui::Begin("Scene Hierarchy & Playback", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        
        // --- 1. THE SCENE HIERARCHY ---
        ImGui::TextDisabled("1. Active Scene Memory");
        if (ImGui::BeginListBox("##Hierarchy", ImVec2(-1, 150))) {
            for (int i = 0; i < (int)sceneObjects.size(); i++) {
                // Build a clean naming format (e.g., "[0] Primitive Cube" or "[1] Hero_01")
                std::string objName = "[" + std::to_string(i) + "] ";
                if (sceneObjects[i].type == 0) {
                    objName += "Primitive Cube";
                } else if (sceneObjects[i].type == 1) {
                    if (sceneObjects[i].profileIndex >= 0 && sceneObjects[i].profileIndex < (int)database.size()) {
                        objName += database[sceneObjects[i].profileIndex].name;
                    } else {
                        objName += "Unlinked Character";
                    }
                }

                if (ImGui::Selectable(objName.c_str(), state.selectedObjectIndex == i)) {
                    state.selectedObjectIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        // --- SAFE DELETION ---
        if (state.selectedObjectIndex >= 0 && state.selectedObjectIndex < (int)sceneObjects.size()) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("X Remove Selected Object", ImVec2(-1, 25))) {
                sceneObjects.erase(sceneObjects.begin() + state.selectedObjectIndex);
                logger.AddLog("[Scene] Object deleted from memory.");
                state.selectedObjectIndex = -1; // Reset selection so we don't hold a dead pointer
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::Button("Select an object to remove...", ImVec2(-1, 25));
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // --- 2. PRIMITIVE SPAWNER ---
        ImGui::TextDisabled("2. Asset Spawning");
        if (ImGui::Button("+ Spawn Primitive Cube", ImVec2(-1, 25))) {
            GameObject newCube;
            newCube.type = 0;
            newCube.position = {0, 1.0f, 0};
            newCube.startFrame = 0;
            newCube.endFrame = 1800;
            sceneObjects.push_back(newCube);
            logger.AddLog("[Scene] Primitive Cube injected.");
        }

        ImGui::Separator();
        
        // --- 3. MASTER EXECUTION STATES ---
        ImGui::TextDisabled("3. Master Execution States");
        ImGui::Spacing();
        
        if (engineState == 0) {
            if (ImGui::Button("► RUN ENGINE (Live)", ImVec2(-1, 30))) { engineState = 1; }
            if (ImGui::Button("► REPLAY TIMELINE", ImVec2(-1, 30))) { engineState = 2; currentFrame = 0; endlessTick = 0; }
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button("● RECORD TIMELINE", ImVec2(-1, 30))) {
                engineState = 3; currentFrame = 0; endlessTick = 0; 
                for (auto& obj : sceneObjects) {
                    for (int f = 0; f <= 1800; f++) obj.bakedPositions[f] = obj.position;
                    obj.hasRecording = true;
                }
                logger.AddLog("[Timeline] Recording armed and active.");
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
            if (ImGui::Button("■ STOP ENGINE", ImVec2(-1, 40))) { 
                engineState = 0; 
                logger.AddLog("[Timeline] Engine halted. State reset to Stopped.");
            }
            ImGui::PopStyleColor();
            
            ImGui::Spacing();
            if (engineState == 1) ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "STATUS: LIVE PHYSICS RUNNING");
            if (engineState == 2) ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "STATUS: REPLAYING TIMELINE");
            if (engineState == 3) ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.0f), "STATUS: RECORDING ACTIVE...");
        }
    }
    ImGui::End();
}