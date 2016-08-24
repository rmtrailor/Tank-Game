#include "Player.h"

#include "Tank.h"
#include "TankCamera.h"
#include "InputHandler.h"
#include "ProjectileManager.h"
#include "World.h"

#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"

/**
 * Constructor
 */
Player::Player(Tank *pTank, TankCamera *cam, InputHandler *input, ProjectileManager *projMger, World *world)
    : mTank(pTank), mCamera(cam), mInputHandler(input), mProjectileManager(projMger), mWorld(world)
{
    x = mTank->getX();
    y = 0.0f;
    z = mTank->getZ();
    count = 0;
    lastTime = 0;
    totalTime = 0;
    ammo = 20;
}

/**
 * Deconstructor
 */
Player::~Player()
{

}

/**
 * Checks if and what keys the player is holding down and moves the player tank
 * accordingly.
 *
 * @param    time    Tick time
 */
void Player::checkMovement(float time)
{
    totalTime += time;
    
    x = mTank->getX();
    z = mTank->getZ();
    xVel = 0;
    zVel = 0;

    if (mInputHandler->IsKeyDown(OIS::KC_W))
    {
        mTank->move(1);
        zVel = mTank->getZVel();
    }
    else if (mInputHandler->IsKeyDown(OIS::KC_S))
    {
        mTank->move(-1);
        zVel = -mTank->getZVel();
    }
    
    if (mInputHandler->IsKeyDown(OIS::KC_A))
    {
        mTank->turn(1);
    }
    else if (mInputHandler->IsKeyDown(OIS::KC_D))
    {
        mTank->turn(-1);
    }

    if (mInputHandler->IsKeyDown(OIS::KC_LEFT))
    {
        mTank->rotateTurret(1);
    }
    else if (mInputHandler->IsKeyDown(OIS::KC_RIGHT))
    {
        mTank->rotateTurret(-1);
    }

    
    // Fire button is in separate conditional so that way we can move and fire at the
    // same time
    if (mInputHandler->IsKeyDown(OIS::KC_SPACE))
    {   
        if (totalTime > lastTime + 2)
        {
            lastTime = totalTime;
            
            if (ammo > 0)
            {
                mTank->fire(time, x, 1.0, 1.0, mTank);
                ammo--;
            }
            else
            {
                std::string prompt = "You're out of ammo!";
                Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel3/Text2")->setCaption(prompt);
            }
        } 
    }

    // SELF-DESTRUCT (secret)
    if (mInputHandler->IsKeyDown(OIS::KC_8))
    {
        mTank->damage(100);
        mTank->setMeshToDestroyed();
    }

}

/**
 * Adds ammo to the player's current ammo count. Max of 99 player ammo
 *
 * @param    amount    The ammount to be added.
 */
void Player::addAmmo(int amount)
{
    ammo += amount;

    if (ammo > 99)
    {
        ammo = 99;
    }
}


