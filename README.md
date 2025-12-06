# 🏓 Pong in C++ (Raylib)

A clean, modern, from-scratch implementation of **Pong** built in **C++17** using **Raylib**.  
This project focuses on functional gameplay, predictable physics, clean input handling, and an AI paddle that
**predicts the ball’s interception point** with a custom trajectory algorithm.

It began as a short homework and grew into a surprisingly polished mini-game with an intentionally clean
architecture suitable as a base for future 2D arcade-style games.
I did it as to challange myself and get a breath of fresh air after too much messy arduino code.

---

## 🎥 Demo

[![Gameplay Demo](https://github.com/Radu734/SinglePlayer-Pong-Raylib/blob/main/Gameplay.gif)](https://github.com/Radu734/SinglePlayer-Pong-Raylib/blob/main/Gameplay.mp4)

---

# ⭐ Features

- 🎮 **Player-controlled paddle**  
  Smooth movement with input-priority logic  
  (pressing UP + DOWN → last pressed wins)

- 🤖 **Unbeatable AI paddle**  
  Predicts future ball position using a multi-step wall-bounce algorithm.

- 🧮 **Realistic ball physics**  
  Angle changes based on impact position  
  Speed increases on paddle hits  
  Continuous movement vector calculation

- 🔊 **Audio System**  
  Background music loop  
  SFX for player hit, AI hit, and scoring  
  Centralized AudioManager for loading and playing sounds

- 🧱 **Collision System**  
  Ball–paddle  
  Ball–wall  
  Scoring with round reset

- 🧼 **Clean architecture**  
  - Object-oriented design  
  - Small, focused classes (Ball, Paddle, Text, AudioManager, Game)  
  - Operator overloads for vector math  
  - Encapsulated input logic  
  - Organized update/draw cycles  

- 🖥️ **Runs at any framerate**  
  Uses `deltaTime` for consistent movement.

---

# 🧩 Code Architecture Overview

### **GameObject Base Class**
All dynamic objects (Ball, Paddle, ScoreText) inherit from `GameObject`  
and implement their own `update()` and `draw()` logic.

### **Versor-Based Movement**
Movement directions use a custom `Versor2f` type, giving:
- simple directional intent  
- normalized float vector logic  
- clean separation between "direction" and "speed"

### **Input System – Priority Logic**
The player paddle uses a **keypress priority system**:  
Holding UP + DOWN → whichever key was pressed last becomes active.  
Releasing it falls back to the first key.  
(Prevents jitter or conflicting input.)

### **AI Prediction**
The bot paddle computes where the ball will collide in the future by:
1. Simulating horizontal travel  
2. Simulating vertical bounces  
3. Reducing the motion through modular reflection  
4. Predicting final Y before the paddle intercepts it  

This makes the AI undefeatable unless modified manually or if a bug occurs.

### **AudioManager**
A fully encapsulated audio subsystem:
- static loading
- static unloading
- type-safe sound IDs
- safe bounds checking

---

# 🎮 Controls

| Key | Action |
|-----|--------|
| **Up Arrow** | Move paddle up |
| **Down Arrow** | Move paddle down |
| **'R'-Key** | Reset Game

---

# 🚀 Build & Run

### **Requirements**
- Raylib 4.x (already linked at the location 'C:/raylib/raylib')
- MinGW-w64 or equivalent compiler
- C++17

### **Build using VSCode Task**
Press:
Ctrl + Shift + B
Select: 
Build Debug / Build Release

### **Run**

Either:

- Use the `run` task from VSCode  
- OR manually run:

```bash
./main.exe
```

---

# 🐛 Known Issues / Limitations

- Screen resizing breaks layout
The game currently assumes a fixed window size.

- Rare tunneling (ball skipping collisions)
Happens at extremely low FPS spikes.
Fix: implement continuous collision detection or clamp max deltaTime.

- No start menu or pause
Currently jumps directly into gameplay.

---

# 🔮 Future Improvements

- Add pause menu, start screen, and difficulty options

- Rewrite ball–paddle collision as continuous instead of discrete

- Add particle effects / screen shake for hits

- Add Powerups (faster ball, random angle boost, paddle stretch)

- Add proper GUI abstraction for menus

- Add optional “human-mode” bot with adjustable reaction speed

- Add choice beween 1-Player and 2-Player modes

---

# 📄 License

This project is open-source under the MIT license.

---

# 🙌 Credits

Developed by Teodosiu Radu
Built with C++17 and Raylib
