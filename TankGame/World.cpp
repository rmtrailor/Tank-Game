#include "World.h"

#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"

#include <ois/ois.h>

#include "TankCamera.h"
#include "InputHandler.h"
#include "AIManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "Tank.h"
#include "ProjectileManager.h"
#include "ItemHandler.h"

#include <stdlib.h>
#include <time.h> 

/**
 * Constructor
 */
World::World(Ogre::SceneManager *sceneManager, InputHandler *input)
    : mSceneManager(sceneManager), mInputHandler(input)
{
    mSceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    mSceneManager->setSkyBox(true, "sky", 1000, false);

    Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) 
	{ 
		iter.getNext()->load(); 
	}

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *overlay = om.getByName("HUD");
	overlay->show();

    MAX_TANKS = 12;
    MAX_POINTS = 5;

    srand (time(NULL));

    initialized = false;
}

/**
 * Tick function
 *
 * @param    time    Time tick
 */
void World::Think(float time)
{
    if (checkWin())
    {
        return;
    }

    mProjectileManager->Think(time);
    mPlayer->checkMovement(time);
    mCamera->Think(time);
    checkTanks(time);
    mItemHandler->Think(time);
    updateHUD();
}

/**
 * Initialize the game world.
 */
void World::init()
{
    if (initialized)
    {
        return;
    }
    mTanks = new Tank*[MAX_TANKS];

    mProjectileManager = new ProjectileManager(this);
    createTanks();
    
    mAIManager->addTanks(mTanks);
    mAIManager->setNumTanks(MAX_TANKS);

    initPlayer();
    mCamera->addPlayer(player);
    placeFloor();
    placeWalls();
    mItemHandler = new ItemHandler(mSceneManager, this, mPlayer);

    initScores();

    initialized = true;
}

/**
 * Creates the tanks in the match.
 */
void World::createTanks() 
{
    //float x, z;
    for (int i = 0; i < MAX_TANKS; i++)
    {
        
        mTanks[i] = new Tank(0, 0, SceneManager(), i, mTanks, this, mProjectileManager);
        setTankPosition(mTanks[i]);
    }
}

/**
 * Checks each tank
 *
 * @param    time    Tick time
 */
void World::checkTanks(float time)
{

    for (int i = 0; i < MAX_TANKS; i++)
    {
        if (mTanks[i] != nullptr)
        {
            if (mTanks[i]->isDead())
            {
                mTanks[i]->addRespawnTime(time);
                
                // If respawn check true then we can respawn the tank
                if (mTanks[i]->respawnCheck())
                {
                    mTanks[i]->resetMesh();
                    setTankPosition(mTanks[i]);

                    if (i == 0)
                    {
                        mCamera->getRenderCamera()->setPosition(player->getSceneNode()->getPosition().x + 10, 4, player->getSceneNode()->getPosition().z);
                        mCamera->getRenderCamera()->lookAt(player->getSceneNode()->getPosition().x, 2, player->getSceneNode()->getPosition().z);
                        player->getCannonNode()->attachObject(mCamera->getRenderCamera());                   
                    }
                }
            }
        }
    }
}

/**
 * Sets a tank's position.
 *
 * @param    tank    Pointer to the tank
 */
void World::setTankPosition(Tank *tank)
{
    if (tank->getID() == 0)
    {
        tank->getSceneNode()->setPosition(0, 0, 0);
        tank->getCannonNode()->setPosition(0, 2.5, 0);
    }
    else 
    {
        float x = rand() % 500;
        float z = rand() % 500;
        tank->getSceneNode()->setPosition(x, 0, z);
        tank->getCannonNode()->setPosition(x, 2.5, z);
    }
}

/**
 * Updates the HUD for any changed information.
 */
void World::updateHUD()
{
    std::string health = "Health: " + std::to_string(mTanks[0]->getHealth());
    Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel/Text1")->setCaption(health);

    std::string ammo = "Ammo: " + std::to_string(mPlayer->getAmmo());
    Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel/Text2")->setCaption(ammo);

    for (int i = 0; i < MAX_TANKS; i++)
    {
        std::string tank = "HUD/Panel4/Text" + std::to_string(i + 1);
        std::string score;
        if (i == 0)
        {
           score = "Player: " + std::to_string(scores[i]);
        }
        else 
        {
            score = "Bot " + std::to_string(i) + " " + std::to_string(scores[i]);
        }
        Ogre::OverlayManager::getSingleton().getOverlayElement(tank)->setCaption(score);
    }
}

/**
 * Initializes the player tank.
 */
void World::initPlayer()
{
    player = mTanks[0];
    mPlayer = new Player(player, mCamera, mInputHandler, mProjectileManager, this);
    mCamera->getRenderCamera()->setPosition(player->getSceneNode()->getPosition().x + 10, 4, player->getSceneNode()->getPosition().z);
    mCamera->getRenderCamera()->lookAt(player->getSceneNode()->getPosition().x, 2, player->getSceneNode()->getPosition().z);
    
    // Attach the camera to the player to make the camera move with the player's tank cannon
    player->getCannonNode()->attachObject(mCamera->getRenderCamera());
    currCameraState = CameraState::player;

    std::string health = "Health: " + std::to_string(mTanks[0]->getHealth());
    Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel/Text1")->setCaption(health);
}

/**
 * Initializes the scores for each tank.
 */
void World::initScores()
{
    scores = new int[MAX_TANKS];

    for (int i = 0; i < MAX_TANKS; i++)
    {
        scores[i] = 0;
    }
}

/**
 * Checks if any Tank has reached the score limit
 *
 * @return    True if a tank has won
 */
bool World::checkWin()
{
    for (int i = 0; i < MAX_TANKS; i++)
    {
        if (scores[i] >= MAX_POINTS)
        {
            std::string prompt;
            if (i == 0)
            {
                prompt = "Player Won!";
            }
            else
            {
                prompt = "Bot " + std::to_string(i) + " won!";
            }
            Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel3/Text2")->setCaption(prompt);
            return true;
        }
    }
    return false;
}

/**
 * Reattaches the camera to the player after they respawn.
 */
void World::reattachCameraToPlayer()
{
    if (currCameraState != CameraState::player)
    {
        mCamera->getRenderCamera()->setOrientation(player->getSceneNode()->getOrientation());
        mCamera->getRenderCamera()->setPosition(player->getSceneNode()->getPosition().x + 10, 4, player->getSceneNode()->getPosition().z);
        mCamera->getRenderCamera()->lookAt(player->getSceneNode()->getPosition().x, 2, player->getSceneNode()->getPosition().z);
        player->getCannonNode()->attachObject(mCamera->getRenderCamera());
        currCameraState = CameraState::player;
    }
}

/**
 * Set camera to world camera.
 */
void World::setCameraToWorld()
{
    if (currCameraState != CameraState::world)
    {
        mCamera->getRenderCamera()->detachFromParent();
        mCamera->getRenderCamera()->setPosition(100, 100, 100);
        mCamera->getRenderCamera()->lookAt(100, 0, 100);
        currCameraState = CameraState::world;
    }    
}

/**
 * Places the floor tiles.
 */
void World::placeFloor()
{
    mFloorNodes = new Ogre::SceneNode*[25];
    int count = 0;
    float z = 0;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            Ogre::Entity *floorMesh = SceneManager()->createEntity("test-floor.mesh");
            mFloorNodes[count] = SceneManager()->getRootSceneNode()->createChildSceneNode();
            mFloorNodes[count]->attachObject(floorMesh);

            mFloorNodes[count]->scale(6, 0, 6);
            mFloorNodes[count]->setPosition((float) i * 20 * 6, -0.5, (float) j * 20 * 6);
            count++;
        }
    }   

}

/**
 * Places the walls.
 */
void World::placeWalls()
{
    int i = 0;
    mWallNodes  = new Ogre::SceneNode*[4];
    mWallMeshes = new Ogre::Entity*[4]; 

    for (i; i < 1; i++)
    {
        mWallMeshes[i] = SceneManager()->createEntity("Wall.mesh");
        mWallNodes[i] = SceneManager()->getRootSceneNode()->createChildSceneNode();
        mWallNodes[i]->attachObject(mWallMeshes[i]);
        mWallNodes[i]->scale(1, 10, 180);
        mWallNodes[i]->setPosition(-55, 0.5, 250);
    }

    for (int j = 0; i < 2; i++, j++)
    {
        mWallMeshes[i] = SceneManager()->createEntity("Wall.mesh");
        mWallNodes[i] = SceneManager()->getRootSceneNode()->createChildSceneNode();
        mWallNodes[i]->attachObject(mWallMeshes[i]);
        mWallNodes[i]->scale(1, 10, 180);
        mWallNodes[i]->setPosition(500, 0.5, 200);
    }
 
    for (int j = 0; i < 3; i++, j++)
    {
        mWallMeshes[i] = SceneManager()->createEntity("Wall.mesh");
        mWallNodes[i] = SceneManager()->getRootSceneNode()->createChildSceneNode();
        mWallNodes[i]->attachObject(mWallMeshes[i]);

        mWallNodes[i]->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)), 
                              Ogre::Node::TransformSpace::TS_WORLD);
        mWallNodes[i]->scale(1, 10, 180);
        mWallNodes[i]->setPosition(300, 0.5, -9);
    }

    
    for (int j = 0; i < 4; i++, j++)
    {
        mWallMeshes[i] = SceneManager()->createEntity("Wall.mesh");
        mWallNodes[i] = SceneManager()->getRootSceneNode()->createChildSceneNode();
        mWallNodes[i]->attachObject(mWallMeshes[i]);

        mWallNodes[i]->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0, 1, 0)), 
                              Ogre::Node::TransformSpace::TS_WORLD);
        mWallNodes[i]->scale(1, 10, 180);
        mWallNodes[i]->setPosition(300, 0.5, 500);
    }
}
