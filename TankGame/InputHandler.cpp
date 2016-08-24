#include "InputHandler.h"
#include "Ogre.h"
#include "OgreStringConverter.h"
#include <OIS/OIS.h>

#include <stdio.h>

/**
 * Input Handler for the game.
 *
 * Given the pointer to the render window for the application.
 */
InputHandler::InputHandler(Ogre::RenderWindow *renderWindow)
	: mRenderWindow(renderWindow)
{
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	renderWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mCurrentKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));
}

/**
 * Deconstructor
 */
InputHandler::~InputHandler()
{
	mInputManager->destroyInputObject(mCurrentKeyboard);
}


/**
 * Registers when a key is pressed down
 *
 * @param     key    The keycode of the pressed key
 *
 * @return           True if the key is pressed down
 */
bool InputHandler::IsKeyDown(OIS::KeyCode key)
{
	return mCurrentKeyboard->isKeyDown(key);
}

/**
 * Registers if a key was pressed down
 *
 * @param     key    The keycode of the pressed key
 *
 * @return           True if the key is pressed down
 */
bool InputHandler::WasKeyDown(OIS::KeyCode key)
{
	return mOldKeys[key] != '\0';
}

/**
 * Tick function for the input handler
 *
 * @param    time    Tick time
 */
void InputHandler::Think(float time)
{
	mCurrentKeyboard->copyKeyStates(mOldKeys);
	mCurrentKeyboard->capture();
}