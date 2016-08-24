#include "MainListener.h"
#include "AIManager.h"
#include "InputHandler.h"
#include "World.h"
#include "TankCamera.h"
#include <ois.h>

/**
 * Constructor
 */
MainListener::MainListener(Ogre::RenderWindow *mainWindow, InputHandler *inputManager, AIManager *aiManager, World *world, TankCamera *cam) 
	: mRenderWindow(mainWindow), mInputHandler(inputManager), mAIManager(aiManager), mWorld(world), mTankCamera(cam)
{
	x = 0;
}

/**
 * Tick loop for the game. Checks each handlers Think function
 *
 * @param     evt    FrameEvent address
 *
 * @return           keepRunning
 */
bool MainListener::frameStarted(const Ogre::FrameEvent &evt)
{
	float time = evt.timeSinceLastFrame;

	if (time > 0.5)
	{
		time = 0.5;
	}

	mAIManager->Think(time);
	mInputHandler->Think(time);
	mWorld->Think(time);
	mTankCamera->Think(time);

	bool keepRunning = true;

	if (mInputHandler->IsKeyDown(OIS::KC_ESCAPE) || mRenderWindow->isClosed())
	{
		keepRunning = false;
	}

	return keepRunning;
}


