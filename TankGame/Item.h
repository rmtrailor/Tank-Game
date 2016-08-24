#ifndef __Item_h_
#define __Item_h_

namespace Ogre {
    class Entity;
    class SceneNode;
    class SceneManager;
}

class World;

enum class ItemType { healthpack, ammo };

class Item 
{
public:
    Item(ItemType type, Ogre::SceneManager *mScnMger);
    ~Item();
    Ogre::SceneManager *SceneManager() { return mSceneManager; }

    void Think(float time);
    void pickUp();
    void respawn();

    ItemType         getType() { return mType; }
    Ogre::Entity*    getItemMesh() { return mItemMesh; }
    Ogre::SceneNode* getSceneNode() { return mItemNode; }
    bool             getPickUpStatus() { return pickedUp; }

protected:
    Ogre::Entity        *mItemMesh;
    Ogre::SceneNode     *mItemNode;
    Ogre::SceneManager  *mSceneManager;
    ItemType            mType;
    bool                pickedUp;
    float               totalRespawnTime, lastRespawnTime;
};



#endif