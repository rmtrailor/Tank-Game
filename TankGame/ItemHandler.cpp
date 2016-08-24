#include "ItemHandler.h"
#include "Item.h"
#include "World.h"
#include "Tank.h"
#include "Player.h"

#include "Ogre.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreAxisAlignedBox.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"

#include <stdlib.h>

ItemHandler::ItemHandler(Ogre::SceneManager *mScnMger, World *world, Player *player)
    : mSceneManager(mScnMger), mWorld(world), mPlayer(player)
{
    maxItems = 20;
    mItemList = new Item*[maxItems];
    addItems();
    mTanks = mWorld->getTanks();
    srand (time(NULL));
    notificationTime = 0;
}

ItemHandler::~ItemHandler()
{

}

void ItemHandler::Think(float time)
{
    notificationTime += time;

    if (notificationTime > 4)
    {
        notificationTime = 0;
        Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel3/Text2")->setCaption("");
    }

    
    for (int i = 0; i < maxItems; i++)
    {
        mItemList[i]->Think(time);
        if (!mItemList[i]->getPickUpStatus())
        {
            checkPickups(mItemList[i]);
        }   
    }

}

void ItemHandler::checkPickups(Item *item)
{
    
    for (int i = 0; i < mWorld->getMaxTanks(); i++)
    {
        Ogre::AxisAlignedBox itemBox = item->getItemMesh()->getWorldBoundingBox();
        Ogre::AxisAlignedBox tankBox = mTanks[i]->getEntity()->getWorldBoundingBox();

        if (itemBox.intersects(tankBox))
        {
            if (item->getType() == ItemType::ammo && i == 0)
            {
                mPlayer->addAmmo(10);
                item->pickUp();

                std::string ammo = "You picked up " + std::to_string(10) + " shells!";
                Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel3/Text2")->setCaption(ammo);
            }
            else if (item->getType() == ItemType::healthpack)
            {
                mTanks[i]->heal(25);
                item->pickUp();
            }
        }
    }
}


void ItemHandler::addItems()
{
    for (int i = 0; i < maxItems; i++)
    {
        int x = rand() % 500;
        int z = rand() % 500;
        Item *newItem;

        if (i < maxItems / 2)
        {
            newItem = new Item(ItemType::ammo, mSceneManager);
            
        }
        else if (i >= maxItems / 2)
        {
            newItem = new Item(ItemType::healthpack, mSceneManager);
        }
        newItem->getSceneNode()->setPosition(x, 0, z);
        mItemList[i] = newItem;
    }
}
