#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

// Ekrani
#define SCREEN_CLOCK 0
#define SCREEN_HEART 1
#define SCREEN_BATTERY 2

// EKSTERNE PROMENLJIVE iz Main.cpp - samo deklaracije!
extern int currentScreen;
extern int hours, minutes, seconds;
extern int bpm;
extern int battery;
extern bool isDKeyPressed;

extern GLFWcursor* heartCursor;
extern GLFWcursor* heartCursorSmall;
extern GLFWcursor* heartRunning1;
extern GLFWcursor* heartRunning2;
extern double lastClickTime;

extern double lastTimeUpdate;
extern double lastBatteryUpdate;
extern double ekgOffset;
extern double lastBpmChange;
extern double ekgCompressionFactor;
extern double lastRunningAnimationTime;
extern bool runningAnimationFrame;

extern double warningStartTime;
extern bool showWarning;
extern bool warningCycleActive;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            isDKeyPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            isDKeyPressed = false;
            if (heartCursor) {  
                glfwSetCursor(window, heartCursor);
            }
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetCursor(window, heartCursorSmall);
        lastClickTime = glfwGetTime();

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float normalizedX = (xpos / width) * 2.0f - 1.0f;
        float normalizedY = -((ypos / height) * 2.0f - 1.0f);

        if (normalizedX > 0.35f && normalizedX < 0.5f && normalizedY > -0.35f && normalizedY < -0.2f) {
            if (currentScreen == SCREEN_CLOCK) {
                currentScreen = SCREEN_HEART;
            }
            else if (currentScreen == SCREEN_HEART) {
                currentScreen = SCREEN_BATTERY;
            }
        }

        if (normalizedX > -0.5f && normalizedX < -0.35f && normalizedY > -0.35f && normalizedY < -0.2f) {
            if (currentScreen == SCREEN_HEART) {
                currentScreen = SCREEN_CLOCK;
            }
            else if (currentScreen == SCREEN_BATTERY) {
                currentScreen = SCREEN_HEART;
            }
        }
    }
}
void updateTimers(GLFWwindow* window) {
    double currentTime = glfwGetTime();

    if (currentTime - lastTimeUpdate >= 1.0) {
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) {
                    hours = 0;
                }
            }
        }
        lastTimeUpdate = currentTime;
    }
    if (currentTime - lastBpmChange >= 0.45) {
        if (isDKeyPressed) {
            if (bpm < 220) bpm += 5;
        }
        else {
            if (bpm > 80) {
                bpm -= 3;  
            }
            else if (bpm < 60) {
                bpm += 2;  
            }
            else {
                int change = rand() % 3 - 1;
                bpm += change;
                if (bpm < 60) bpm = 60;
                if (bpm > 80 && !isDKeyPressed) bpm = 80;
            }
        }
        lastBpmChange = currentTime;
    }
    if (isDKeyPressed) {
        ekgOffset -= 0.005f + (ekgCompressionFactor * 0.005f);
        if (ekgCompressionFactor < 3.0f) {
            ekgCompressionFactor += 0.003f;
        }
    }
    else {
        ekgOffset -= 0.003f;
        if (ekgCompressionFactor > 0.0f) {
            ekgCompressionFactor -= 0.002f;
        }
        if (ekgCompressionFactor < 0.0f) ekgCompressionFactor = 0.0f;
    }
    if (ekgOffset < -1.0f) ekgOffset = 0.0f;

    if (bpm >= 200 && currentScreen == SCREEN_HEART) {
        if (!warningCycleActive) {
            warningCycleActive = true;
            showWarning = true;
            warningStartTime = currentTime;
        }
        else {
            double elapsed = currentTime - warningStartTime;
            if (elapsed >= 3.0) {
                showWarning = !showWarning;
                warningStartTime = currentTime;
            }
        }
    }
    else {
        warningCycleActive = false;
        showWarning = false;
    }

    if (isDKeyPressed) {
        if (currentTime - lastRunningAnimationTime >= 0.15) {
            runningAnimationFrame = !runningAnimationFrame;
            if (runningAnimationFrame) {
                if (heartRunning1) glfwSetCursor(window, heartRunning1);
            }
            else {
                if (heartRunning2) glfwSetCursor(window, heartRunning2);
            }
            lastRunningAnimationTime = currentTime;
        }
    }
    if (currentTime - lastBatteryUpdate >= 10.0) {  
        if (battery > 0) battery--;
        lastBatteryUpdate = currentTime;
    }

    if (currentTime - lastClickTime > 0.1 && lastClickTime > 0.0) {
        if (!isDKeyPressed && heartCursor) {  
            glfwSetCursor(window, heartCursor);
        }
        lastClickTime = 0.0;
    }
}