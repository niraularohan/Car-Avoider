#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL_ttf.h>
#include <sstream>

struct OpponentCar
{
    SDL_Rect rect;
    int speed;
};

int main(int argc, char *argv[])
{
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 800;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cout << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Car Avoider", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Init();

    TTF_Font *font = TTF_OpenFont("opensansregular.ttf", 24);
    if (!font)
    {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Surface *imageSurface = IMG_Load("roadforgame.png");
    if (imageSurface == nullptr)
    {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);
    if (texture == nullptr)
    {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *carSurface = IMG_Load("playercar.png");
    if (carSurface == nullptr)
    {
        std::cout << "Failed to load car image: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *carTexture = SDL_CreateTextureFromSurface(renderer, carSurface);
    SDL_FreeSurface(carSurface);
    if (carTexture == nullptr)
    {
        std::cout << "Failed to create car texture: " << SDL_GetError() << std::endl;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *opponentCarSurface = IMG_Load("opponentcar.png");
    if (opponentCarSurface == nullptr)
    {
        std::cout << "Failed to load opponent car image: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(carTexture);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *opponentCarTexture = SDL_CreateTextureFromSurface(renderer, opponentCarSurface);
    SDL_FreeSurface(opponentCarSurface);
    if (opponentCarTexture == nullptr)
    {
        std::cout << "Failed to create opponent car texture: " << SDL_GetError() << std::endl;
        SDL_DestroyTexture(carTexture);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int playerCarX = 215;
    int playerCarSpeed = 200;

    const int numberOfOpponentCars = 3;
    const int opponentCarWidth = 81;
    const int opponentCarHeight = 157;
    const int constantSpeed = 20;

    const int frameRate = 60;
    const int frameDelay = 1000 / frameRate;

    Uint32 startTime = SDL_GetTicks();
    int score = 0;
    int lastscore = 0;

    std::vector<OpponentCar> opponentCars;

    const int positions[3] = {15, 215, 415};

    std::srand(std::time(nullptr));

    OpponentCar currentOpponentCar;
    currentOpponentCar.rect = {positions[std::rand() % 3], -(std::rand() % 800 + 200), opponentCarWidth, opponentCarHeight};
    currentOpponentCar.speed = constantSpeed;
    opponentCars.push_back(currentOpponentCar);

    bool quit = false;
    SDL_Event event;

    bool isGameOver = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_a && playerCarX > 15)
                {
                    playerCarX -= playerCarSpeed;
                }
                else if (event.key.keysym.sym == SDLK_d && playerCarX < 415)
                {
                    playerCarX += playerCarSpeed;
                }
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = (currentTime - startTime) / 1000;

        if (!isGameOver)
        {
            lastscore = score;
            score = elapsedTime;
            bool isCollision = false;
            SDL_Rect playerCarBoundingBox = {playerCarX, 620, 81, 157};

            for (OpponentCar &car : opponentCars)
            {
                SDL_Rect opponentCarBoundingBox = car.rect;

                if (SDL_HasIntersection(&playerCarBoundingBox, &opponentCarBoundingBox))
                {
                    isCollision = true;
                    break;
                }
            }

            if (isCollision)
            {
                isGameOver = true;
            }
            else
            {
                for (OpponentCar &car : opponentCars)
                {
                    car.rect.y += car.speed;

                    if (car.rect.y > 800)
                    {
                        car.rect.y = -(std::rand() % 800 + 200);
                        car.rect.x = positions[std::rand() % 3];
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_Rect playerCarRect = {playerCarX, 620, 81, 157};
        SDL_RenderCopy(renderer, carTexture, nullptr, &playerCarRect);

        for (OpponentCar &car : opponentCars)
        {
            SDL_RenderCopy(renderer, opponentCarTexture, nullptr, &car.rect);
        }

        SDL_Color textColor = {255, 255, 255};
        std::stringstream scoreText;
        scoreText << "Score: " << score;
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText.str().c_str(), textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_FreeSurface(textSurface);

        int scoreX = WINDOW_WIDTH - textWidth - 10;
        int scoreY = 10;

        SDL_Rect textRect = {scoreX, scoreY, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_DestroyTexture(textTexture);

        if (isGameOver)
        {
            SDL_Surface *gameOverSurface = IMG_Load("gameover.png");
            if (gameOverSurface != nullptr)
            {
                SDL_Texture *gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
                SDL_Rect gameOverRect = {0, 0, 500, 800};
                SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
                SDL_DestroyTexture(gameOverTexture);
                SDL_FreeSurface(gameOverSurface);
            }

            std::stringstream lastScoreText;
            lastScoreText << "Your Score: " << lastscore;
            SDL_Surface *lastScoreSurface = TTF_RenderText_Solid(font, lastScoreText.str().c_str(), textColor);
            SDL_Texture *lastScoreTexture = SDL_CreateTextureFromSurface(renderer, lastScoreSurface);

            int lastScoreTextWidth = lastScoreSurface->w;
            int lastScoreTextHeight = lastScoreSurface->h;
            SDL_FreeSurface(lastScoreSurface);
            int lastScoreX = (500 - lastScoreTextWidth) / 2;
            int lastScoreY = 500;

            SDL_Rect lastScoreRect = {lastScoreX, lastScoreY, lastScoreTextWidth, lastScoreTextHeight};
            SDL_RenderCopy(renderer, lastScoreTexture, nullptr, &lastScoreRect);
            SDL_DestroyTexture(lastScoreTexture);
            SDL_RenderPresent(renderer);
            SDL_Delay(3000);
            return 1;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(frameDelay);
    }

    SDL_DestroyTexture(opponentCarTexture);
    SDL_DestroyTexture(carTexture);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
