#include <algorithm>
#include <sstream>
#include <iostream>
#include <learnopengl/filesystem.h>
#include <irrklang/irrKlang.h>
using namespace irrklang;
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
GameObject* Fruit;
GameObject* Hearts;
GameObject* Hearts2;
GameObject* Hearts3;
GameObject* Pause; 
GameObject* Fire;
GameObject* Background;
PostProcessor* Effects;
ISoundEngine* SoundEngine = createIrrKlangDevice();
TextRenderer* Text;

float ShakeTime = 0.0f;
Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0), Lives(3), Points()
{
}

Game::~Game()
{
    delete Hearts;
    delete Hearts2;
    delete Hearts3;
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Background;
    delete Effects;
    delete Fire;
    delete Text;
    SoundEngine->drop();
}

void Game::Init()
{
    SoundEngine->play2D("LosingControl.mp3", true);

    ResourceManager::LoadShader("sprite.vs", "sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("particle.vs", "particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("post_processing.vs", "post_processing.fs", nullptr, "postprocessing");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // load textures
    ResourceManager::LoadTexture("resources/textures/AdobeStock_408749277.jpg", false, "background");
    ResourceManager::LoadTexture("resources/textures/1.png", true, "background1");
    ResourceManager::LoadTexture("resources/textures/awesomeface2.png", true, "face");
    ResourceManager::LoadTexture("resources/textures/2.png", true, "background2");
    ResourceManager::LoadTexture("resources/textures/hearts.png", true, "hearts");
    ResourceManager::LoadTexture("resources/textures/hearts2.png", true, "hearts2");
    ResourceManager::LoadTexture("resources/textures/hearts3.png", true, "hearts3");
    ResourceManager::LoadTexture("resources/textures/space3.png", true, "spaceship");
    ResourceManager::LoadTexture("resources/textures/empty.png", true, "empty");
    ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("resources/textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("resources/textures/powerup_speed2.png", true, "powerup_speed");


    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("resources/fonts/OCRAEXT.TTF", 24);
    // load levels
    GameLevel one; one.Load("resources/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("resources/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("resources/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("resources/levels/four.lvl", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    glm::vec2 playerPos = glm::vec2(PLAYER_SIZE.x / 15.0f, this->Height / 2.0f - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("spaceship"));

    glm::vec2 FruitPos = glm::vec2(this->Width / 2.0f - FRUIT_SIZE.x, this->Height / 10.0f - FRUIT_SIZE.y);
    Fruit = new GameObject(FruitPos, FRUIT_SIZE, ResourceManager::GetTexture("powerup_speed"));

    glm::vec2 PausePos = glm::vec2(this->Width / 2.0f - 200, this->Height / 2.0f - 170);
    Pause = new GameObject(PausePos, PAUSE_SIZE, ResourceManager::GetTexture("pause"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    glm::vec2 backgroundPos = glm::vec2(0.0f, 0.0f);
    Background = new GameObject(backgroundPos, glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("background"));

    glm::vec2 hearts3Pos = glm::vec2(0.0f, 0.0f);
    Hearts3 = new GameObject(hearts3Pos, glm::vec2(HEART_SIZE), ResourceManager::GetTexture("hearts3"));
    glm::vec2 hearts2Pos = glm::vec2(0.0f, 0.0f);
    Hearts2 = new GameObject(hearts2Pos, glm::vec2(HEART_SIZE), ResourceManager::GetTexture("hearts2"));
    glm::vec2 heartsPos = glm::vec2(0.0f, 0.0f);
    Hearts = new GameObject(heartsPos, glm::vec2(HEART_SIZE), ResourceManager::GetTexture("hearts"));
}
int Game::RandomInt()
{
    int randNum = rand() % 5 + 1;
    return randNum;
}

void Game::Update(float dt)
{  
    this->Points = this->Points;
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

    this->UpdatePowerUps(dt);
    // reduce shake time
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }
    if (Lives <= 0) // did ball reach bottom edge?
    {
        this->ResetLevel();
        this->State = GAME_MENU;
        this->ResetPlayer();
    }
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetLevel();
        this->ResetPlayer();
        Effects->Chaos = true;
        this->State = GAME_WIN;
    }
}

void Game::ProcessInput(float dt)
{

    float velocity = PLAYER_VELOCITY/5 * dt;

    /*  FIXME
        for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        box.Position.y += velocity * RandomInt();
        if (box.Position.y >= 1000)
        {
            velocity = -velocity;
        }
        else if (box.Position.y <=100)
        {
            velocity = -velocity;
        }
    }
    */

    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->Keys[GLFW_KEY_UP] && !this->KeysProcessed[GLFW_KEY_UP])
        {
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = true;
        }
        if (this->Keys[GLFW_KEY_DOWN] && !this->KeysProcessed[GLFW_KEY_DOWN])
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            this->Level = (this->Level - 1) % 4;
            this->KeysProcessed[GLFW_KEY_S] = true;
        }
    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            Effects->Chaos = false;
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE || this->State == GAME_ATTACK)
    {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_SPACE])
        {
            this->State = GAME_ATTACK;
        }
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_LEFT])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_UP])
        {
            if (Player->Position.y >= 0.0f)
            {
                Player->Position.y -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_DOWN])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
            {
                Player->Position.y += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
            {
                Player->Position.y += velocity;
            }
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN || this->State == GAME_ATTACK || this->State == GAME_HURT || this->State == GAME_LOSE)
    {
        Effects->BeginRender();
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
       // Background->Draw(*Renderer);
        // here we end the effects to scroll only the background and not the rest of the scene
        Effects->EndRender();
        Effects->Render(glfwGetTime());
        this->Levels[this->Level].Draw(*Renderer);
        Player->Draw(*Renderer);
        for (PowerUp& powerUp : this->PowerUps)
            if (!powerUp.Destroyed)
                powerUp.Draw(*Renderer);
        Particles->Draw();
        if (Lives ==3) 
            Hearts3->Draw(*Renderer);
        if (Lives == 2)
            Hearts2->Draw(*Renderer);
        if (Lives == 1)
            Hearts->Draw(*Renderer);
        Ball->Draw(*Renderer);

       // std::stringstream ss; ss << this->Lives;
      //  std::stringstream pp; pp << this->Points;
       // Text->RenderText("Points:" + pp.str(), 5.0f, 5.0f, 1.0f);
    }
    if (this->State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", this->Width / 2.0f - 200.0f, this->Height / 2.0f, 1.0f);
        Text->RenderText("Press UP or DOWN to select level", this->Width / 2.0f - 225.0f, this->Height / 2.0f + 20.0f, 0.75f);

    }
    if (this->State == GAME_ATTACK)
    {
        glm::vec2 playerPos = glm::vec2(Player->Position.x, Player->Position.y);
        glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
        Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
        Ball->Draw(*Renderer);
        this->State = GAME_ACTIVE;
    }
    
    if (this->State == GAME_WIN)
    {
        Text->RenderText("You WON!!!", this->Width / 2.0f - 70.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText("Press ENTER to retry or ESC to quit", this->Width / 2.0f - 260.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    if (this->State == GAME_LOSE)
    {
        Text->RenderText("You LOST!!!", this->Width / 2.0f - 70.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText("Press ENTER to retry or ESC to quit", this->Width / 2.0f - 260.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Lives = 3;
    this->Points = 0;
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
    Effects->Chaos = Effects->Confuse = false;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::ResetBall()
{

    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
    Ball->Color = glm::vec3(1.0f);
}



void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                powerUp.Activated = false;
                --Lives;
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}
void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(5)) // 1 in 55 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(1.5f, 1.5f, 0.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed"))); // banana

}

void ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {

    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return true;
    }
    return false;
}

bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);
void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (box.Destroyed)
        {
            this->Points + 100;
        }
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) 
            {
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                    SoundEngine->play2D("resources/audio/solid.wav", false);
                }
                else
                {   
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                    SoundEngine->play2D("resources/audio/bleep.mp3", false);
                }
            }
        }
    }

    for (PowerUp& powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            if (powerUp.Position.y >= this->Height)
            {
                powerUp.Destroyed = true;
            }
            if (CheckCollision(*Player, powerUp))
            {	
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
                powerUp.Activated = true;
                SoundEngine->play2D("resources/audio/solid.wav", false);
                this->Points + 100;
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (std::get<0>(result))
    {
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); 
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        SoundEngine->play2D("resources/audio/bleep.wav", false);
    }
}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{

    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{

    glm::vec2 center(one.Position + one.Radius);
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;
    if (glm::length(difference) < one.Radius) 
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}


Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;

    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}
