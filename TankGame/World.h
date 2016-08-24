#ifndef __World_h_
#define __World_h_

namespace Ogre {
    class Entity;
    class SceneNode;
    class SceneManager;
}

class TankCamera;
class InputHandler;
class AIManager;
class Tank;
class Player;
class ProjectileManager;
class ItemHandler;

enum class CameraState { player, world };

class World 
{
public:	
	World(Ogre::SceneManager *sceneManager, InputHandler *input);
	
    void Think(float time);
	void addCamera(TankCamera *c) { mCamera = c; }
	void addAIManager(AIManager *ai) { mAIManager = ai; }
	Ogre::SceneManager *SceneManager() { return mSceneManager; }
    Tank** getTanks() { return mTanks; }
    int  getMaxTanks() { return MAX_TANKS; }
    Ogre::Entity** getWallMeshes() { return mWallMeshes; }

    // initialization functions
    void init();
    void createTanks();
    void initPlayer();
    void placeFloor();
    void placeWalls();
    void initScores();

    // game functions
    void checkPlayerMovement();
    void checkTanks(float time);
    void setTankPosition(Tank *tank);
    void reattachCameraToPlayer();
    void setCameraToWorld();
    void updateHUD();
    void addPoint(int i) { scores[i]++; }
    bool checkWin();
    
    CameraState getCameraState() { return currCameraState; }

protected:
	Ogre::SceneManager *mSceneManager;
	InputHandler       *mInputHandler;
	TankCamera         *mCamera;
	AIManager          *mAIManager;
    Player             *mPlayer;
    ProjectileManager  *mProjectileManager;
    ItemHandler        *mItemHandler;

    bool initialized;

	// Enter world variables 
    Ogre::Entity       **mWallMeshes;
    Ogre::SceneNode    *mFloorNode, *camNode, **mFloorNodes, **mWallNodes;
    Tank               **mTanks;   // array of tanks
    Tank               *player;
    int                MAX_TANKS, *scores, MAX_POINTS;
    CameraState        currCameraState;
};

#endif