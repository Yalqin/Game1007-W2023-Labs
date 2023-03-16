// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> // Allows us to use features from the SDL Library
#include <SDL_Image.h>
#include <vector> //std::vector is an array with variable size

/*
 Lab 9 (week 10)
Improve movement precision by storing floating point position so we can move distances less than a pixel in one frame
Improve encapsulation by hiding the position behind a getter and setter function
Encapsulation means "hiding details that we don't need to worry about"

Allow us to load a sprite sheet image, then draw one frame at a time from the image
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
	struct Vec2 // This struct represents a 2-dimensional vector
	{
		float x = 0;
		float y = 0;
	};

	//Declaring a struct or class declares a new type of object we can make. After this is declared, we can make Sprite variables that
	//have all of the contained data fields, and functions
	struct Sprite
	{
	private: //private fields cannot be accessed outside the struct or class
			
		SDL_Texture* pTexture;
		SDL_Rect src;
		SDL_Rect dst;
	//	int animationCurrentFrame = 0;
	//	float animationTime01 = 0.0f;//0 is frame 0, 1 is frameCount, multiply by frameCount to get currentFrame e.g. 0 * frameCount = 0. 1 * frameCount = 5 //Option A

		float animationCurrentFrame = 0.0f; // same as storing which frame, but must be converted back to int to draw (e.g. frame 3.4 becomes frame 3) // Option B

	public: //public fields can be accessed from outside the struct or class
		double rotation = 0;
		SDL_RendererFlip flipState = SDL_FLIP_NONE;
		Vec2 position; // position of the sprite on the screen
		int animationFrameCount = 1;

		//Getter and setter functions
		//Functions which change the state of a struct or class are sometimes called Mutators, or setters
		void SetSize(Vec2 sizeWidthHeight)
		{
			dst.w = sizeWidthHeight.x;
			dst.h = sizeWidthHeight.y;
		}

		//This and the other SetSize have the exact same name, but take different data. The correct one will be chosen by the compiler based on what parameters you put in it
		void SetSize(int w, int h)
		{
			dst.w = w;
			dst.h = h;
		}

		//Functions which access data from within are sometimes called Accessors, or getters
		Vec2 GetSize()
		{
			Vec2 returnVec = {dst.w,dst.h};
			return returnVec;
		}

		//For animation, we want to know the dimensions of one frame of animation
		void SetSpriteSheetFrameSize(int width, int height)
		{
			src.w = width;
			src.h = height;
		}
		
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
			//At this point, the width and the height of the texture should be placed at the memory addresses of src.w and src.h
			dst = SDL_Rect{ 0,0,src.w,src.h};
		}

		Sprite(SDL_Renderer* renderer, const char* filePathToLoad, int frameSizeX, int frameSizeY, int frameCount) : Sprite(renderer, filePathToLoad)
		{
			SetSpriteSheetFrameSize(frameSizeX, frameSizeY);
			SetSize(frameSizeX, frameSizeY);
			animationFrameCount = frameCount;
		}

		//This draw function can be called on individual variables of type Fund::Sprite, 
		//which will use their own variables to call SDL_RenderCopy. So, we can declare and draw new sprites with two lines:
		//Fund::Sprite myNewSprite = Sprite(pRenderer, "somefile.png");
		//myNewSprite.Draw(pRenderer);
		void Draw(SDL_Renderer* renderer)
		{
			dst.x = position.x;
			dst.y = position.y;
			src.x = src.w * (int)animationCurrentFrame;
			SDL_RenderCopyEx(renderer, pTexture, &src, &dst, rotation, NULL, flipState);
		}

		void NextFrame()
		{
			SetFrame(animationCurrentFrame + 1);
		}

		void SetFrame( int frame)
		{
			animationCurrentFrame = frame % animationFrameCount;
			src.x = src.w * animationCurrentFrame;
		}

		void AddFrameTime(float timeScale) // Call this in game update loop
		{
			//game FPS = 60
			//deltaTime = 1/60 seconds
			//sprite target FPS = 12
			//So animationCurrentFrame should go up by some amount each frame such that it increases by 1, 12 times per second
			//NextFrame();
			animationCurrentFrame += timeScale;
			if (animationCurrentFrame >= animationFrameCount)
			{
				animationCurrentFrame -= animationFrameCount;
			}
		}
	};
}

//After the declaration of the Fund::Sprite struct, we can make our own variables of this Type!
Fund::Sprite spriteStarshipEnterprise;
Fund::Sprite klingonShip1;

Fund::Sprite heli; //For animation lab

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

	Fund::Vec2 shipSize = spriteStarshipEnterprise.GetSize();
	int shipWidth = shipSize.x / 4;
	int shipHeight = shipSize.y / 4;

	//Describe location to paste to on the screen
	spriteStarshipEnterprise.SetSize(shipWidth, shipHeight);

	spriteStarshipEnterprise.position.x = 50;
	spriteStarshipEnterprise.position.y = (SCREEN_HEIGHT / 2) - (shipHeight / 2);

	klingonShip1 = Fund::Sprite(pRenderer, "../Assets/textures/d7_small.png");
	klingonShip1.position = { 500, 300 };
	klingonShip1.flipState = SDL_FLIP_HORIZONTAL;
	klingonShip1.rotation = 10.0;

	//helicopter.png is a sprite sheet, so we made a new constructor for that type of loading
	int heliWidth = 128;
	int heliHeight = 55;
	int heliFrameCount = 5;
	heli = Fund::Sprite(pRenderer, "../Assets/textures/helicopter.png", heliWidth, heliHeight, heliFrameCount);
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
		spriteStarshipEnterprise.position.y -= ((playerMoveSpeedPxPerSec * deltaTime) + 0.5f);
	}

	if (isDownPressed)
	{
		spriteStarshipEnterprise.position.y += ((playerMoveSpeedPxPerSec * deltaTime) + 0.5f);
	}

	//If shooting and our shooting is off cooldown
	if (isShootPressed && playerFireCooldownTimerSec <= 0.0f)
	{
		std::cout << "Shoot\n";
		
		//Create new bullet
		Fund::Sprite bullet = Fund::Sprite(pRenderer, "../Assets/textures/bullet.png");

		//Start bullet at player sprite position
		bullet.position.x = spriteStarshipEnterprise.position.x + spriteStarshipEnterprise.GetSize().x;
		bullet.position.y = spriteStarshipEnterprise.position.y + (spriteStarshipEnterprise.GetSize().y/2) - (bullet.GetSize().y / 2);
		
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

		someBullet->position.x += bulletSpeedPx * deltaTime;
	}

	//animate helicopter
	heli.AddFrameTime(0.2);
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
	heli.Draw(pRenderer);

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

