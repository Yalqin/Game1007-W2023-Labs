// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h>
#include <SDL_image.h>

/*
Lab 6: Let's break down our main to start over, opening a window, and drawing a sprite to a given location on the window
*/

//Global variables are accessible from any context
constexpr float FPS = 60.0f; //target frames per second
constexpr float DELAY_TIME = 1000.0f / FPS; // target time between frames in ms
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;

bool isGameRunning = true;

//Anything with a star to its right is a pointer type. SDL_Window* is a memory address of an SDL_Window
//Pointers can also be set to point to memory address 0, meaning "no address". We call these null pointers
//Computer memory is addressible by one number, like a home address on one very, very long street. No house is at address 0
SDL_Window* pWindow = nullptr; // assigning a pointer to nullptr means the address 0
SDL_Renderer* pRenderer = NULL; // NULL is the address 0. =NULL is practically the same as = nullptr
SDL_Texture* pMySprite = NULL;
SDL_Rect mySpriteSrc;
SDL_Rect mySpriteDst;

//Set up game window, start SDL features, etc. Returns true if successful, false otherwise.
bool Initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// Display Main SDL Window
	// Get pointer to SDL_Window object
	pWindow = SDL_CreateWindow("Joss's Lab Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0
	);
	if (pWindow == NULL) // If window address has not been set...
	{
		std::cout << "Window creation failed! " << SDL_GetError() << std::endl;
		return false;
	}
	else
	{
		std::cout << "Window creation success!\n";
	}

	//Get pointer to SDL_Renderer object to use for drawing sprites
	pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
	if (pRenderer == NULL)
	{
		std::cout << "Renderer creation failed! " << SDL_GetError() << std::endl;
		return false;
	}
	else
	{
		std::cout << "Renderer creation success!\n";
	}
	return true;
}

void Load()
{
	//Load texture...
	//SDL_Texture * IMG_LoadTexture(SDL_Renderer *renderer, const char *file);
	pMySprite = IMG_LoadTexture(pRenderer, "../Assets/textures/enterprise.png");
	if (pMySprite == NULL)
	{
		std::cout << "Image load failed! " << SDL_GetError() << std::endl;
	}
	else
	{
		std::cout << "Image load success!\n";
	}

	mySpriteSrc.x = 0;
	mySpriteSrc.y = 0;
	mySpriteSrc.w = 643;
	mySpriteSrc.h = 296;

	int shipWidth =	 mySpriteSrc.w / 4;
	int shipHeight = mySpriteSrc.h / 4;
	mySpriteDst.w = shipWidth;
	mySpriteDst.h = shipHeight;
	mySpriteDst.x = (WINDOW_WIDTH / 8); // start with the left eighth of the screen as open space
	mySpriteDst.y = (WINDOW_HEIGHT / 2) - shipHeight/2; // exactly centered vertically
}

void Input()
{
}

void Update()
{
	mySpriteDst.x = mySpriteDst.x + 1;
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 20, 255);
	SDL_RenderClear(pRenderer);

	//the & before a variable asks for that object's memory address
	int result = SDL_RenderCopy(pRenderer, pMySprite, &mySpriteSrc, &mySpriteDst);
	if (result != 0)
	{
		std::cout << "Render failed! " << SDL_GetError() << std::endl;
	}

	//Show the BackBuffer which we have been drawing to prior. This is part of a common rendering technique called Double Buffering.
	SDL_RenderPresent(pRenderer);
}

/**
 * \brief Program Entry Point
 */
int main(int argc, char* args[])
{
	// show and position the application console
	AllocConsole();
	auto console = freopen("CON", "w", stdout);
	const auto window_handle = GetConsoleWindow();
	MoveWindow(window_handle, 100, 700, 800, 200, TRUE);

	isGameRunning = Initialize();

	Load();

	// Main Game Loop, each iteration of this loop is one frame of the game
	while (isGameRunning)
	{
		//Get time in ms at the start of the frame
		const float frame_start = static_cast<float>(SDL_GetTicks());

		//Calling three functions in sequence...
		Input();	// Input from player
		Update();	// Update game state according to past state and input
		Draw();		// Draw to screen to show players new game state

		//Get time that has passed this frame
		const float frame_time = static_cast<float>(SDL_GetTicks()) - frame_start;
		if (frame_time < DELAY_TIME) // If we completed our input-update-draw in less than target time...
		{
			//Wait for the difference. Do nothing.
			SDL_Delay(static_cast<int>(DELAY_TIME - frame_time));
		}

		// delta time. In seconds, how much time has passed since start of frame this time? 
		const auto delta_time = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;
	}

	getchar();
	return 0;
}

