#pragma once
#ifndef __GAME__
#define __GAME__

// Core Libraries
#include <iostream>
#include <string>
#include <vector>
#include "SceneState.h"

#include <SDL.h>

// Game Managers
#include "CollisionManager.h"

// Scenes
#include "StartScene.h"
#include "PlayScene.h"
#include "EndScene.h"

#include "Config.h"

#include "ImGuiWindowFrame.h"

//F(x) = 2 * x in math says function named F is dependent on parameter x, returns x * 2, which we can assume is y
//Every Function in C++ takes 0 or more inputs, called parameters, or arguments. They also all have a return -- the output.
//Input->Function->Output, Arguments->Function->Return value
// 
//int main() <- a function delcaration that takes no parameters and returns a variable of type int
//void main() <- void is a return type which means "nothing". This function does not give us an output value when it ends.

//The class keyword allows us to define new Types of variables, and the data and functionality inside them
//The data and functions inside the curly braces used to define this class are called members
//The word "class" here means "type of thing"
//This file contains the declaration of the class and part of its definition, within the curly braces.
//The definition of its individual member functions are contained in Game.cpp
class Game
{
public: // public: marks an area containing members accessible from outside the class (like with the . member of object operator)
	// This is a Function which returns a variable of type Game. 
	// For our purposes, this will be our way of accessing the singular Game
	// Access the Game by writing the line Game::Instance().whateveryouwantfromit
	static Game& Instance() //static means this function belongs to the class and not any instance
	{
		static Game instance;
		return instance;
	}

	//Member Functions are functions which belong to a certain object of a certain type.
	//We can access them by using the . operator on any variable to access its member Functions and Variables
	//std::cout says "look inside the area std to find the thing called cout"
	//a.func() says "look inside the variable a to find the member function called func()"
	//a.var says "look inside the variable a to find the member variable called var"

	// simply set the isRunning variable to true
	void Init();
	bool Init(const char* title, int x, int y, int width, int height, bool fullscreen);

	// public life cycle functions
	void Render() const; // Draw to screen
	void Update() const; // Update game state
	void HandleEvents() const; // Input

	void Clean() const;
	void Start();
	void Quit();

	// getter and setter functions
	[[nodiscard]] glm::vec2 GetMousePosition() const;

	void SetFrames(Uint32 frames);
	[[nodiscard]] Uint32 GetFrames() const;

	[[nodiscard]] float GetDeltaTime() const;
	void SetDeltaTime(float time);

	[[nodiscard]] bool IsRunning() const;
	void ChangeSceneState(SceneState new_state);

	[[nodiscard]] SDL_Window* GetWindow() const;

	//private Member variables. These are variables which this class has access to, but others which use the class do not.
private: //private: marks a private section
	Game();
	~Game();

	// game properties
	bool m_bRunning;
	Uint32 m_frames;
	float m_deltaTime{};
	glm::vec2 m_mousePosition;

	// scene variables
	Scene* m_pCurrentScene;
	SceneState m_currentSceneState;

	// storage structures
	std::shared_ptr<SDL_Window> m_pWindow;
};


#endif /* defined (__GAME__) */

