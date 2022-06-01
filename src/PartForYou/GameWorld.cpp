#include <sstream>

#include "GameWorld.h"

GameWorld::GameWorld(): m_life(3), m_player(), m_data() { }

void GameWorld::Init() {
    // Initialize game status

    // Add player
    this->m_player = std::make_unique<Player>(
        IMGID_DAWNBREAKER, // image id
        300, 100, // x, y
        0, // direction
        0, // layer
        1.0, // size
        *this // game world
    );

    // Add stars
    for (int i = 0; i < 30; i++) {
        int x = randInt(0, WINDOW_WIDTH - 1);
        int y = randInt(0, WINDOW_HEIGHT - 1);
        double size = randInt(10, 40) / 100.00;
        this->m_data.push_back(std::make_unique<Star>(
            IMGID_STAR, // image id
            x, y, // x, y
            0, // direction
            4, // layer
            size, // size
            *this // game world
        ));
    }
}

LevelStatus GameWorld::Update() {
    // Add status
    if (randInt(1, 30) == 1) {
        int x = randInt(0, WINDOW_WIDTH - 1);
        int y = WINDOW_HEIGHT - 1;
        double size = randInt(10, 40) / 100.00;
        this->m_data.push_back(std::make_unique<Star>(
            IMGID_STAR, // image id 
            x, y, // x, y
            0, // direction
            4, // layer
            size, // size
            *this // game world
        ));
    }

    // Decide if add ship
    int level = this->GetLevel();
    int required = 3 * level;
    int destroyed = this->m_player->GetDestroyed();
    int toDestroy = required - destroyed;
    int maxOnScreen = (5 + level) / 2;
    int allowed = std::min(maxOnScreen, toDestroy);
    int onScreen = std::count_if(this->m_data.begin(), this->m_data.end(), 
        [](const std::unique_ptr<GameObject>& obj) {
            return obj->GetType() == GameObject::ObjectType::TypeAlphaShip \
                || obj->GetType() == GameObject::ObjectType::TypeSigmaShip \
                || obj->GetType() == GameObject::ObjectType::TypeOmegaShip;
    });

    // Select ship type and add ship
    if (onScreen < allowed) {
        int x = randInt(0, WINDOW_WIDTH - 1);
        int y = WINDOW_HEIGHT - 1;

        int p1 = 6;
        int p2 = 2 * std::max(level - 1, 0);
        int p3 = 3 * std::max(level - 2, 0);
        int r = randInt(1, p1 + p2 + p3);
        if (r <= p1) {
            this->m_data.push_back(std::make_unique<AlphaShip>(
                IMGID_ALPHATRON, // image id
                x, y, // x, y
                180, // direction
                0, // layer
                1.0, // size
                *this, // game world
                20 + 2 * level, // health
                4 + level, // damage
                2 + level / 5 // speed                
            ));
        } else if (r <= p1 + p2) {
            this->m_data.push_back(std::make_unique<SigmaShip>(
                IMGID_SIGMATRON, // image id
                x, y, // x, y
                180, // direction
                0, // layer
                1.0, // size
                *this, // game world
                25 + 5 * level, // health
                2 + level / 5 // speed
            ));
        } else if (r <= p1 + p2 + p3) {
            this->m_data.push_back(std::make_unique<OmegaShip>(
                IMGID_OMEGATRON, // image id
                x, y, // x, y
                180, // direction
                0, // layer
                1.0, // size
                *this, // game world
                20 + level, // health
                2 + 2 * level, // damage
                3 + level / 4 // speed
            ));
        }
    }

    // Update all objects
    for (const std::unique_ptr<GameObject>& obj : this->m_data) {
        obj->Update();
    } 
    this->m_player->Update();

    // Check if player is dead
    if (this->m_player->GetIsDead()) {
        this->m_life--;
        return LevelStatus::DAWNBREAKER_DESTROYED;
    }

    // Check if level is completed
    if (this->m_player->GetDestroyed() >= required) {
        return LevelStatus::LEVEL_CLEARED;
    }

    // Delete all destroyed objects
    this->m_data.remove_if([&](const std::unique_ptr<GameObject>& obj) {
        return obj->GetIsDead();
    });

    // Show message
    std::stringstream message;
    message << "HP: " << this->m_player->GetHealth() << "/100   Meteors: " \
        << this->m_player->GetMeteor() << "   Lives: " << this->m_life \
        << "   Level: " << this->GetLevel() \
        << "   Enemies: " << this->m_player->GetDestroyed() << "/" << required \
        << "   Score: " << this->GetScore();
    this->SetStatusBarMessage(message.str());

    return LevelStatus::ONGOING;
}

void GameWorld::CleanUp() {
    this->m_player = nullptr;
    this->m_data.clear();
}


bool GameWorld::IsGameOver() const {
    return this->m_life <= 0;
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////GameWorld////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameWorld::AddObject(std::unique_ptr<GameObject> obj) {
    this->m_data.push_back(std::move(obj));
}


std::list<std::unique_ptr<GameObject>>& GameWorld::GetObjects() {
    return this->m_data;
}