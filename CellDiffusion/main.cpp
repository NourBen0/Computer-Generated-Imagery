// ============================================================
// Projet Synthèse d'images — TP0 à TP5
// Deux objets texturés : Planète (TP5 N1) + Météorite (TP5 N2)
// ============================================================
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* w, int width, int height) { glViewport(0, 0, width, height); }

const unsigned int W = 800, H = 600;
const float ASPECT = (float)W / H;

// TP2 : Shader couleur unie
const char* vCol = R"(#version 330 core
layout(location=0) in vec2 aPos; uniform mat4 T;
void main(){ gl_Position = T * vec4(aPos,0,1); })";
const char* fCol = R"(#version 330 core
out vec4 C; uniform vec3 color; uniform float alpha;
void main(){ C = vec4(color, alpha); })";

// TP4 : Shader une texture
const char* vTex = R"(#version 330 core
layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
out vec2 UV; uniform mat4 T;
void main(){ gl_Position = T * vec4(aPos,0,1); UV = aUV; })";

// TP5 Notion 1 : texture * couleur
const char* fTexCol = R"(#version 330 core
in vec2 UV; out vec4 C;
uniform sampler2D tex; uniform vec3 ourColor; uniform float alpha;
void main(){ C = texture(tex,UV) * vec4(ourColor,1.0); C.a *= alpha; })";

// TP5 Notion 2 : mix() deux textures
const char* fTexMix = R"(#version 330 core
in vec2 UV; out vec4 C;
uniform sampler2D texture1; uniform sampler2D texture2;
uniform float mixRatio; uniform float alpha;
void main(){ C = mix(texture(texture1,UV), texture(texture2,UV), mixRatio); C.a *= alpha; })";

// ============================================================
// Fonctions utilitaires
// ============================================================
unsigned int makeShader(const char* v, const char* f) {
    int ok; char log[512];
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &v, NULL); glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(vs, 512, NULL, log); std::cout << log; }
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &f, NULL); glCompileShader(fs);
    unsigned int p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p);
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}

// TP4 : chargement texture avec stb_image + mipmaps
unsigned int loadTex(const char* path) {
    unsigned int id; glGenTextures(1, &id); glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int w, h, n; stbi_set_flip_vertically_on_load(true); // TP5
    unsigned char* d = stbi_load(path, &w, &h, &n, 0);
    if (d) {
        GLenum fmt = n == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, d);
        glGenerateMipmap(GL_TEXTURE_2D); // TP4
        std::cout << "Texture: " << path << " (" << w << "x" << h << ")\n";
    }
    else { std::cout << "ERREUR texture: " << path << "\n"; glDeleteTextures(1, &id); return 0; }
    stbi_image_free(d); return id;
}

// TP2 : créer VAO/VBO/EBO avec UV (stride 4 floats : x,y,u,v)
unsigned int makeMesh(const std::vector<float>& v, const std::vector<unsigned int>& idx) {
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * 4, v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * 4, idx.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return VAO;
}

// Générer cercle avec UV (pour planète et météorite)
void makeCircle(std::vector<float>& v, std::vector<unsigned int>& idx, int seg, float irregularity = 0.0f, unsigned seed = 0) {
    srand(seed);
    v.push_back(0); v.push_back(0); v.push_back(0.5f); v.push_back(0.5f); // centre
    for (int i = 0;i <= seg;i++) {
        float a = 2.f * 3.14159f * i / seg;
        float r = 1.0f - irregularity * ((rand() % 100) / 100.f);
        float x = r * cos(a), y = r * sin(a);
        v.push_back(x); v.push_back(y);
        v.push_back(x * .5f + .5f); v.push_back(y * .5f + .5f);
    }
    for (int i = 1;i <= seg;i++) { idx.push_back(0); idx.push_back(i); idx.push_back(i + 1); }
}

// Dessiner un objet texturé avec transformation
void draw(unsigned int shader, unsigned int VAO, int n, glm::vec2 pos, float angle, glm::vec2 scale) {
    glm::mat4 t = glm::mat4(1);
    t = glm::translate(t, glm::vec3(pos, 0));
    t = glm::rotate(t, glm::radians(angle), glm::vec3(0, 0, 1));
    t = glm::scale(t, glm::vec3(scale, 1));
    glUniformMatrix4fv(glGetUniformLocation(shader, "T"), 1, GL_FALSE, glm::value_ptr(t));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
}

// ============================================================
// MAIN
// ============================================================
int main() {
    // TP0 : Init GLFW + GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(W, H, "Synthese - Planete + Meteorite", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H); // TP1 : viewport
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TP2 : Compilation des shaders
    unsigned int sColor = makeShader(vCol, fCol);
    unsigned int sTexCol = makeShader(vTex, fTexCol);  // TP5 N1
    unsigned int sTexMix = makeShader(vTex, fTexMix);  // TP5 N2

    // Assigner les unités AVANT la boucle (TP5 N2)
    glUseProgram(sTexMix);
    glUniform1i(glGetUniformLocation(sTexMix, "texture1"), 0); // GL_TEXTURE0
    glUniform1i(glGetUniformLocation(sTexMix, "texture2"), 1); // GL_TEXTURE1

    // TP4 : Chargement des textures
    unsigned int texPlanet = loadTex("textures/planet_texture.jpg");
    unsigned int texRock1 = loadTex("textures/asteroid_texture.jpg");
    unsigned int texRock2 = loadTex("textures/asteroid_texture2.jpg");

    // TP2 : Créer les meshes
    std::vector<float> pV; std::vector<unsigned int> pI;
    makeCircle(pV, pI, 64);
    unsigned int planetVAO = makeMesh(pV, pI);

    // Météorite 1 — forme irrégulière seed 42
    std::vector<float> aV; std::vector<unsigned int> aI;
    makeCircle(aV, aI, 12, 0.35f, 42);
    unsigned int asteroidVAO = makeMesh(aV, aI);

    // Météorite 2 — forme différente seed 99, plus petite
    std::vector<float> a2V; std::vector<unsigned int> a2I;
    makeCircle(a2V, a2I, 10, 0.45f, 99);
    unsigned int asteroid2VAO = makeMesh(a2V, a2I);

    // État animation météorite 1
    glm::vec2 astPos(-0.7f, -0.5f);
    glm::vec2 astVel(0.50f, 0.38f);
    float astAngle = 0.f;

    // État animation météorite 2 — trajectoire opposée
    glm::vec2 ast2Pos(0.7f, 0.6f);
    glm::vec2 ast2Vel(-0.35f, -0.45f);
    float ast2Angle = 0.f;

    float lastFrame = 0.f;

    // TP1 : Boucle de rendu
    while (!glfwWindowShouldClose(win)) {
        float now = (float)glfwGetTime();
        float dt = now - lastFrame; lastFrame = now;

        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        // Mise à jour météorite 1
        astPos += astVel * dt;
        astAngle += 60.f * dt;
        if (astPos.x > 0.85f || astPos.x < -0.85f) astVel.x = -astVel.x;
        if (astPos.y > 0.85f || astPos.y < -0.85f) astVel.y = -astVel.y;
        astPos.x = glm::clamp(astPos.x, -0.85f, 0.85f);
        astPos.y = glm::clamp(astPos.y, -0.85f, 0.85f);

        // Mise à jour météorite 2 (rotation inverse, vitesse différente)
        ast2Pos += ast2Vel * dt;
        ast2Angle -= 45.f * dt;
        if (ast2Pos.x > 0.85f || ast2Pos.x < -0.85f) ast2Vel.x = -ast2Vel.x;
        if (ast2Pos.y > 0.85f || ast2Pos.y < -0.85f) ast2Vel.y = -ast2Vel.y;
        ast2Pos.x = glm::clamp(ast2Pos.x, -0.85f, 0.85f);
        ast2Pos.y = glm::clamp(ast2Pos.y, -0.85f, 0.85f);

        // --- Rendu ---
        glClearColor(0.f, 0.f, 0.05f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TP5 Notion 1 : PLANÈTE = texture × couleur verte
        glUseProgram(sTexCol);
        glUniform3f(glGetUniformLocation(sTexCol, "ourColor"), 0.2f, 1.0f, 0.3f);
        glUniform1f(glGetUniformLocation(sTexCol, "alpha"), 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texPlanet);
        draw(sTexCol, planetVAO, (int)pI.size(),
            { -0.30f, 0.15f }, now * 15.f, { 0.40f / ASPECT, 0.40f });

        // TP5 Notion 2 : MÉTÉORITE 1 = mix(texRock1, texRock2, 0.3)
        glUseProgram(sTexMix);
        glUniform1f(glGetUniformLocation(sTexMix, "mixRatio"), 0.3f);
        glUniform1f(glGetUniformLocation(sTexMix, "alpha"), 1.0f);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texRock1);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texRock2);
        draw(sTexMix, asteroidVAO, (int)aI.size(),
            astPos, astAngle, { 0.18f / ASPECT, 0.18f });

        // TP5 Notion 2 : MÉTÉORITE 2 = mix(texRock1, texRock2, 0.6) — ratio différent
        glUniform1f(glGetUniformLocation(sTexMix, "mixRatio"), 0.6f);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texRock1);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texRock2);
        draw(sTexMix, asteroid2VAO, (int)a2I.size(),
            ast2Pos, ast2Angle, { 0.13f / ASPECT, 0.13f });

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}