#pragma once
#include <vector>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstring>
#include "include/raylib.h"
#include "include/imgui/imgui.h"
#include "NexusLogger.hpp"
#include "NexusStorage.hpp" 

extern std::string OpenImageDialog();

struct WorldState {
    bool showUI = false;
    Texture2D groundTex = {0}; 
    Model groundModel = {0}; 
    
    int islandRadius = 30;       
    
    // --- AUTO-NAMING REGISTRY STATES ---
    char worldNameInput[64] = "World_01"; 
    bool hasGeneratedFloor = false;
    std::string registeredWorldName = "None";
    std::string activeFloorMethodType = "None";

    float GetTerrainHeight(Vector3 worldPos) {
        return 0.0f; // Absolute flat floor baseline for our 3D navigation isolation test
    }

    void ClearWorld(LoggerState& logger) {
        if (groundModel.meshes != NULL) {
            UnloadModel(groundModel);
            groundModel = {0};
            hasGeneratedFloor = false;
            registeredWorldName = "None";
            activeFloorMethodType = "None";
            logger.AddLog("[World] Active floor geometry released from VRAM cache.");
        }
    }

    void GenerateWorld(LoggerState& logger, std::vector<GameObject>& sceneObjects) {
        ClearWorld(logger); 
        
        float gSize = (float)islandRadius * 20.0f;

        if (std::strlen(worldNameInput) == 0) {
            std::strcpy(worldNameInput, "Untitled_World");
        }
        registeredWorldName = std::string(worldNameInput);

        // THE FUSED METHOD: Pure Heightmap Baseplate Geometry
        Image baselineNoise = GenImagePerlinNoise((int)gSize / 10, (int)gSize / 10, 0, 0, 2.0f);
        Mesh gMesh = GenMeshHeightmap(baselineNoise, (Vector3){ gSize, 0.0f, gSize }); 
        UnloadImage(baselineNoise); 
        
        groundModel = LoadModelFromMesh(gMesh);
        
        // THE FUSED BIND: Apply the custom texture directly to the new 3D grid if one is loaded
        if (groundTex.id != 0) {
            groundModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = groundTex;
        }

        hasGeneratedFloor = true;
        activeFloorMethodType = "Textured Heightmap Baseplate";
        logger.AddLog("[World] Compiled '" + registeredWorldName + "' via Fused Heightmap.");

        for (auto& obj : sceneObjects) {
            if (obj.type == 1) { 
                obj.position = (Vector3){ 0.0f, 2.0f, 0.0f }; 
                break;
            }
        }
    }
};

inline void DrawWorldWindow(WorldState& world, float topMenuH, LoggerState& logger, std::vector<GameObject>& sceneObjects) {
    if (!world.showUI) return;

    ImGui::SetNextWindowPos(ImVec2(360, topMenuH), ImGuiCond_FirstUseEver); 
    ImGui::SetNextWindowSize(ImVec2(380, 420), ImGuiCond_FirstUseEver); 

    if (ImGui::Begin("Procedural World Engine", &world.showUI, ImGuiWindowFlags_NoCollapse)) {

        ImGui::TextDisabled("1. Environment Identity & Scale");
        ImGui::InputText("World Identity Tag", world.worldNameInput, IM_ARRAYSIZE(world.worldNameInput));
        ImGui::SliderInt("Island Scale Footprint", &world.islandRadius, 15, 60);

        ImGui::Separator();
        
        ImGui::TextDisabled("2. Architecture Configuration");
        
        // FUSED PIPELINE: Persistent texture uploader for the heightmap baseplate
        if (ImGui::Button("Upload Custom Texture", ImVec2(-1, 30))) { 
            std::string p = OpenImageDialog(); 
            if(!p.empty()) { 
                if(world.groundTex.id != 0) UnloadTexture(world.groundTex); 
                world.groundTex = LoadTexture(p.c_str()); 
                if(world.groundModel.meshes != NULL) world.groundModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = world.groundTex;
                logger.AddLog("[Asset] Custom texture bound to VRAM.");
            } 
        }

        ImGui::Separator();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("COMPILE FLOOR GEOMETRY", ImVec2(-1, 40))) {
            world.GenerateWorld(logger, sceneObjects);
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("CLEAR FLOOR VRAM", ImVec2(-1, 30))) {
            world.ClearWorld(logger);
        }
        ImGui::PopStyleColor();

        ImGui::Separator();
        
        ImGui::TextDisabled("3. Active Generation Registry");
        if (world.hasGeneratedFloor) {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "[Loaded] Name: %s", world.registeredWorldName.c_str());
            ImGui::Text("         Pipeline: %s", world.activeFloorMethodType.c_str());
            ImGui::Text("         Vertices: %d", world.groundModel.meshes[0].vertexCount);
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[ Registry State: VRAM Clear ]");
        }
    }
    ImGui::End();
}

inline void UnloadWorld(WorldState& world) {
    if (world.groundTex.id != 0) UnloadTexture(world.groundTex);
    if (world.groundModel.meshes != NULL) UnloadModel(world.groundModel); 
}