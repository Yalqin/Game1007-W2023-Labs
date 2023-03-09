// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <vector> //std::vector allows us to make arrays which change size https://en.cppreference.com/w/cpp/container/vector

/*
Lab 8 (week 9): Let's get our sprite moving and able to shoot projectiles!
To get input from the keyboard we will use the function SDL_PollEvent https://wiki.libsdl.org/SDL2/SDL_PollEvent
To shoot projectiles, we may need a projectile class with velocity
When we are ready to scale up, we can start using containers, like arrays or std::vector
*/

//Global variables are accessible from any context
constexpr float FPS = 60.0f; //target frames per second
constexpr float DELAY_TIME = 1000.0f / FPS; // target time between frames in ms
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;
float deltaTime = 1.0f / FPS;

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
		double rotationDegrees = 0.0;
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
Sprite playerShip = Sprite(); // Call the Default Constructor to initialize this Sprite variable
Sprite klingonShip; 

// When declaring an std::vector, you specify what Type it contains in the angle brackets.
std::vector<Sprite> bullets; //Sprites representing bullets get put into this container


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
	playerShip = Sprite(pRenderer, "../Assets/textures/enterprise.png");

	int shipWidth = playerShip.src.w / 4;
	int shipHeight = playerShip.src.h / 4;
	playerShip.dst.w = shipWidth;
	playerShip.dst.h = shipHeight;
	playerShip.dst.x = (WINDOW_WIDTH / 8); // start with the left eighth of the screen as open space
	playerShip.dst.y = (WINDOW_HEIGHT / 2) - shipHeight / 2; // exactly centered vertically

	klingonShip = Sprite(pRenderer, "../Assets/textures/d7_small.png");
	klingonShip.SetPosition(700, 400);

	klingonShip.flipState = SDL_FLIP_HORIZONTAL;
	klingonShip.rotationDegrees = 10;
}

//Player input variables
bool isUpPressed = false;
bool isDownPressed = false;
bool isShootPressed = false;
const float playerMoveSpeedPx = 600.0f; //pixels per second 
const float playerShootCooldownDuration = 0.1f; //time between shots
float playerShootCooldownTimer = 0.0f; //ticks down to determine when we can shoot again
const float bulletSpeed = 1000.0f; //pixels per second

void Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{  // poll until all events are handled!
		// decide what to do with this event.

		switch (event.type)
		{
		case(SDL_KEYDOWN): //If a key was pressed...
			//Respond to keys differently
			switch (event.key.keysym.scancode)
			{
			case(SDL_SCANCODE_W):
				isUpPressed = true; // Keep track of the held state of the input
				break;
			case(SDL_SCANCODE_S):
				isDownPressed = true;
				break;
			case(SDL_SCANCODE_SPACE):
				isShootPressed = true;
				break;
			}
			break;
		case(SDL_KEYUP):
			switch (event.key.keysym.scancode)
			{
			case(SDL_SCANCODE_W):
				isUpPressed = false; // When released, update the held state of our input
				break;
			case(SDL_SCANCODE_S):
				isDownPressed = false;
				break;
			case(SDL_SCANCODE_SPACE):
				isShootPressed = false;
				break;
			}
			break;
		}
	}
}

void Update()
{
	//Move player ship accord
	if (isUpPressed)
	{
		playerShip.dst.y -= playerMoveSpeedPx * deltaTime; // px/s * deltaTime = px moved this frame
	}
	if (isDownPressed)
	{
		playerShip.dst.y += playerMoveSpeedPx * deltaTime;
	}

	if (isShootPressed)
	{
		//If player shoot is off cooldown
		if (playerShootCooldownTimer <= 0.0f)
		{
			//Make a new bullet
			Sprite bullet = Sprite(pRenderer, "../Assets/textures/bullet.png");

			//Set bullet start position
			bullet.dst.x = playerShip.dst.x + playerShip.dst.w - bullet.dst.w;
			bullet.dst.y = playerShip.dst.y + (playerShip.dst.h / 2) - (bullet.dst.h/2);

			//Add bullet to our dynamic array of bullets. 
			bullets.push_back(bullet); //So far we have not yet dealt with getting rid of them afterward...

			//Reset cooldown timer
			playerShootCooldownTimer = playerShootCooldownDuration; 
		}
	}

	//Tick down shoot cooldown
	playerShootCooldownTimer -= deltaTime;

	//std::vector::size() tells us how many elements are in it
	for (int i = 0; i < bullets.size(); i++) //For each item in the container...
	{
		//Move each bullet
		bullets[i].dst.x += bulletSpeed * deltaTime;
	}

}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 20, 255);
	SDL_RenderClear(pRenderer);

	//Draw all our bullets
	for (int i = 0; i < bullets.size(); i++) //For each item in the container...
	{
		bullets[i].Draw(pRenderer); // Draw each one
	}

	playerShip.Draw(pRenderer); //Call the member function Draw on my Sprite object
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
		// deltaTime = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;
	}

	getchar();
	return 0;
}

