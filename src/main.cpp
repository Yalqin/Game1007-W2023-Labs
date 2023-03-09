// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> // Allows us to use features from the SDL Library
#include <SDL_Image.h>

/*
Lab 8 (week 9): Let's get our sprite moving and able to shoot projectiles!

To get input from the keyboard we will use the function SDL_PollEvent https://wiki.libsdl.org/SDL2/SDL_PollEvent
To shoot projectiles, we may need a projectile class with velocity 
When we are ready to scale up, we can start using containers, like arrays or std::vector
*/

//All these variables are considered "Global variables" which can be accessed from any Function.
constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;

SDL_Window* pWindow = nullptr; //This is a pointer to SDL_Window. It stores a memory location which we can use later
SDL_Renderer* pRenderer = nullptr;
bool isGameRunning = true;

namespace Fund
{
	//Declaring a struct or class declares a new type of object we can make. After this is declared, we can make Sprite variables that
	//have all of the contained data fields, and functions
	struct Sprite
	{
	public:
		//public fields can be accessed from outside the struct or class
		SDL_Texture* pTexture;
		SDL_Rect src;
		SDL_Rect dst;
		double rotation = 0;
		SDL_RendererFlip flipState = SDL_FLIP_NONE;

		//This is a constructor. This is a special type of function used when creating an object.
		//The compiler knows it's a constructor because it has parentheses like a function, 
		//has the SAME NAME as the struct or class, and has no return type. 
		//This one has no arguments. In that case it's called the Default Constructor and is used to set default values
		Sprite()
		{
			std::cout << "Sprite default constructor\n";
			pTexture = nullptr;
			src = SDL_Rect{ 0,0,0,0 };
			dst = SDL_Rect{ 0,0,0,0 };
		}

		//Constructors can have arguments as well, which is handy when we need to make them different
		Sprite(SDL_Renderer* renderer, const char* filePathToLoad)
		{
			std::cout << "Sprite filepath constructor\n";
;			src = SDL_Rect{ 0,0,0,0 };
			pTexture = IMG_LoadTexture(renderer, filePathToLoad); //load into our pTexture pointer
			if (pTexture == NULL) // if failed load
			{
				std::cout << "Image failed to load: " << SDL_GetError() << std::endl;
			}
			SDL_QueryTexture(pTexture, NULL, NULL, &src.w, &src.h); //ask for the dimensions of the texture
			dst = SDL_Rect{ 0,0,src.w,src.h};
			//At this point, the width and the height of the texture should be placed at the memory addresses of src.w and src.h
		}

		//This draw function can be called on individual variables of type Fund::Sprite, 
		//which will use their own variables to call SDL_RenderCopy. So, we can declare and draw new sprites with two lines:
		//Fund::Sprite myNewSprite = Sprite(pRenderer, "somefile.png");
		//myNewSprite.Draw(pRenderer);
		void Draw(SDL_Renderer* renderer)
		{
			SDL_RenderCopyEx(renderer, pTexture, &src, &dst, rotation, NULL, flipState);
		}
	};
}

//After the declaration of the Fund::Sprite struct, we can make our own variables of this Type!
Fund::Sprite spriteStarshipEnterprise;
Fund::Sprite klingonShip1;
Fund::Sprite klingonShip2;

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

	spriteStarshipEnterprise = Fund::Sprite(pRenderer, fileToLoad);

	int shipWidth = spriteStarshipEnterprise.src.w / 4;
	int shipHeight = spriteStarshipEnterprise.src.h / 4;

	//Describe location to paste to on the screen
	spriteStarshipEnterprise.dst.w = shipWidth;
	spriteStarshipEnterprise.dst.h = shipHeight;
	spriteStarshipEnterprise.dst.x = 50;
	spriteStarshipEnterprise.dst.y = (SCREEN_HEIGHT / 2) - (shipHeight / 2);


	klingonShip1 = Fund::Sprite(pRenderer, "../Assets/textures/d7_small.png");
	klingonShip1.dst.x = 500;
	klingonShip1.dst.y = 300;
	klingonShip1.flipState = SDL_FLIP_HORIZONTAL;
	klingonShip1.rotation = 10.0;
}

void Input()
{

}

void Update()
{
	spriteStarshipEnterprise.dst.x += 2;
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);

	spriteStarshipEnterprise.Draw(pRenderer);
	klingonShip1.Draw(pRenderer);

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

