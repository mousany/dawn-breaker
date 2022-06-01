#ifndef GAMEOBJECTS_H__
#define GAMEOBJECTS_H__

#include "ObjectBase.h"
#include "GameWorld.h"

class GameWorld;

// Inheritage from ObjectBase
// --------------------------
// Properties:
//  imageID, 
//  x, y, 
//  direction, 
//  layer, 
//  size
// Methods:
//  GetX(), GetY(), MoveTo(int x, int y)
//  GetDirection(), SetDirection(int direction)
//  GetLayer(), 
//  GetSize(), SetSize(double size)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////GameObject//////////////////////////////
//////////////////////////////////////////////////////////////////////////
class GameObject : public ObjectBase {

public:

    using ObjectType = enum {
        TypePlayer,
        TypeStar, 
        TypeExplosion, 
        TypeMeteor,
        TypeBlueBullet,
        TypeRedBullet,
        TypeAlphaShip,
        TypeSigmaShip, 
        TypeOmegaShip, 
        TypeHealthWidget,
        TypeUpgradeWidget,
        TypeMeteorWidget
    };

    GameObject(int, int, int, int, int, double, 
        GameWorld& gameWorld, ObjectType, int, int, int, int, int);
    virtual ~GameObject() = default;

    GameWorld& GetGameWorld() const;
    ObjectType GetType() const;
    int GetHealth() const;
    void SetHealth(int);
    int GetDamage() const;
    void SetDamage(int);
    int GetSpeed() const;
    void SetSpeed(int);
    int GetEnergy() const;
    void SetEnergy(int);   
    int GetScore() const; 

    bool GetIsDead() const;
    void SetIsDead();

    bool operator&(const GameObject&) const;

private:

    GameWorld& m_gameWorld;
    ObjectType m_type;
    int m_health;
    int m_damage;
    int m_speed;
    int m_energy;
    int m_score;
    
};

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////Player////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Player : public GameObject {

public:

    Player(int, int, int, int, int, double, GameWorld&);
    virtual ~Player() = default;

    void Update() override;
    
    int GetUpgrade() const;
    void SetUpgrade(int);
    int GetMeteor() const;
    void SetMeteor(int);
    int GetDestroyed() const;
    void SetDestroyed(int);

private:

    int m_upgrade;
    int m_meteor;
    int m_destroyed;

};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Star/////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Star : public GameObject {

public:

    Star(int, int, int, int, int, double, GameWorld&);
    virtual ~Star() = default;

    void Update() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////Explosion////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Explosion : public GameObject {

public:

    Explosion(int, int, int, int, int, double, GameWorld&);
    virtual ~Explosion() = default;

    void Update() override;

private:

    int m_time;

};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////BlueBullet///////////////////////////////
//////////////////////////////////////////////////////////////////////////
class BlueBullet : public GameObject {

public:

    BlueBullet(int, int, int, int, int, double, GameWorld&, int);
    virtual ~BlueBullet() = default;

    void Update() override;

private:



};


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////Meteor////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Meteor : public GameObject {

public:

    Meteor(int, int, int, int, int, double, GameWorld&);
    virtual ~Meteor() = default;

    void Update() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////RedBullet////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class RedBullet : public GameObject {

public:

    RedBullet(int, int, int, int, int, double, GameWorld&, int);
    virtual ~RedBullet() = default;

    void Update() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////EnemyShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class EnemyShip : public GameObject {

public:

    EnemyShip(int, int, int, int, int, double, GameWorld&, ObjectType, 
        int, int, int, int, int, int, int);
    virtual ~EnemyShip() = default;

    void Update() override;

    int GetTime() const;
    void SetTime(int);
    int GetStrategy() const;
    void SetStrategy(int);

    bool Collapse();
    void Choose();
    void Move();

    virtual void Rebirth() = 0;
    virtual void Attack() = 0;
    virtual void Refuel() = 0;

private:

    int m_time;
    int m_strategy;

};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////AlphaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class AlphaShip : public EnemyShip {

public:

    AlphaShip(int, int, int, int, int, double, GameWorld&, int, int, int);
    virtual ~AlphaShip() = default;

    void Rebirth() override;
    void Attack() override;    
    void Refuel() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////SigmaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class SigmaShip : public EnemyShip {

public:

    SigmaShip(int, int, int, int, int, double, GameWorld&, int, int);
    virtual ~SigmaShip() = default;  

    void Rebirth() override;
    void Attack() override;
    void Refuel() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////OmegaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class OmegaShip : public EnemyShip {

public:

    OmegaShip(int, int, int, int, int, double, GameWorld&, int, int, int);
    virtual ~OmegaShip() = default;

    void Rebirth() override;
    void Attack() override;
    void Refuel() override;

private:



};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////SnackWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
class SnackWidget : public GameObject {

public:

    SnackWidget(int, int, int, int, int, double, GameWorld&, ObjectType, 
        int, int, int, int, int);
    virtual ~SnackWidget() = default;

    void Update() override;

    virtual void Effect() = 0;

private:



};


//////////////////////////////////////////////////////////////////////////
////////////////////////////////HealthWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
class HealthWidget : public SnackWidget {

public:

    HealthWidget(int, int, int, int, int, double, GameWorld&);
    virtual ~HealthWidget() = default;

    void Effect() override;

private:



};

//////////////////////////////////////////////////////////////////////////
////////////////////////////////UpgradeWidget/////////////////////////////
//////////////////////////////////////////////////////////////////////////
class UpgradeWidget : public SnackWidget {

public:

    UpgradeWidget(int, int, int, int, int, double, GameWorld&);
    virtual ~UpgradeWidget() = default;

    void Effect() override;

private:



};


//////////////////////////////////////////////////////////////////////////
////////////////////////////////MeteorWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
class MeteorWidget : public SnackWidget {

public:

    MeteorWidget(int, int, int, int, int, double, GameWorld&);
    virtual ~MeteorWidget() = default;

    void Effect() override;

private:



};

#endif // GAMEOBJECTS_H__