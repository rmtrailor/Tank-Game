#ifndef __Tank_h_
#define __Tank_h_

namespace Ogre {
    class Entity;
    class SceneNode;
    class SceneManager;
    class AxisAlignedBox;
    class Vector3;
    class Quaternion;
}

class World;
class ProjectileManager;

class Tank
{
public:
    Tank(float initX, float initZ, Ogre::SceneManager* mSceneMgr, int id, Tank **tanks, World *world, 
        ProjectileManager *projectileManager);
    ~Tank();

    Ogre::Entity    *getEntity() { return tankMesh; }
    Ogre::SceneNode *getSceneNode() { return mTankNode; }
    Ogre::SceneManager *SceneManager() { return mSceneManager; }

    Ogre::Entity    *getBodyMesh()   { return bodyMesh;   } 
    Ogre::Entity    *getWheelsMesh() { return wheelsMesh; } 
    Ogre::Entity    *getCannonMesh() { return cannonMesh; } 
    Ogre::Entity    *getGunMesh()    { return gunMesh;    }

    Ogre::SceneNode *getBodyNode()   { return bodyNode;   }
    Ogre::SceneNode *getWheelsNode() { return wheelsNode; }
    Ogre::SceneNode *getCannonNode() { return cannonNode; }

    void setMeshToDestroyed();
    void resetMesh();


    int getHealth();
    void heal(int amount);
    void damage(int amount);
    void move(int direction);
    void turn(int direction);
    void rotateTurret(int direction);
    bool isDead();
    void fire(float time, float x, float y, float z, Tank *mTank);
    void addRespawnTime(float time) { respawnTime += time; }
    bool respawnCheck();
    bool isRespawning() { return respawning; }

    void setTankPosition(Ogre::Vector3 vect);
    void setTankOrientation(Ogre::Quaternion quat);
    bool checkCollision(int direction);
    bool tankCollision(Tank *otherTank);
    bool worldCollision(Ogre::Entity *worldObj);
    bool projectileCollision(Ogre::Entity *projObj);
    //const Ogre::AxisAlignedBox *getAAB() { return myBox; }
    //void setBox(Ogre::AxisAlignedBox box) { myBox = &box; }

    float getX()            { return x; }
    float getZ()            { return z; }
    float getXVel()         { return xVel; }
    float getZVel()         { return zVel; }
    float getAngVelocity()  { return angVelocity; }
    int   getID()           { return mID; }

    void setXVel(float vel) { xVel = vel; }
    void setZVel(float vel) { zVel = vel; }

protected:
    Ogre::Entity        *tankMesh, *bodyMesh, *wheelsMesh, *cannonMesh, *gunMesh;
    Ogre::SceneNode     *mTankNode, *bodyNode, *wheelsNode, *cannonNode;
    Ogre::SceneManager  *mSceneManager;

    World *mWorld;
    ProjectileManager   *mProjectileManager;

    Tank **mTanks;
    int health, mID;
    float x, z;
    float xVel, zVel, angVelocity;
    float respawnTime, respawnTimeLimit;
    bool left, right, front, back;
    bool respawning;
};

#endif