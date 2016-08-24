#include "TankGame.h"
#include "World.h"
#include "InputHandler.h"
#include "AIManager.h"
#include "TankCamera.h"
#include "MainListener.h"

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgreFontManager.h"
#include "OgreOverlaySystem.h"

TankGame::TankGame()
{
	mTankFrameListener = 0;
	mRoot = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
    mResourcePath = "";
#endif
}

TankGame::~TankGame()
{
	if (mTankFrameListener)
	{
		delete mTankFrameListener;
	}
	if (mRoot)
	{
		delete mRoot;
	}
}

void TankGame::createCamera()
{
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setPosition(Ogre::Vector3(0, 0, -100)); // set to 0,0,0 for now. change starting point later
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));
}

void TankGame::createFrameListener(void)
{
	mTankFrameListener = new MainListener(mWindow, mInputHandler, mAIManager, mWorld, mTankCamera);
	mRoot->addFrameListener(mTankFrameListener);
}


void TankGame::createViewports(void)
{
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}


/* *
 *  Function:   createScene
 *  -----------------------
 *  Initializes handlers and camera. Then adds the camera and the manager to the world.
 *
 *  returns         nothing
 */
void TankGame::createScene()
{
    mInputHandler = new InputHandler(mWindow);
    mWorld        = new World(mSceneMgr, mInputHandler);
    mTankCamera   = new TankCamera(mCamera, mWorld, mInputHandler);
    mAIManager    = new AIManager(mWorld);
    mWorld->addCamera(mTankCamera);
    mWorld->addAIManager(mAIManager);
    mWorld->init();
}


bool TankGame::setup(void)
{
    Ogre::String pluginsPath;
    Ogre::String configPath;

#ifndef OGRE_STATIC_LIB

#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
	// add _d suffix to config files
	pluginsPath = mResourcePath + "plugins_d.cfg";
	configPath = mResourcePath + "ogre_d.cfg";
#else
	pluginsPath = mResourcePath + "plugins.cfg";
	configPath = mResourcePath + "ogre.cfg";

#endif // endf of OGRE_DEBUG_MODE ... 

#endif // end of ifndef OGRE_STATIC_LIB

    mRoot = new Ogre::Root(pluginsPath, configPath, mResourcePath + "Ogre.log");

    setupResources();

    bool isConfigured = configure();
    if (!isConfigured) 
    {
        return false;
    }

    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "PongSMInstance");
    createCamera();
    createViewports();

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    createScene();

    createFrameListener();

    return true;
}


/* *
 *  Function:   configure
 *  ---------------------
 *  Configures the application. Returns false if the user chooses to abandon configuration.
 *
 *  returns         whether or not the application was configured
 */
bool TankGame::configure(void)
{
    if (mRoot->showConfigDialog())
    {
        mWindow = mRoot->initialise(true);
        return true;
    }
    else
    {
        return false;
    }
}


void TankGame::setupResources(void)
{
    Ogre::ConfigFile cf;

#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
    // add _d suffix to config files
    cf.load(mResourcePath + "resources_d.cfg");
#else
	cf.load(mResourcePath + "resources.cfg");
#endif

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String secName, typeName, archName;

     while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }
}


void TankGame::destroyScene()
{
    delete mWorld;
    delete mAIManager;
    delete mTankCamera;
    delete mInputHandler;
}


void TankGame::go(void)
{
    if (!setup())
    {
        return;
    }

    mRoot->startRendering();

    // clean up
    destroyScene();
}