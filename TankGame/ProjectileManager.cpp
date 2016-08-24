#include "ProjectileManager.h"
#include "Projectile.h"
#include "World.h"
#include "Tank.h"

#include "Ogre.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"

#include <stdlib.h>
#include <time.h> 

/**
 * Constructor
 */
ProjectileManager::ProjectileManager(World *world)
    : mWorld(world)
{

    MAX_PROJECTILES = 100;
    mProjectiles = new Projectile*[100];

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        mProjectiles[i] = nullptr;
    }

    currNumProj = 0;
    totalDamageTime = 0;
    lastDamageTime = 0;

    srand (time(NULL));
}

/**
 * Deconstructor
 */
ProjectileManager::~ProjectileManager()
{
    
}

/**
 * Tick function for the projectile manager. Goes through each projectile and calls their
 * Think function.
 *
 * @param    time    Tick time
 */
void ProjectileManager::Think(float time)
{
    if (currNumProj == 0)
    {
        return;
    }

    totalDamageTime += time;

    if (totalDamageTime > 4)
    {
        std::string blankText = "";
        Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel2/Text1")->setCaption(blankText);
        lastDamageTime = 0;
        totalDamageTime = 0;
    }

    int localCount = 0;

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (localCount == currNumProj)
        {
            break;
        }

        if (mProjectiles[i] != nullptr)
        {
            mProjectiles[i]->Think(time);
            localCount++;

            bool collided = checkCollisions(mProjectiles[i]);

            if (mProjectiles[i]->hasHit())
            {
                mProjectiles[i]->~Projectile();
                mProjectiles[i] = nullptr;
                localCount--;
                currNumProj--;
            }

        }
    }
}

/**
 * Checks the collisions for a projectile.
 *
 * @param     proj    Pointer to the projectile being checked
 *
 * @return            Whether a projectile collided
 */
bool ProjectileManager::checkCollisions(Projectile *proj)
{
    Tank **tanks = mWorld->getTanks();
    bool collided = false;

    for (int i = 0; i < mWorld->getMaxTanks(); i++)
    {
        if (tanks[i] != nullptr && !tanks[i]->isRespawning()) {
            if (i != proj->getTankID()) {

                collided = tanks[i]->projectileCollision(proj->getEntity());

                if (collided)
                {
                    int damageAmount = rand() % 100 + 1;      
                 
                    proj->isHit();
                    tanks[i]->damage(damageAmount);

                    if (proj->getTankID() == 0)
                    {
                        std::string damageText = "You hit Tank " + std::to_string(i) + " for " + 
                            std::to_string(damageAmount) + " damage!";
                        Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel2/Text1")->setCaption(damageText);
                        lastDamageTime = 0;
                        totalDamageTime = 0;
                    }

                    
                    if (tanks[i]->isDead() && !tanks[i]->isRespawning())
                    {
                        tanks[i]->setMeshToDestroyed();

                        if (proj->getTankID() == 0)
                        {
                           std::string damageText = "You destroyed Tank " + std::to_string(i) + "!";
                           Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel2/Text1")->setCaption(damageText);
                           lastDamageTime = 0;
                           totalDamageTime = 0;
                        }

                        mWorld->addPoint(proj->getTankID());
                    }

                    if (i == 0)
                    {
                       std::string health = "Health: " + std::to_string(tanks[i]->getHealth());
                       Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Panel/Text1")->setCaption(health);
                    }

                    return collided; 
                }
            }
        }
    }
    return collided;
}

/**
 * After a shot is fired from a tank, this function is called to add a projectile to
 * the projectiles array.
 *
 * @param    x       Initial x-coord position
 * @param    y       Initial y-coord position
 * @param    z       Initial z-coord position
 * @param    tank    Pointer to the tank that fired the shot
 */
void ProjectileManager::addProjectile(float x, float y, float z, Tank *tank)
{
    if (currNumProj == MAX_PROJECTILES - 1) 
    {
        return;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (mProjectiles[i] == nullptr)
        {
            Projectile *newProjectile = new Projectile(x, y, z, mWorld->SceneManager(), tank);
            mProjectiles[i] = newProjectile;
            currNumProj++;
            break;
        }

    }
}
