// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> // Allows us to use features from the SDL Library
#include <SDL_Image.h>
#include <vector> //std::vector is an array with variable size

/*
Lab 8 (week 9): Let's get our sprite moving and able to shoot projectiles!

To get input from the keyboard we will use the function SDL_PollEvent https://wiki.libsdl.org/SDL2/SDL_PollEvent
To shoot projectiles, we may need a projectile class with velocity 
When we are ready to scale up, we can start using containers, like arrays or std::vector
*/

//All these variables are considered "Global variables" which can be accessed from any Function.
constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS; // target deltaTime in ms
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;

//At 60FPS, how much time passes each frame?
//60 frames per second. Every second, we pass 60 frames
//How many seconds pass for each frame?
float deltaTime = 1.0f / FPS; // time passing between frames in seconds

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
//Fund::Sprite bullet;
std::vector<Fund::Sprite> bulletContainer; //std::vector is a class which allows changing size. This is a dynamic array of Fund::Sprite


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// INIT ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// LOAD ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// INPUT ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Input variables
bool isUpPressed = false;
bool isDownPressed = false;
bool isShootPressed = false;
float playerMoveSpeedPxPerSec = 65.0f; // Px/sec, px/sec * sec = px
float playerFireRepeatDelaySec = 1.0; // seconds
float playerFireCooldownTimerSec = 0.0f; // seconds
float bulletSpeedPx = 1000.0f; // Px/sec

void Input()
{
	SDL_Event event; // Event data polled each time
	while (SDL_PollEvent(&event))  // poll until all events are handled!
	{ 
		// decide what to do with this event.
		switch (event.type)
		{
		case(SDL_KEYDOWN):
		{
			SDL_Scancode key = event.key.keysym.scancode;
			switch (key)
			{
			case(SDL_SCANCODE_W):
			{
				isUpPressed = true;
				break;
			}
			case(SDL_SCANCODE_S):
			{
				isDownPressed = true;
				break;
			}
			case(SDL_SCANCODE_SPACE):
			{
				isShootPressed = true;
				break;
			}
			break;
			}
			break;
		}
		case(SDL_KEYUP):
		{
			SDL_Scancode key = event.key.keysym.scancode;
			switch (key)
			{
			case(SDL_SCANCODE_W):
			{
				isUpPressed = false;
				break;
			}
			case(SDL_SCANCODE_S):
			{
				isDownPressed = false;
				break;
			}
			case(SDL_SCANCODE_SPACE):
			{
				isShootPressed = false;
				break;
			}
			}
			break;
		}
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// UPDATE //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Update() // called every frame at FPS
{
	//spriteStarshipEnterprise.dst.x += 2;
	if (isUpPressed)
	{
		spriteStarshipEnterprise.dst.y -= ((playerMoveSpeedPxPerSec * deltaTime) + 0.5f);
	}

	if (isDownPressed)
	{
		spriteStarshipEnterprise.dst.y += ((playerMoveSpeedPxPerSec * deltaTime) + 0.5f);
	}

	//If shooting and our shooting is off cooldown
	if (isShootPressed && playerFireCooldownTimerSec <= 0.0f)
	{
		std::cout << "Shoot\n";
		
		//Create new bullet
		Fund::Sprite bullet = Fund::Sprite(pRenderer, "../Assets/textures/bullet.png");

		//Start bullet at player sprite position
		bullet.dst.x = spriteStarshipEnterprise.dst.x + spriteStarshipEnterprise.dst.w;
		bullet.dst.y = spriteStarshipEnterprise.dst.y + (spriteStarshipEnterprise.dst.h/2) - (bullet.dst.h/2);
		
		//Add bullet to container (to the end of the array)
		bulletContainer.push_back(bullet);

		//Reset cooldown
		playerFireCooldownTimerSec = playerFireRepeatDelaySec;
	}
	//Tick down the time for our firing cooldown
	playerFireCooldownTimerSec -= deltaTime;

	//move ALL bullets across screen
	for (int i = 0; i < bulletContainer.size(); i++)
	{
		//To access elements of an array, we use the array access operator []

		//Get a reference to the bullet in the container
		Fund::Sprite* someBullet = &bulletContainer[i];

		someBullet->dst.x += bulletSpeedPx * deltaTime;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DRAW ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);

	//Draw ALL bullets on screen
	for (int i = 0; i < bulletContainer.size(); i++)
	{
		Fund::Sprite* someBullet = &bulletContainer[i];
		someBullet->Draw(pRenderer);
	}

	spriteStarshipEnterprise.Draw(pRenderer);
	klingonShip1.Draw(pRenderer);

	// RenderPresent shows the hidden space we were drawing to called the BackBuffer. 
	// For more information why we use this, look up Double Buffering Rendering
	SDL_RenderPresent(pRenderer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// MAIN  ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		//deltaTime = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;
	}

	return 0;
}

