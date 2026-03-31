#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <stdlib.h>

using namespace std;

// --- Data Structures ---
enum SortAlgo { BUBBLE, SELECTION, INSERTION };

struct Bar {
    int value;
    float currentX;
    float targetX;
    bool isConfirmedSorted;
    bool isVictoryGreen;
};

struct Visualizer {
    vector<Bar> bars;
    int n = 50;
    int i = 0, j = 0;
    bool active = false;
    bool isFinished = false;
    int sweepIndex = -1;
    float displaySpeed = 15.0f;
    float timer = 0.0f;
    SortAlgo currentAlgo = BUBBLE;
    bool dropdownEditMode = false;
    int dropdownActive = 0;
    int prevDropdownActive = 0;
};

// --- Logic Helpers ---

void ResetSortState(Visualizer& v) {
    v.active = false;
    v.isFinished = false;
    v.sweepIndex = -1;
    v.timer = 0.0f;
    v.i = (v.currentAlgo == INSERTION) ? 1 : 0;
    v.j = 0;
    for (auto& bar : v.bars) {
        bar.isConfirmedSorted = false;
        bar.isVictoryGreen = false;
    }
}

float GetXPos(int index, int n, int width) { return index * ((float)width / n); }

void SyncPositions(Visualizer& v, int width) {
    for (int k = 0; k < (int)v.bars.size(); k++) v.bars[k].targetX = GetXPos(k, v.n, width);
}

void GenerateArray(Visualizer& v, int width) {
    v.bars.clear();
    vector<int> values;
    for (int i = 0; i < v.n; i++) values.push_back(GetRandomValue(10, 450));
    shuffle(values.begin(), values.end(), random_device());
    for (int k = 0; k < v.n; k++) {
        float x = GetXPos(k, v.n, width);
        v.bars.push_back({ values[k], x, x, false, false });
    }
    ResetSortState(v);
}

void DrawHighlightedCode(Font font, const char* code, int posX, int posY) {
    int lineOffset = 0; int charOffset = 0;
    float fontSize = 20.0f; float spacing = 1.5f;
    string word = "";
    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '\n') { lineOffset += (int)(fontSize + 6); charOffset = 0; continue; }
        if (isalnum(code[i]) || code[i] == '_') { word += code[i]; }
        else {
            Color color = { 210, 210, 210, 255 };
            if (word == "if" || word == "for" || word == "while" || word == "int") color = { 86, 156, 214, 255 };
            else if (word == "std" || word == "swap" || word == "data") color = { 78, 201, 176, 255 };
            else if (!word.empty() && isdigit(word[0])) color = { 181, 206, 168, 255 };
            if (!word.empty()) {
                DrawTextEx(font, word.c_str(), { (float)posX + charOffset, (float)posY + lineOffset }, fontSize, spacing, color);
                charOffset += (int)MeasureTextEx(font, word.c_str(), fontSize, spacing).x;
                word = "";
            }
            char symbol[2] = { code[i], '\0' };
            DrawTextEx(font, symbol, { (float)posX + charOffset, (float)posY + lineOffset }, fontSize, spacing, { 110, 110, 110, 255 });
            charOffset += (int)MeasureTextEx(font, symbol, fontSize, spacing).x;
        }
    }
}

int main() {
    const int sw = 1280; const int sh = 720;
    InitWindow(sw, sh, "Sorting Visualizer - Replay Mode");
    InitAudioDevice(); SetTargetFPS(144);

    Font consolas = LoadFontEx("C:\\Windows\\Fonts\\consola.ttf", 32, 0, 250);
    Visualizer vis;
    GenerateArray(vis, sw / 2);

    const char* bubbleCode = "if (data[j] > data[j + 1]) {\n    swap(data[j], data[j + 1]);\n}\nj++;";
    const char* selectionCode = "int min_idx = i;\nfor (int k = i + 1; k < n; k++) {\n    if (data[k] < data[min_idx])\n        min_idx = k;\n}\nswap(data[min_idx], data[i]);\ni++;";
    const char* insertionCode = "int val = data[i];\nint k = i - 1;\nwhile (k >= 0 && data[k] > val) {\n    data[k + 1] = data[k];\n    k--;\n}\ndata[k + 1] = val;\ni++;";

    float sliderN = (float)vis.n;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        for (auto& bar : vis.bars) bar.currentX += (bar.targetX - bar.currentX) * 15.0f * dt;

        // --- Sorting Logic ---
        if (vis.active && !vis.isFinished) {
            vis.timer += dt * ((vis.displaySpeed / 25.0f) * 100);
            if (vis.timer >= 1.0f) {
                vis.timer = 0.0f;
                if (vis.currentAlgo == BUBBLE && vis.i < (int)vis.bars.size()) {
                    if (vis.j < (int)vis.bars.size() - vis.i - 1) {
                        if (vis.bars[vis.j].value > vis.bars[vis.j + 1].value) {
                            swap(vis.bars[vis.j], vis.bars[vis.j + 1]);
                            SyncPositions(vis, sw / 2);
                        }
                        vis.j++;
                    }
                    else { vis.bars[vis.bars.size() - vis.i - 1].isConfirmedSorted = true; vis.j = 0; vis.i++; }
                }
                else if (vis.currentAlgo == SELECTION && vis.i < (int)vis.bars.size() - 1) {
                    int m = vis.i;
                    for (int k = vis.i + 1; k < (int)vis.bars.size(); k++) if (vis.bars[k].value < vis.bars[m].value) m = k;
                    swap(vis.bars[m], vis.bars[vis.i]);
                    vis.bars[vis.i].isConfirmedSorted = true;
                    SyncPositions(vis, sw / 2); vis.i++;
                }
                else if (vis.currentAlgo == INSERTION && vis.i < (int)vis.bars.size()) {
                    Bar key = vis.bars[vis.i]; int k = vis.i - 1;
                    while (k >= 0 && vis.bars[k].value > key.value) { vis.bars[k + 1] = vis.bars[k]; k--; }
                    vis.bars[k + 1] = key; SyncPositions(vis, sw / 2);
                    for (int idx = 0; idx <= vis.i; idx++) vis.bars[idx].isConfirmedSorted = true;
                    vis.i++;
                }
                else { vis.isFinished = true; vis.sweepIndex = 0; vis.active = false; }
            }
        }

        // --- Victory Sweep ---
        if (vis.isFinished && vis.sweepIndex < (int)vis.bars.size() && vis.sweepIndex != -1) {
            vis.timer += dt * vis.displaySpeed * 20.0f;
            if (vis.timer >= 1.0f) {
                vis.timer = 0.0f;
                vis.bars[vis.sweepIndex].isVictoryGreen = true;
                vis.sweepIndex++;
            }
        }

        BeginDrawing();
        ClearBackground({ 30, 30, 30, 255 });
        float leftWidth = sw / 2.0f;
        DrawRectangle(0, 0, (int)leftWidth, sh, { 15, 15, 15, 255 });

        float barW = leftWidth / vis.bars.size();
        for (int k = 0; k < (int)vis.bars.size(); k++) {
            float h = ((float)vis.bars[k].value / 500.0f) * (sh - 300);
            Color c = GRAY;
            if (vis.bars[k].isVictoryGreen) c = LIME;
            else if (vis.active && k == vis.j) c = RED;
            else if (vis.bars[k].isConfirmedSorted) c = SKYBLUE;
            DrawRectangleRec({ vis.bars[k].currentX, sh - h, barW - 1, h }, c);
        }

        DrawRectangle(0, 0, (int)leftWidth, 240, { 45, 45, 48, 255 });

        if (GuiButton({ 20, 20, 200, 40 }, "GENERATE NEW")) {
            vis.n = (int)sliderN;
            GenerateArray(vis, sw / 2);
        }

        // --- DYNAMIC START/PAUSE/REPLAY BUTTON ---
        const char* btnLabel = "START SORT";
        if (vis.isFinished) btnLabel = "REPLAY SORT";
        else if (vis.active) btnLabel = "PAUSE SORT";

        if (GuiButton({ 230, 20, 200, 40 }, btnLabel)) {
            if (vis.isFinished) {
                ResetSortState(vis); // Reset indices but keep same bars
                vis.active = true;
            }
            else {
                vis.active = !vis.active;
            }
        }

        GuiSlider({ 80, 80, 300, 30 }, "SIZE", TextFormat("%d", (int)sliderN), &sliderN, 10, 300);
        GuiSlider({ 80, 130, 300, 30 }, "SPEED", TextFormat("%dx", (int)vis.displaySpeed), &vis.displaySpeed, 1, 50);

        if (GuiDropdownBox({ 20, 180, 400, 40 }, "BUBBLE;SELECTION;INSERTION", &vis.dropdownActive, vis.dropdownEditMode)) {
            vis.dropdownEditMode = !vis.dropdownEditMode;
        }

        if (vis.dropdownActive != vis.prevDropdownActive) {
            vis.currentAlgo = (SortAlgo)vis.dropdownActive;
            vis.prevDropdownActive = vis.dropdownActive;
            ResetSortState(vis); // Switch algo without reshuffling
        }

        DrawRectangle((int)leftWidth, 0, (int)leftWidth, sh, { 30, 30, 30, 255 });
        DrawTextEx(consolas, "SOURCE CODE PREVIEW", { leftWidth + 20, 20 }, 24, 2, GOLD);
        DrawLine((int)leftWidth + 20, 50, sw - 20, 50, DARKGRAY);
        const char* code = (vis.currentAlgo == BUBBLE) ? bubbleCode : (vis.currentAlgo == SELECTION ? selectionCode : insertionCode);
        DrawHighlightedCode(consolas, code, (int)leftWidth + 30, 80);

        EndDrawing();
    }
    UnloadFont(consolas);
    CloseAudioDevice(); CloseWindow();
    return 0;
}