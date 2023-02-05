#include "game_level.h"
#include <fstream>
#include <sstream>

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) 
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) 
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}
void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); 
    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height; 	
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if (tileData[y][x] == 1) // solid
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("empty"), glm::vec3(1.0f, 1.0f, 1.0f));
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] == 2)	// non solid 
            {
                glm::vec3 color = glm::vec3(1.0f); // white underneath
                if (tileData[y][x] == 2)
                    color = glm::vec3(0.0f, 1.3f, 0.0f); 

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("spaceship"), color));
            }
            else if (tileData[y][x] ==3 )	// non solid 
            {
                glm::vec3 color = glm::vec3(1.0f); 
                if (tileData[y][x] == 3)
                    color = glm::vec3(1.0f, 0.0f, 1.0f); 

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("spaceship"), color));
            }
            else if (tileData[y][x] > 3)	// non solid 
            {
                glm::vec3 color = glm::vec3(1.0f); 

                if (tileData[y][x] == 4)
                    color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
                else if (tileData[y][x] == 5)
                    color = glm::vec3(0.0f, 1.5f, 1.5f); //teal

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("spaceship"), color));
            }
        }
    }
}
