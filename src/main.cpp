#include "screen.h"
#include <numeric>
#include <cmath>
#include <iostream>
#include <unordered_map>

bool gameStarted = false;
int delay = 100;

int pY = INT_MAX;
int pX = INT_MAX;
void input(Screen& screen)
{
    while(SDL_PollEvent(&screen.e))
    {
        if(screen.e.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
        if(screen.e.type == SDL_KEYDOWN)
        {
            switch(screen.e.key.keysym.sym)
            {
                case SDLK_f:
                    gameStarted = true;
                    break;
                
                case SDLK_r:
                    gameStarted = false;
                    screen.clear();
                    break;

                case SDLK_1:
                    // 10 fps
                    delay = 100;
                    break;

                case SDLK_2:
                    // 20 fps
                    delay = 50;

                case SDLK_3:
                    //~ 30fps
                    delay = 33;
                    break;
                
                case SDLK_4:
                    // 40 fps
                    delay = 25;
                    break;

                case SDLK_5:
                    // 60 fps
                    delay = 16;
                    break;

                case SDLK_6:
                    // 100 fps
                    delay = 10;
                    break;
                
                case SDLK_7:
                    // 200 fps
                    delay = 5;
                    break;

                case SDLK_8:
                    // UNLIMITED POWER
                    delay = 0;
                    break;

            }
        }
        if (screen.e.type == SDL_MOUSEBUTTONDOWN) 
        {
            int mouseX, mouseY;
            Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

            screen.pixel(mouseX / screen.SCALE, mouseY / screen.SCALE);


            pX = mouseX;
            pY = mouseY;
        }
        else if (screen.e.type == SDL_MOUSEMOTION && screen.e.button.button == SDL_BUTTON_LEFT) 
        {
            int mouseX, mouseY;
            Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
            screen.line(pX / screen.SCALE, mouseX / screen.SCALE, pY / screen.SCALE, mouseY / screen.SCALE);
            pX = mouseX;
            pY = mouseY;
        }
    }
}

int main(int argc, char** argv)
{
    int width;
    int height;
    int scale;
    std::cout << "Instructions: Use the left mouse button to draw on the window, and to interact during the game of life" << std::endl;
    std::cout << "              Use 1-8 to control simulation speed and drawing smoothness" << std::endl;
    std::cout << "              Press F to start the game of life" << std::endl;
    std::cout << "              Press R during the game of life to reset the window" << std::endl << std::endl;
    std::cout << "Enter the width, height, and scale of your window (Ex: 200 200 1):";
    std::cin >> width >> height >> scale;
    Screen screen(width, height, scale);

    while (true)
    {
        input(screen); // Handle user input

        screen.show(); // Render the current state

        if (gameStarted)
        {
            // Collect coordinates of live cells
            std::vector<Vector2Int> liveCells;
            std::unordered_map<Vector2Int, int, Vector2IntHash> deadNeighborCount;

            for (const auto& pair : screen.points)
            {
                const auto& point = pair.first;
                liveCells.push_back(point);
            }

            std::vector<Vector2Int> pixelsToAdd;
            std::vector<Vector2Int> pixelsToRemove;

            for (const auto& liveCell : liveCells)
            {
                int liveNeighbors = 0;
                for (int i = liveCell.x - 1; i <= liveCell.x + 1; ++i)
                {
                    for (int j = liveCell.y - 1; j <= liveCell.y + 1; ++j)
                    {
                        if (i == liveCell.x && j == liveCell.y) continue; // Skip the live cell itself

                        Vector2Int neighbor(i, j);
                        if (screen.points.find(neighbor) != screen.points.end())
                        {
                            liveNeighbors++;
                        }
                        else
                        {
                            // Increment counter for dead neighbor
                            ++deadNeighborCount[neighbor];
                        }
                    }
                }

                // Apply rules of the Game of Life
                if (liveNeighbors < 2 || liveNeighbors > 3)
                {
                    pixelsToRemove.emplace_back(liveCell.x, liveCell.y);
                }
            }

            for(const auto& pair : deadNeighborCount)
            {
                const auto& cell = pair.first;
                if(pair.second == 3)
                {
                    // If a dead cell has exactly 3 live neighbors, mark it for addition
                    pixelsToAdd.emplace_back(cell.x, cell.y);
                }
            }

            // Add new pixels
            for (const auto& pixel : pixelsToAdd)
            {
                screen.pixel(pixel.x, pixel.y);
            }

            // Remove pixels
            for (const auto& pixel : pixelsToRemove)
            {
                screen.removePixel(pixel.x, pixel.y);
            }
        }

        // Add a delay to control the speed of the simulation
        SDL_Delay(delay);
    }

    return 0;
}
