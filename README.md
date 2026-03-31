# SortingAlgorithms: Interactive Algorithm Visualizer

**SortingAlgorithms** is a high-performance, real-time sorting algorithm visualizer built with **C++** and **Raylib**. It features smooth bar animations, dynamic syntax highlighting of the underlying logic, and a "Victory Sweep" effect to confirm a sorted state.

---

## Features

* **Real-time Animation:** Watch bars slide and swap with smooth interpolation.
* **Live Code Preview:** A dedicated panel showing the C++ implementation of the active algorithm with VS Code-style syntax highlighting.
* **Three Classic Algorithms:**
    * **Bubble Sort:** Watch the largest elements "bubble" to the top.
    * **Selection Sort:** See the algorithm scan for the minimum and lock it into place.
    * **Insertion Sort:** Observe the sorted sub-list grow from left to right.
* **Interactive Controls:**
    * Adjust array **Size** (10 to 300 bars) and **Speed** on the fly.
    * **Start/Pause/Replay:** Compare algorithm efficiency on the exact same data set without reshuffling.
* **Victory Sweep:** A satisfying "staircase" green animation that triggers once the sort is mathematically proven.

---

## Prerequisites

Before building, ensure you have the following:

1.  **C++ Compiler:** (MSVC, MinGW, or Clang).
2.  **Raylib:** This project requires `raylib.h` and the corresponding library files. [Download Raylib here](https://www.raylib.com/).
3.  **Windows OS:** The UI uses the system font `C:\Windows\Fonts\consola.ttf`. 
    * *Note: For Linux/macOS, simply update the font path in `main.cpp` to a local .ttf file.*

---

## Building the Project

### 1. Clone the Repository
```
git clone https://github.com/justPoosay/SortingAlgorithms.git
cd SortingAlgorithms
```

### 2. Manual Compilation
Ensure `raylib` is in your include/lib paths:
```
g++ main.cpp -o SortingAlgorithms.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

### 3. Visual Studio Setup
1. Create a new Empty C++ Project
2. Add `main.cpp` and `raygui.h` to your project
3. In **Project Properties**:
    * **VC++ Directories > Include Directories**: Add path to Raylib `include`
    * **VC++ Directories > Library Directories**: Add path to Raylib `lib`
    * **Linker > Input > Additional Dependencies**: Add `raylib.lib;winmm.lib;gdi32.lib;user32.lib;shell32.lib;`
4. Set Build Configuration to **Release | x64** and press **F5**

## How to Use
1. **Select Algorithm**: Use the dropdown menu to pick Bubble, Selection, or Insertion
2. **Generate Data**: Click **GENERATE** to create a fresh randomized array
3. **Start Sorting**: Click **START**
4. **Analyze**: Watch the **SOURCE CODE PREVIEW** on the right to see which line of code is executing
5. **Replay**: Once finished, click **REPLAY** to watch the same data set be sorted again (great for comparing speeds)