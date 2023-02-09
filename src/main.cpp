// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>

#include "Game.h" //TODO: What is this?

constexpr float FPS = 60.0f; //target frames per second
constexpr float DELAY_TIME = 1000.0f / FPS; // target time between frames in ms

/**
 * \brief Program Entry Point
 */
int main(int argc, char* args[])
{
	Uint32 frames = 0;

	// show and position the application console
	AllocConsole();
	auto console = freopen("CON", "w", stdout);
	const auto window_handle = GetConsoleWindow();
	MoveWindow(window_handle, 100, 700, 800, 200, TRUE);

	// Display Main SDL Window
	Game::Instance().Init("SDL_Engine v0.29", 100, 100, 800, 600, false);

	// Main Game Loop, each iteration of this loop is one frame of the game
	while (Game::Instance().IsRunning())
	{
		//Get time in ms at the start of the frame
		const float frame_start = static_cast<float>(SDL_GetTicks());

		//Calling three functions in sequence...
		Game::Instance().HandleEvents();	// Input from player
		Game::Instance().Update();			// Update game state according to past state and input
		Game::Instance().Render();			// Draw to screen to show players new game state

		//Get time that has passed this frame
		const float frame_time = static_cast<float>(SDL_GetTicks()) - frame_start;
		if (frame_time < DELAY_TIME) // If we completed our input-update-draw in less than target time...
		{
			//Wait for the difference. Do nothing.
			SDL_Delay(static_cast<int>(DELAY_TIME - frame_time));
		}

		// delta time. In seconds, how much time has passed since start of frame this time? 
		const auto delta_time = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;
		
		//Setting deltaTime variable to be used in Game?
		Game::Instance().SetDeltaTime(delta_time);

		frames++; // count frames passed since first frame
		Game::Instance().SetFrames(frames);

	}

	Game::Instance().Clean();
	return 0;
}

