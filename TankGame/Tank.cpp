#include "Tank.h"

#include "World.h"
#include "ProjectileManager.h"
#include "CollisionManager.h"

#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

/**
 * Constructor
 */
Tank::Tank(float initX, float initZ, Ogre::SceneManager* mSceneMgr, int id, Tank **tanks, World *world, ProjectileManager *projectileManager) 
    : x(initX), z(initZ), mSceneManager(mSceneMgr), mID(id), mTanks(tanks), mWorld(world), mProjectileManager(projectileManager)
{
    health      = 100;
    xVel        = 0.04f;
    zVel        = 0.04f;
    angVelocity = 0.3f;

    front = true;
    back  = true;
    left  = true;
    right = true;

    tankMesh = SceneManager()->createEntity("BodyGood.mesh");
    cannonMesh = SceneManager()->createEntity("TurretGood.mesh");
    gunMesh = SceneManager()->createEntity("GunGood.mesh");

    mTankNode = SceneManager()->getRootSceneNode()->createChildSceneNode();
    mTankNode->attachObject(tankMesh);
    
    cannonNode = SceneManager()->getRootSceneNode()->createChildSceneNode();
    cannonNode->attachObject(cannonMesh);
    cannonNode->attachObject(gunMesh);

    mTankNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)));
    cannonNode->setOrientation(mTankNode->getOrientation());

    respawning = false;
}

/**
 * Deconstructor
 */
Tank::~Tank()
{
    delete mTankNode;
    delete wheelsNode;
    delete cannonNode;
}

/**
 * Sets the Tank to display a destroyed mesh when destroyed
 */
void Tank::setMeshToDestroyed()
{
    // Just in case this function is called when it shouldn't be
    if (respawning)
    {
        return;
    }


    mTankNode->detachAllObjects();
    bodyMesh = SceneManager()->createEntity("TankDestroyed.mesh");
    mTankNode->attachObject(bodyMesh);

    cannonNode->detachAllObjects();

    mTankNode->setPosition(mTankNode->getPosition().x, 2.5, mTankNode->getPosition().z);

    respawnTime = 0;
    respawnTimeLimit = 10;
    respawning = true;
}

/**
 * Resets the Tank mesh to the normal one when the tank respawns.
 */
void Tank::resetMesh()
{
    // Just in case this function is called when it shouldn't be
    if (!respawning)
    {
        return;
    }

    mTankNode->detachAllObjects();
    bodyMesh = SceneManager()->createEntity("BodyGood.mesh");
    mTankNode->attachObject(bodyMesh);

    cannonNode->attachObject(cannonMesh);
    cannonNode->attachObject(gunMesh);

    health = 100;

    respawning = false;
}

/**
 * If the tank is destroyed, checks if the tank should respawn.
 *
 * @return    Whether the tank should respawn or not
 */
bool Tank::respawnCheck()
{
    if (respawnTime >= respawnTimeLimit)
    {
        return true; // can respawn
    }
    return false;
}


/**
 * Gets the health of the tank.
 *
 * @return    Tank's health
 */
int Tank::getHealth()
{
    return health;
}

/**
 * Heals the tank a certain amount. Cannot bring the player's health over 100
 *
 * @param    amount    The amount to heal or add to the player's health
 */
void Tank::heal(int amount)
{
    health += amount;

    // Lock health to 100 max, possibly change later for power ups
    if (health > 100)
    {
        health = 100;
    }
}

/**
 * Dameages the tank a certain amount. Cannot bring the player's health below 0.
 *
 * @param    amount    The amount to damage or deduct from the player's health
 */
void Tank::damage(int amount)
{
    health -= amount;

    if (health < 0)
    {
        health = 0;
    }
}

/**
 * Move the tank in a specified direction.
 *
 * @param    direction    > 0 forward
 *                        < 0 backwards
 */
void Tank::move(int direction)
{
    if (isDead())
    {
        return;
    }

    if (direction > 0) // move future forward
    {
        z += 5 * zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(2 * -xVel, 0, 0));
        setTankPosition(newPosition);
    }
    else if (direction < 0) // < 0, move backwards 
    {
        z -= 5 * zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(2 * xVel, 0, 0));
        setTankPosition(newPosition);
    }

    bool collided = checkCollision(direction);

    if (direction > 0 && !collided) // move forward
    {
        z += zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(-xVel, 0, 0));
        setTankPosition(newPosition);
    }
    else if (direction > 0 && collided) // move forward
    {
        z -= zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(2 * xVel, 0, 0));
        setTankPosition(newPosition);
    }
    else if (direction < 0 && !collided) // < 0, move backwards 
    {
        z -= zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(xVel, 0, 0));
        setTankPosition(newPosition);
    }
    else if (direction < 0 && collided) // < 0, move backwards 
    {
        z += zVel;
        Ogre::Vector3 newPosition = (mTankNode->getPosition() + mTankNode->getOrientation() * Ogre::Vector3(2 * -xVel, 0, 0));
        setTankPosition(newPosition);
    }
}

/**
 * Sets the tank position.
 *
 * @param    vect    The vector representing the position
 */
void Tank::setTankPosition(Ogre::Vector3 vect)
{
    mTankNode->setPosition(vect.x, vect.y, vect.z);
    cannonNode->setPosition(vect.x, vect.y + 2.5, vect.z);
}

/**
 * Checks the collisions from a particular direction.
 *
 * @param     direction    The direction of collision
 *
 * @return                 Whether or no there was a collision
 */
bool Tank::checkCollision(int direction)
{
    bool collided = false;
    for (int i = 0; i < 12; i++)
    {   
        if (mTanks[i] != nullptr) 
        {
            if (i != mID)
            {          
                collided = tankCollision(mTanks[i]);

                if (collided) 
                {
                    return collided;
                }
            }
        }

    }

    Ogre::Entity** walls = mWorld->getWallMeshes();

    for (int i = 0; i < 4; i++)
    {
       if (walls[i] != nullptr)
       {
           collided = worldCollision(walls[i]);
        
           if (collided)
           {
               return collided;
           }
       }
    }


    return collided;
}

/**
 * Checks if the tank collided with another tank
 *
 * @param     otherTank    Pointer to the other tank
 *
 * @return                 Whether or not there was a collision
 */
bool Tank::tankCollision(Tank *otherTank)
{
    Ogre::AxisAlignedBox myBodyBox = this->getEntity()->getWorldBoundingBox();
    Ogre::AxisAlignedBox myCannonBox = this->getCannonMesh()->getWorldBoundingBox();
    Ogre::AxisAlignedBox myGunBox = this->getGunMesh()->getWorldBoundingBox();

    Ogre::AxisAlignedBox theirBodyBox = otherTank->getEntity()->getWorldBoundingBox();
    Ogre::AxisAlignedBox theirCannonBox = otherTank->getCannonMesh()->getWorldBoundingBox();
    Ogre::AxisAlignedBox theirGunBox = otherTank->getGunMesh()->getWorldBoundingBox();

    if (myBodyBox.intersects(theirBodyBox))
    {
        return true;
    }
    else if (myCannonBox.intersects(theirCannonBox))
    {
        return true;
    }
    else if (myGunBox.intersects(theirGunBox))
    {
        return true;
    }


    return false;
}

/**
 * Checks for collisions with anything in the world (environment collision)
 *
 * @param     worldObj    Pointer to the world object
 *
 * @return                Whether or not there was a collision
 */
bool Tank::worldCollision(Ogre::Entity *worldObj)
{
    Ogre::AxisAlignedBox myBodyBox = this->getEntity()->getWorldBoundingBox();
    Ogre::AxisAlignedBox objBox = worldObj->getWorldBoundingBox();

    if (myBodyBox.intersects(objBox))
    {
        return true;
    }

    return false;
}

/**
 * Checks for collisions with a projectile
 *
 * @param     projObj    Pointer to the projectile object
 *
 * @return               Whether or not there was a collision
 */
bool Tank::projectileCollision(Ogre::Entity *projObj)
{
    Ogre::AxisAlignedBox myBodyBox = this->getEntity()->getWorldBoundingBox();
    Ogre::AxisAlignedBox myCannonBox = this->getCannonMesh()->getWorldBoundingBox();
    Ogre::AxisAlignedBox myGunBox = this->getGunMesh()->getWorldBoundingBox();

    Ogre::AxisAlignedBox objBox = projObj->getWorldBoundingBox();

    if (myBodyBox.intersects(objBox))
    {
        return true;
    }
    else if (myGunBox.intersects(objBox))
    {
        return true;
    }
    else if (myCannonBox.intersects(objBox))
    {
        return true;
    }

    return false;
}

/**
 * Turns the tank
 *
 * @param    direction    The direction to be turned
 */
void Tank::turn(int direction)
{
    if (isDead())
    {
        return;
    }
    
    if (direction > 0) // turn right
    {
        mTankNode->rotate(Ogre::Quaternion(Ogre::Degree(angVelocity), Ogre::Vector3(0, 1, 0)), Ogre::Node::TransformSpace::TS_WORLD);
    }
    else // < 0, turn left 
    {
        mTankNode->rotate(Ogre::Quaternion(Ogre::Degree(-angVelocity), Ogre::Vector3(0, 1, 0)), Ogre::Node::TransformSpace::TS_WORLD);
    }

}

/**
 * Rotates the turret.
 *
 * @param    direction    The rotation direction
 */
void Tank::rotateTurret(int direction)
{
    if (isDead())
    {
        return;
    }

    if (direction > 0) // turn right
    {
        cannonNode->rotate(Ogre::Quaternion(Ogre::Degree(angVelocity), Ogre::Vector3(0, 1, 0)), Ogre::Node::TransformSpace::TS_WORLD);
    }
    else // < 0, turn left 
    {
        cannonNode->rotate(Ogre::Quaternion(Ogre::Degree(-angVelocity), Ogre::Vector3(0, 1, 0)), Ogre::Node::TransformSpace::TS_WORLD);
    }
}



// void Tank::setTankOrientation(Ogre::Quaternion quat)
// {
    
// }

/**
 * Checks if the Tank is destroyed or not.
 *
 * @return    True if destroyed
 */
bool Tank::isDead()
{
    if (health <= 0)
    {
        return true;
    }

    return false;
}

/**
 * Fires a projectile from the tank's cannon.
 *
 * @param    time     Tick time
 * @param    x        x-coord
 * @param    y        y-coord
 * @param    z        z-coord
 * @param    mTank    Pointer to the tank firing the projectile
 */
void Tank::fire(float time, float x, float y, float z, Tank *mTank)
{
        mProjectileManager->addProjectile(x, 2.0f, z, mTank);
}
