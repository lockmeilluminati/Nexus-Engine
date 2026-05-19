<div align="center">

# 🌌 Nexus Engine
**An Interactive 2D/3D C++ Game Engine Powered by Raylib & ImGui**

[![Language](https://shields.io)](https://cppreference.com)
[![Graphics Backend](https://shields.io)](https://raylib.com)
[![UI Framework](https://shields.io)](https://github.com)
[![Status](https://shields.io)]()

<br />

<!-- 🎬 THE PLAYABLE VIDEO EMBED FIX -->
[![Nexus Game Engine Update](https://youtube.com)](https://www.youtube.com/watch?v=Ysq5wOBsQrw)

<br />

</div>

## 🚀 About Nexus
**Nexus** is a fast, lightweight, and highly interactive game engine built on top of **Raylib** and **Dear ImGui**. The engine is designed to bridge the gap between rapid scene prototyping and real-time visualization. With its built-in character controllers, runtime storage abstractions, and cinematic sequencing modules, Nexus allows you to go from a blank canvas to a working sandbox environment in under 5 minutes.

---

## ✨ Core Features & Tech Stack

### 🎨 Rendering & UI Layout
*   **Raylib Integration:** Leverages raw hardware acceleration via OpenGL for fast 2D and 3D graphics rendering.
*   **Advanced Editor Tooling:** Integrated with `rlImGui` and `ImGuizmo` to provide 3D transformation gizmos for translating, rotating, and scaling objects natively in the editor workspace.

### 🛠️ Modular Systems
*   **Character Handling (`NexusCharacter.hpp`):** Out-of-the-box character abstraction for quick gameplay setup and testing.
*   **Environment Generation (`NexusCity.hpp`):** Specialized modules tailored for spawning urban layouts, buildings, and procedural grids.
*   **Dynamic Data Core (`NexusStorage.hpp`):** Centralised scene serialization and asset routing architecture.
*   **Scene Management (`NexusSceneManager.hpp`):** Fast state-switching pipelines to cleanly transition between editor mode and runtime execution.

### 🎬 Cinematic Timeline Control
*   **Sequencer Integration (`ImSequencer`):** Native animation tracking and track sequencing directly inside the editor layout.
*   **Timeline Trackers (`NexusTimeline.hpp`):** Designed for managing keyframes, asset triggers, and experimental movie-making utilities.

---

## 🗺️ Development Roadmap
We are transforming Nexus from an interactive renderer into a full-scale ecosystem. Here is the active plan:

### 🌍 World & Scene Generation (High Priority)
- [x] Fast player and scene initialization
- [x] ImGui / ImGuizmo scene integration
- [ ] Comprehensive World Editing tools (for dynamic object and item placement)
- [ ] Procedural biomes (Mountains, Oceans, and custom mesh layouts)
- [ ] Structure population tools (Trees, dynamic buildings)

### 🏎️ Gameplay Mechanics
- [ ] Drivable vehicle physics loops (Cars)
- [ ] Cross-platform multiplayer networking architecture

### 🎥 Editor & Cinematics
- [x] Base timeline sequencer implementation
- [ ] Advanced movie-making runtime rendering pipeline

---

## 📦 File Structures Overview
A quick reference to the project codebase structure:
*   `/include` — Core header controls.
*   `main.cpp` — Engine entry point and window orchestration loop.
*   `project.nexus` — Default project sandbox metadata configuration.
*   `WindowsDialogs.cpp` — Native OS window interactions for resource file picking.

---

## 🔧 Getting Started

### Cloning the Workspace
Be sure to pull recursively to guarantee all engine dependencies download properly:
```bash
git clone --recursive https://github.com
```

---

## 🤝 Contributing
If you enjoy **#vibecoding**, tooling setups, or low-level graphics optimization, contributions to the world generators and asset pipelines are always appreciated!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Open a Pull Request

---

<div align="center">
  <sub>Built with Raylib & ImGui by lockmeilluminati. 🚀</sub>
</div>
