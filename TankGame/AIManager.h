#ifndef __AIManager_h_
#define __AIManager_h_

namespace Ogre {
    class Vector3;
}


class World;
class Tank;

class AIManager
{
public:
	AIManager(World *world);
	~AIManager();

	void Think(float time);
    void addTanks(Tank **tank) { mTanks = tank; }
    void setNumTanks(int num);
    void updateDirection(float time, int i);
    void processActions(Tank *tank, int i);
    void initDestinations();

protected:
	World         *mWorld;
    Tank          **mTanks;
    int           *selections, turn, turretTurn;
    Ogre::Vector3 **destinations;
    int           numTanks;
    float         lastTime, totalTime;
    float         lastFireTime, totalFireTime;
    int           selection;
};


#endif