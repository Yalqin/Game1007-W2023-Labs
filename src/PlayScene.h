#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

//This is the main game world. Example shows megaman and airplane
//Update() here is likely where your gameplay code will happen
//Start() here is likely where you will set initial conditions of the game
class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override; //Called end of each frame while in this Scene
	virtual void Update() override; //Called each frame after HandleEvents
	virtual void Clean() override;
	virtual void HandleEvents() override; // Called at the start of each frame
	virtual void Start() override; // Called on the first frame when in this Scene
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

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