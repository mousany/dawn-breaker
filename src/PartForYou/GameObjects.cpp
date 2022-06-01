#include <algorithm>

#include "GameObjects.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////Utilities///////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void Destroy(std::unique_ptr<GameObject>& target) {
    target->GetGameWorld().AddObject(
        std::make_unique<Explosion>(
            IMGID_EXPLOSION, // image id
            target->GetX(), target->GetY(), // x, y
            0, // direction
            3, // layer
            4.5, // size
            target->GetGameWorld() // game world
        )
    );                        
    target->SetIsDead();
    target->GetGameWorld().m_player->SetDestroyed(
        target->GetGameWorld().m_player->GetDestroyed() + 1
    );
    target->GetGameWorld().IncreaseScore(target->GetScore());
}

static void Destroy(std::unique_ptr<GameObject>&& target) {
    Destroy(target);
    target.release();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////GameObject//////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameObject::GameObject(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld, ObjectType type, 
        int health, int damage, int speed, int energy, int score): 
    ObjectBase(imageID, x, y, direction, layer, size), 
    m_gameWorld(gameWorld), m_type(type), m_health(health), 
    m_damage(damage), m_speed(speed), m_energy(energy), m_score(score) { }

GameWorld& GameObject::GetGameWorld() const {
    return this->m_gameWorld;
}

GameObject::ObjectType GameObject::GetType() const {
    return this->m_type;
}

int GameObject::GetHealth() const {
    return this->m_health;
}

void GameObject::SetHealth(int health) {
    this->m_health = health;
}

int GameObject::GetDamage() const {
    return this->m_damage;
}

void GameObject::SetDamage(int damage) {
    this->m_damage = damage;
}

int GameObject::GetSpeed() const {
    return this->m_speed;
}

void GameObject::SetSpeed(int speed) {
    this->m_speed = speed;
}

int GameObject::GetEnergy() const {
    return this->m_energy;
}

void GameObject::SetEnergy(int energy) {
    this->m_energy = energy;
}

int GameObject::GetScore() const {
    return this->m_score;
}

bool GameObject::GetIsDead() const {
    return this->m_health <= 0;
}

void GameObject::SetIsDead() {
    this->m_health = 0;
}

bool GameObject::operator&(const GameObject& other) const {
    double d = sqrt(pow(this->GetX() - other.GetX(), 2) + 
        pow(this->GetY() - other.GetY(), 2));
    return d < 30.0 * (this->GetSize() + other.GetSize());
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////Player////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Player::Player(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld):
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypePlayer, 100, 0, 4, 10, 0), 
    m_upgrade(0), m_meteor(0), m_destroyed(0) { }
// x = 300, y = 100, direction = 0, layer = 0, size = 1.0
// health = 100, energy = 10, upgrade = 0, meteor = 0, destroyed = 0

int Player::GetUpgrade() const {
    return this->m_upgrade;
}

void Player::SetUpgrade(int upgrade) {
    this->m_upgrade = upgrade;
}

int Player::GetMeteor() const {
    return this->m_meteor;
}

void Player::SetMeteor(int meteor) {
    this->m_meteor = meteor;
}

int Player::GetDestroyed() const {
    return this->m_destroyed;
}

void Player::SetDestroyed(int destroyed) {
    this->m_destroyed = destroyed;
}
    
void Player::Update() {
    // Check if the player is dead
    if (this->GetIsDead()) {
        return;
    }

    // Listen to move request
    int nowX = this->GetX();
    int nowY = this->GetY();
    if (this->GetGameWorld().GetKey(KeyCode::LEFT) && nowX >= 4) {
        this->MoveTo(nowX - 4, nowY);
    } 
    if (this->GetGameWorld().GetKey(KeyCode::RIGHT) && nowX <= WINDOW_WIDTH - 5) {
        this->MoveTo(nowX + 4, nowY);
    }
    if (this->GetGameWorld().GetKey(KeyCode::DOWN) && nowY >= 54) {
        this->MoveTo(nowX, nowY - 4);
    }
    if (this->GetGameWorld().GetKey(KeyCode::UP) && nowY <= WINDOW_HEIGHT - 5) {
        this->MoveTo(nowX, nowY + 4);
    }

    // Listen to shoot1 request
    if (this->GetGameWorld().GetKey(KeyCode::FIRE1) && this->GetEnergy() >= 10) {
        this->SetEnergy(this->GetEnergy() - 10);
        this->GetGameWorld().AddObject(std::make_unique<BlueBullet>(
            IMGID_BLUE_BULLET, // imageID
            nowX, nowY + 50, // x, y
            0, // direction
            1, // layer
            0.5 + 0.1 * this->m_upgrade, // size
            this->GetGameWorld(), // gameWorld
            5 + 3 * this->m_upgrade //damage
        ));
    }

    // Listen to shoot2 request
    if (this->GetGameWorld().GetKeyDown(KeyCode::FIRE2) && this->m_meteor > 0) {
        this->m_meteor--;
        this->GetGameWorld().AddObject(std::make_unique<Meteor>(
            IMGID_METEOR, // imageID
            nowX, nowY + 100, // x, y
            0, // direction
            1, // layer
            2.0, // size
            this->GetGameWorld() // gameWorld
        ));
    }

    // Restore energy
    if (this->GetEnergy() < 10) {
        this->SetEnergy(this->GetEnergy() + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Star/////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Star::Star(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeStar, 1, 0, 1, 0, 0) { }
// x = ?, y = ?, direction = 0, layer = 4, size = ?

void Star::Update() {
    // Check if the star is dead
    if (this->GetY() < 0) {
        this->SetIsDead();
        return;
    }

    // Move the star
    this->MoveTo(this->GetX(), this->GetY() - 1);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////Explosion////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Explosion::Explosion(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeExplosion, 1, 0, 0, 0, 0), 
    m_time(0) { }

void Explosion::Update() {
    // Update explosion size
    this->SetSize(this->GetSize() - 0.2);

    // Check if the explosion is finished
    this->m_time++;
    if (this->m_time >= 20) {
        this->SetIsDead();
    }
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////BlueBullet////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BlueBullet::BlueBullet(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld, int damage): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeBlueBullet, 1, damage, 6, 0, 0) { }

void BlueBullet::Update() {
    // Check if the bullet is dead
    if (this->GetIsDead()) {
        return;
    }

    // Check if the bullet is out of the screen
    if (this->GetY() >= WINDOW_HEIGHT) {
        this->SetIsDead();
        return;
    }

    // Check if the bullet hit the enemy
    std::for_each(this->GetGameWorld().GetObjects().begin(), 
        this->GetGameWorld().GetObjects().end(), 
        [this](std::unique_ptr<GameObject>& obj) {
            if (obj->GetIsDead()) {
                return;
            }
            ObjectType type = obj->GetType();
            if (type == TypeAlphaShip || type == TypeSigmaShip || type == TypeOmegaShip) {
                if (*obj & *this) {
                    obj->SetHealth(obj->GetHealth() - this->GetDamage());
                    this->SetIsDead();
                    if (obj->GetIsDead()) {
                        Destroy(obj);
                    }
                    return;
                }
            }
        }
    );

    // Move the bullet
    this->MoveTo(this->GetX(), this->GetY() + 6);

    // Recheck if the bullet hit the enemy
    std::for_each(this->GetGameWorld().GetObjects().begin(), 
        this->GetGameWorld().GetObjects().end(), 
        [this](std::unique_ptr<GameObject>& obj) {
            if (obj->GetIsDead()) {
                return;
            }            
            ObjectType type = obj->GetType();
            if (type == TypeAlphaShip || type == TypeSigmaShip || type == TypeOmegaShip) {
                if (*obj & *this) {
                    obj->SetHealth(obj->GetHealth() - this->GetDamage());
                    this->SetIsDead();
                    if (obj->GetIsDead()) {
                        Destroy(obj);
                    }
                    return;
                }
            }
        }
    );    

}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////Meteor////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Meteor::Meteor(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
    ObjectType::TypeMeteor, 1, 1e5, 2, 0, 0) { }

void Meteor::Update() {
    // Check if the meteor is dead
    if (this->GetIsDead()) {
        return;
    }

    // Check if the meteor is out of the screen
    if (this->GetY() >= WINDOW_HEIGHT) {
        this->SetIsDead();
        return;
    }

    // Check if the meteor hit the enemy
    std::for_each(this->GetGameWorld().GetObjects().begin(), 
        this->GetGameWorld().GetObjects().end(), 
        [this](std::unique_ptr<GameObject>& obj) {
            if (obj->GetIsDead()) {
                return;
            }
            ObjectType type = obj->GetType();
            if (type == TypeAlphaShip || type == TypeSigmaShip || type == TypeOmegaShip) {
                if (*obj & *this) {
                    Destroy(obj);                     
                    return;
                }
            }
        }
    );

    // Move the meteor
    this->MoveTo(this->GetX(), this->GetY() + 2);
    this->SetDirection((this->GetDirection() + 5) % 360);

    // Recheck if the meteor hit the enemy
    std::for_each(this->GetGameWorld().GetObjects().begin(), 
        this->GetGameWorld().GetObjects().end(), 
        [this](std::unique_ptr<GameObject>& obj) {
            if (obj->GetIsDead()) {
                return;
            }            
            ObjectType type = obj->GetType();
            if (type == TypeAlphaShip || type == TypeSigmaShip || type == TypeOmegaShip) {
                if (*obj & *this) {
                    Destroy(obj);
                    return;
                }
            }
        }
    );
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////RedBullet////////////////////////////////
//////////////////////////////////////////////////////////////////////////
RedBullet::RedBullet(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld, int damage): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeRedBullet, 1, damage, 2, 0, 0) { }

void RedBullet::Update() {
    // Check if the bullet is dead
    if (this->GetIsDead()) {
        return;
    }

    // Check if the bullet is out of the screen
    if (this->GetY() < 0) {
        this->SetIsDead();
        return;
    }

    // Check if the bullet hit the player
    if (*this & *this->GetGameWorld().m_player) {
        this->GetGameWorld().m_player->SetHealth(
            this->GetGameWorld().m_player->GetHealth() - this->GetDamage());
        this->SetIsDead();
        return;
    }

    // Move the bullet
    if (this->GetDirection() == 180) {
        this->MoveTo(this->GetX(), this->GetY() - 6);
    } else if (this->GetDirection() == 162) {
        this->MoveTo(this->GetX() + 2, this->GetY() - 6);
    } else if (this->GetDirection() == 198) {
        this->MoveTo(this->GetX() - 2, this->GetY() - 6);
    }

    // Recheck if the bullet hit the player
    if (*this & *this->GetGameWorld().m_player) {
        this->GetGameWorld().m_player->SetHealth(
            this->GetGameWorld().m_player->GetHealth() - this->GetDamage());
        this->SetIsDead();
        return;
    }    
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////EnemyShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
EnemyShip::EnemyShip(int imageID, int x, int y, int direction, int layer, 
        double size, GameWorld& gameWorld, ObjectType type, int health, 
        int damage, int speed, int energy, int score, int time, int strategy): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld,
        type, health, damage, speed, energy, score), 
    m_time(time), m_strategy(strategy) { }

int EnemyShip::GetTime() const {
    return this->m_time;
}

void EnemyShip::SetTime(int time) {
    this->m_time = time;
}

int EnemyShip::GetStrategy() const {
    return this->m_strategy;
}

void EnemyShip::SetStrategy(int strategy) {
    this->m_strategy = strategy;
}

bool EnemyShip::Collapse() {
    std::for_each(this->GetGameWorld().GetObjects().begin(), 
        this->GetGameWorld().GetObjects().end(), 
        [this](std::unique_ptr<GameObject>& obj) {
            if (obj->GetIsDead()) {
                return;
            }
            ObjectType type = obj->GetType();
            if (type == TypeBlueBullet) {
                if (*obj & *this) {
                    this->SetHealth(this->GetHealth() - obj->GetDamage());                     
                    obj->SetIsDead();
                }
            } else if (type == TypeMeteor) {
                if (*obj & *this) {
                    this->SetIsDead();
                }
            }
        }
    );
    if (*this & *this->GetGameWorld().m_player) {
        this->GetGameWorld().m_player->SetHealth(
            this->GetGameWorld().m_player->GetHealth() - 20);
        this->SetIsDead();
    }
    if (this->GetIsDead()) {
        Destroy(std::unique_ptr<GameObject>(this));
        return true;
    }
    return false;
}

void EnemyShip::Choose() {
    if (this->GetTime() <= 0) {
        int r = randInt(1, 3);
        if (r == 1) {
            this->SetStrategy(180);
        } else if (r == 2) {
            this->SetStrategy(162);
        } else if (r == 3) {
            this->SetStrategy(198);
        }
        this->SetTime(randInt(10, 50));
    } else if (this->GetX() < 0) {
        this->SetStrategy(162);
        this->SetTime(randInt(10, 50));
    } else if (this->GetX() >= WINDOW_WIDTH) {
        this->SetStrategy(198);
        this->SetTime(randInt(10, 50)); 
    }
}

void EnemyShip::Move() {
    this->SetTime(this->GetTime() - 1);
    if (this->GetStrategy()== 180) {
        this->MoveTo(this->GetX(), this->GetY() - this->GetSpeed());
    } else if (this->GetStrategy() == 198) {
        this->MoveTo(this->GetX() - this->GetSpeed(), this->GetY() - this->GetSpeed());
    } else if (this->GetStrategy() == 162) {
        this->MoveTo(this->GetX() + this->GetSpeed(), this->GetY() - this->GetSpeed());
    } 
}

void EnemyShip::Update() {
    // Check if the ship is dead
    if (this->GetIsDead()) {
        return;
    }

    // Check if the ship is out of the screen
    if (this->GetY() < 0) {
        this->SetIsDead();
        return;
    }

    // Check if the ship hit the player or his bullets
    if (this->Collapse()) {
        this->Rebirth();
        return;
    }

    // Attack the player
    this->Attack();

    // Fuel up energy
    this->Refuel();

    // Generate new strategy
    this->Choose();

    // Move the ship
    this->Move();

    // Recheck if the ship hit the player or his bullets
    if (this->Collapse()) {
        this->Rebirth();
        return;
    }        
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////AlphaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
AlphaShip::AlphaShip(int imageID, int x, int y, int direction, int layer, 
        double size, GameWorld& gameWorld, int health, int damage, int speed): 
    EnemyShip(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeAlphaShip, health, damage, speed, 25, 50, 0, 180) {}

void AlphaShip::Rebirth() { }

void AlphaShip::Attack() { 
    if (abs(this->GetX() - this->GetGameWorld().m_player->GetX()) <= 10) {
        if (this->GetEnergy() >= 25) {
            if (randInt(1, 100) <= 25) {
                this->SetEnergy(this->GetEnergy() - 25);
                this->GetGameWorld().AddObject(std::make_unique<RedBullet>(
                    IMGID_RED_BULLET, // image id
                    this->GetX(), this->GetY() - 50, // x, y
                    180, // direction
                    1, // layer
                    0.5, // size
                    this->GetGameWorld(), // game world
                    this->GetDamage() // damage
                ));
            }
        }
    }
}

void AlphaShip::Refuel() {
    if (this->GetEnergy() < 25) {
        this->SetEnergy(this->GetEnergy() + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////SigmaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
SigmaShip::SigmaShip(int imageID, int x, int y, int direction, int layer, 
        double size, GameWorld& gameWorld, int health, int speed): 
    EnemyShip(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeAlphaShip, health, 0, speed, 0, 100, 0, 180) {}

void SigmaShip::Rebirth() {
    if (randInt(1, 100) <= 20) {
        this->GetGameWorld().AddObject(std::make_unique<HealthWidget>(
            IMGID_HP_RESTORE_GOODIE, // image id
            this->GetX(), this->GetY(), // x, y
            0, // direction
            2, // layer
            0.5, // size
            this->GetGameWorld() // game world
        ));
    }
}

void SigmaShip::Attack() {
    if (abs(this->GetX() - this->GetGameWorld().m_player->GetX()) <= 10) {
        this->SetStrategy(180);
        this->SetTime(WINDOW_HEIGHT);
        this->SetSpeed(10);
    }    
}

void SigmaShip::Refuel() { }


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////OmegaShip////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OmegaShip::OmegaShip(int imageID, int x, int y, int direction, int layer, 
        double size, GameWorld& gameWorld, int health, int damage, int speed): 
    EnemyShip(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeAlphaShip, health, damage, speed, 50, 200, 0, 180) {}

void OmegaShip::Rebirth() { 
    if (randInt(1, 100) <= 40) {
        if (randInt(1, 100) <= 80) {
            this->GetGameWorld().AddObject(std::make_unique<UpgradeWidget>(
                IMGID_POWERUP_GOODIE, // image id
                this->GetX(), this->GetY(), // x, y
                0, // direction
                2, // layer
                0.5, // size
                this->GetGameWorld() // game world
            ));
        } else {
            this->GetGameWorld().AddObject(std::make_unique<MeteorWidget>(
                IMGID_METEOR_GOODIE, // image id
                this->GetX(), this->GetY(), // x, y
                0, // direction
                2, // layer
                0.5, // size
                this->GetGameWorld() // game world
            ));
        }
    }
}

void OmegaShip::Attack() { 
    if (this->GetEnergy() >= 50) {
        this->SetEnergy(this->GetEnergy() - 50);
        this->GetGameWorld().AddObject(std::make_unique<RedBullet>(
            IMGID_RED_BULLET, // image id
            this->GetX(), this->GetY() - 50, // x, y
            162, // direction
            1, // layer
            0.5, // size
            this->GetGameWorld(), // game world
            this->GetDamage() // damage
        ));
        this->GetGameWorld().AddObject(std::make_unique<RedBullet>(
            IMGID_RED_BULLET, // image id
            this->GetX(), this->GetY() - 50, // x, y
            198, // direction
            1, // layer
            0.5, // size
            this->GetGameWorld(), // game world
            this->GetDamage() // damage
        ));        
    }
}

void OmegaShip::Refuel() { 
    if (this->GetEnergy() < 50) {
        this->SetEnergy(this->GetEnergy() + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////SnackWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
SnackWidget::SnackWidget(int imageID, int x, int y, int direction, int layer, 
        double size, GameWorld& gameWorld, ObjectType type, int health, 
        int damage, int speed, int energy, int score): 
    GameObject(imageID, x, y, direction, layer, size, gameWorld, 
        type, health, damage, speed, energy, score) {}

void SnackWidget::Update() {
    // Check if the widget is dead
    if (this->GetIsDead()) {
        return;
    }

    // Check if the widget is out of the screen
    if (this->GetY() < 0) {
        this->SetIsDead();
        return;
    }

    // Check if the widget is colliding with the player
    if (*this & *this->GetGameWorld().m_player) {
        this->Effect();
        this->GetGameWorld().IncreaseScore(this->GetScore());
        this->SetIsDead();
        return;
    }

    // Move the widget
    this->MoveTo(this->GetX(), this->GetY() - 2);

    // Recheck if the widget is colliding with the player
    if (*this & *this->GetGameWorld().m_player) {
        this->Effect();
        this->GetGameWorld().IncreaseScore(this->GetScore());
        this->SetIsDead();
        return;
    }        
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////////HealthWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
HealthWidget::HealthWidget(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    SnackWidget(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeHealthWidget, 1, 0, 2, 0, 20) { }

void HealthWidget::Effect() {
    this->GetGameWorld().m_player->SetHealth(
        std::min(this->GetGameWorld().m_player->GetHealth() + 50, 100));
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////UpgradeWidget/////////////////////////////
//////////////////////////////////////////////////////////////////////////
UpgradeWidget::UpgradeWidget(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    SnackWidget(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeUpgradeWidget, 1, 0, 2, 0, 20) { }

void UpgradeWidget::Effect() {
    this->GetGameWorld().m_player->SetUpgrade(
        this->GetGameWorld().m_player->GetUpgrade() + 1);
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////MeteorWidget//////////////////////////////
//////////////////////////////////////////////////////////////////////////
MeteorWidget::MeteorWidget(int imageID, int x, int y, int direction, 
        int layer, double size, GameWorld& gameWorld): 
    SnackWidget(imageID, x, y, direction, layer, size, gameWorld, 
        ObjectType::TypeMeteorWidget, 1, 0, 2, 0, 20) { }

void MeteorWidget::Effect() {
    this->GetGameWorld().m_player->SetMeteor(
        this->GetGameWorld().m_player->GetMeteor() + 1);    
}