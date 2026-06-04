# Computer-Generated-Imagery Project

## Space Explorer

An interactive space simulation/game developed in C++ with OpenGL, demonstrating fundamental concepts of image synthesis and computer graphics.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Technical Concepts](#technical-concepts)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Compilation](#compilation)
- [Controls](#controls)
- [Project Structure](#project-structure)
- [Shader Implementations](#shader-implementations)
- [Dependencies](#dependencies)
- [Author](#author)

---

## Overview

Space Explorer is an interactive space scene where you pilot a spaceship through an asteroid field with a gas giant planet in the background. The project demonstrates practical application of rendering concepts in computer graphics.

---

## Features

- Spaceship control with keyboard input (rotation and movement)
- Particle system for engine exhaust trail
- Textured planet with ring and atmospheric effect
- Animated asteroids with autonomous movement and edge bouncing
- Starfield background with twinkling stars
- Advanced visual effects including transparency, texture blending, and glow halos

---

## Technical Concepts

| Concept | Implementation |
|---------|----------------|
| Window Management | GLFW for window creation and input handling |
| OpenGL Initialization | GLAD for loading OpenGL functions |
| Rendering Pipeline | Vertex and fragment shaders for GPU-based rendering |
| Geometry Management | VAO, VBO, EBO for efficient mesh storage |
| Texture Mapping | UV coordinates, mipmaps, sampler2D uniforms |
| Texture Blending | Mixing two textures with adjustable ratio |
| Color Modulation | Multiplying texture color with vertex color |
| Transformations | Translation, rotation, scaling using GLM matrices |
| Transparency | Alpha blending with GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA |

---

## Prerequisites

- Windows / Linux / macOS
- CMake (>= 3.10) or manual compilation
- C++17 compatible compiler (Visual Studio 2022+, GCC 11+, Clang 14+)
- OpenGL 3.3 or higher

---

## Installation

### 1. Clone the repository

```bash
git clone https://github.com/NourBen0/Computer-Generated-Imagery.git
cd Computer-Generated-Imagery
