#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "Util.h"

// EXTERNE PROMENLJIVE iz Main.cpp - samo deklaracije!
extern int currentScreen;
extern int hours, minutes, seconds;
extern int bpm;
extern int battery;

extern unsigned int digitTextures[10];
extern unsigned int arrowRightTex, arrowLeftTex, nameTex, ekgTex, warningTex, percentTex, bpmLabelTex, grassTex, watchFrameTex;

extern unsigned int shaderProgram;
extern unsigned int colorShaderProgram;

extern double ekgOffset;
extern double ekgCompressionFactor;

extern bool showWarning;

extern unsigned int VAOrect, VBOrect;
extern unsigned int VAOcolor, VBOcolor;

void preprocessTexture(unsigned int& texture, const char* path) {
    texture = loadImageToTexture(path);
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void preprocessTextureRepeat(unsigned int& texture, const char* path) {
    texture = loadImageToTexture(path);
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void initVAOs() {
    glGenVertexArrays(1, &VAOrect);
    glGenBuffers(1, &VBOrect);

    glBindVertexArray(VAOrect);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrect);

    // Alociramo buffer dovoljne veličine za maksimalni broj verteksa
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAOcolor);
    glGenBuffers(1, &VBOcolor);

    glBindVertexArray(VAOcolor);
    glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);

    // Alociramo buffer dovoljne veličine za maksimalni broj verteksa
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void drawRect(float x, float y, float width, float height, unsigned int texture) {
    glUseProgram(shaderProgram);

    float vertices[] = {
        x, y, 0.0f, 0.0f,
        x + width, y, 1.0f, 0.0f,
        x + width, y + height, 1.0f, 1.0f,
        x, y + height, 0.0f, 1.0f
    };

    glBindVertexArray(VAOrect);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrect);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawEKG(float x, float y, float width, float height, float offset, float scale) {
    glUseProgram(shaderProgram);

    float texRepeat = scale;

    float vertices[] = {
        x, y, offset, 0.0f,
        x + width, y, offset + texRepeat, 0.0f,
        x + width, y + height, offset + texRepeat, 1.0f,
        x, y + height, offset, 1.0f
    };

    glBindVertexArray(VAOrect);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrect);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindTexture(GL_TEXTURE_2D, ekgTex);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawColorRect(float x, float y, float width, float height, float r, float g, float b) {
    glUseProgram(colorShaderProgram);

    float vertices[] = {
        x, y,
        x + width, y,
        x + width, y + height,
        x, y + height
    };

    glBindVertexArray(VAOcolor);
    glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUniform4f(glGetUniformLocation(colorShaderProgram, "uColor"), r, g, b, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawDigit(int digit, float x, float y, float size) {
    if (digit >= 0 && digit <= 9) {
        drawRect(x, y, size, size * 1.5f, digitTextures[digit]);
    }
}

void drawNumber(int number, float x, float y, float size, int digitCount) {
    float spacing = size * 1.1f;
    for (int i = digitCount - 1; i >= 0; i--) {
        int digit = (number / (int)pow(10, i)) % 10;
        drawDigit(digit, x, y, size);
        x += spacing;
    }
}

void drawClockScreen() {
    drawRect(-1.0f, -1.0f, 2.0f, 2.0f, grassTex);
    drawRect(-0.95f, 0.85f, 0.5f, 0.13f, nameTex);

    if (battery == 0) {
        drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.0f, 0.0f, 0.0f);
    }
    else {
        drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.1f, 0.1f, 0.15f);

        // Cenriranje vremena
        float digitSize = 0.12f;
        float digitSpacing = digitSize * 1.1f;
        float colonWidth = 0.015f;
        float colonSpacing = 0.01f;

        float totalWidth = 6 * digitSpacing + 2 * (colonWidth + 2 * colonSpacing);
        float startX = -totalWidth / 2.0f;

        // Sati
        drawNumber(hours, startX, 0.0f, digitSize, 2);
        startX += 2 * digitSpacing;

        // Prva dvotačka
        startX += colonSpacing;
        drawColorRect(startX, 0.1f, colonWidth, 0.03f, 1, 1, 1);
        drawColorRect(startX, 0.02f, colonWidth, 0.03f, 1, 1, 1);
        startX += colonWidth + colonSpacing;

        // Minuti
        drawNumber(minutes, startX, 0.0f, digitSize, 2);
        startX += 2 * digitSpacing;

        // Druga dvotačka
        startX += colonSpacing;
        drawColorRect(startX, 0.1f, colonWidth, 0.03f, 1, 1, 1);
        drawColorRect(startX, 0.02f, colonWidth, 0.03f, 1, 1, 1);
        startX += colonWidth + colonSpacing;

        // Sekunde
        drawNumber(seconds, startX, 0.0f, digitSize, 2);

        drawRect(0.35f, -0.35f, 0.15f, 0.15f, arrowRightTex);
    }

    drawRect(-0.75f, -0.75f, 1.5f, 1.5f, watchFrameTex);
}

void drawHeartScreen() {
    drawRect(-1.0f, -1.0f, 2.0f, 2.0f, grassTex);
    drawRect(-0.95f, 0.85f, 0.5f, 0.13f, nameTex);

    if (battery == 0) {
        drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.0f, 0.0f, 0.0f);
    }
    else {
        if (bpm >= 200 && showWarning) {
            drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 1.0f, 0.0f, 0.0f);

            // ARNING poruka
            drawRect(-0.5f, -0.05f, 1.0f, 0.5f, warningTex);

            
            float digitSize = 0.15f;
            float digitSpacing = digitSize * 1.1f;
            int digitCount = 3;
            float totalWidth = digitSpacing * digitCount;
            float startX = -totalWidth / 2.0f;

            drawNumber(bpm, startX, -0.15f, digitSize, digitCount);

            drawRect(-0.5f, -0.35f, 0.15f, 0.15f, arrowLeftTex);
            drawRect(0.35f, -0.35f, 0.15f, 0.15f, arrowRightTex);
        }
        else {
            drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.15f, 0.05f, 0.05f);
            drawRect(-0.25f, 0.25f, 0.5f, 0.18f, bpmLabelTex);
            drawNumber(bpm, -0.3f, 0.0f, 0.18f, 3);

            float ekgScale = 3.0f + ekgCompressionFactor;
            drawEKG(-0.5f, -0.35f, 1.0f, 0.2f, ekgOffset, ekgScale);

            drawRect(-0.5f, -0.35f, 0.15f, 0.15f, arrowLeftTex);
            drawRect(0.35f, -0.35f, 0.15f, 0.15f, arrowRightTex);
        }
    }

    drawRect(-0.75f, -0.75f, 1.5f, 1.5f, watchFrameTex);
}

void drawBatteryScreen() {
    drawRect(-1.0f, -1.0f, 2.0f, 2.0f, grassTex);
    drawRect(-0.95f, 0.85f, 0.5f, 0.13f, nameTex);

    if (battery == 0) {
        drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.0f, 0.0f, 0.0f);
    }
    else {
        drawColorRect(-0.48f, -0.52f, 0.98f, 0.99f, 0.05f, 0.1f, 0.15f);

        // Ista veličina kao BPM na heart ekranu
        float digitSize = 0.18f;
        float digitSpacing = digitSize * 1.1f;
        float percentSize = 0.20f;
        float percentWidth = percentSize * 0.8f;  

        if (battery == 100) {
            // 3 cifre + procenat
            float totalWidth = 3 * digitSpacing + percentWidth;
            float startX = -totalWidth / 2.0f;
            drawNumber(battery, startX, 0.15f, digitSize, 3);
            drawRect(startX + 3 * digitSpacing, 0.15f, percentWidth, digitSize * 1.5f, percentTex);
        }
        else if (battery >= 10) {
            // 2 cifre + procenat
            float totalWidth = 2 * digitSpacing + percentWidth;
            float startX = -totalWidth / 2.0f;
            drawNumber(battery, startX, 0.15f, digitSize, 2);
            drawRect(startX + 2 * digitSpacing, 0.15f, percentWidth, digitSize * 1.5f, percentTex);
        }
        else {
            // 1 cifra + procenat
            float totalWidth = digitSpacing + percentWidth;
            float startX = -totalWidth / 2.0f;
            drawNumber(battery, startX, 0.15f, digitSize, 1);
            drawRect(startX + digitSpacing, 0.15f, percentWidth, digitSize * 1.5f, percentTex);
        }

        float batteryWidth = 0.7f;
        float batteryHeight = 0.25f;
        float batteryX = -0.35f;
        float batteryY = -0.25f;

        // Okvir baterije
        drawColorRect(batteryX, batteryY, batteryWidth, 0.02f, 1.0f, 1.0f, 1.0f);
        drawColorRect(batteryX, batteryY + batteryHeight, batteryWidth, 0.02f, 1.0f, 1.0f, 1.0f);
        drawColorRect(batteryX, batteryY, 0.02f, batteryHeight, 1.0f, 1.0f, 1.0f);
        drawColorRect(batteryX + batteryWidth, batteryY, 0.02f, batteryHeight, 1.0f, 1.0f, 1.0f);

        // Punjenje zalepljeno DESNO
        float fillWidth = (batteryWidth - 0.08f) * (battery / 100.0f);
        float fillX = batteryX + batteryWidth - 0.04f - fillWidth;

        float r, g, b;
        if (battery > 20) {
            r = 0.0f; g = 1.0f; b = 0.0f;
        }
        else if (battery > 10) {
            r = 1.0f; g = 1.0f; b = 0.0f;
        }
        else {
            r = 1.0f; g = 0.0f; b = 0.0f;
        }
        drawColorRect(fillX, batteryY + 0.04f, fillWidth, batteryHeight - 0.08f, r, g, b);

        drawRect(-0.5f, -0.35f, 0.15f, 0.15f, arrowLeftTex);
    }

    drawRect(-0.75f, -0.75f, 1.5f, 1.5f, watchFrameTex);
}