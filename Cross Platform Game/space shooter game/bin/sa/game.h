#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
#include "power_up.h"


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN, 
    GAME_ATTACK, 
    GAME_LOSE, 
    GAME_HURT

};


enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision; 

const glm::vec2 PLAYER_SIZE(200.0f, 140.0f);
const glm::vec2 ENEMY_SIZE(200.0f, 140.0f);
const glm::vec2 FRUIT_SIZE(8.0f, 8.0f);
const glm::vec2 PAUSE_SIZE(300.0f, 300.0f);
const glm::vec2 HEART_SIZE(180.0f, 70.0f);

const float PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, 950.0f);
const float BALL_RADIUS = 10.0f;

class Game
{
public:
    GameState               State;
    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel>  Levels;
    std::vector<PowerUp>    PowerUps;
    unsigned int            Level;
    unsigned int            Lives;
    unsigned int            Points;
    Game(unsigned int width, unsigned int height);
    ~Game();
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void ResetLevel();
    void ResetBall();
    int RandomInt();
    void ResetPlayer();
    void SpawnPowerUps(GameObject& block);
    void UpdatePowerUps(float dt);
};

#endif