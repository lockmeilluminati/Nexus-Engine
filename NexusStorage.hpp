#pragma once
#include <vector>
#include <string>
#include "include/raylib.h"

// --- CORE DATA STRUCTURES ---

// UPGRADED: 4-Way Directional Walk Cycle Memory
struct WalkCycle {
    bool isConfigured = false;
    int fps = 12;
    std::vector<std::string> upPaths; 
    std::vector<Texture2D> upFrames;  
    std::vector<std::string> downPaths;  
    std::vector<Texture2D> downFrames;
    std::vector<std::string> leftPaths; 
    std::vector<Texture2D> leftFrames;  
    std::vector<std::string> rightPaths;  
    std::vector<Texture2D> rightFrames;   
};

struct AnimationSequence {
    char name[64] = "New_Sequence";
    int fps = 12;
    int mappedKey = 0; 
    
    std::vector<std::string> framePaths; 
    std::vector<Texture2D> frames;       
};

struct CharacterProfile {
    char name[64] = "Hero_01";
    std::string frontPath = "";
    std::string backPath = "";
    Texture2D frontSprite = {0};
    Texture2D backSprite = {0};
    
    WalkCycle walkCycle; 
    std::vector<AnimationSequence> sequences; 
};

struct GameObject {
    int type = 0; 
    Vector3 position = {0, 0, 0};
    Vector3 scale = {2, 2, 2};
    float velocityY = 0.0f;
    
    // UPGRADED: 0 = Up (W), 1 = Down (S), 2 = Left (A), 3 = Right (D)
    int facingDirection = 1; 
    
    int activeAnimation = -1; 
    int profileIndex = -1;
    
    int startFrame = 0;
    int endFrame = 1800;
    
    bool hasRecording = false;
    Vector3 bakedPositions[1805]; 
};

// --- FUNCTION DECLARATIONS ---
std::string OpenSaveDialog();
std::string OpenLoadDialog();
std::string OpenImageDialog();

void SaveProject(const std::vector<GameObject>& sceneObjects, const std::vector<CharacterProfile>& database, const std::string& filepath);
void LoadProject(std::vector<GameObject>& sceneObjects, std::vector<CharacterProfile>& database, const std::string& filepath);