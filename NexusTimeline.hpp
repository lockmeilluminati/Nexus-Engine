#pragma once
#include <vector>
#include <stdio.h>
#include "ImSequencer.h"
#include "NexusStorage.hpp" // Pulls in the GameObject DNA

// The Timeline Bridge: Transforms our sceneObjects vector into tracks for ImSequencer UI
struct NexusTimelineBridge : public ImSequencer::SequenceInterface {
    std::vector<GameObject>& objects;
    
    NexusTimelineBridge(std::vector<GameObject>& sceneObjects) : objects(sceneObjects) {}
    
    int GetFrameMin() const override { return 0; }
    int GetFrameMax() const override { return 1800; }
    int GetItemCount() const override { return (int)objects.size(); }
    int GetItemTypeCount() const override { return 1; }
    const char* GetItemTypeName(int typeIndex) const override { return "Cube Primitive"; }
    
    const char* GetItemLabel(int index) const override {
        static char labelName[32];
        sprintf(labelName, "Cube Node [%d]", index);
        return labelName;
    }
    
    void Get(int index, int** start, int** end, int* type, unsigned int* color) override {
        GameObject& obj = objects[index];
        if (start) *start = &obj.startFrame;
        if (end)   *end = &obj.endFrame;
        if (type)  *type = obj.type;
        if (color) *color = 0xFF4444CC; // Crimson/Red block color tint in hex
    }
    
    void Add(int type) override {}
    
    void Del(int index) override {
        if (index >= 0 && index < objects.size()) {
            objects.erase(objects.begin() + index);
        }
    }
    
    void Duplicate(int index) override {}
    void Copy() override {}
    void Paste() override {}
};