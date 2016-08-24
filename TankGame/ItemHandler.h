#ifndef __ItemHandler_h_
#define __ItemHandler_h_

namespace Ogre {
    class SceneManager;
}

class Item;
class Tank;
class World;
class Player;

class ItemHandler
{
public:
    ItemHandler(Ogre::SceneManager *mScnMger, World *world, Player *player);
    ~ItemHandler();

    void Think(float time);
    void addItems();
    void checkPickups(Item *item);
protected:
    World              *mWorld;
    Ogre::SceneManager *mSceneManager;
    Item               **mItemList;
    Tank               **mTanks;
    Player             *mPlayer;
    int                maxItems;
    float              notificationTime;
};



#endif