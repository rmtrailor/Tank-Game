#ifndef __Projectile_h_
#define __Projectile_h_

namespace Ogre {
    class Entity;
    class SceneNode;
    class SceneManager;
    class Vector3;
    class Quaternion;
    class Math;
}

class Tank;

class Projectile
{
public:
    Projectile(float initX, float initY, float initZ, Ogre::SceneManager *sceneMgr, Tank *tank);
    ~Projectile(); // important 
    void Think(float time);
    
    Ogre::Entity *getEntity() { return mProjectileMesh; }
    Ogre::SceneManager *SceneManager() { return mSceneManager; }
    void isHit() { hit = true; }
    bool hasHit() { return hit; }
    int getTankID();

protected:
    Ogre::Entity        *mProjectileMesh;
    Ogre::SceneNode     *mProjectileNode;
    Ogre::SceneManager  *mSceneManager;
    Ogre::Vector3       *initPosition;
    Ogre::Quaternion    *initOrientation;
    float               x, y, z, xVel, yVel, zVel;
    Tank                *mTank;
    bool                isFalling, hit;
    float               MAX_THRESHOLD;
    float               MAX_HEIGHT;
};

#endif