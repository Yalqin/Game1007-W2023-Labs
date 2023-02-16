// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> // Allows us to use features from the SDL Library
#include <SDL_Image.h>
#include "main.h"

/*
Let's use SDL to open a window, and render some sprites at given locations and scales
*/

//All these variables are considered "Global variables" which can be accessed from any Function.
constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;

SDL_Window* pWindow = nullptr; //This is a pointer to SDL_Window. It stores a memory location which we can use later
SDL_Renderer* pRenderer = nullptr;
bool isGameRunning = true;

SDL_Texture* pMySprite = nullptr; //Texture is another word for image in game programming
SDL_Rect mySpriteSrc;
SDL_Rect mySpriteDst;

//Initialize SDL, open a window and set up renderer. Returns false if failed
bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL Init Failed: " << SDL_GetError();
		return false;
	}
	std::cout << "SDL Init Success\n";

	//Create and assign our SDL_Window pointer
	pWindow = SDL_CreateWindow("Joss's Lab Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0
	);

	if (pWindow == NULL) // Check if the last operation succeeded
	{
		std::cout << "Window creation failed: " << SDL_GetError();
		return false;
	}
	else
	{
		std::cout << "Window creation success\n";
	}

	//Create and assign our SDL_Renderer pointer
	pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
	
	if (pRenderer == NULL) // If CreateRenderer failed...
	{
		std::cout << "Renderer creation failed: " << SDL_GetError();
		return false;
	}
	else
	{
		std::cout << "Renderer creation success\n";
	}

	return true;
}

void Load()
{
	char* fileToLoad = "../Assets/textures/enterprise.png";
	//SDL_Texture * IMG_LoadTexture(SDL_Renderer *renderer, const char *file);
	pMySprite = IMG_LoadTexture(pRenderer, fileToLoad);
	if (pMySprite == NULL)
	{
		std::cout << "Image failed to load: " << fileToLoad;
	}

	//Describe location to copy from in the texture
	mySpriteSrc.x = 0;
	mySpriteSrc.y = 0;
	mySpriteSrc.w = 643;
	mySpriteSrc.h = 296;

	int shipWidth = mySpriteSrc.w / 4;
	int shipHeight = mySpriteSrc.h / 4;

	//Describe location to paste to on the screen
	mySpriteDst.w = shipWidth;
	mySpriteDst.h = shipHeight;
	mySpriteDst.x = 50;
	mySpriteDst.y = (SCREEN_HEIGHT / 2) - (shipHeight / 2);
}

void Input()
{

}

void Update()
{
	mySpriteDst.x += 2;
}

void Draw()
{
		/*
		* Will use...
		* int SDL_RenderCopy(SDL_Renderer * renderer,
                   SDL_Texture * texture,
                   const SDL_Rect * srcrect,
                   const SDL_Rect * dstrect);
		*/
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);

	SDL_RenderCopy(pRenderer, pMySprite, &mySpriteSrc, &mySpriteDst);
	// RenderPresent shows the hidden space we were drawing to called the BackBuffer. 
	// For more information why we use this, look up Double Buffering Rendering
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

	// Display Main SDL Window
	isGameRunning = Init();

	Load();

	// Main Game Loop, one iteration of this loop is one game frame
	while (isGameRunning)
	{

		//time at the start of the frame, in ms
		const auto frame_start = static_cast<float>(SDL_GetTicks());

		Input();  // take player input

		Update(); // update game state (presumably based on other conditions and input)

		Draw();  // draw to screen to show new game state to player

		//Figure out how long we need to wait for the next frame timing.
		//Current time - time at start of frame = time elapsed during this frame
		const float frame_time = static_cast<float>(SDL_GetTicks()) - frame_start;
		if (frame_time < DELAY_TIME) // if time passed is less than the time we have for each frame...
		{
			//Do nothing for a while until it's time for the next frame
			SDL_Delay(static_cast<int>(DELAY_TIME - frame_time));
		}

		// delta time. Time elapsed this frame, in seconds
		const float delta_time = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;
	}

	return 0;
}

