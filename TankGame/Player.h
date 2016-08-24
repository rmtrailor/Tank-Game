#ifndef __Player_h_
#define __Player_h_

namespace Ogre {
    class SceneNode;
    class SceneManager;
}

class Tank;
class TankCamera;
class InputHandler;
class ProjectileManager;
class World;



class Player
{
public:
    Player(Tank *pTank, TankCamera *cam, InputHandler *input, ProjectileManager *projMger, World *world);
    ~Player();

    void checkMovement(float time);
    void addAmmo(int amount);
    int  getAmmo() { return ammo; }

protected:
    World               *mWorld;
    Tank                *mTank;
    TankCamera          *mCamera;
    InputHandler        *mInputHandler;
    ProjectileManager   *mProjectileManager;
    float               x, y, z, xVel, zVel;
    float               lastTime, totalTime;
    int                 ammo, count;
};


#endif