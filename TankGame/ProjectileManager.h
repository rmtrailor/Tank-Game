#ifndef __ProjectileManager_h_
#define __ProjectileManager_h_

namespace Ogre {
    class SceneNode;
    class SceneManager;
}

class Projectile;
class Tank;
class World;

class ProjectileManager
{
public:
    ProjectileManager(World *world);
    ~ProjectileManager();
    void Think(float time);
    void addProjectile(float x, float y, float z, Tank *tank);
    bool checkCollisions(Projectile *proj);

protected:
    World               *mWorld;
    Projectile          **mProjectiles;
    int                 MAX_PROJECTILES;
    int                 currNumProj;
    float               totalDamageTime, lastDamageTime;
};

#endif