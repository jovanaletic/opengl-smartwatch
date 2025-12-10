// ============================================
// Main.cpp - Glavni fajl sa main funkcijom i globalnim promenljivama
// ============================================
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "Util.h"

// Ekrani
#define SCREEN_CLOCK 0
#define SCREEN_HEART 1
#define SCREEN_BATTERY 2

// Deklaracije funkcija iz drugih fajlova
void preprocessTexture(unsigned int& texture, const char* path);
void preprocessTextureRepeat(unsigned int& texture, const char* path);
void initVAOs();
void drawClockScreen();
void drawHeartScreen();
void drawBatteryScreen();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void updateTimers(GLFWwindow* window);

// GLOBALNE PROMENLJIVE
int currentScreen = SCREEN_CLOCK;
int hours = 0, minutes = 0, seconds = 0;
int bpm = 70;
int battery = 100;
bool isDKeyPressed = false;

unsigned int digitTextures[10];
unsigned int arrowRightTex, arrowLeftTex, nameTex, ekgTex, warningTex, percentTex, bpmLabelTex, grassTex, watchFrameTex;
GLFWcursor* heartCursor;
GLFWcursor* heartCursorSmall;
GLFWcursor* heartRunning1;
GLFWcursor* heartRunning2;
double lastClickTime = 0.0;

unsigned int shaderProgram;
unsigned int colorShaderProgram;

double lastTimeUpdate = 0.0;
double lastBatteryUpdate = 0.0;
double ekgOffset = 0.0;
double lastBpmChange = 0.0;
double ekgCompressionFactor = 0.0;
double lastRunningAnimationTime = 0.0;
bool runningAnimationFrame = false;

double warningStartTime = 0.0;
bool showWarning = false;
bool warningCycleActive = false;

unsigned int VAOrect, VBOrect;
unsigned int VAOcolor, VBOcolor;

int main() {
    srand((unsigned int)time(NULL));
    bpm = 60 + rand() % 21;

    time_t now = time(0);
    tm* ltm = localtime(&now);
    hours = ltm->tm_hour;
    minutes = ltm->tm_min;
    seconds = ltm->tm_sec;

    if (!glfwInit()) {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Smartwatch", monitor, NULL);

    if (window == NULL) {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    shaderProgram = createShader("basic.vert", "basic.frag");
    colorShaderProgram = createShader("color.vert", "color.frag");

    initVAOs();

    char path[256];
    for (int i = 0; i < 10; i++) {
        snprintf(path, sizeof(path), "Resources/digit%d.png", i);
        preprocessTexture(digitTextures[i], path);
    }
    preprocessTexture(arrowRightTex, "Resources/arrow_right.png");
    preprocessTexture(arrowLeftTex, "Resources/arrow_left.png");
    preprocessTexture(nameTex, "Resources/name.png");
    preprocessTextureRepeat(ekgTex, "Resources/ekg.png");
    preprocessTexture(warningTex, "Resources/warning.png");
    preprocessTexture(percentTex, "Resources/percent.png");
    preprocessTexture(bpmLabelTex, "Resources/bpm_label.png");
    preprocessTexture(grassTex, "Resources/grass.jpeg");
    preprocessTexture(watchFrameTex, "Resources/watch_frame.png");

    heartCursor = loadImageToCursor("Resources/heart.png");
    heartCursorSmall = loadImageToCursor("Resources/heart_small.png");
    heartRunning1 = loadImageToCursor("Resources/heart_running1.png");
    heartRunning2 = loadImageToCursor("Resources/heart_running2.png");
    if (heartCursor) {
        glfwSetCursor(window, heartCursor);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        double initFrameTime = glfwGetTime();

        updateTimers(window);

        glClear(GL_COLOR_BUFFER_BIT);

        if (currentScreen == SCREEN_CLOCK) {
            drawClockScreen();
        }
        else if (currentScreen == SCREEN_HEART) {
            drawHeartScreen();
        }
        else if (currentScreen == SCREEN_BATTERY) {
            drawBatteryScreen();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - initFrameTime < 1.0 / 75.0) {}
    }
    if (heartCursor) glfwDestroyCursor(heartCursor);
    if (heartCursorSmall) glfwDestroyCursor(heartCursorSmall);
    if (heartRunning1) glfwDestroyCursor(heartRunning1);
    if (heartRunning2) glfwDestroyCursor(heartRunning2);

    for (int i = 0; i < 10; i++) {
        glDeleteTextures(1, &digitTextures[i]);
    }
    glDeleteTextures(1, &arrowRightTex);
    glDeleteTextures(1, &arrowLeftTex);
    glDeleteTextures(1, &nameTex);
    glDeleteTextures(1, &ekgTex);
    glDeleteTextures(1, &warningTex);
    glDeleteTextures(1, &percentTex);
    glDeleteTextures(1, &bpmLabelTex);
    glDeleteTextures(1, &grassTex);
    glDeleteTextures(1, &watchFrameTex);

    glDeleteBuffers(1, &VBOrect);
    glDeleteVertexArrays(1, &VAOrect);
    glDeleteBuffers(1, &VBOcolor);
    glDeleteVertexArrays(1, &VAOcolor);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(colorShaderProgram);

    glfwTerminate();
    return 0;
}