// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include "SDL_Image.h"
#include "SDL.h"

constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS;

bool isGameRunning = true;
Uint32 framesSinceGameStart = 0;
SDL_Window* pWindow = nullptr;
SDL_Renderer* pRenderer = nullptr;
SDL_Texture* pTexture;
SDL_Rect src;
SDL_Rect dst;

bool Init()
{
	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return false;
	}
	std::cout << "SDL Init success" << std::endl;

	int flags = SDL_WINDOW_SHOWN;


	// if succeeded create our window
	pWindow = SDL_CreateWindow("hello", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, flags);

	// create a new SDL Renderer and store it in the Singleton
	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


	return true;
}

void Start()
{
	const char* fileToLoad = "../Assets/textures/plane.png";
	pTexture = IMG_LoadTexture(pRenderer, fileToLoad);
	if (pTexture == nullptr)
	{
		std::cout << "Texture failed to load: " << fileToLoad;
	}

	SDL_QueryTexture(pTexture, NULL, NULL, &dst.w, &dst.h);
	src.w = dst.w;
	src.h = dst.h;
}

void Input()
{
	//SDL_PollEvent
}

void Update()
{
	//Gameplay
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	SDL_RenderClear(pRenderer);

	dst.x = 50;
	dst.y = 100;
	int result = SDL_RenderCopy(pRenderer, pTexture, &src, &dst);

	SDL_RenderPresent(pRenderer);
}

int main(int argc, char* args[])
{
	AllocConsole();
	auto console = freopen("CON", "w", stdout);
	const auto window_handle = GetConsoleWindow();
	MoveWindow(window_handle, 100, 700, 800, 200, TRUE);

	isGameRunning = Init();

	Start();

	// Main Game Loop
	while (isGameRunning)
	{
		const float frame_start = static_cast<float>(SDL_GetTicks());

		//Input
		Input();
		//Update
		Update();
		//Draw
		Draw();

		if (const float frame_time = static_cast<float>(SDL_GetTicks()) - frame_start;
			frame_time < DELAY_TIME)
		{
			SDL_Delay(static_cast<int>(DELAY_TIME - frame_time));
		}

		// delta time
		const float delta_time = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;

		framesSinceGameStart++;
	}

	SDL_Quit();

	return 0;
}

