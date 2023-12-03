# OpenGL Space Shooter Game

## About the Project
This is a Space Shooter Game programmed in C++ using OpenGL and GLFW libraries. The goal is to take out the spaceships without taking damage. Each hit removes one heart from your life pool. 

## Controls

The enemy spaceship pattern can be changed with the UP and DOWN keys. The spaceship controls are WASD and UP, DOWN, LEFT, RIGHT. Press SPACE to attack. 

![space](https://github.com/sheraadams/Space-Shooter-Game/assets/110789514/6f4bbc16-0267-4eed-a0c6-b9ca179797bc)

<p align="center">
  <img src="https://user-images.githubusercontent.com/110789514/216853127-afb3bd9a-f72f-44e1-8d86-89718bf26ca4.gif" alt="spaceshooter_AdobeExpress">
</p>

## Parallax scrolling effects
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

## Game controls
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

## Photoshop tutorial

I added a Photoshop tutorial where I design the [spaceships and other game assets](https://youtu.be/WE-DJ-A5yTY). I also added a tutorial that demonstrates how to [configure your projects in Visual Studio](https://youtu.be/ZFx30Zmo1yI).

## References 

View my [references including LearnOpenGL.com](https://github.com/sheraadams/Space-Shooter-Game/blob/main/REFERENCES.md) here.

<div style="text-align: center;">
  <p><strong>Proudly crafted with ❤️ by <a href="https://github.com/sheraadams" target="_blank">Shera Adams</a>.</strong></p>
</div>
