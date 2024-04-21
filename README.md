# OpenGL Space Shooter Game

See the code walk-through on [YouTube](https://youtu.be/ZqfGpTG0-Pc).

## About the Project
This is a Space Shooter Game programmed in C++ using OpenGL and GLFW libraries. The goal is to take out the spaceships without taking damage. Each hit removes one heart from your life pool. 

## Keyboard Controls

The enemy spaceship pattern can be changed with the UP and DOWN keys. The spaceship controls are WASD and UP, DOWN, LEFT, RIGHT. Press SPACE to attack. 

![space](https://github.com/sheraadams/Space-Shooter-Game/assets/110789514/6f4bbc16-0267-4eed-a0c6-b9ca179797bc)

<p align="center">
  <img src="https://user-images.githubusercontent.com/110789514/216853127-afb3bd9a-f72f-44e1-8d86-89718bf26ca4.gif" alt="spaceshooter_AdobeExpress">
</p>

## Parallax Scrolling Effects
We can accomplish the parallax scrolling background effect that gives the illusion of horizontal movement with the following code 

```cpp
// post_processing.vs
    else if(parallax)
    {
       TexCoords = vec2(texture.x - .8 * time/6, texture.y );
    }
    else if(parallaxslow)
    {
       TexCoords = vec2(texture.x- .8 * time/6, texture.y);
    }
```

## Keyboard Controls
We set the key controls in the processInput(dt) function where delta time is a parameter. 
With the following code, we set WASD and  UP, DOWN, LEFT, RIGHT to contol the spaceship's movement.
```cpp
void Game::ProcessInput(float dt)
{
    float velocity = PLAYER_VELOCITY/5 * dt;
    // ...
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
// ...
}
```
## Render Loop

The render loop draws our items to the screen depending on the game state (GAME_ACTIVE, GAME_MENU, GAME_WIN, GAME_ATTACK, GAME_HURT, or GAME_LOSE)

```cpp
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

```

## Collision Checking

With the following code, we check for collisions between our game objects:
```cpp
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
```
## Photoshop Tutorials

Here is a Photoshop tutorial where I design the [spaceships and other game assets](https://youtu.be/WE-DJ-A5yTY) and here is a tutorial that demonstrates how to [configure your projects in Visual Studio](https://youtu.be/ZFx30Zmo1yI).

## XCode Usage

1. Open a terminal in the project directory and run the code to remove the old build folder and generate a new one with the Xcode project file. 

```bash
rm -rf build 
mkdir build
cd build
cmake -G Xcode ..
```

2. Set the working directory in Xcode using Product > Scheme > Edit Scheme > Run Debug > Options > Working Directory > (Check Set Custom) > Project directory/build.

3. Build and run the project.

## References 

View my [references including LearnOpenGL.com](https://github.com/sheraadams/Space-Shooter-Game/blob/main/references.md) here.

<div style="text-align: center;">
  <p><strong>Proudly crafted with ❤️ by <a href="https://github.com/sheraadams" target="_blank">Shera Adams</a>.</strong></p>
</div>
