#pragma once
#include <vector>
#include <string>
#include <cstring>
#include "include/raylib.h"
#include "include/imgui/imgui.h"
#include "NexusStorage.hpp"
#include "NexusLogger.hpp"

struct CharacterImporterState {
    bool showUI = true;
    
    // --- Draft Profile Memory ---
    char inputName[64] = "Hero_01";
    std::string draftFrontPath = "";
    std::string draftBackPath = "";
    Texture2D draftFrontTex = {0};
    Texture2D draftBackTex = {0};
    std::vector<AnimationSequence> draftSequences; 
    int selectedDraftSeqIndex = -1;
    
    // --- Draft 4-Way Walk Cycle Memory ---
    bool isWalkConfigured = false;
    int draftWalkFps = 12;
    std::vector<std::string> draftWalkUpPaths;
    std::vector<Texture2D> draftWalkUpTex;
    std::vector<std::string> draftWalkDownPaths;
    std::vector<Texture2D> draftWalkDownTex;
    std::vector<std::string> draftWalkLeftPaths;
    std::vector<Texture2D> draftWalkLeftTex;
    std::vector<std::string> draftWalkRightPaths;
    std::vector<Texture2D> draftWalkRightTex;

    // --- Draft Custom Sequence Memory ---
    char inputSeqName[64] = "Dance";
    int inputFps = 12;
    int inputMappedKey = 0; 
    bool isListeningForKey = false; 
    std::vector<std::string> draftFramePaths;
    std::vector<Texture2D> draftFrames; 
    
    // --- Database Memory ---
    std::vector<CharacterProfile> database;
    int selectedDatabaseIndex = -1;
};

inline void DrawCharacterImporterWindow(CharacterImporterState& state, std::vector<GameObject>& sceneObjects, float x, float y, float h, LoggerState& logger) {
    if (!state.showUI) return;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, h - y - 220), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Character Profile Importer", &state.showUI, ImGuiWindowFlags_NoCollapse)) {
        
        // --- 1. IDENTITY ---
        ImGui::TextDisabled("1. Identity");
        ImGui::InputText("Character Name", state.inputName, IM_ARRAYSIZE(state.inputName));
        ImGui::Separator();

        // --- 2. BASE SPRITES ---
        ImGui::TextDisabled("2. Base Sprites (Idle)");
        if (ImGui::Button("Upload Front View", ImVec2(-1, 25))) {
            std::string path = OpenImageDialog();
            if (!path.empty()) {
                if (state.draftFrontTex.id != 0) UnloadTexture(state.draftFrontTex);
                state.draftFrontPath = path;
                state.draftFrontTex = LoadTexture(path.c_str());
                logger.AddLog("[Asset] Loaded Front Sprite.");
            }
        }
        if (state.draftFrontTex.id != 0) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Front Preview:");
            ImGui::Image((void*)(intptr_t)state.draftFrontTex.id, ImVec2(64, 64));
            ImGui::Spacing();
        }

        if (ImGui::Button("Upload Back View", ImVec2(-1, 25))) {
            std::string path = OpenImageDialog();
            if (!path.empty()) {
                if (state.draftBackTex.id != 0) UnloadTexture(state.draftBackTex);
                state.draftBackPath = path;
                state.draftBackTex = LoadTexture(path.c_str());
                logger.AddLog("[Asset] Loaded Back Sprite.");
            }
        }
        if (state.draftBackTex.id != 0) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Back Preview:");
            ImGui::Image((void*)(intptr_t)state.draftBackTex.id, ImVec2(64, 64));
            ImGui::Spacing();
        }
        ImGui::Separator();

        // --- 3. NATIVE MOVEMENT (4-WAY) ---
        ImGui::TextDisabled("3. Native Movement");
        if (ImGui::Button(state.isWalkConfigured ? "Edit Walk Cycle" : "Configure Walk Cycle", ImVec2(-1, 35))) {
            ImGui::OpenPopup("Walk Cycle Editor");
        }
        
        if (state.isWalkConfigured) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "[ 4-Way Walk Cycle Active ]");
        }

        if (ImGui::BeginPopupModal("Walk Cycle Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Build your dynamic directional animations.");
            ImGui::SliderInt("Walk FPS", &state.draftWalkFps, 1, 60);
            ImGui::Separator();

            auto RenderDirectionRow = [&](const char* title, const char* id, std::vector<std::string>& paths, std::vector<Texture2D>& texs) {
                ImGui::Text("%s", title);
                if (ImGui::Button(TextFormat("+ Add Frame##%s", id), ImVec2(120, 25))) {
                    std::string p = OpenImageDialog();
                    if (!p.empty()) {
                        paths.push_back(p);
                        texs.push_back(LoadTexture(p.c_str()));
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(TextFormat("Clear##%s", id), ImVec2(60, 25))) {
                    for (auto& t : texs) UnloadTexture(t);
                    texs.clear();
                    paths.clear();
                }
                
                if (!texs.empty()) {
                    ImGui::Spacing();
                    for (size_t i = 0; i < texs.size(); i++) {
                        ImGui::Image((void*)(intptr_t)texs[i].id, ImVec2(40, 40));
                        ImGui::SameLine();
                    }
                    ImGui::NewLine();
                }
                ImGui::Separator();
            };

            ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "Vertical Movement (W & S)");
            RenderDirectionRow("Down Walk (Front View) - 'S'", "Down", state.draftWalkDownPaths, state.draftWalkDownTex);
            RenderDirectionRow("Up Walk (Back View) - 'W'", "Up", state.draftWalkUpPaths, state.draftWalkUpTex);
            
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "Horizontal Movement (A & D)");
            RenderDirectionRow("Left Walk (Side Profile) - 'A'", "Left", state.draftWalkLeftPaths, state.draftWalkLeftTex);
            RenderDirectionRow("Right Walk (Side Profile) - 'D'", "Right", state.draftWalkRightPaths, state.draftWalkRightTex);

            ImGui::Spacing();
            if (ImGui::Button("Save & Close", ImVec2(140, 30))) {
                state.isWalkConfigured = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(140, 30))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        // --- 4. CUSTOM ANIMATION SEQUENCE BUILDER ---
        ImGui::TextDisabled("4. Custom Action Sequences");
        ImGui::InputText("Action Name", state.inputSeqName, IM_ARRAYSIZE(state.inputSeqName));
        ImGui::SliderInt("Target FPS", &state.inputFps, 1, 60);
        
        ImGui::Text("Activation Trigger:");
        if (state.isListeningForKey) {
            ImGui::Button("[ Press Any Key Now... ]", ImVec2(-1, 25));
            int key = GetKeyPressed();
            if (key != 0) { 
                state.inputMappedKey = key;
                state.isListeningForKey = false;
            }
        } else {
            std::string btnText = state.inputMappedKey == 0 ? "[ Click to Map Key ]" : "[ Mapped Keycode: " + std::to_string(state.inputMappedKey) + " ]";
            if (ImGui::Button(btnText.c_str(), ImVec2(-1, 25))) {
                state.isListeningForKey = true;
            }
            if (state.inputMappedKey != 0) {
                if (ImGui::Button("Clear Keybind", ImVec2(-1, 20))) state.inputMappedKey = 0;
            }
        }

        if (ImGui::Button("+ Add Frame to Sequence", ImVec2(-1, 25))) {
            std::string path = OpenImageDialog();
            if (!path.empty()) {
                state.draftFramePaths.push_back(path);
                state.draftFrames.push_back(LoadTexture(path.c_str()));
                logger.AddLog("[Sequence] Frame injected into draft buffer.");
            }
        }

        if (!state.draftFrames.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Frame Previews:");
            float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            for (int i = 0; i < (int)state.draftFrames.size(); i++) {
                ImGui::Image((void*)(intptr_t)state.draftFrames[i].id, ImVec2(45, 45));
                float lastThumbnailX2 = ImGui::GetItemRectMax().x;
                float nextThumbnailX2 = lastThumbnailX2 + ImGui::GetStyle().ItemSpacing.x + 45; 
                if (i + 1 < (int)state.draftFrames.size() && nextThumbnailX2 < windowVisibleX2) ImGui::SameLine();
            }
            if (ImGui::Button("Clear Draft Frames", ImVec2(-1, 20))) {
                for (auto& tex : state.draftFrames) UnloadTexture(tex);
                state.draftFrames.clear(); state.draftFramePaths.clear();
            }
        }

        ImGui::Spacing();
        
        if (ImGui::Button("SAVE SEQUENCE TO CHARACTER", ImVec2(-1, 30))) {
            if (!state.draftFrames.empty()) {
                AnimationSequence seq;
                std::strcpy(seq.name, state.inputSeqName);
                seq.fps = state.inputFps;
                seq.mappedKey = state.inputMappedKey;
                seq.framePaths = state.draftFramePaths;
                seq.frames = state.draftFrames;
                
                state.draftSequences.push_back(seq);
                state.draftFramePaths.clear(); state.draftFrames.clear();
                logger.AddLog("[Sequence] Custom action saved into draft character.");
            }
        }

        if (!state.draftSequences.empty()) {
            ImGui::Text("Active Sequences:");
            if (ImGui::BeginListBox("##SeqList", ImVec2(-1, 60))) {
                for (int i = 0; i < (int)state.draftSequences.size(); i++) {
                    if (ImGui::Selectable(state.draftSequences[i].name, state.selectedDraftSeqIndex == i)) {
                        state.selectedDraftSeqIndex = i;
                    }
                }
                ImGui::EndListBox();
            }
            if (state.selectedDraftSeqIndex >= 0 && state.selectedDraftSeqIndex < (int)state.draftSequences.size()) {
                if (ImGui::Button("Delete Selected Sequence", ImVec2(-1, 20))) {
                    for(auto& t : state.draftSequences[state.selectedDraftSeqIndex].frames) UnloadTexture(t);
                    state.draftSequences.erase(state.draftSequences.begin() + state.selectedDraftSeqIndex);
                    state.selectedDraftSeqIndex = -1;
                }
            }
        }
        ImGui::Separator();

        // --- 5. FULL PROFILE SAVING ---
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
        if (ImGui::Button("SAVE/UPDATE FULL PROFILE", ImVec2(-1, 40))) {
            if (!state.draftFrames.empty()) {
                AnimationSequence autoSeq;
                std::strcpy(autoSeq.name, state.inputSeqName);
                autoSeq.fps = state.inputFps;
                autoSeq.mappedKey = state.inputMappedKey;
                autoSeq.framePaths = state.draftFramePaths;
                autoSeq.frames = state.draftFrames;
                state.draftSequences.push_back(autoSeq);
                
                state.draftFramePaths.clear(); state.draftFrames.clear();
                logger.AddLog("[System] Auto-packaged orphaned frames into profile.");
            }

            CharacterProfile p;
            std::strcpy(p.name, state.inputName);
            p.frontPath = state.draftFrontPath;
            p.backPath = state.draftBackPath;
            p.frontSprite = state.draftFrontTex;
            p.backSprite = state.draftBackTex;
            p.sequences = state.draftSequences;
            
            p.walkCycle.isConfigured = state.isWalkConfigured;
            p.walkCycle.fps = state.draftWalkFps;
            if (state.isWalkConfigured) {
                for(auto& path : state.draftWalkUpPaths) { p.walkCycle.upPaths.push_back(path); p.walkCycle.upFrames.push_back(LoadTexture(path.c_str())); }
                for(auto& path : state.draftWalkDownPaths) { p.walkCycle.downPaths.push_back(path); p.walkCycle.downFrames.push_back(LoadTexture(path.c_str())); }
                for(auto& path : state.draftWalkLeftPaths) { p.walkCycle.leftPaths.push_back(path); p.walkCycle.leftFrames.push_back(LoadTexture(path.c_str())); }
                for(auto& path : state.draftWalkRightPaths) { p.walkCycle.rightPaths.push_back(path); p.walkCycle.rightFrames.push_back(LoadTexture(path.c_str())); }
            }

            int targetIndex = -1;
            for (int i = 0; i < (int)state.database.size(); i++) {
                if (std::string(state.database[i].name) == std::string(state.inputName)) { targetIndex = i; break; }
            }

            if (targetIndex != -1) {
                state.database[targetIndex] = p; 
                logger.AddLog("[Database] Updated existing profile: " + std::string(p.name));
            } else {
                state.database.push_back(p);
                logger.AddLog("[Database] Created new profile: " + std::string(p.name));
            }
        }
        ImGui::PopStyleColor();
        ImGui::Separator();

        // --- 6. SAVED DATABASE REGISTRY ---
        ImGui::TextDisabled("6. Saved Character Database");
        if (ImGui::BeginListBox("##CharList", ImVec2(-1, 80))) {
            for (int i = 0; i < (int)state.database.size(); i++) {
                if (ImGui::Selectable(state.database[i].name, state.selectedDatabaseIndex == i)) {
                    state.selectedDatabaseIndex = i;
                }
            }
            ImGui::EndListBox();
        }
        
        if (state.selectedDatabaseIndex >= 0 && state.selectedDatabaseIndex < (int)state.database.size()) {
            if (ImGui::Button("Edit", ImVec2(50, 25))) {
                auto& p = state.database[state.selectedDatabaseIndex];
                std::strcpy(state.inputName, p.name);
                
                state.draftFrontPath = p.frontPath;
                if(!p.frontPath.empty()) state.draftFrontTex = LoadTexture(p.frontPath.c_str());
                state.draftBackPath = p.backPath;
                if(!p.backPath.empty()) state.draftBackTex = LoadTexture(p.backPath.c_str());
                
                state.draftSequences.clear();
                for(auto& seq : p.sequences) {
                    AnimationSequence newSeq;
                    std::strcpy(newSeq.name, seq.name);
                    newSeq.fps = seq.fps;
                    newSeq.mappedKey = seq.mappedKey;
                    newSeq.framePaths = seq.framePaths;
                    for(auto& path : seq.framePaths) newSeq.frames.push_back(LoadTexture(path.c_str()));
                    state.draftSequences.push_back(newSeq);
                }
                
                state.isWalkConfigured = p.walkCycle.isConfigured;
                state.draftWalkFps = p.walkCycle.fps;
                
                state.draftWalkUpTex.clear(); state.draftWalkUpPaths.clear();
                state.draftWalkDownTex.clear(); state.draftWalkDownPaths.clear();
                state.draftWalkLeftTex.clear(); state.draftWalkLeftPaths.clear();
                state.draftWalkRightTex.clear(); state.draftWalkRightPaths.clear();

                if (p.walkCycle.isConfigured) {
                    for(auto& path : p.walkCycle.upPaths) { state.draftWalkUpPaths.push_back(path); state.draftWalkUpTex.push_back(LoadTexture(path.c_str())); }
                    for(auto& path : p.walkCycle.downPaths) { state.draftWalkDownPaths.push_back(path); state.draftWalkDownTex.push_back(LoadTexture(path.c_str())); }
                    for(auto& path : p.walkCycle.leftPaths) { state.draftWalkLeftPaths.push_back(path); state.draftWalkLeftTex.push_back(LoadTexture(path.c_str())); }
                    for(auto& path : p.walkCycle.rightPaths) { state.draftWalkRightPaths.push_back(path); state.draftWalkRightTex.push_back(LoadTexture(path.c_str())); }
                }
                logger.AddLog("[Database] Loaded profile into editor.");
            }
            ImGui::SameLine();
            
            if (ImGui::Button("Delete", ImVec2(60, 25))) {
                auto& p = state.database[state.selectedDatabaseIndex];
                if (p.frontSprite.id != 0) UnloadTexture(p.frontSprite);
                if (p.backSprite.id != 0) UnloadTexture(p.backSprite);
                for (auto& seq : p.sequences) for (auto& tex : seq.frames) UnloadTexture(tex);
                
                for (auto& tex : p.walkCycle.upFrames) UnloadTexture(tex);
                for (auto& tex : p.walkCycle.downFrames) UnloadTexture(tex);
                for (auto& tex : p.walkCycle.leftFrames) UnloadTexture(tex);
                for (auto& tex : p.walkCycle.rightFrames) UnloadTexture(tex);
                
                state.database.erase(state.database.begin() + state.selectedDatabaseIndex);
                state.selectedDatabaseIndex = -1;
                logger.AddLog("[Database] Deleted profile.");
            }
            ImGui::SameLine();
            
            if (ImGui::Button("Add to Scene", ImVec2(-1, 25))) {
                GameObject newChar;
                newChar.type = 1;
                newChar.profileIndex = state.selectedDatabaseIndex; 
                newChar.position = {0, 0.0f, 0};
                newChar.scale = {2, 2, 2};
                newChar.startFrame = 0;
                newChar.endFrame = 1800;
                sceneObjects.push_back(newChar);
                logger.AddLog("[Scene] Spawned character node into viewport.");
            }
        }
    }
    ImGui::End();
}

inline void UnloadCharacterImporter(CharacterImporterState& state) {
    if (state.draftFrontTex.id != 0) UnloadTexture(state.draftFrontTex);
    if (state.draftBackTex.id != 0) UnloadTexture(state.draftBackTex);
    
    for(auto& tex : state.draftWalkUpTex) UnloadTexture(tex);
    for(auto& tex : state.draftWalkDownTex) UnloadTexture(tex);
    for(auto& tex : state.draftWalkLeftTex) UnloadTexture(tex);
    for(auto& tex : state.draftWalkRightTex) UnloadTexture(tex);
    
    for (auto& tex : state.draftFrames) if (tex.id != 0) UnloadTexture(tex);
    for (auto& seq : state.draftSequences) for (auto& tex : seq.frames) if (tex.id != 0) UnloadTexture(tex);
    
    for (auto& profile : state.database) {
        if (profile.frontSprite.id != 0) UnloadTexture(profile.frontSprite);
        if (profile.backSprite.id != 0) UnloadTexture(profile.backSprite);
        
        for (auto& tex : profile.walkCycle.upFrames) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.downFrames) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.leftFrames) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.rightFrames) UnloadTexture(tex);
        
        for (auto& seq : profile.sequences) {
            for (auto& tex : seq.frames) if (tex.id != 0) UnloadTexture(tex);
        }
    }
}