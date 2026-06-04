# 🚀 Projet Final - Synthèse d'Images

## 🌌 Space Explorer

Un jeu/visualisation spatiale interactive développé en C++ avec OpenGL, intégrant les concepts fondamentaux de la synthèse d'images (TP0 à TP5).

---

## 📋 Table des matières

- [Aperçu](#aperçu)
- [Fonctionnalités](#fonctionnalités)
- [Concepts techniques abordés](#concepts-techniques-abordés)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Compilation](#compilation)
- [Contrôles](#contrôles)
- [Structure du projet](#structure-du-projet)
- [Démonstration des concepts](#démonstration-des-concepts)
- [Dépendances](#dépendances)
- [Auteurs](#auteurs)

---

## 🎮 Aperçu

**Space Explorer** est une scène spatiale interactive où vous pilotez un vaisseau à travers un champ d'astéroïdes, avec une planète gazeuse en arrière-plan. Le projet démontre l'application pratique des concepts de rendu 3D/2D appris durant le cours de synthèse d'images.

---

## ✨ Fonctionnalités

- 🚀 **Contrôle du vaisseau** - Rotation et déplacement avec les touches du clavier
- 💨 **Système de particules** - Effet de traînée réaliste du moteur
- 🌍 **Planète texturée** - Avec anneau et effet atmosphérique
- ☄️ **Astéroïdes animés** - Mouvement autonome et rebonds sur les bords
- ⭐ **Fond étoilé** - Étoiles scintillantes + fond texturé
- 🎨 **Effets visuels avancés** - Transparence, mélange de textures, halos lumineux

---

## 🎓 Concepts techniques abordés

| TP | Concept | Implémentation |
|----|---------|----------------|
| **TP0** | GLFW + GLAD | Initialisation de la fenêtre et chargement des fonctions OpenGL |
| **TP1** | Fenêtre, boucle de rendu, input | Gestion clavier, viewport, redimensionnement |
| **TP2** | VAO, VBO, EBO, shaders | Création de meshes, compilation de shaders, glDrawElements |
| **TP4** | Textures, mipmaps, UV | Chargement textures avec stb_image, coordonnées UV, sampler2D |
| **TP5** | Concepts avancés | Mix de textures, superposition texture+couleur, glActiveTexture |

---

## 💻 Prérequis

- **Windows / Linux / macOS**
- **CMake** (≥ 3.10) ou compilation manuelle
- **Compilateur C++17** (Visual Studio 2022+, GCC 11+, Clang 14+)
- **OpenGL 3.3+**

---

## 📦 Installation

### 1. Cloner le dépôt

```bash
git clone https://github.com/NourBen0/Computer-Generated-Imagery.git
cd Computer-Generated-Imagery
