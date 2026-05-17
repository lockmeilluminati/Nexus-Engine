#include <stddef.h>
#include <math.h>     
#include <vector>     
#include <iostream>
#include <string>
#include "include/raylib.h"
#include "include/rlImGui.h"
#include "include/imgui/imgui.h"
#include "include/raymath.h"

#include "NexusStorage.hpp"   
#include "NexusTimeline.hpp"  
#include "NexusLogger.hpp"       
#include "NexusCharacter.hpp" 
#include "NexusCity.hpp" 
#include "NexusCamera.hpp"
#include "NexusSceneManager.hpp" 

int main(void) {
    InitWindow(1280, 720, "NEXUS ENGINE v17.0 | 4-WAY DIRECTIONAL MOVEMENT");
    SetWindowState(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    CameraController camController;
    rlImGuiSetup(true);

    std::vector<GameObject> sceneObjects;
    CharacterImporterState importer; 
    WorldState world; 
    LoggerState logger; 
    SceneManagerState sceneManager; 

    int engineState = 0; 
    bool exitRequested = false; 
    int currentFrame = 0, endlessTick = 0;
    bool expanded = true; int selectedEntry = -1; int firstFrame = 0;

    DisableCursor(); 
    logger.AddLog("[System] Nexus Engine v17.0 Active.");
    logger.AddLog("[System] 4-Way physics matrix online.");

    while (!WindowShouldClose() && !exitRequested) {
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        camController.Update(camera, sceneObjects, IsCursorHidden());
        
        float cameraFloor = world.GetTerrainHeight(camera.position) + 0.5f; 
        if (camera.position.y < cameraFloor) camera.position.y = cameraFloor;

        for (auto& obj : sceneObjects) {
            if (obj.type == 1 && obj.profileIndex == -1 && !importer.database.empty()) {
                obj.profileIndex = 0; 
            }
        }

        auto executePhysics = [&](GameObject& obj) {
            float floorHeight = world.GetTerrainHeight(obj.position); 
            obj.position.y += obj.velocityY;
            
            if (obj.position.y > floorHeight) {
                obj.velocityY -= 0.02f; 
            } else { 
                obj.position.y = floorHeight; 
                obj.velocityY = 0.0f; 
            }

            Vector3 fwd = Vector3Normalize(Vector3Subtract(camera.target, camera.position)); fwd.y = 0;
            Vector3 rgt = { -fwd.z, 0, fwd.x };
            float speedModifier = IsKeyDown(KEY_LEFT_SHIFT) ? 0.45f : 0.18f;
            bool isMoving = false;
            
            // --- 4-WAY DIRECTIONAL INPUT CAPTURE ---
            if (IsKeyDown(KEY_W)) { obj.position = Vector3Add(obj.position, Vector3Scale(fwd, speedModifier)); obj.facingDirection = 0; isMoving = true; } // Up/Back
            if (IsKeyDown(KEY_S)) { obj.position = Vector3Subtract(obj.position, Vector3Scale(fwd, speedModifier)); obj.facingDirection = 1; isMoving = true; } // Down/Front
            if (IsKeyDown(KEY_A)) { obj.position = Vector3Subtract(obj.position, Vector3Scale(rgt, speedModifier)); obj.facingDirection = 2; isMoving = true; } // Left
            if (IsKeyDown(KEY_D)) { obj.position = Vector3Add(obj.position, Vector3Scale(rgt, speedModifier)); obj.facingDirection = 3; isMoving = true; } // Right
            
            if (IsKeyPressed(KEY_SPACE) && obj.position.y <= floorHeight + 0.01f) obj.velocityY = 0.48f; 
            
            if (obj.profileIndex >= 0 && obj.profileIndex < (int)importer.database.size()) {
                auto& p = importer.database[obj.profileIndex];
                int customOverrideIndex = -1;
                
                // Priority 1: Custom Sequences (e.g. dancing/attacking)
                for (int i = 0; i < (int)p.sequences.size(); i++) {
                    if (p.sequences[i].mappedKey != 0 && IsKeyDown(p.sequences[i].mappedKey)) {
                        customOverrideIndex = i;
                    }
                }

                // Animation State Hierarchy
                if (customOverrideIndex != -1) {
                    obj.activeAnimation = customOverrideIndex; // 0+
                } else if (isMoving && p.walkCycle.isConfigured) {
                    obj.activeAnimation = -2; // Native Walk
                } else {
                    obj.activeAnimation = -1; // Idle Base
                }
            }
        };

        if (engineState == 1) {
            endlessTick++;
            for (auto& obj : sceneObjects) { if (obj.type == 1) executePhysics(obj); }
        } else if (engineState == 2) {
            endlessTick++; currentFrame++; 
            if (currentFrame > 1800) currentFrame = 0;
            for (auto& obj : sceneObjects) { if (obj.type == 1 && obj.hasRecording) obj.position = obj.bakedPositions[currentFrame]; }
        } else if (engineState == 3) {
            endlessTick++; currentFrame++; 
            if (currentFrame > 1800) { engineState = 0; currentFrame = 1800; }
            for (auto& obj : sceneObjects) {
                if (obj.type == 1) { 
                    executePhysics(obj);
                    obj.bakedPositions[currentFrame] = obj.position;
                    obj.hasRecording = true;
                }
            }
        } else {
            for (auto& obj : sceneObjects) {
                if (obj.type == 1 && obj.hasRecording && currentFrame >= 0 && currentFrame <= 1800) {
                    obj.position = obj.bakedPositions[currentFrame];
                }
            }
        }

        BeginDrawing();
            ClearBackground((Color){ 140, 210, 240, 255 }); 
            BeginMode3D(camera);
                
                if (world.groundModel.meshes != NULL) {
                    DrawModel(world.groundModel, (Vector3){0, 0.0f, 0}, 1.0f, WHITE); 
                }

                for (auto& obj : sceneObjects) {
                    if (obj.type == 0) { DrawCube(obj.position, 2, 2, 2, RED); DrawCubeWires(obj.position, 2, 2, 2, MAROON); }
                }
                
                BeginBlendMode(BLEND_ALPHA); 
                for (auto& obj : sceneObjects) {
                    if (obj.type == 1) {
                        bool drew2D = false;
                        
                        if (obj.profileIndex >= 0 && obj.profileIndex < (int)importer.database.size()) {
                            auto& p = importer.database[obj.profileIndex];
                            
                            // Base Idle Sprites based on last known direction (0 = Back view, others = Front view)
                            Texture2D tex = (obj.facingDirection == 0) ? p.backSprite : p.frontSprite;
                            
                            if (obj.activeAnimation >= 0 && obj.activeAnimation < (int)p.sequences.size()) {
                                auto& activeSeq = p.sequences[obj.activeAnimation];
                                if (!activeSeq.frames.empty()) {
                                    int fIdx = (int)(endlessTick * (activeSeq.fps / 60.0f)) % activeSeq.frames.size();
                                    tex = activeSeq.frames[fIdx];
                                }
                            } 
                            // --- 4-WAY WALK RENDERER ---
                            else if (obj.activeAnimation == -2 && p.walkCycle.isConfigured) {
                                std::vector<Texture2D>* animFrames = nullptr;
                                
                                if (obj.facingDirection == 0) animFrames = &p.walkCycle.upFrames;
                                else if (obj.facingDirection == 1) animFrames = &p.walkCycle.downFrames;
                                else if (obj.facingDirection == 2) animFrames = &p.walkCycle.leftFrames;
                                else if (obj.facingDirection == 3) animFrames = &p.walkCycle.rightFrames;
                                
                                if (animFrames && !animFrames->empty()) {
                                    int fIdx = (int)(endlessTick * (p.walkCycle.fps / 60.0f)) % animFrames->size();
                                    tex = (*animFrames)[fIdx];
                                }
                            }

                            if (tex.id != 0) {
                                DrawBillboard(camera, tex, Vector3Add(obj.position, {0, 1.5f, 0}), 3.0f, WHITE);
                                drew2D = true; 
                            }
                        }

                        if (!drew2D) {
                            EndBlendMode(); 
                            DrawCube(Vector3Add(obj.position, {0, 1.5f, 0}), 1.2f, 3.0f, 1.2f, DARKBLUE);
                            BeginBlendMode(BLEND_ALPHA); 
                        } else {
                            DrawCubeWires(Vector3Add(obj.position, {0, 1.5f, 0}), 1.2f, 3.0f, 1.2f, Fade(BLACK, 0.15f));
                        }
                    }
                }
                EndBlendMode();
            EndMode3D();

            if (IsCursorHidden()) {
                DrawLine(GetScreenWidth()/2 - 10, GetScreenHeight()/2, GetScreenWidth()/2 + 10, GetScreenHeight()/2, RED);
                DrawLine(GetScreenWidth()/2, GetScreenHeight()/2 - 10, GetScreenWidth()/2, GetScreenHeight()/2 + 10, RED);
            }

            rlImGuiBegin();
                if (ImGui::BeginMainMenuBar()) {
                    if (ImGui::BeginMenu("File")) {
                        if (ImGui::MenuItem("Save Project As...", "Ctrl+Shift+S")) { std::string p = OpenSaveDialog(); if(!p.empty()) SaveProject(sceneObjects, importer.database, p); }
                        if (ImGui::MenuItem("Load Project...", "Ctrl+O")) { std::string p = OpenLoadDialog(); if(!p.empty()) LoadProject(sceneObjects, importer.database, p); }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Exit Engine", "Alt+F4")) exitRequested = true;
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Tools")) {
                        if (ImGui::MenuItem("Character Importer")) importer.showUI = !importer.showUI; 
                        if (ImGui::MenuItem("Procedural World Gen")) world.showUI = !world.showUI;
                        ImGui::EndMenu();
                    }
                    ImGui::EndMainMenuBar();
                }

                if (importer.showUI) DrawCharacterImporterWindow(importer, sceneObjects, 20, 220, GetScreenHeight(), logger);
                if (world.showUI) DrawWorldWindow(world, 20, logger, sceneObjects); 
                
                DrawLoggerWindow(logger, GetScreenWidth(), GetScreenHeight());
                DrawSceneManagerWindow(sceneManager, sceneObjects, importer.database, engineState, currentFrame, endlessTick, logger);

                ImGui::SetNextWindowPos(ImVec2(0, GetScreenHeight() - 220), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(GetScreenWidth() * 0.7f, 220), ImGuiCond_Always);
                if (ImGui::Begin("Master Internal Level Timeline", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
                    NexusTimelineBridge bridge(sceneObjects);
                    ImSequencer::Sequencer(&bridge, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_CHANGE_FRAME);
                }
                ImGui::End();

            rlImGuiEnd();
            DrawFPS(10, 30);
        EndDrawing();
    }
    
    UnloadCharacterImporter(importer);
    UnloadWorld(world);
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}