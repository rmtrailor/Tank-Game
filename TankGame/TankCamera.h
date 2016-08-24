#ifndef __TankCamera_h_
#define __TankCamera_h_


namespace Ogre
{
    class Camera;
    class Vector3;
    class Quaternion;
}
class World;
class InputHandler; 
class Tank;

class TankCamera
{
public:
	TankCamera(Ogre::Camera *renderCamera, World *world, InputHandler *input);
	void            Think(float time);
    void            updateCamera(float newX, float newY, float newZ, float pXVel, float pYVel, float pZVel, Ogre::SceneNode *pTank);
    void            turnCamera(Ogre::Quaternion orientation);

    void            addPlayer(Tank *pTank) { mPlayerTank = pTank; }
    Ogre::Camera*   getRenderCamera() { return mRenderCamera; }

protected:
	Ogre::Camera    *mRenderCamera;
	World           *mWorld;
	InputHandler    *mInputHandler;
    Tank            *mPlayerTank;
    Ogre::SceneNode *mTank;
    float           x, y, z, playerX, playerY, playerZ, xVel, yVel, zVel;
};

#endif