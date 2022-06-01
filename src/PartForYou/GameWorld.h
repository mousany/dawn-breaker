#ifndef GAMEWORLD_H__
#define GAMEWORLD_H__

#include <list>

#include "GameObjects.h"
#include "WorldBase.h"

class GameObject;
class Player;

class GameWorld : public WorldBase {

public:

    GameWorld();
    virtual ~GameWorld() = default;

    virtual void Init() override;
    virtual LevelStatus Update() override;
    virtual void CleanUp() override;
    virtual bool IsGameOver() const override;

    void AddObject(std::unique_ptr<GameObject>);
    std::list<std::unique_ptr<GameObject>>& GetObjects();


    std::unique_ptr<Player> m_player;

private:

    int m_life;    
    std::list<std::unique_ptr<GameObject>> m_data;

};

#endif  // !GAMEWORLD_H__
