#include "Item.h"

#include "Ogre.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

/**
 * Constructor
 *
 * Passed the type of the item and a pointer to the scene manager.
 */
Item::Item(ItemType type, Ogre::SceneManager *mScnMger)
    : mType(type), mSceneManager(mScnMger)
{
    if (mType == ItemType::ammo)
    {
        mItemMesh = SceneManager()->createEntity("Cube.002.mesh");
    }
    else if (mType == ItemType::healthpack) 
    {
        mItemMesh = SceneManager()->createEntity("MediKit_closed.mesh");
    }
    mItemNode = SceneManager()->getRootSceneNode()->createChildSceneNode();
    mItemNode->attachObject(mItemMesh);
    
    pickedUp = false;
    totalRespawnTime = 0;
    lastRespawnTime = 0;
}

/**
 * Deconstructor
 */
Item::~Item()
{

}

/**
 * Tick function for the time. Basically checks when the item itself must respawn after
 * being picked up.
 *
 * @param    time    Tick time
 */
void Item::Think(float time)
{
    if (pickedUp)
    {
        totalRespawnTime += time;
        
        if (totalRespawnTime > 10)
        {
            respawn();
        }
    }
}


void Item::respawn()
{
    if (mType == ItemType::healthpack)
    {
        mItemNode->detachAllObjects();
        mItemMesh = SceneManager()->createEntity("MediKit_closed.mesh");
    }
    mItemNode->attachObject(mItemMesh);
    pickedUp = false;
}



void Item::pickUp()
{
    if (!pickedUp)
    {
        pickedUp = true;
        totalRespawnTime = 0;
        lastRespawnTime = 0;
        mItemNode->detachAllObjects();

        if (mType == ItemType::healthpack)
        {
            mItemMesh = SceneManager()->createEntity("MediKit_opened.mesh");
            mItemNode->attachObject(mItemMesh);
        }

    }
}
