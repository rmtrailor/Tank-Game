#ifndef __MainListener_h_
#define __MainListener_h_

#include "Ogre.h"
#include "OgreFrameListener.h"


//
namespace Ogre
{
	class RenderWindow;
}

class InputHandler;
class AIManager;
class World;
class TankCamera;

class MainListener : public Ogre::FrameListener
{
public:
	MainListener(Ogre::RenderWindow *mainWindow, InputHandler *inputHandler, AIManager *aiManager, World *world, TankCamera *cam);

	bool frameStarted(const Ogre::FrameEvent &evt);


protected:
	InputHandler       *mInputHandler;
	AIManager          *mAIManager;
	World              *mWorld;
	TankCamera         *mTankCamera;
	Ogre::RenderWindow *mRenderWindow;
	int                x;
};


#endif