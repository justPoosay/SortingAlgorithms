#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <vector>
#include <algorithm>
#include <random>
#include <stdlib.h>

enum SortAlgo { BUBBLE, SELECTION };

struct Visualizer {
    std::vector<int> data;
    int n = 50;
    int m = 100;
    int i = 0, j = 0;
    bool active = false;
    float speed = 1.0f;
    float timer = 0.0f;
    SortAlgo currentAlgo = BUBBLE;
};

void ResetArray(Visualizer& v) {
    v.data.clear();
    v.i = 0; v.j = 0; v.active = false;
    for (int i = 0; i < v.n; i++) {
        v.data.push_back(GetRandomValue(1, v.m));
    }
}

void ShuffleArray(Visualizer& v) {
    std::shuffle(v.data.begin(), v.data.end(), std::random_device());
    v.i = 0; v.j = 0; v.active = false;
}

Sound GenerateBeep() {
    Wave wave = { 0 };
    wave.frameCount = 44100 * 0.05f;
    wave.sampleRate = 44100;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(wave.frameCount * wave.channels * sizeof(short));

    short* samples = (short*)wave.data;
    for (int k = 0; k < wave.frameCount; k++) {
        samples[k] = (k % 40 < 20) ? 4000 : -4000;
    }

    Sound snd = LoadSoundFromWave(wave);
    free(wave.data);
    return snd;
}

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib Sorting Visualizer Pro");
    InitAudioDevice();
    SetTargetFPS(120);

    Visualizer vis;
    ResetArray(vis);
    Sound beep = GenerateBeep();

    while (!WindowShouldClose()) {
        if (vis.active) {
            vis.timer += GetFrameTime() * (vis.speed * 100);

            if (vis.timer >= 1.0f) {
                vis.timer = 0.0f;

                if (vis.currentAlgo == BUBBLE) {
                    if (vis.i < (int)vis.data.size()) {
                        if (vis.j < (int)vis.data.size() - vis.i - 1) {
                            if (vis.data[vis.j] > vis.data[vis.j + 1]) {
                                std::swap(vis.data[vis.j], vis.data[vis.j + 1]);

                                SetSoundPitch(beep, 0.5f + ((float)vis.data[vis.j] / vis.m));
                                PlaySound(beep);
                            }
                            vis.j++;
                        }
                        else { vis.j = 0; vis.i++; }
                    }
                    else { vis.active = false; }
                }
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(0x101012FF));

        float barWidth = (float)screenWidth / vis.n;
        for (int k = 0; k < (int)vis.data.size(); k++) {
            float height = ((float)vis.data[k] / vis.m) * (screenHeight - 160);

            Color barColor = Fade(SKYBLUE, 0.8f);
            if (vis.active && (k == vis.j || k == vis.j + 1)) barColor = RED;
            if (!vis.active && vis.i > 0) barColor = LIME;

            DrawRectangleRec({ k * barWidth, screenHeight - height, barWidth - 1, height }, barColor);
        }

        DrawRectangle(10, 10, 320, 140, Fade(BLACK, 0.6f));
        DrawRectangleLines(10, 10, 320, 140, DARKGRAY);

        if (GuiButton({ 20, 20, 90, 30 }, "GENERATE")) ResetArray(vis);
        if (GuiButton({ 120, 20, 90, 30 }, "SHUFFLE")) ShuffleArray(vis);
        if (GuiButton({ 220, 20, 90, 30 }, vis.active ? "PAUSE" : "START")) vis.active = !vis.active;

        float tempN = (float)vis.n;
        float tempM = (float)vis.m;
        float tempSpeed = vis.speed;

        GuiSlider({ 80, 60, 200, 15 }, "Size N", TextFormat("%d", vis.n), &tempN, 5, 500);
        GuiSlider({ 80, 80, 200, 15 }, "Max M", TextFormat("%d", vis.m), &tempM, 10, 1000);
        GuiSlider({ 80, 100, 200, 15 }, "Speed", TextFormat("%.1fx", vis.speed), &tempSpeed, 0.1f, 5.0f);

        vis.n = (int)tempN;
        vis.m = (int)tempM;
        vis.speed = tempSpeed;

        GuiLabel({ 20, 120, 100, 20 }, "ALGO:");
        if (GuiButton({ 80, 120, 80, 20 }, "BUBBLE")) vis.currentAlgo = BUBBLE;

        EndDrawing();
    }

    UnloadSound(beep);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}