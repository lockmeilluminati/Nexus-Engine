#include "NexusStorage.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void SaveProject(const std::vector<GameObject>& sceneObjects, const std::vector<CharacterProfile>& database, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return;

    // Bumped to V5 to support the new 4-Way dynamic frame walk cycles
    file << "NEXUS_PROJECT_V5\n";
    
    file << "DB_COUNT " << database.size() << "\n";
    for (const auto& profile : database) {
        file << "PROFILE " << profile.name << "\n";
        file << "PATHS " << (profile.frontPath.empty() ? "NONE" : profile.frontPath) << " " 
             << (profile.backPath.empty() ? "NONE" : profile.backPath) << "\n";
        
        // --- SECURE SERIALIZATION FOR 4-WAY DIRECTIONAL WALK ---
        file << "WALK " << profile.walkCycle.isConfigured << " " << profile.walkCycle.fps << " " 
             << profile.walkCycle.upPaths.size() << " " << profile.walkCycle.downPaths.size() << " "
             << profile.walkCycle.leftPaths.size() << " " << profile.walkCycle.rightPaths.size() << "\n";
             
        if (profile.walkCycle.isConfigured) {
            for (const auto& p : profile.walkCycle.upPaths) file << "W_UP " << p << "\n";
            for (const auto& p : profile.walkCycle.downPaths) file << "W_DOWN " << p << "\n";
            for (const auto& p : profile.walkCycle.leftPaths) file << "W_LEFT " << p << "\n";
            for (const auto& p : profile.walkCycle.rightPaths) file << "W_RIGHT " << p << "\n";
        }

        file << "SEQ_COUNT " << profile.sequences.size() << "\n";
        for (const auto& seq : profile.sequences) {
            file << "SEQ " << seq.name << " " << seq.fps << " " << seq.mappedKey << " " << seq.framePaths.size() << "\n";
            for (const auto& p : seq.framePaths) {
                file << "FRAME " << p << "\n"; 
            }
        }
    }

    file << "OBJ_COUNT " << sceneObjects.size() << "\n";
    for (const auto& obj : sceneObjects) {
        file << "OBJ " << obj.type << " " << obj.profileIndex << " " 
             << obj.position.x << " " << obj.position.y << " " << obj.position.z << " " 
             << obj.hasRecording << "\n";
             
        if (obj.hasRecording) {
            for (int i = 0; i <= 1800; i++) {
                file << obj.bakedPositions[i].x << " " << obj.bakedPositions[i].y << " " << obj.bakedPositions[i].z << " ";
            }
            file << "\n";
        }
    }
    file.close();
}

void LoadProject(std::vector<GameObject>& sceneObjects, std::vector<CharacterProfile>& database, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string header;
    file >> header;
    if (header != "NEXUS_PROJECT_V5") return;

    for (auto& profile : database) {
        if (profile.frontSprite.id != 0) UnloadTexture(profile.frontSprite);
        if (profile.backSprite.id != 0) UnloadTexture(profile.backSprite);
        
        // Clear 4-Way Walk memory securely
        for (auto& tex : profile.walkCycle.upFrames) if (tex.id != 0) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.downFrames) if (tex.id != 0) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.leftFrames) if (tex.id != 0) UnloadTexture(tex);
        for (auto& tex : profile.walkCycle.rightFrames) if (tex.id != 0) UnloadTexture(tex);
        
        for (auto& seq : profile.sequences) {
            for (auto& tex : seq.frames) if (tex.id != 0) UnloadTexture(tex);
        }
    }
    database.clear();
    sceneObjects.clear();

    std::string token;
    
    file >> token; 
    int dbCount = 0;
    file >> dbCount;
    
    for (int i = 0; i < dbCount; i++) {
        CharacterProfile p;
        file >> token >> p.name; 
        
        std::string fPath, bPath;
        file >> token >> fPath >> bPath; 
        
        if (fPath != "NONE") { p.frontPath = fPath; p.frontSprite = LoadTexture(fPath.c_str()); }
        if (bPath != "NONE") { p.backPath = bPath; p.backSprite = LoadTexture(bPath.c_str()); }
        
        // --- RECONSTRUCT 4-WAY DIRECTIONAL WALK CYCLE ---
        int isWalkConf = 0, walkFps = 12, upCount = 0, downCount = 0, leftCount = 0, rightCount = 0;
        file >> token >> isWalkConf >> walkFps >> upCount >> downCount >> leftCount >> rightCount; 
        
        p.walkCycle.isConfigured = (isWalkConf != 0);
        p.walkCycle.fps = walkFps;

        if (p.walkCycle.isConfigured) {
            auto loadDirection = [&](int count, std::vector<std::string>& paths, std::vector<Texture2D>& frames) {
                for (int f = 0; f < count; f++) {
                    std::string path;
                    file >> token >> path; 
                    paths.push_back(path);
                    frames.push_back(LoadTexture(path.c_str()));
                }
            };
            
            loadDirection(upCount, p.walkCycle.upPaths, p.walkCycle.upFrames);
            loadDirection(downCount, p.walkCycle.downPaths, p.walkCycle.downFrames);
            loadDirection(leftCount, p.walkCycle.leftPaths, p.walkCycle.leftFrames);
            loadDirection(rightCount, p.walkCycle.rightPaths, p.walkCycle.rightFrames);
        }
        
        int seqCount = 0;
        file >> token >> seqCount; 
        
        for (int s = 0; s < seqCount; s++) {
            AnimationSequence seq;
            int frameCount = 0;
            file >> token >> seq.name >> seq.fps >> seq.mappedKey >> frameCount; 
            
            for (int f = 0; f < frameCount; f++) {
                std::string framePath;
                file >> token >> framePath; 
                seq.framePaths.push_back(framePath);
                seq.frames.push_back(LoadTexture(framePath.c_str())); 
            }
            p.sequences.push_back(seq);
        }
        database.push_back(p);
    }

    file >> token; 
    int objCount = 0;
    file >> objCount;
    
    for (int i = 0; i < objCount; i++) {
        GameObject obj;
        file >> token >> obj.type >> obj.profileIndex 
             >> obj.position.x >> obj.position.y >> obj.position.z 
             >> obj.hasRecording; 
             
        if (obj.hasRecording) {
            for (int f = 0; f <= 1800; f++) {
                file >> obj.bakedPositions[f].x >> obj.bakedPositions[f].y >> obj.bakedPositions[f].z;
            }
        }
        sceneObjects.push_back(obj);
    }
    file.close();
}