/* Features:
- Basic Pong game with player and unbeatable Bot paddles
- Paddle prediction for ball interception by the bot
- Ball physics with paddle and wall collisions - where the ball bounces off at angles based on hit position
- Score tracking and display
- Speed increase on paddle hit
- Reset functionality
- Clean code structure with classes and operator overloads
- Clean imput handling with priority for simultaneous key presses example: Hold UP and DOWN, last pressed takes priority, release last pressed key to go back to the first key
- Sound effects for paddle hits and scoring
- Background music loop
*/

/* Bugs / Problems / Possible Improvements:
- Screen resize messes up the game, needs rework to support dynamic screen sizes based on the current display size
- Lag spikes can cause the ball to "tunnel" through paddles/walls if it moves too far in one frame Fix: Implement continuous collision detection or limit max deltaTime
- No start screen or pause functionality
*/

#include "raylib.h"
#include "raymath.h"
#include <string>
#include <cmath>
#include <iostream>
#include <vector>

constexpr int screenWidth  = 800;
constexpr int screenHeight = 450;

struct AudioManager {

    enum SoundEffectID {
        PlayerPaddleHit = 0,
        AIPaddleHit = 1,
        ScorePoint = 2
    };

    static void loadSoundEffects() {
        hitSounds.resize(4); // 2 hit sounds + 2 scoring sounds

        try {

        hitSounds[SoundEffectID::PlayerPaddleHit] = (LoadSound("resources/Player_Hit_Sound.mp3")); // PlayerPaddleHit
        hitSounds[SoundEffectID::AIPaddleHit]     = (LoadSound("resources/Enemy_Hit_Sound.mp3")); // AIPaddleHit
        hitSounds[SoundEffectID::ScorePoint]      = (LoadSound("resources/Scoring_Sound1.mp3")); // ScorePoint
        hitSounds[SoundEffectID::ScorePoint + 1]  = (LoadSound("resources/Scoring_Sound2.mp3")); // ScorePoint
        
        } catch (const std::exception& e) {
            std::cerr << "Error loading sound effects: " << e.what() << "\n";
            hitSounds.clear(); // don't accept half-loaded sound effects. If one fails, all fail. 
        }
    }

    static void initializeBackgroundMusic() {
        backgroundMusic = LoadMusicStream("resources/Background_Music.mp3");
        backgroundMusic.looping = true;
        PlayMusicStream(backgroundMusic);
    }

    static void updateMusicStream() {
        UpdateMusicStream(backgroundMusic);
    }

    static void unloadSound() {
        if (hitSounds.empty()) return;

        for (auto& sound : hitSounds) {
            if (sound.stream.buffer != 0) { // check if sound was loaded
                UnloadSound(sound);
            }
        }
        hitSounds.clear();
        
        if (backgroundMusic.stream.buffer != 0) { // check if music was loaded
            UnloadMusicStream(backgroundMusic);
        }
    }

    static void playSoundEffect(SoundEffectID id) {
        if (id >= 0 && id < hitSounds.size()) {
            PlaySound(hitSounds[id]);
        }
        else {
            std::cerr << "Sound effect ID out of range! ID: " << id << "\n";
        }
    }

private:
    inline static std::vector<Sound> hitSounds;
    inline static Music backgroundMusic;
};

#pragma region OperatorOverloads

inline AudioManager::SoundEffectID operator+(const AudioManager::SoundEffectID id, const int offset) {
    return static_cast<AudioManager::SoundEffectID>(static_cast<int>(id) + offset);
}

#pragma endregion

struct GameObject {
    Vector2 position;
    Vector2 unitVector;
    float speed;
    Color color;

    GameObject(Vector2 position = { 0.0, 0.0 }, Vector2 unitVector = { 0, 0 }, float speed = 0.0f, Color color = WHITE) 
        : position(position), unitVector(unitVector), speed(speed), color(color) {}

    virtual void update(float deltaTime) {
        position += normaliseUnitVector(unitVector) * speed * deltaTime;
    }
    virtual void draw() = 0;
    // target unitVectors towards the given point
    virtual void pointTowards(Vector2 target) final {
        Vector2 direction = target - position;
        unitVector = normaliseUnitVector(direction);
    }
    const Vector2 normaliseUnitVector(Vector2 vec) const {
        float len = std::sqrt(vec.x * vec.x + vec.y * vec.y);

        if (len == 0) return {0.0f, 0.0f};
        return {vec.x / len, vec.y / len};
    }
};

#pragma region GameObjects
struct Paddle : GameObject {
    Vector2 size;

    Paddle(Vector2 position = { 0.0, 0.0 }, Vector2 sz = { 10.0, 10.0 }, Vector2 unitVector = { 0, 0 }, float speed = 0.0f, Color color = WHITE) 
    : GameObject(position, unitVector, speed, color), size(sz) {}

    void draw() override{
        DrawRectangleV(position, size, color);
    }

    Rectangle toRectangle() const {
        return { position.x, position.y, size.x, size.y };
    }

    Vector2 getCenter() const {
        return { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
    }

    void update(float deltaTime) override {
        Vector2 delta = normaliseUnitVector(unitVector) * speed * deltaTime;

        Vector2 newPosition = position + delta;

        // Clamp within screen bounds
        if (newPosition.y >= 0 && newPosition.y + size.y <= screenHeight) {
            position.y = newPosition.y;
        }
        if (newPosition.x >= 0 && newPosition.x + size.x <= screenWidth) {
            position.x = newPosition.x;
        }
    }
};

struct Ball : GameObject {
    float radius;

    Ball(Vector2 position = { 0.0, 0.0 }, Vector2 unitVector = { 0, 0 }, float radius = 5.0f, float speed = 0.0f, Color color = WHITE) 
        : GameObject(position, unitVector, speed, color), radius(radius) {}

    void draw() override {
        DrawCircleV(position, radius, color);
    }
};

struct ScoreText {
    Vector2 position;
    int fontSize;
    Color color;

    ScoreText(Vector2 pos = {0.0, 0.0}, int fSize = 20, Color color = WHITE) : position(pos), fontSize(fSize), color(color) {}

    // Scores for left and right players
    // text is centered horizontally
    void draw() {
        std::string scoreText = std::to_string(scoreLeft) + " - " + std::to_string(scoreRight);
        int textWidth = MeasureText(scoreText.c_str(), fontSize);
        DrawText(scoreText.c_str(), static_cast<int>(position.x - textWidth / 2.0f), static_cast<int>(position.y), fontSize, color);
    }

    void incrementScore(bool isRight = true) {
        isRight ? scoreRight++ : scoreLeft++;
    }

    void resetScore() {
        scoreLeft = 0;
        scoreRight = 0;
    }

    enum Player {
        Left = 0,
        Right = 1
    };

private:
    int scoreLeft = 0;
    int scoreRight = 0;
};

struct Text {
    Vector2 position;
    std::string content;
    int fontSize;
    Color color;

    Text(Vector2 pos = {0.0, 0.0}, std::string txt = "Lorem Ipsum", int fSize = 20, Color color = WHITE) 
        : position(pos), content(txt), fontSize(fSize), color(color) {}

    void draw() {
        DrawText(content.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), fontSize, color);
    }
    void centerHorizontally(int screenWidth) {
        int textWidth = MeasureText(content.c_str(), fontSize);
        position.x = (screenWidth - textWidth) / 2.0f;
    }
};
#pragma endregion

#pragma region Game
struct Game {

    Game() {
        // for initializing variables and game state

        const float paddleMovementSpeed = 300.0f;
        const Vector2 paddleSize = { 10.0f, 100.0f };
        const float halfPaddleHeight = paddleSize.y / 2.0f;
        const float objectWallOffset = 50.0f;
        const float ballInitialSpeed = 400.0f;
        const float ballRadius = 7.0f;
        const int textFontSize = 10;
        const float textRowOffset = textFontSize + 20.0f;

        leftPaddle = Paddle({ objectWallOffset, screenHeight / 2.0f - halfPaddleHeight }, paddleSize, { NONE, NONE }, paddleMovementSpeed, WHITE);
        rightPaddle = Paddle({ screenWidth - objectWallOffset, screenHeight / 2.0f - halfPaddleHeight }, paddleSize, { NONE, NONE }, paddleMovementSpeed, WHITE);
        ball = Ball({ screenWidth / 2.0f, screenHeight / 2.0f }, { RIGHT, randomValidUnitVector() }, ballRadius, ballInitialSpeed, WHITE);
        scoreText = ScoreText({ screenWidth / 2.0f, 20.0f }, 30, WHITE);

        speedText = Text({ screenWidth / 2.0f, objectWallOffset }, "Speed:" + std::to_string(static_cast<int>(ball.speed)), textFontSize + 10, LIGHTGRAY);
        speedRecordText = Text({ screenWidth / 2.0f, objectWallOffset + textRowOffset }, "Speed Record: 0", textFontSize, LIGHTGRAY);
        resetHintText = Text({ screenWidth / 2.0f, screenHeight - objectWallOffset }, "Press 'R' to Restart the Game", textFontSize, LIGHTGRAY);
        
        speedText.centerHorizontally(screenWidth);
        speedRecordText.centerHorizontally(screenWidth);
        resetHintText.centerHorizontally(screenWidth);

        InitAudioDevice();
        AudioManager::loadSoundEffects();
        AudioManager::initializeBackgroundMusic();
    }
    
    void readInput() {

        // for things that move by player input
        switch (getCurrentVerticalKey())   
        {
        case KEY_UP:
            leftPaddle.unitVector.y = UP;
            break;
        case KEY_DOWN:
            leftPaddle.unitVector.y = DOWN;
            break;
        default:
            leftPaddle.unitVector = { NONE, NONE };
            break;
        }

        if (IsKeyPressed(KEY_R)) {
            resetGame();
        }
    }

    void update(float& deltaTime) {
        // for things that move by themselves

        handlePaddleBallCollision(leftPaddle);
        handlePaddleBallCollision(rightPaddle);
        handleWallBallCollision();
        handlePointScoring();
        handleAIPaddleMovement();

        leftPaddle.update(deltaTime);
        rightPaddle.update(deltaTime);
        ball.update(deltaTime);

        speedText.content = "Speed: " + std::to_string(static_cast<int>(ball.speed));
        speedText.centerHorizontally(screenWidth);

        speedRecordText.content = "Speed Record: " + std::to_string(getSpeedRecord());
        speedRecordText.centerHorizontally(screenWidth);

        AudioManager::updateMusicStream();
    }
    
    void draw() {
        
        BeginDrawing();
        
        ClearBackground(BLACK);

        leftPaddle.draw();
        rightPaddle.draw();
        ball.draw();

        scoreText.draw();
        speedText.draw();
        resetHintText.draw();
        speedRecordText.draw();

        EndDrawing();
    }

    ~Game() {
        AudioManager::unloadSound();
        CloseAudioDevice();
    }

private:
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    ScoreText scoreText;
    Text speedText;
    Text speedRecordText;
    Text resetHintText;

    // priority for the newest press between the two keys
    int getCurrentVerticalKey() {
        static int activeKey = -1;   // KEY_UP, KEY_DOWN, or -1
        bool upPressed = IsKeyDown(KEY_UP);
        bool downPressed = IsKeyDown(KEY_DOWN);

        if (!upPressed && !downPressed) {
            activeKey = -1; // no keys pressed
            return activeKey;
        }
        if (upPressed ^ downPressed) { // only one key is pressed
            activeKey = upPressed ? KEY_UP : KEY_DOWN;
            return activeKey;
        }
        if (upPressed && downPressed) { // both keys are pressed
            return activeKey == KEY_UP ? KEY_DOWN : KEY_UP;
        }

        return activeKey;
    }

    void resetRound() {
        ball.position = { screenWidth / 2.0f, screenHeight / 2.0f };
        ball.unitVector = { RIGHT, randomValidUnitVector() }; // send ball towards last scorer
        ball.speed = 400.0f;
        handleAIPaddleMovement(true); // reset AI prediction
    }
    void resetGame() {
        leftPaddle.position = { 50.0f, screenHeight / 2.0f - 50.0f };
        rightPaddle.position = { screenWidth - 50.0f, screenHeight / 2.0f - 50.0f };
        ball.position = { screenWidth / 2.0f, screenHeight / 2.0f };
        ball.unitVector = { RIGHT, randomValidUnitVector() };
        ball.speed = 400.0f;
        scoreText.resetScore();
        getSpeedRecord(true); // reset speed record
        handleAIPaddleMovement(true); // reset AI prediction
    }
    void handlePaddleBallCollision(Paddle& paddle) {

        if (CheckCollisionCircleRec(ball.position, ball.radius, paddle.toRectangle())) {
            // unitVector y component is adjusted based on collision point
            ball.unitVector.x *= -1;
            Vector2 paddleCenter = paddle.getCenter();
            ball.unitVector.y = (ball.position.y - paddleCenter.y) / (paddle.size.y / 2.0f);
            // increase speed exponentially based on current speed and angle
            ball.speed += 20.0f * (ball.speed / 400.0f) * std::fabs(ball.unitVector.y);

            AudioManager::playSoundEffect((&paddle == &leftPaddle) ? AudioManager::PlayerPaddleHit : AudioManager::AIPaddleHit);
        }
    }
    void handleWallBallCollision() {
        if (ball.position.y - ball.radius <= 0 || 
            ball.position.y + ball.radius >= screenHeight) {
            ball.unitVector.y *= -1;
        }
    }
    void handlePointScoring() {
        if (ball.position.x + ball.radius >= screenWidth) {
            scoreText.incrementScore(scoreText.Player::Left); // left player scores
            AudioManager::playSoundEffect(AudioManager::ScorePoint + GetRandomValue(0, 1));
            resetRound();
        }
        else if (ball.position.x - ball.radius <= 0) {
            scoreText.incrementScore(scoreText.Player::Right); // right player scores
            AudioManager::playSoundEffect(AudioManager::ScorePoint + GetRandomValue(0, 1));
            resetRound();
        }
    }
    void handleAIPaddleMovement(bool resetFunction = false) { 
        static float predictedY = predictBallY(rightPaddle.position.x + rightPaddle.size.x / 2.0f) - rightPaddle.size.y / 2.0f;
        static int lastFrameBallDirX = LEFT;

        if (resetFunction) {
            predictedY = predictBallY(rightPaddle.position.x + rightPaddle.size.x / 2.0f) - rightPaddle.size.y / 2.0f;
            lastFrameBallDirX = ball.unitVector.x;
            return;
        }

        if (lastFrameBallDirX == LEFT && ball.unitVector.x == RIGHT) {
            // ball changed direction towards AI paddle, make new prediction
            predictedY = predictBallY(rightPaddle.position.x + rightPaddle.size.x / 2.0f) - rightPaddle.size.y / 2.0f;
        }

        if (ball.unitVector.x == LEFT) {
            // return to center when ball is moving away
            rightPaddle.pointTowards({ rightPaddle.position.x, screenHeight / 2.0f - rightPaddle.size.y / 2.0f });
        }
        else rightPaddle.pointTowards({ rightPaddle.position.x, predictedY }); // aim towards predicted position

        lastFrameBallDirX = (ball.unitVector.x != LEFT) ? RIGHT : LEFT;
    }
    float predictBallY(float targetX) {
        // Step 1: raw straight-line intersection
        float dx = targetX - ball.position.x;
        float dy = dx * (ball.unitVector.y / ball.unitVector.x);    // slope direction
        float y_raw = ball.position.y + dy;

        // Step 2: triangle-wave reflection
        float period = 2 * screenHeight;
        float y_mod = fmod(y_raw, period);

        if (y_mod < 0) y_mod += period;

        if (y_mod <= screenHeight)
            return y_mod;

        return period - y_mod;
    }
    float randomValidUnitVector() {
        float unitVector = GetRandomValue(-1000, 1000) / 1000.0f;
        if (unitVector == 0) unitVector = -1; // prevent vertical only movement
        return unitVector;
    }
    int getSpeedRecord(bool resetRecord = false) const {
        static int currentSpeedRecord = 0;
        
        if (resetRecord) {
            currentSpeedRecord = 0;
            return currentSpeedRecord;
        }
        if (ball.speed > currentSpeedRecord) {
            currentSpeedRecord = static_cast<int>(ball.speed);
        }
        return currentSpeedRecord;
    }
    enum unitVectorDirection {
        UP = -1,
        DOWN = 1,
        LEFT = -1,
        RIGHT = 1,
        NONE = 0
    }; 
};
#pragma endregion

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    Game game;

    InitWindow(screenWidth, screenHeight, "Pong Game - Raylib");

    Image icon = LoadImage("resources/icon.png");
    SetWindowIcon(icon);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        game.readInput();
        game.update(deltaTime);
        game.draw();
    }

    CloseWindow();        // Close window and OpenGL context
    UnloadImage(icon);

    return 0;
}