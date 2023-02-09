#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

//This class defines what is in the main gameplay section of our game. This is where players, enemies, bullets, etc. will live
//This class has a Start(), Update() and Draw() function which are called by the framework at given times
class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override; // Called each frame we are in this scene after Update()
	virtual void Update() override; // Called each frame we are in this scene 
	virtual void Clean() override;
	virtual void HandleEvents() override; // Input
	virtual void Start() override; // Called on the first frame we are in this Scene
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	//Declaring a variable of type Plane*. This is a pointer to type Plane. Plane is a class defined in Plane.h 
	Plane* m_pPlaneSprite{};
	Player* m_pPlayer{};
	bool m_playerFacingRight{};

	// UI Items
	Button* m_pBackButton{};
	Button* m_pNextButton{};
	Label* m_pInstructionsLabel{};

	// Input Control
	int m_pCurrentInputType{};
	void GetPlayerInput();
	void GetKeyboardInput();
};

#endif /* defined (__PLAY_SCENE__) */