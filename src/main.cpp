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

//You can declare your own Namespaces to solve naming conflicts.
namespace Fund
{
	//The 'struct' keyword and 'class' keyword are used to declare new Types we can instantiate.
	struct Sprite
	{
	public: 
		//These are public fields. Member variables which compose our new type Sprite. 
		//Public means that things outside this struct or class can access them
		SDL_Texture* pTexture;
		SDL_Rect src;
		SDL_Rect dst;
		double rotationDegrees;
		SDL_RendererFlip flipState = SDL_FLIP_NONE;

		//Constructors are special Member Functions of a class or struct which are used when making objects of this type
		//The compiler knows a Constructor by the fact that it has the same name as the class and does not have a return type
		//The Constructor with no arguments is called the Default Constructor and is used when no other constructor is explicitly called
		Sprite()
		{
			std::cout << "Sprite Default Constructor!" << std::endl;
			pTexture = nullptr;
			src = { 0,0,0,0 };
			dst = { 0,0,0,0 };
		}

		//A non-default constructor can take arguments
		Sprite(SDL_Renderer* renderer, const char* imageFilePath)
		{
			std::cout << "Sprite Constructor!" << std::endl;
			pTexture = IMG_LoadTexture(renderer, imageFilePath);
			if (pTexture == NULL)
			{
				std::cout << "Image load failed! " << SDL_GetError() << std::endl;
			}
			else
			{
				std::cout << "Image load success: " << imageFilePath << std::endl;
			}

			//Query for our texture's width and height to set a default source rect value spanning the whole texture
			if (SDL_QueryTexture(pTexture, NULL, NULL, &src.w, &src.h) != 0)
			{
				std::cout << "Query Texture failed! " << SDL_GetError() << std::endl;
			}
			//At this point, SDLQueryTexture has presumably used the addresses for src.w and h to place the width and height into that memory
			src.x = 0;
			src.y = 0;

			dst.x = 0;
			dst.y = 0;
			dst.w = src.w;
			dst.h = src.h;
		}

		//We can also declare Member Functions which are "called on" instances of this struct or class
		void Draw(SDL_Renderer* renderer) 
		{
			int result = SDL_RenderCopyEx(renderer, pTexture, &src, &dst, rotationDegrees, NULL, flipState);
			if (result != 0)
			{
				std::cout << "Render failed! " << SDL_GetError() << std::endl;
			}
		}

		void SetPosition(int x, int y)
		{
			dst.x = x;
			dst.y = y;
		}
	};
}

using namespace Fund;

//Thanks to the Sprite struct, I can now make variables of this new Type
Sprite myNewSpriteObject = Sprite(); // Call the Default Constructor to initialize this Sprite variable
Sprite klingonShip;

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
	myNewSpriteObject = Sprite(pRenderer, "../Assets/textures/enterprise.png");

	int shipWidth = myNewSpriteObject.src.w / 4;
	int shipHeight = myNewSpriteObject.src.h / 4;
	myNewSpriteObject.dst.w = shipWidth;
	myNewSpriteObject.dst.h = shipHeight;
	myNewSpriteObject.dst.x = (WINDOW_WIDTH / 8); // start with the left eighth of the screen as open space
	myNewSpriteObject.dst.y = (WINDOW_HEIGHT / 2) - shipHeight/2; // exactly centered vertically

	klingonShip = Sprite(pRenderer, "../Assets/textures/d7_small.png");
	klingonShip.SetPosition(700, 400);

	klingonShip.flipState = SDL_FLIP_HORIZONTAL;
	klingonShip.rotationDegrees = 10;
}

void Input()
{
}

void Update()
{
	myNewSpriteObject.dst.x += 1;
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 20, 255);
	SDL_RenderClear(pRenderer);

	myNewSpriteObject.Draw(pRenderer); //Call the member function Draw on my Sprite object
	klingonShip.Draw(pRenderer);

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

