#include "AIManager.h"
#include "World.h"
#include "Tank.h"

#include "Ogre.h"

#include <stdlib.h>

/**
 * Constructor
 * Takes in pointer to the world
 */
AIManager::AIManager(World *world)
	: mWorld(world)
{
    lastTime = 0;
    totalTime = 0;
    lastFireTime = 0;
    totalFireTime = 0;
    turn = 0;
    turretTurn = 0;
}

/**
 * Deconstructor
 */
AIManager::~AIManager()
{

}

/**
 * Tink function for each Tank's AI 'thinking'
 * 
 * @param   time    current time tick
 */
void AIManager::Think(float time)
{
    for (int i = 1; i < mWorld->getMaxTanks(); i++)
    {
        if (mTanks[i] != nullptr && !mTanks[i]->isDead())
        {
            updateDirection(time, i);
            processActions(mTanks[i], i);
        }        
    }
}

/**
 * Initialize the number of tanks in the match
 * 
 * @param   num     The number of tanks
 */
void AIManager::setNumTanks(int num)
{
    numTanks = num;
    selections = new int[num];
}

/**
 * Updates the direction of the tank
 * 
 * @param   time    Time tick
 * @param   i       Tank ID
 */
void AIManager::updateDirection(float time, int i)
{
    totalTime += time;
    if (totalTime > lastTime + 5) 
    {
        lastTime = totalTime;
        selections[i] = rand() % 2;

        turn = rand() % 3;
        turretTurn = rand() % 3;
    }
 
    totalFireTime += time;
    if (totalFireTime > lastFireTime + 3)
    {
        lastFireTime = totalFireTime;
        int shouldFire = rand() % 2;

        if (shouldFire == 1)
        {
            mTanks[i]->fire(time, mTanks[i]->getSceneNode()->getPosition().x, 
                mTanks[i]->getSceneNode()->getPosition().y, 
                mTanks[i]->getSceneNode()->getPosition().z, mTanks[i]);
        }
    }
}


/**
 * Process the actions for each tank
 *
 * @param    tank    Pointer to the current tank
 * @param    i       Tank ID
 */
void AIManager::processActions(Tank *tank, int i)
{
    //int selection = rand() % 100; // 5 options (5th to do nothing)
    if (selections[i] == 1)
    {
        if (tank->getSceneNode()->getPosition().x >= 500 || tank->getSceneNode()->getPosition().z >= 500
            || tank->getSceneNode()->getPosition().x <= 0 || tank->getSceneNode()->getPosition().z < 0)
        {
            selections[i] = 2;
            turn = 1;
        }
        else 
        {
            tank->move(1);
        }

    }
    else if (selections[i] == 2)
    {
        tank->move(-1);
    }

    if (turn == 1 && selections[i] != 0)
    {
        tank->turn(-1);
    }
    else if (turn == 2 && selections[i] != 0)
    {
        tank->turn(1);
    }

    if (turretTurn == 1)
    {
        tank->rotateTurret(-1);
    }
    else if (turn == 2)
    {
        tank->rotateTurret(1);
    }

}

/**
 * Initialize the destination for the tanks
 */
void AIManager::initDestinations()
{
    destinations = new Ogre::Vector3*[mWorld->getMaxTanks()];

    for (int i = 1; i < mWorld->getMaxTanks(); i++)
    {
        float x = rand() % 500;
        float z = rand() % 500;

        destinations[i] = new Ogre::Vector3(x, 0, z);
    }

}
