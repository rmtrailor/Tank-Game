#include "Projectile.h"
#include "Tank.h"

#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

/**
 * Constructor
 */
Projectile::Projectile(float initX, float initY, float initZ, Ogre::SceneManager *sceneMgr, Tank *tank)
    : x(initX), y(initY), z(initZ), mSceneManager(sceneMgr), mTank(tank)
{
    xVel            = 0.45f;
    yVel            = 0.001f;
    zVel            = 0.45f;
    MAX_THRESHOLD   = 0.002f;
    hit             = false;
    isFalling       = false;

    initPosition = new Ogre::Vector3(mTank->getCannonNode()->getPosition());
    initOrientation = new Ogre::Quaternion(mTank->getCannonNode()->getOrientation());

    // Create the mesh and add attach the texture
    mProjectileMesh = mSceneManager->createEntity("ball.mesh");
    mProjectileNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
    mProjectileNode->attachObject(mProjectileMesh);

    mProjectileNode->setPosition(mTank->getCannonNode()->getPosition());
    MAX_HEIGHT      = mProjectileNode->getPosition().y + MAX_THRESHOLD;

    mProjectileNode->setOrientation(mTank->getCannonNode()->getOrientation());
    mProjectileNode->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)), Ogre::Node::TransformSpace::TS_WORLD);
    mProjectileNode->scale(0.05, 0.05, 1.5);

    if (mProjectileNode->getOrientation().x == 0)
    {
        xVel = 0;
    }

    if (mProjectileNode->getOrientation().w == 0)
    {
        zVel = 0;
    }
}

/**
 * Constructor
 */
Projectile::~Projectile()
{
    delete mProjectileNode;
}

/**
 * Tick function for the project. The project continues on it's parabolic path and checks
 * for collisions.
 *
 * @param    time    Tick time
 */
void Projectile::Think(float time)
{

    if (!isFalling && mProjectileNode->getPosition().y >= MAX_HEIGHT)
    {
        isFalling = true;
        yVel = -yVel * 3;
    }

    x += xVel;
    y += yVel;
    z += zVel;
    Ogre::Vector3 moveVector(-xVel, yVel, -zVel);
    Ogre::Vector3 newPosition = (mProjectileNode->getPosition() + mProjectileNode->getOrientation() * moveVector);
    mProjectileNode->setPosition(newPosition);

    if (mProjectileNode->getPosition().y < -1) 
    {
        hit = true;
    }

}

/**
 * Gets the Tank ID of the tank that fired the projectile
 *
 * @return    The tank ID
 */
int Projectile::getTankID()
{
    return mTank->getID();
}
