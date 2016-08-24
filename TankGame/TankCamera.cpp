#include "OgreCamera.h"
#include "TankCamera.h"
#include "World.h"
#include "OgreVector3.h"
#include "InputHandler.h"
#include "Tank.h"
#include <iostream>

/**
 * Constructor
 */
TankCamera::TankCamera(Ogre::Camera *renderCamera, World *world, InputHandler *input)
	: mRenderCamera(renderCamera), mWorld(world), mInputHandler(input)
{
	mRenderCamera->setNearClipDistance(3);
}

void TankCamera::Think(float time)
{

}

/**
 * Updates the camera position based on the tank's location.
 *
 * @param    newX     New x-coord
 * @param    newY     New y-coord
 * @param    newZ     New z-coord
 * @param    pXVel    New x-velocity
 * @param    pYVel    New y-velocity
 * @param    pZVel    New z-velocity
 * @param    pTank    Pointer to the scenenode for the player tank
 */
void TankCamera::updateCamera(float newX, float newY, float newZ, float pXVel, float pYVel, float pZVel, Ogre::SceneNode *pTank)
{
    x       = newX;
    y       = newY;
    z       = newZ;
    xVel = pXVel;
    yVel = pYVel;
    zVel = pZVel;
    mTank = pTank;

    Ogre::Vector3 newCameraPosition(x, y, z);
    Ogre::Vector3 newPosition(newCameraPosition + mTank->getOrientation() * Ogre::Vector3(xVel, yVel, zVel));
    mRenderCamera->setPosition(newPosition);
    mRenderCamera->lookAt(mTank->getPosition());
}


void TankCamera::turnCamera(Ogre::Quaternion orientation)
{
    
}
