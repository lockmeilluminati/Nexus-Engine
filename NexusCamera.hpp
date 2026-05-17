#pragma once
#include <vector>
#include <math.h>
#include "include/raylib.h"
#include "include/raymath.h"
#include "NexusStorage.hpp" 

struct CameraController {
    // --- 3RD PERSON RPG DEFAULT MATRICES ---
    float angleX = 1.57f;   // 90 degrees: Places the camera strictly behind the character's spawn rotation
    float angleY = 0.25f;   // Lowered pitch: Brings the camera down closer to the floor level
    float distance = 12.0f; // Tightened distance: Pulls the lens right up behind the skeleton
    Vector3 focusTarget = { 0.0f, 0.0f, 0.0f };

    void Update(Camera3D& camera, const std::vector<GameObject>& sceneObjects, bool isCursorHidden) {
        if (isCursorHidden) {
            Vector2 mouseDelta = GetMouseDelta();
            angleX -= mouseDelta.x * 0.005f;
            angleY += mouseDelta.y * 0.005f;
            
            // Allow the camera to dip much closer to the floor without locking up
            if (angleY > 1.4f) angleY = 1.4f;
            if (angleY < 0.05f) angleY = 0.05f; 
        }

        // Zoom limits tightened for a more cinematic tracking scale
        distance -= GetMouseWheelMove() * 2.0f;
        if (distance < 3.0f) distance = 3.0f;     
        if (distance > 150.0f) distance = 150.0f; 

        bool characterFound = false;
        for (const auto& obj : sceneObjects) {
            if (obj.type == 1) { 
                float distToTarget = Vector3Distance(focusTarget, obj.position);
                
                // Camera Deadzone: Smooth pan if the skeleton moves more than 1.0 unit from the center
                if (distToTarget > 1.0f) {
                    focusTarget = Vector3Lerp(focusTarget, obj.position, 0.08f); // Slightly faster catch-up for live movement
                }
                characterFound = true;
                break; 
            }
        }

        if (characterFound) {
            // Anchor the tracking lens perfectly to the dummy's torso (Y + 1.5f) to match the new sprite offset
            camera.target = Vector3Add(focusTarget, (Vector3){0, 1.5f, 0}); 
            
            // Orbital calculations wrapping around the new lower torso anchor
            camera.position.x = focusTarget.x + (distance * cosf(angleY) * cosf(angleX));
            camera.position.y = focusTarget.y + (distance * sinf(angleY)) + 1.5f; 
            camera.position.z = focusTarget.z + (distance * cosf(angleY) * sinf(angleX));
        } else {
            camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
            camera.position.x = distance * cosf(angleY) * cosf(angleX);
            camera.position.y = distance * sinf(angleY);
            camera.position.z = distance * cosf(angleY) * sinf(angleX);
        }
    }
};