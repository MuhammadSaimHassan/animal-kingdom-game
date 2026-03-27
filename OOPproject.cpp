// Student Name: Muhammad Saim Hassan
// Student ID: i24-3159
// Section: A

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>  
#include <iomanip>

using namespace std;
using namespace sf;



// Game Constants - These define the core parameters of our game

const int WINDOW_WIDTH = 1024;                  // Width of game window
const int WINDOW_HEIGHT = 1024;                 // Height of game window
const int MAX_PETS = 4;                         // Maximum number of pets a player can have
const int MAX_GUILD_MEMBERS = 4;                // Maximum members in a guild
const int MAX_BATTLE_PETS = 4;                  // Maximum pets in a battle
const int INITIAL_COINS = 500;                  // Starting coins for player
const int SHOP_ITEMS_COUNT = 4;                 // Number of items in shop
const int TRAINING_COST = 50;                   // Cost to train a pet
const int POINTS_TO_WIN = 10;                   // Points needed to win a battle

class Pet; // Forward declaration 

/* PlayerInventory class - Manages all items the player has collected
   This includes healing potions, mana potions, speed buffs, and shields*/
class PlayerInventory {
private:
    int healingPotions;       // Number of healing potions
    int manaPotions;         // Number of mana potions
    int speedBuffs;         // Number of speed buff items
    int shields;           // Number of shield items

public:

    // Constructor initializes all counts to 0
    PlayerInventory() : healingPotions(0), manaPotions(0), speedBuffs(0), shields(0) {}

    // Getters for item counts
    int getHealingPotions() const { return healingPotions; }
    int getManaPotions() const { return manaPotions; }
    int getSpeedBuffs() const { return speedBuffs; }
    int getShields() const { return shields; }


    void addHealingPotion(int count = 1) { healingPotions += count; }
    void addManaPotion(int count = 1) { manaPotions += count; }
    void addSpeedBuff(int count = 1) { speedBuffs += count; }
    void addShield(int count = 1) { shields += count; }

    // Methods to use items - returns true if successful
    bool useHealingPotion() {
        if (healingPotions > 0) {
            healingPotions--;
            return true;
        }
        return false;
    }

    bool useManaPotion() {
        if (manaPotions > 0) {
            manaPotions--;
            return true;
        }
        return false;
    }

    bool useSpeedBuff() {
        if (speedBuffs > 0) {
            speedBuffs--;
            return true;
        }
        return false;
    }

    bool useShield() {
        if (shields > 0) {
            shields--;
            return true;
        }
        return false;
    }

    // Save inventory to file
    void saveToFile(const string& filename) const {
        ofstream out(filename);
        if (out) {
            out << healingPotions << " "
                << manaPotions << " "
                << speedBuffs << " "
                << shields;
        }
    }
    // Load inventory from file
    void loadFromFile(const string& filename) {
        ifstream in(filename);
        if (in) {
            in >> healingPotions
                >> manaPotions
                >> speedBuffs
                >> shields;
        }
    }
};

enum class GameState { MENU, PET_SELECTION, GUILD_MANAGEMENT, MATCH, SHOP, TRAINING, BATTLE, BATTLE_ITEM_SELECTION };
enum class PetType { DRAGON, PHOENIX, UNICORN, GRIFFIN, COUNT };
enum class GuildRole { TANK, HEALER, DAMAGE_DEALER, NONE };
enum class BattleType { ONE_VS_ONE, TWO_VS_TWO, GUILD_WAR };
enum class ArenaType { STANDARD, WALLED, TRAPPED };
/* Pet class - Represents a magical pet with attributes and abilities
   Each pet has stats (health, attack, defense, speed) and special abilities */
class Pet {

private:
    

    string name;              // Name of the pet
    PetType type;       // Species/type of pet
    Vector2f position = Vector2f(0, 0);             // Current position (for battle movement)
    int health;             // Current health points
    int attack;             // Attack strength
    int defense;             // Defense capability
    int speed;              // Movement speed
    int level;                   // Current level
    int xp;          // Current XP
    int energy;                 // Current energy (for attacks)
    int specialAbilityCharges;     // Charges for special abilities
    bool hasRebirthed;             // Flag for Phoenix rebirth ability
    bool isStunned;              // Stun status
    bool isShielded;               // Shield status

    // Private method to handle leveling up
    void levelUp() {
        if (xp >= 100) {
            level++;
            xp -= 100;
            health += 10;
            attack += 5;
            defense += 5;
            speed += 3;
        }
    }
public:

    // Constructor - initializes pet with base stats
    Pet(const string& name, PetType type)
        : name(name), type(type), health(100), attack(50), defense(50),
        speed(50), level(1), xp(0), energy(100), specialAbilityCharges(2),
        hasRebirthed(false), isStunned(false), isShielded(false) {
    }

    // Position getters/setters
    Vector2f getPosition() const { return position; }
    
    // Movement methods
    void setPosition(float x, float y) { position = Vector2f(x, y); }
    void move(float offsetX, float offsetY) {
        position.x += offsetX;
        position.y += offsetY;
    }
    float getMovementSpeed() const { return speed * 0.5f; } // Scale speed for movement

    // Basic getters for pet attributes
    const string& getName() const { return name; }
    PetType getType() const { return type; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getSpeed() const { return speed; }
    int getLevel() const { return level; }
    int getXp() const { return xp; }
    int getEnergy() const { return energy; }
    bool getIsStunned() const { return isStunned; }
    bool getIsShielded() const { return isShielded; }

    void increaseHealth(int amount) { health = min(health + amount, 100 + level * 10); }
    void increaseAttack(int amount) { attack += amount; }
    void increaseDefense(int amount) { defense += amount; }
    void increaseSpeed(int amount) { speed += amount; }
    void restoreEnergy(int amount) { energy = min(energy + amount, 100); }
    void addSpecialCharge() { specialAbilityCharges = min(specialAbilityCharges + 1, 2); }


    // Get names of special abilities based on pet type
    string getSpecialAbility1() const {
        switch (type) {
        case PetType::DRAGON: return "Fireball";
        case PetType::PHOENIX: return "Flame Heal";
        case PetType::UNICORN: return "Speed Burst";
        case PetType::GRIFFIN: return "Claw Strike";
        default: return "";
        }
    }

    string getSpecialAbility2() const {
        switch (type) {
        case PetType::DRAGON: return "Dragon Roar";
        case PetType::PHOENIX: return "Rebirth";
        case PetType::UNICORN: return "Magic Shield";
        case PetType::GRIFFIN: return "Dive Bomb";
        default: return "";
        }
    }

    int basicAttack() {
        energy = max(0, energy - 10);
        return attack + (rand() % 10);
    }

    int specialAttack1() {
        if (specialAbilityCharges <= 0) return 0;
        specialAbilityCharges--;

        switch (type) {
        case PetType::DRAGON: return attack * 2 + (rand() % 20);
        case PetType::PHOENIX: takeDamage(-30); return 0;
        case PetType::UNICORN: speed += 20; return 0;
        case PetType::GRIFFIN: return attack * 3 / 2 + (rand() % 15);
        default: return 0;
        }
    }

    void specialAttack2(Pet* target = nullptr) {
        if (specialAbilityCharges <= 0) return;
        specialAbilityCharges--;

        switch (type) {
        case PetType::DRAGON: if (target) target->stun(); break;
        case PetType::PHOENIX: break;
        case PetType::UNICORN: isShielded = true; break;
        case PetType::GRIFFIN: if (target) { target->takeDamage(attack / 2); target->stun(); } break;
        }
    }

    void takeDamage(int damage) {
        if (isShielded) {
            damage /= 2;
            isShielded = false;
        }

        if (type == PetType::PHOENIX && health <= 0 && !hasRebirthed) {
            health = 50;
            hasRebirthed = true;
            return;
        }

        int actualDamage = max(1, damage - defense / 2);
        health = max(0, health - actualDamage);
    }

    void stun() { isStunned = true; }
    void clearStun() { isStunned = false; }
    void prepareForBattle() {
        energy = 100;
        specialAbilityCharges = 2;
        hasRebirthed = false;
        isStunned = false;
        isShielded = false;
        position = Vector2f(0, 0); // Reset position
    }
    void gainXp(int amount) { xp += amount; levelUp(); }

    // Operator overloading for easy printing
    friend ostream& operator<<(ostream& os, const Pet& pet) {
        string typeStr; 
        switch (pet.type) {
        case PetType::DRAGON: typeStr = "Dragon"; break;
        case PetType::PHOENIX: typeStr = "Phoenix"; break;
        case PetType::UNICORN: typeStr = "Unicorn"; break;
        case PetType::GRIFFIN: typeStr = "Griffin"; break;
        default: typeStr = "Unknown";
        }

        os << pet.name << " (Type: " << typeStr
            << ", Lvl: " << pet.level << ", HP: " << pet.health
            << ", ATK: " << pet.attack << ", DEF: " << pet.defense
            << ", SPD: " << pet.speed << ")";
        return os;
    }



};

/* TrainingCamp class - Handles pet training mechanics
  Allows players to spend coins to improve their pets' stats*/
class TrainingCamp {

private:
    float trainingTime;  // Time left in current training
    bool isTraining;    // Flag for active training
    Pet* currentPet;   // Pointer to pet being trained
    int trainingType; // 0=HP, 1=ATACKK, 2=DEF, 3=SPD
    int coins;       // Cost of current training

public:
    TrainingCamp() : isTraining(false), trainingTime(0), currentPet(nullptr), trainingType(0), coins(0) {}

    void startTraining(Pet* pet, int statType, int cost, int& playerCoins) {
        if (!pet || isTraining || playerCoins < cost) return;

        playerCoins -= cost;
        currentPet = pet;
        trainingType = statType;
        trainingTime = 3.0f;
        isTraining = true;
    }

    void update(float deltaTime) {
        if (!isTraining) return;
        trainingTime -= deltaTime;

        if (trainingTime <= 0) {
            completeTraining();
            isTraining = false;
        }
    }

    void completeTraining() {
        if (!currentPet) return;

        switch (trainingType) {
        case 0: currentPet->increaseHealth(10); break;
        case 1: currentPet->increaseAttack(5); break;
        case 2: currentPet->increaseDefense(5); break;
        case 3: currentPet->increaseSpeed(3); break;
        }
        currentPet = nullptr;
    }

    bool getIsTraining() const { return isTraining; }
    float getTrainingProgress() const { return 1.0f - (trainingTime / 3.0f); }
};


/**
 * Guild class - Represents a player's guild/team of pets
 * Manages membership and roles of pets in the guild
 */
class Guild {

private:
    string name;            // Name of the guild
    Pet* members[MAX_GUILD_MEMBERS];      // Array of member pets
    GuildRole roles[MAX_GUILD_MEMBERS];   // Roles for each member
    int memberCount;    // Current number of members

public:
    Guild() : name("My Guild"), memberCount(0) {
        for (int i = 0; i < MAX_GUILD_MEMBERS; i++) {
            members[i] = nullptr;
            roles[i] = GuildRole::NONE;
        }
    }

    // Add a new member to the guild with specified role
    bool addMember(Pet* pet, GuildRole role) {
        if (memberCount >= MAX_GUILD_MEMBERS || !pet) return false;
        members[memberCount] = pet;
        roles[memberCount] = role;
        memberCount++;
        return true;
    }

    // Remove a member from the guild by index
    bool removeMember(int index) {
        if (index < 0 || index >= memberCount) return false;
        for (int i = index; i < memberCount - 1; i++) {
            members[i] = members[i + 1];
            roles[i] = roles[i + 1];
        }
        memberCount--;
        return true;
    }

    Pet* getMember(int index) const {
        if (index < 0 || index >= memberCount) return nullptr;
        return members[index];
    }

    GuildRole getRole(int index) const {
        if (index < 0 || index >= memberCount) return GuildRole::NONE;
        return roles[index];
    }

    bool setRole(int index, GuildRole role) {
        if (index < 0 || index >= memberCount) return false;
        roles[index] = role;
        return true;
    }

    int getMemberCount() const { return memberCount; }
    const string& getName() const { return name; }

    void setName(const string& newName) { name = newName; }
};

class Shop {
public:
    struct ShopItem {
        int cost = 0;
        string name;
        string description;
    };

    Shop() : playerCoins(INITIAL_COINS) {
        items[0] = { 100, "Healing Potion", "Restores 30 HP during battle" };
        items[1] = { 150, "Mana Potion", "Refills special ability charges" };
        items[2] = { 200, "Speed Buff", "Temporarily boosts speed by 20" };
        items[3] = { 250, "Shield", "Reduces damage by 50% for next attack" };
    }

    bool buyItem(int itemIndex, PlayerInventory& inventory) {
        if (itemIndex < 0 || itemIndex >= SHOP_ITEMS_COUNT) return false;
        if (playerCoins < items[itemIndex].cost) return false;

        playerCoins -= items[itemIndex].cost;

        switch (itemIndex) {
        case 0: inventory.addHealingPotion(); break;
        case 1: inventory.addManaPotion(); break;
        case 2: inventory.addSpeedBuff(); break;
        case 3: inventory.addShield(); break;
        }

        return true;
    }

    void setCoins(int amount) { playerCoins = amount; }
    void addCoins(int amount) { playerCoins += amount; }
    int getCoins() const { return playerCoins; }
    const ShopItem& getItem(int index) const {
        static ShopItem emptyItem = { 0, "", "" };
        return (index >= 0 && index < SHOP_ITEMS_COUNT) ? items[index] : emptyItem;
    }

private:
    int playerCoins;
    ShopItem items[SHOP_ITEMS_COUNT];
};





class BattleSystem {
public:


    BattleSystem() : battleType(BattleType::ONE_VS_ONE), playerWon(false), battleEnded(false),
        playerPoints(0), enemyPoints(0), arenaType(ArenaType::STANDARD) {
        resetPets();
    }
    static const int MAX_OBSTACLES = 3;
    // Movement methods
    void movePlayerPet(float offsetX, float offsetY) {
        if (playerPets[0]) {
            Vector2f newPos = playerPets[0]->getPosition() + Vector2f(offsetX, offsetY);

            // Battle area boundaries
            const float minX = -300.f, maxX = 300.f;
            const float minY = -200.f, maxY = 200.f;

            // Check boundaries
            if (newPos.x < minX || newPos.x > maxX || newPos.y < minY || newPos.y > maxY)
                return;

            // Convert to screen position for obstacle collision check
            Vector2f screenPos = newPos + Vector2f(250, 400);
            FloatRect playerBounds(screenPos.x - 25, screenPos.y - 25, 50, 50);

            for (int i = 0; i < MAX_OBSTACLES; ++i) {
                if (obstacles[i].getGlobalBounds().intersects(playerBounds)) {
                    return; // collision with obstacle, cancel move
                }
            }

            // No collision, apply move
            playerPets[0]->move(offsetX, offsetY);
        }
    }


    void setupBattle(Pet* playerPet, Pet* enemyPet, BattleType type = BattleType::ONE_VS_ONE, ArenaType arena = ArenaType::STANDARD) {
        resetPets();
        battleType = type;
        arenaType = arena; // Set arena type
        playerPoints = 0;
        enemyPoints = 0;

        if (playerPet) {
            playerPets[0] = playerPet;
            playerPetCount = 1;
            playerPet->setPosition(0, 0);  // Add this line
            playerPet->prepareForBattle();
        }
        if (enemyPet) {
            enemyPets[0] = enemyPet;
            enemyPetCount = 1;
            enemyPet->setPosition(0, 0);   // Add this line
            enemyPet->prepareForBattle();
        }

        playerWon = false;
        battleEnded = false;
        gameClock.restart();
        lastEnemyAttack = Time::Zero;
        lastPlayerAttack = Time::Zero;
        lastAbilityUse = Time::Zero;
        lastEnemyMove = Time::Zero;

        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            float x = static_cast<float>(rand() % 600 - 300); // Range: -300 to 300
            float y = static_cast<float>(rand() % 400 - 200); // Range: -200 to 200
            obstaclePositions[i] = Vector2f(x, y);
            obstacles[i].setSize(Vector2f(50, 50));
            obstacles[i].setFillColor(Color::Red);
            obstacles[i].setOrigin(25, 25);
            obstacles[i].setPosition(x + 250, y + 400); // shift to match pet drawing area
        }

    }

    void update(PlayerInventory& inventory) {
        if (battleEnded) return;

        Time currentTime = gameClock.getElapsedTime();

        // Enemy movement AI
        if ((currentTime - lastEnemyMove).asSeconds() > 0.8f) {
            if (enemyPets[0]) {
                float moveX = (rand() % 3 - 1) * enemyPets[0]->getMovementSpeed();
                float moveY = (rand() % 3 - 1) * enemyPets[0]->getMovementSpeed();
                enemyPets[0]->move(moveX, moveY);
                lastEnemyMove = currentTime;
            }
        }

        // Apply trap effect if in TRAPPED arena
        if (arenaType == ArenaType::TRAPPED && (currentTime - lastTrapCheck).asSeconds() > 1.0f) {
            applyTrapEffects();
            lastTrapCheck = currentTime;
        }

        if ((currentTime - lastEnemyAttack).asSeconds() > attackCooldown) {
            performEnemyAttack();
            lastEnemyAttack = currentTime;
        }

        checkBattleEnd();
    }
    const RectangleShape& getObstacle(int index) const {
        return obstacles[index];
    }


    void playerAttack() {
        if (battleEnded) return;

        Time currentTime = gameClock.getElapsedTime();
        if ((currentTime - lastPlayerAttack).asSeconds() > attackCooldown) {
            if (playerPets[0] && enemyPets[0]) {
                int damage = playerPets[0]->basicAttack();
                if (arenaType == ArenaType::WALLED) damage = static_cast<int>(damage * 0.8f); // 20% damage reduction
                if (damage > 0) {
                    enemyPets[0]->takeDamage(damage);
                    playerPoints += 1; // Award 1 point for basic attack
                }
            }
            lastPlayerAttack = currentTime;
        }
        checkBattleEnd();
    }

    void playerUseAbility(int abilityIndex) {
        if (battleEnded) return;

        Time currentTime = gameClock.getElapsedTime();
        if ((currentTime - lastAbilityUse).asSeconds() > abilityCooldown) {
            if (playerPets[0] && enemyPets[0]) {
                int damage = 0;
                if (abilityIndex == 0) {
                    damage = playerPets[0]->specialAttack1();
                    if (damage > 0) {
                        if (arenaType == ArenaType::WALLED) damage = static_cast<int>(damage * 0.8f); // 20% damage reduction
                        enemyPets[0]->takeDamage(damage);
                        playerPoints += 2; // Award 2 points for special attack
                    }
                }
                else if (abilityIndex == 1) {
                    playerPets[0]->specialAttack2(enemyPets[0]);
                    if (playerPets[0]->getType() != PetType::PHOENIX) {
                        playerPoints += 2; // Award 2 points for special attack
                    }
                }
            }
            lastAbilityUse = currentTime;
        }
        checkBattleEnd();
    }

    void playerUseItem(int itemIndex, PlayerInventory& inventory) {
        if (battleEnded || !playerPets[0]) return;

        bool itemUsed = false;
        switch (itemIndex) {
        case 0:
            if (inventory.useHealingPotion()) {
                playerPets[0]->increaseHealth(30);
                itemUsed = true;
            }
            break;
        case 1:
            if (inventory.useManaPotion()) {
                playerPets[0]->addSpecialCharge();
                playerPets[0]->addSpecialCharge();
                itemUsed = true;
            }
            break;
        case 2:
            if (inventory.useSpeedBuff()) {
                playerPets[0]->increaseSpeed(20);
                itemUsed = true;
            }
            break;
        case 3:
            if (inventory.useShield()) {
                playerPets[0]->increaseDefense(50);
                itemUsed = true;
            }
            break;
        }

        if (!itemUsed) {
            showMessage("You don't have this item!");
        }
    }

    void logBattleResult(bool playerWon, const string& playerPetName, const string& enemyPetName,
        BattleType battleType, ArenaType arenaType, int playerPoints, int enemyPoints) {
        ofstream logFile("battle_log.txt", ios::app);  // Append mode

        if (logFile) {
            // Get current time
            time_t now = time(nullptr);
            tm timeinfo;
            localtime_s(&timeinfo, &now);

            // Format time
            logFile << put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << " | ";

            // Battle type
            string battleStr;
            switch (battleType) {
            case BattleType::ONE_VS_ONE: battleStr = "1v1"; break;
            case BattleType::TWO_VS_TWO: battleStr = "2v2"; break;
            case BattleType::GUILD_WAR: battleStr = "Guild War"; break;
            }

            // Arena type
            string arenaStr;
            switch (arenaType) {
            case ArenaType::STANDARD: arenaStr = "Standard"; break;
            case ArenaType::WALLED: arenaStr = "Walled"; break;
            case ArenaType::TRAPPED: arenaStr = "Trapped"; break;
            }

            // Write log entry
            logFile << (playerWon ? "VICTORY" : "DEFEAT") << " | "
                << "Player: " << playerPetName << " vs Enemy: " << enemyPetName << " | "
                << "Type: " << battleStr << " | "
                << "Arena: " << arenaStr << " | "
                << "Score: " << playerPoints << "-" << enemyPoints << "\n";
        }
    }

    bool isBattleEnded() const { return battleEnded; }
    bool didPlayerWin() const { return playerWon; }
    Pet* getPlayerPet() const { return playerPets[0]; }
    Pet* getEnemyPet() const { return enemyPets[0]; }
    BattleType getBattleType() const { return battleType; }
    ArenaType getArenaType() const { return arenaType; }
    int getPlayerPoints() const { return playerPoints; }
    int getEnemyPoints() const { return enemyPoints; }

    void setMessageCallback(void (*callback)(const string&)) {
        showMessage = callback;
    }

private:
    void resetPets() {
        for (int i = 0; i < MAX_BATTLE_PETS; i++) {
            playerPets[i] = nullptr;
            enemyPets[i] = nullptr;
        }
        playerPetCount = 0;
        enemyPetCount = 0;
    }

    void applyTrapEffects() {
        if (playerPets[0] && (rand() % 100) < 10) { // 10% chance to stun
            playerPets[0]->stun();
            showMessage(playerPets[0]->getName() + " was stunned by a trap!");
        }
        if (enemyPets[0] && (rand() % 100) < 10) {
            enemyPets[0]->stun();
            showMessage(enemyPets[0]->getName() + " was stunned by a trap!");
        }
    }

    void performEnemyAttack() {
        if (!playerPets[0] || !enemyPets[0]) return;

        if (enemyPets[0]->getIsStunned()) {
            enemyPets[0]->clearStun();
            return;
        }

        int action = rand() % 3;

        switch (action) {
        case 0: {
            int damage = enemyPets[0]->basicAttack();
            if (arenaType == ArenaType::WALLED) damage *= 0.8f; // 20% damage reduction
            playerPets[0]->takeDamage(damage);
            enemyPoints += 1; // Award 1 point for basic attack
            break;
        }
        case 1:
            if (enemyPets[0]->getType() == PetType::PHOENIX) {
                enemyPets[0]->specialAttack1();
            }
            else {
                int damage = enemyPets[0]->specialAttack1();
                if (damage > 0) {
                    if (arenaType == ArenaType::WALLED) damage = static_cast<int>(damage * 0.8f); // 20% damage reduction
                    playerPets[0]->takeDamage(damage);
                    enemyPoints += 2; // Award 2 points for special attack
                }
            }
            break;
        case 2:
            enemyPets[0]->specialAttack2(playerPets[0]);
            if (enemyPets[0]->getType() != PetType::PHOENIX) {
                enemyPoints += 2; // Award 2 points for special attack
            }
            break;
        }
    }

    void checkBattleEnd() {
        bool enemyDefeated = enemyPets[0] ? enemyPets[0]->getHealth() <= 0 : false;
        bool playerDefeated = playerPets[0] ? playerPets[0]->getHealth() <= 0 : false;

        if (enemyDefeated) {
            playerPoints += 5;
            battleEnded = true;
            playerWon = true;
        }
        else if (playerDefeated) {
            enemyPoints += 5;
            battleEnded = true;
            playerWon = false;
        }
        else if (playerPoints >= POINTS_TO_WIN) {
            battleEnded = true;
            playerWon = true;
        }
        else if (enemyPoints >= POINTS_TO_WIN) {
            battleEnded = true;
            playerWon = false;
        }

        if (battleEnded) {
            string playerName = playerPets[0] ? playerPets[0]->getName() : "None";
            string enemyName = enemyPets[0] ? enemyPets[0]->getName() : "None";
            logBattleResult(playerWon, playerName, enemyName, battleType, arenaType, playerPoints, enemyPoints);
        }
    }

    RectangleShape obstacles[MAX_OBSTACLES];
    Vector2f obstaclePositions[MAX_OBSTACLES];

    BattleType battleType;
    ArenaType arenaType; // New: Arena type for obstacles
    Pet* playerPets[MAX_BATTLE_PETS];
    Pet* enemyPets[MAX_BATTLE_PETS];
    Time lastEnemyMove;
    int playerPetCount;
    int enemyPetCount;
    bool playerWon;
    bool battleEnded;
    int playerPoints; // New: Player's points
    int enemyPoints;  // New: Enemy's points

    Clock gameClock;
    Time lastPlayerAttack;
    Time lastEnemyAttack;
    Time lastAbilityUse;
    Time lastTrapCheck; // New: For trap effect timing
    float attackCooldown = 1.0f;
    float abilityCooldown = 3.0f;

    void (*showMessage)(const string&) = nullptr;
};

class MagicalPetsKingdom {

private:
    static const int MAX_OBSTACLES = 5;
    sf::RectangleShape obstacles[MAX_OBSTACLES];
    sf::Vector2f obstaclePositions[MAX_OBSTACLES] = {
        {300, 300}, {400, 400}, {500, 250}, {350, 500}, {600, 350}
    };
    RenderWindow window;
    Font font;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Music backgroundMusic;

    Texture dragonTexture, phoenixTexture, unicornTexture, griffinTexture;
    Sprite dragonSprite, phoenixSprite, unicornSprite, griffinSprite;

    GameState currentState;
    Shop shop;
    Guild guild;
    BattleSystem battleSystem;
    PlayerInventory playerInventory;
    TrainingCamp trainingCamp;

    int selectedPetIndex;
    int playerPetCount;
    BattleType battleType;
    Pet* selectedBattlePet;
    int selectedItemIndex;

    unique_ptr<Pet> playerPets[MAX_PETS];
    unique_ptr<Pet> enemyPets[MAX_PETS];

    RectangleShape buttons[20];
    Text buttonTexts[20];
    int buttonCount;

    Text menuTexts[30];
    int menuTextCount;

    bool showMessageFlag = false;
    Text messageText;
    Clock messageClock;

    bool loadResources() {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            cerr << "Failed to load font\n";
            return false;
        }

        if (!backgroundMusic.openFromFile("background.ogg")) {
            cerr << "Failed to load background music\n";
        }
        else {
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(50);
            backgroundMusic.play();
        }

        if (!backgroundTexture.loadFromFile("background.jpg")) {
            cerr << "Failed to load background image\n";
            return false;
        }
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / backgroundTexture.getSize().x,
            static_cast<float>(WINDOW_HEIGHT) / backgroundTexture.getSize().y
        );

        if (!dragonTexture.loadFromFile("dragon.png")) {
            cerr << "Failed to load dragon image\n";
            return false;
        }
        dragonSprite.setTexture(dragonTexture);

        if (!phoenixTexture.loadFromFile("phoenix.png")) {
            cerr << "Failed to load phoenix image\n";
            return false;
        }
        phoenixSprite.setTexture(phoenixTexture);

        if (!unicornTexture.loadFromFile("unicorn (2).png")) {
            cerr << "Failed to load unicorn image\n";
            return false;
        }
        unicornSprite.setTexture(unicornTexture);

        if (!griffinTexture.loadFromFile("griffin.png")) {
            cerr << "Failed to load griffin image\n";
            return false;
        }
        griffinSprite.setTexture(griffinTexture);

        constexpr float spriteScale = 0.3f;
        dragonSprite.setScale(spriteScale, spriteScale);
        phoenixSprite.setScale(spriteScale, spriteScale);
        unicornSprite.setScale(spriteScale, spriteScale);
        griffinSprite.setScale(spriteScale, spriteScale);

        messageText.setFont(font);
        messageText.setCharacterSize(24);
        messageText.setFillColor(Color::Red);
        messageText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 50);

        return true;
    }

    void initializePets() {
        playerPets[0] = make_unique<Pet>("Dragon", PetType::DRAGON);
        playerPets[1] = make_unique<Pet>("Phoenix", PetType::PHOENIX);
        playerPets[2] = make_unique<Pet>("Unicorn", PetType::UNICORN);
        playerPets[3] = make_unique<Pet>("Griffin", PetType::GRIFFIN);
        playerPetCount = 4;

        enemyPets[0] = make_unique<Pet>("Wild Dragon", PetType::DRAGON);
        enemyPets[1] = make_unique<Pet>("Wild Phoenix", PetType::PHOENIX);
        enemyPets[2] = make_unique<Pet>("Wild Unicorn", PetType::UNICORN);
        enemyPets[3] = make_unique<Pet>("Wild Griffin", PetType::GRIFFIN);
    }

    void initMenu() {
        buttonCount = 0;
        menuTextCount = 0;

        switch (currentState) {
        case GameState::MENU: initMainMenu(); break;
        case GameState::PET_SELECTION: initPetSelectionMenu(); break;
        case GameState::GUILD_MANAGEMENT: initGuildMenu(); break;
        case GameState::MATCH: initMatchMenu(); break;
        case GameState::SHOP: initShopMenu(); break;
        case GameState::TRAINING: initTrainingMenu(); break;
        case GameState::BATTLE: initBattleMenu(); break;
        case GameState::BATTLE_ITEM_SELECTION: initBattleItemMenu(); break;
        }
    }

    void initMainMenu() {
        addMenuText("MAGICAL PETS KINGDOM", 72, Color(255, 215, 0), WINDOW_WIDTH / 2, 100, true);

        createButton("SELECT PETS", WINDOW_WIDTH / 2 - 150, 250, Color(0, 100, 200), 300, 60);
        createButton("GUILD MANAGEMENT", WINDOW_WIDTH / 2 - 150, 350, Color(0, 100, 200), 300, 60);
        createButton("BATTLE ARENA", WINDOW_WIDTH / 2 - 150, 450, Color(0, 100, 200), 300, 60);
        createButton("ITEM SHOP", WINDOW_WIDTH / 2 - 150, 550, Color(0, 100, 200), 300, 60);
        createButton("TRAINING CAMP", WINDOW_WIDTH / 2 - 150, 650, Color(150, 0, 200), 300, 60);
        createButton("EXIT", WINDOW_WIDTH / 2 - 150, 750, Color(200, 50, 50), 300, 60);
    }

    void initPetSelectionMenu() {
        addMenuText("SELECT YOUR PETS", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 50, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        addMenuText("AVAILABLE PETS", 36, Color::White, WINDOW_WIDTH / 4, 120, true);

        float startY = 180;
        for (int i = 0; i < playerPetCount; i++) {
            if (playerPets[i]) {
                Color btnColor = (selectedPetIndex == i) ? Color::Green : Color(0, 100, 200);
                createButton(playerPets[i]->getName(), WINDOW_WIDTH / 4 - 150, startY + i * 100, btnColor, 300, 80);

                Sprite* petSprite = getPetSprite(playerPets[i]->getType());
                if (petSprite) {
                    petSprite->setPosition(WINDOW_WIDTH / 4 + 180, startY + i * 100);
                }
            }
        }

        if (selectedPetIndex >= 0 && selectedPetIndex < playerPetCount) {
            Pet* pet = playerPets[selectedPetIndex].get();

            addMenuText("PET DETAILS", 36, Color::White, 3 * WINDOW_WIDTH / 4, 120, true);

            Sprite* petSprite = getPetSprite(pet->getType());
            if (petSprite) {
                petSprite->setScale(0.5f, 0.5f);
                petSprite->setPosition(3 * WINDOW_WIDTH / 4 - 100, 180);
                petSprite->setScale(0.3f, 0.3f);
            }

            stringstream ss;
            ss << *pet; // Use overloaded operator
            addMenuText(ss.str(), 28, Color::White, 3 * WINDOW_WIDTH / 4, 300, true);
            addMenuText("Energy: " + to_string(pet->getEnergy()), 28, Color::White, 3 * WINDOW_WIDTH / 4, 340, true);

            addMenuText("Abilities:", 28, Color::Yellow, 3 * WINDOW_WIDTH / 4, 400, true);
            addMenuText("1. " + pet->getSpecialAbility1(), 24, Color::White, 3 * WINDOW_WIDTH / 4, 440, true);
            addMenuText("2. " + pet->getSpecialAbility2(), 24, Color::White, 3 * WINDOW_WIDTH / 4, 480, true);

            createButton("ADD TO GUILD", 3 * WINDOW_WIDTH / 4 - 100, 520, Color::Green, 200, 50);
        }
    }

    void initGuildMenu() {
        addMenuText("GUILD MANAGEMENT", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 50, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        addMenuText("GUILD: " + guild.getName(), 36, Color::Cyan, WINDOW_WIDTH / 2, 120, true);

        float startY = 180;
        for (int i = 0; i < guild.getMemberCount(); i++) {
            Pet* pet = guild.getMember(i);
            GuildRole role = guild.getRole(i);
            string roleStr;
            switch (role) {
            case GuildRole::TANK: roleStr = "Tank"; break;
            case GuildRole::HEALER: roleStr = "Healer"; break;
            case GuildRole::DAMAGE_DEALER: roleStr = "Damage"; break;
            default: roleStr = "None";
            }

            RectangleShape card(Vector2f(800, 100));
            card.setPosition(WINDOW_WIDTH / 2 - 400, startY + i * 120);
            card.setFillColor(Color(0, 0, 100, 200));
            card.setOutlineThickness(2);
            card.setOutlineColor(Color::White);

            Sprite* petSprite = getPetSprite(pet->getType());
            if (petSprite) {
                petSprite->setPosition(WINDOW_WIDTH / 2 - 380, startY + i * 120 + 10);
            }

            stringstream ss;
            ss << *pet << " (" << roleStr << ")";
            addMenuText(ss.str(), 28, Color::White, WINDOW_WIDTH / 2 - 250, startY + i * 120 + 20);

            createButton("CHANGE ROLE", WINDOW_WIDTH / 2 + 200, startY + i * 120 + 20, Color::Magenta, 150, 30);
            createButton("REMOVE", WINDOW_WIDTH / 2 + 200, startY + i * 120 + 60, Color::Red, 150, 30);
        }

        if (guild.getMemberCount() < MAX_GUILD_MEMBERS) {
            createButton("ADD MEMBER", WINDOW_WIDTH / 2 - 100, startY + guild.getMemberCount() * 120 + 20, Color::Green, 200, 50);
        }

        if (guild.getMemberCount() > 0) {
            createButton("START GUILD WAR", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT - 100, Color(255, 165, 0), 300, 60);
        }
    }

    void initMatchMenu() {
        addMenuText("BATTLE ARENA", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 100, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        createButton("1 vs 1 BATTLE", WINDOW_WIDTH / 2 - 200, 250, Color(200, 0, 0), 400, 80);
        createButton("2 vs 2 BATTLE", WINDOW_WIDTH / 2 - 200, 350, Color(0, 0, 200), 400, 80);
        createButton("GUILD WAR", WINDOW_WIDTH / 2 - 200, 450, Color(200, 0, 200), 400, 80);

        addMenuText("Select battle type and choose your pet(s)", 28, Color::White, WINDOW_WIDTH / 2, 600, true);
    }

    void initShopMenu() {
        addMenuText("ITEM SHOP", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 50, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        addMenuText("COINS: " + to_string(shop.getCoins()), 36, Color(255, 215, 0), WINDOW_WIDTH - 200, 50);

        float startX = WINDOW_WIDTH / 2 - 200;
        float startY = 150;

        for (int i = 0; i < SHOP_ITEMS_COUNT; i++) {
            const auto& item = shop.getItem(i);

            RectangleShape card(Vector2f(400, 120));
            card.setPosition(startX, startY);
            card.setFillColor(Color(0, 0, 100, 200));
            card.setOutlineThickness(2);
            card.setOutlineColor(Color::White);

            addMenuText(item.name, 28, Color::Yellow, startX + 10, startY + 10);
            addMenuText(item.description, 20, Color::White, startX + 10, startY + 40);
            addMenuText("Price: " + to_string(item.cost) + " coins", 20, Color::Cyan, startX + 10, startY + 70);

            if (shop.getCoins() >= item.cost) {
                createButton("BUY", startX + 300, startY + 40, Color::Green, 80, 40);
            }
            else {
                createButton("CAN'T AFFORD", startX + 260, startY + 40, Color(100, 100, 100), 140, 40);
            }

            startY += 140;
        }
    }

    void initTrainingMenu() {
        buttonCount = 0;
        menuTextCount = 0;

        addMenuText("TRAINING CAMP", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 50, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        if (selectedPetIndex >= 0 && selectedPetIndex < playerPetCount) {
            Pet* pet = playerPets[selectedPetIndex].get();

            stringstream ss;
            ss << *pet;
            addMenuText(ss.str(), 36, Color::Cyan, WINDOW_WIDTH / 2, 120, true);
            addMenuText("Coins: " + to_string(shop.getCoins()), 24, Color::Yellow, WINDOW_WIDTH - 100, 50);

            createButton("HP Training (+10 HP) - 50 coins", WINDOW_WIDTH / 2 - 200, 200, Color::Red, 400, 50);
            createButton("ATK Training (+5 ATK) - 50 coins", WINDOW_WIDTH / 2 - 200, 280, Color(200, 100, 0), 400, 50);
            createButton("DEF Training (+5 DEF) - 50 coins", WINDOW_WIDTH / 2 - 200, 360, Color(0, 100, 200), 400, 50);
            createButton("SPD Training (+3 SPD) - 50 coins", WINDOW_WIDTH / 2 - 200, 440, Color::Green, 400, 50);

            if (trainingCamp.getIsTraining()) {
                RectangleShape progressBg(Vector2f(400, 20));
                progressBg.setPosition(WINDOW_WIDTH / 2 - 200, 520);
                progressBg.setFillColor(Color(70, 70, 70));
                window.draw(progressBg);

                RectangleShape progressBar(Vector2f(400 * trainingCamp.getTrainingProgress(), 20));
                progressBar.setPosition(WINDOW_WIDTH / 2 - 200, 520);
                progressBar.setFillColor(Color::Yellow);
                window.draw(progressBar);

                addMenuText("Training in progress...", 24, Color::White, WINDOW_WIDTH / 2, 550, true);
            }
        }
        else {
            addMenuText("Select a pet first!", 36, Color::Red, WINDOW_WIDTH / 2, 300, true);
        }
    }

    void initBattleMenu() {
        addMenuText("BATTLE!", 72, Color::Red, WINDOW_WIDTH / 2, 50, true);
        createButton("FLEE", 50, 50, Color(200, 50, 50), 120, 50);

        // Display arena type
        string arenaStr;
        switch (battleSystem.getArenaType()) {
        case ArenaType::STANDARD: arenaStr = "Standard Arena"; break;
        case ArenaType::WALLED: arenaStr = "Walled Arena (20% damage reduction)"; break;
        case ArenaType::TRAPPED: arenaStr = "Trapped Arena (10% stun chance)"; break;
        }
        addMenuText(arenaStr, 28, Color::Yellow, WINDOW_WIDTH / 2, 120, true);

        // Display points
        addMenuText("Player Points: " + to_string(battleSystem.getPlayerPoints()), 24, Color::Green, WINDOW_WIDTH / 4, 150, true);
        addMenuText("Enemy Points: " + to_string(battleSystem.getEnemyPoints()), 24, Color::Red, 3 * WINDOW_WIDTH / 4, 150, true);

        Pet* playerPet = battleSystem.getPlayerPet();
        if (playerPet) {
            createButton("ATTACK", WINDOW_WIDTH - 250, 600, Color(200, 0, 0), 200, 50);
            createButton(playerPet->getSpecialAbility1(), WINDOW_WIDTH - 250, 500, Color(200, 100, 0), 200, 50);
            createButton(playerPet->getSpecialAbility2(), WINDOW_WIDTH - 250, 400, Color(200, 0, 100), 200, 50);
            createButton("USE ITEM", WINDOW_WIDTH - 250, 300, Color(0, 100, 200), 200, 50);
        }

        addMenuText("Potions: " + to_string(playerInventory.getHealingPotions()), 20, Color::White, WINDOW_WIDTH - 250, 200);
        addMenuText("Mana: " + to_string(playerInventory.getManaPotions()), 20, Color::White, WINDOW_WIDTH - 250, 230);
        addMenuText("Buffs: " + to_string(playerInventory.getSpeedBuffs()), 20, Color::White, WINDOW_WIDTH - 250, 260);
        addMenuText("Shields: " + to_string(playerInventory.getShields()), 20, Color::White, WINDOW_WIDTH - 250, 290);
    }

    void initBattleItemMenu() {
        addMenuText("SELECT ITEM", 60, Color(255, 215, 0), WINDOW_WIDTH / 2, 50, true);
        createButton("BACK", 50, 50, Color(200, 50, 50), 120, 50);

        float startX = WINDOW_WIDTH / 2 - 200;
        float startY = 150;

        if (playerInventory.getHealingPotions() > 0) {
            createButton("HEAL POTION", startX, startY, Color::Green, 200, 50);
            addMenuText("Available: " + to_string(playerInventory.getHealingPotions()), 20, Color::White, startX + 220, startY + 25);
            startY += 70;
        }

        if (playerInventory.getManaPotions() > 0) {
            createButton("MANA POTION", startX, startY, Color::Blue, 200, 50);
            addMenuText("Available: " + to_string(playerInventory.getManaPotions()), 20, Color::White, startX + 220, startY + 25);
            startY += 70;
        }

        if (playerInventory.getSpeedBuffs() > 0) {
            createButton("SPEED BUFF", startX, startY, Color::Yellow, 200, 50);
            addMenuText("Available: " + to_string(playerInventory.getSpeedBuffs()), 20, Color::White, startX + 220, startY + 25);
            startY += 70;
        }

        if (playerInventory.getShields() > 0) {
            createButton("SHIELD", startX, startY, Color::Magenta, 200, 50);
            addMenuText("Available: " + to_string(playerInventory.getShields()), 20, Color::White, startX + 220, startY + 25);
        }
    }

    void createButton(const string& text, float x, float y,
        Color color = Color(0, 100, 200),
        float width = 200, float height = 50) {
        if (buttonCount >= 20) return;

        buttons[buttonCount] = RectangleShape(Vector2f(width, height));
        buttons[buttonCount].setPosition(x, y);
        buttons[buttonCount].setFillColor(color);
        buttons[buttonCount].setOutlineThickness(2);
        buttons[buttonCount].setOutlineColor(Color::White);

        buttonTexts[buttonCount].setFont(font);
        buttonTexts[buttonCount].setString(text);
        buttonTexts[buttonCount].setCharacterSize(24);
        buttonTexts[buttonCount].setFillColor(Color::White);

        FloatRect textRect = buttonTexts[buttonCount].getLocalBounds();
        buttonTexts[buttonCount].setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        buttonTexts[buttonCount].setPosition(x + width / 2.0f, y + height / 2.0f);

        buttonCount++;
    }

    void addMenuText(const string& text, int size, const Color& color,
        float x, float y, bool center = false) {
        if (menuTextCount >= 30) return;

        menuTexts[menuTextCount].setFont(font);
        menuTexts[menuTextCount].setString(text);
        menuTexts[menuTextCount].setCharacterSize(size);
        menuTexts[menuTextCount].setFillColor(color);

        if (center) {
            FloatRect textRect = menuTexts[menuTextCount].getLocalBounds();
            menuTexts[menuTextCount].setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);
            menuTexts[menuTextCount].setPosition(x, y);
        }
        else {
            menuTexts[menuTextCount].setPosition(x, y);
        }

        menuTextCount++;
    }

    Sprite* getPetSprite(PetType type) {
        switch (type) {
        case PetType::DRAGON: return &dragonSprite;
        case PetType::PHOENIX: return &phoenixSprite;
        case PetType::UNICORN: return &unicornSprite;
        case PetType::GRIFFIN: return &griffinSprite;
        default: return nullptr;
        }
    }

    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                handleButtonClicks(mousePos);
            }
            if (currentState == GameState::BATTLE) {
                if (event.type == Event::KeyPressed) {
                    float moveSpeed = 5.0f;
                    if (battleSystem.getPlayerPet()) {
                        moveSpeed = battleSystem.getPlayerPet()->getMovementSpeed();
                    }

                    if (event.key.code == Keyboard::W) {
                        battleSystem.movePlayerPet(0, -moveSpeed); // Up
                    }
                    else if (event.key.code == Keyboard::S) {
                        battleSystem.movePlayerPet(0, moveSpeed); // Down
                    }
                    else if (event.key.code == Keyboard::A) {
                        battleSystem.movePlayerPet(-moveSpeed, 0); // Left
                    }
                    else if (event.key.code == Keyboard::D) {
                        battleSystem.movePlayerPet(moveSpeed, 0); // Right
                    }
                }
            }
        }
    }

    void handleButtonClicks(const Vector2f& mousePos) {
        for (int i = 0; i < buttonCount; i++) {
            if (buttons[i].getGlobalBounds().contains(mousePos)) {
                string buttonText = buttonTexts[i].getString();

                switch (currentState) {
                case GameState::MENU: handleMainMenuButtons(buttonText); break;
                case GameState::PET_SELECTION: handlePetSelectionButtons(buttonText, i); break;
                case GameState::GUILD_MANAGEMENT: handleGuildButtons(buttonText, i); break;
                case GameState::MATCH: handleMatchButtons(buttonText); break;
                case GameState::SHOP: handleShopButtons(buttonText, i); break;
                case GameState::TRAINING: handleTrainingButtons(buttonText, i); break;
                case GameState::BATTLE: handleBattleButtons(buttonText); break;
                case GameState::BATTLE_ITEM_SELECTION: handleBattleItemButtons(buttonText, i); break;
                }
                break;
            }
        }
    }

    void handleMainMenuButtons(const string& buttonText) {
        if (buttonText == "SELECT PETS") {
            currentState = GameState::PET_SELECTION;
            selectedPetIndex = 0;
        }
        else if (buttonText == "GUILD MANAGEMENT") {
            currentState = GameState::GUILD_MANAGEMENT;
        }
        else if (buttonText == "BATTLE ARENA") {
            currentState = GameState::MATCH;
        }
        else if (buttonText == "ITEM SHOP") {
            currentState = GameState::SHOP;
        }
        else if (buttonText == "TRAINING CAMP") {
            currentState = GameState::TRAINING;
            selectedPetIndex = 0;
        }
        else if (buttonText == "EXIT") {
            window.close();
            return;
        }
        initMenu();
    }

    void handlePetSelectionButtons(const string& buttonText, int buttonIndex) {
        if (buttonText == "BACK") {
            currentState = GameState::MENU;
        }
        else if (buttonText == "ADD TO GUILD") {
            if (selectedPetIndex >= 0 && selectedPetIndex < playerPetCount) {
                if (guild.getMemberCount() < MAX_GUILD_MEMBERS) {
                    guild.addMember(playerPets[selectedPetIndex].get(), GuildRole::NONE);
                }
            }
        }
        else {
            for (int i = 0; i < playerPetCount; i++) {
                if (buttonText == playerPets[i]->getName()) {
                    selectedPetIndex = i;
                    break;
                }
            }
        }
        initMenu();
    }

    void handleGuildButtons(const string& buttonText, int buttonIndex) {
        if (buttonText == "BACK") {
            currentState = GameState::MENU;
        }
        else if (buttonText == "ADD MEMBER") {
            currentState = GameState::PET_SELECTION;
        }
        else if (buttonText == "START GUILD WAR") {
            if (guild.getMemberCount() > 0) {
                selectedBattlePet = guild.getMember(0);
                Pet* enemyPet = enemyPets[rand() % playerPetCount].get();
                battleSystem.setupBattle(selectedBattlePet, enemyPet, BattleType::GUILD_WAR, ArenaType::TRAPPED); // Use TRAPPED arena for guild wars
                currentState = GameState::BATTLE;
            }
        }
        else if (buttonText == "CHANGE ROLE") {
            int memberIndex = (buttonIndex - 2) / 3;
            if (memberIndex >= 0 && memberIndex < guild.getMemberCount()) {
                GuildRole currentRole = guild.getRole(memberIndex);
                GuildRole newRole = static_cast<GuildRole>((static_cast<int>(currentRole) + 1) % 3);
                guild.setRole(memberIndex, newRole);
            }
        }
        else if (buttonText == "REMOVE") {
            int memberIndex = (buttonIndex - 3) / 3;
            if (memberIndex >= 0 && memberIndex < guild.getMemberCount()) {
                guild.removeMember(memberIndex);
            }
        }
        initMenu();
    }

    void handleMatchButtons(const string& buttonText) {
        if (buttonText == "BACK") {
            currentState = GameState::MENU;
        }
        else if (buttonText == "1 vs 1 BATTLE") {
            battleType = BattleType::ONE_VS_ONE;
            currentState = GameState::PET_SELECTION;
        }
        else if (buttonText == "2 vs 2 BATTLE") {
            battleType = BattleType::TWO_VS_TWO;
            currentState = GameState::PET_SELECTION;
        }
        else if (buttonText == "GUILD WAR") {
            battleType = BattleType::GUILD_WAR;
            if (guild.getMemberCount() > 0) {
                selectedBattlePet = guild.getMember(0);
                Pet* enemyPet = enemyPets[rand() % playerPetCount].get();
                battleSystem.setupBattle(selectedBattlePet, enemyPet, BattleType::GUILD_WAR, ArenaType::TRAPPED);
                currentState = GameState::BATTLE;
            }
        }
        initMenu();
    }

    void handleShopButtons(const string& buttonText, int buttonIndex) {
        if (buttonText == "BACK") {
            currentState = GameState::MENU;
        }
        else if (buttonText == "BUY") {
            int itemIndex = (buttonIndex - 2) / 3;
            if (itemIndex >= 0 && itemIndex < SHOP_ITEMS_COUNT) {
                if (shop.buyItem(itemIndex, playerInventory)) {
                    showMessage("Purchased " + shop.getItem(itemIndex).name + "!");
                }
                else {
                    showMessage("Not enough coins!");
                }
            }
        }
        initMenu();
    }

    void handleTrainingButtons(const string& buttonText, int buttonIndex) {
        if (buttonText == "BACK") {
            currentState = GameState::MENU;
        }
        else if (buttonText.find("HP Training") != string::npos) {
            int currentCoins = shop.getCoins();
            trainingCamp.startTraining(playerPets[selectedPetIndex].get(), 0, TRAINING_COST, currentCoins);
            shop.setCoins(currentCoins);
        }
        else if (buttonText.find("ATK Training") != string::npos) {
            int currentCoins = shop.getCoins();
            trainingCamp.startTraining(playerPets[selectedPetIndex].get(), 1, TRAINING_COST, currentCoins);
            shop.setCoins(currentCoins);
        }
        else if (buttonText.find("DEF Training") != string::npos) {
            int currentCoins = shop.getCoins();
            trainingCamp.startTraining(playerPets[selectedPetIndex].get(), 2, TRAINING_COST, currentCoins);
            shop.setCoins(currentCoins);
        }
        else if (buttonText.find("SPD Training") != string::npos) {
            int currentCoins = shop.getCoins();
            trainingCamp.startTraining(playerPets[selectedPetIndex].get(), 3, TRAINING_COST, currentCoins);
            shop.setCoins(currentCoins);
        }
        initMenu();
    }

    void handleBattleButtons(const string& buttonText) {
        if (buttonText == "FLEE") {
            currentState = GameState::MENU;
        }
        else if (buttonText == "USE ITEM") {
            currentState = GameState::BATTLE_ITEM_SELECTION;
        }
        else {
            Pet* playerPet = battleSystem.getPlayerPet();
            if (playerPet) {
                if (buttonText == "ATTACK") {
                    battleSystem.playerAttack();
                }
                else if (buttonText == playerPet->getSpecialAbility1()) {
                    battleSystem.playerUseAbility(0);
                }
                else if (buttonText == playerPet->getSpecialAbility2()) {
                    battleSystem.playerUseAbility(1);
                }
            }
        }
        checkBattleEnd();
    }

    void handleBattleItemButtons(const string& buttonText, int buttonIndex) {
        if (buttonText == "BACK") {
            currentState = GameState::BATTLE;
        }
        else if (buttonText == "HEAL POTION") {
            battleSystem.playerUseItem(0, playerInventory);
            currentState = GameState::BATTLE;
        }
        else if (buttonText == "MANA POTION") {
            battleSystem.playerUseItem(1, playerInventory);
            currentState = GameState::BATTLE;
        }
        else if (buttonText == "SPEED BUFF") {
            battleSystem.playerUseItem(2, playerInventory);
            currentState = GameState::BATTLE;
        }
        else if (buttonText == "SHIELD") {
            battleSystem.playerUseItem(3, playerInventory);
            currentState = GameState::BATTLE;
        }
        initMenu();
    }

    void checkBattleEnd() {
        if (battleSystem.isBattleEnded()) {
            if (battleSystem.didPlayerWin()) {
                shop.addCoins(100);
                if (selectedBattlePet) {
                    selectedBattlePet->gainXp(50);
                }
                showMessage("You won the battle! +100 coins");
            }
            else {
                showMessage("You lost the battle!");
            }
            currentState = GameState::MENU;
            initMenu();
        }
    }

    void showMessage(const string& message) {
        messageText.setString(message);
        showMessageFlag = true;
        messageClock.restart();
    }

    void update(float deltaTime) {
        if (currentState == GameState::BATTLE) {
            battleSystem.update(playerInventory);
            checkBattleEnd();
        }
        else if (currentState == GameState::TRAINING) {
            trainingCamp.update(deltaTime);
        }

        if (showMessageFlag && messageClock.getElapsedTime().asSeconds() > 3.0f) {
            showMessageFlag = false;
        }
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);

        for (int i = 0; i < menuTextCount; i++) {
            window.draw(menuTexts[i]);
        }

        for (int i = 0; i < buttonCount; i++) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }

        if (currentState == GameState::BATTLE || currentState == GameState::BATTLE_ITEM_SELECTION) {
            renderBattleStatus();
        }

        if (currentState == GameState::PET_SELECTION || currentState == GameState::GUILD_MANAGEMENT) {
            for (int i = 0; i < buttonCount; i++) {
                if (buttons[i].getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))) {
                    string buttonText = buttonTexts[i].getString();
                    for (int j = 0; j < playerPetCount; j++) {
                        if (buttonText == playerPets[j]->getName()) {
                            Sprite* petSprite = getPetSprite(playerPets[j]->getType());
                            if (petSprite) {
                                petSprite->setPosition(buttons[i].getPosition().x + buttons[i].getSize().x + 20,
                                    buttons[i].getPosition().y);
                                window.draw(*petSprite);
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (currentState == GameState::GUILD_MANAGEMENT) {
            for (int i = 0; i < guild.getMemberCount(); i++) {
                Sprite* petSprite = getPetSprite(guild.getMember(i)->getType());
                if (petSprite) {
                    petSprite->setPosition(WINDOW_WIDTH / 2 - 380, 180 + i * 120 + 10);
                    window.draw(*petSprite);
                }
            }
        }

        if (currentState == GameState::PET_SELECTION && selectedPetIndex >= 0 && selectedPetIndex < playerPetCount) {
            Sprite* petSprite = getPetSprite(playerPets[selectedPetIndex]->getType());
            if (petSprite) {
                petSprite->setScale(0.5f, 0.5f);
                petSprite->setPosition(3 * WINDOW_WIDTH / 4 - 100, 180);
                window.draw(*petSprite);
                petSprite->setScale(0.3f, 0.3f);
            }
        }

        if (currentState == GameState::TRAINING && trainingCamp.getIsTraining()) {
            RectangleShape progressBg(Vector2f(400, 20));
            progressBg.setPosition(WINDOW_WIDTH / 2 - 200, 520);
            progressBg.setFillColor(Color(70, 70, 70));
            window.draw(progressBg);

            RectangleShape progressBar(Vector2f(400 * trainingCamp.getTrainingProgress(), 20));
            progressBar.setPosition(WINDOW_WIDTH / 2 - 200, 520);
            progressBar.setFillColor(Color::Yellow);
            window.draw(progressBar);
        }

        if (showMessageFlag) {
            window.draw(messageText);
        }

        window.display();
    }

    void renderBattleStatus()
    {
        for (int i = 0; i < BattleSystem::MAX_OBSTACLES; ++i) {
            window.draw(battleSystem.getObstacle(i));
        }

        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            obstacles[i].setSize(Vector2f(50, 50));
            obstacles[i].setFillColor(Color(128, 0, 0)); // Dark red
            obstacles[i].setPosition(obstaclePositions[i]);
        }



        Pet* playerPet = battleSystem.getPlayerPet();
        if (playerPet) {
            renderPetInBattle(playerPet, 250, 400, Color::Green);
        }

        Pet* enemyPet = battleSystem.getEnemyPet();
        if (enemyPet) {
            renderPetInBattle(enemyPet, 650, 400, Color::Red);
        }
    }

    void renderPetInBattle(Pet* pet, float baseX, float baseY, const Color& color) {

        if (!pet) return;
        Vector2f pos = pet->getPosition();
        Sprite* petSprite = getPetSprite(pet->getType());
        if (petSprite) {
            petSprite->setScale(0.5f, 0.5f);
            Vector2f pos = pet->getPosition();
            petSprite->setPosition(baseX + pos.x, baseY + pos.y);
            window.draw(*petSprite);
            petSprite->setScale(0.3f, 0.3f);
        }

        // Health bar (position relative to pet)
        RectangleShape healthBarBg(Vector2f(200, 20));
        healthBarBg.setPosition(baseX + pos.x - 100, baseY + pos.y - 80);
        healthBarBg.setFillColor(Color(70, 70, 70));
        window.draw(healthBarBg);

        RectangleShape healthBar(Vector2f(
            static_cast<float>(pet->getHealth()) * 2.0f,
            20.0f));
        healthBar.setPosition(baseX + pos.x - 100, baseY + pos.y - 80);
        healthBar.setFillColor(color);
        window.draw(healthBar);


        // Pet name and status
        stringstream ss;
        ss << pet->getName();
        Text nameText(ss.str(), font, 24);
        nameText.setPosition(baseX + pos.x - nameText.getLocalBounds().width / 2,
            baseY + pos.y - 110);
        window.draw(nameText);

        if (pet->getIsStunned()) {
            Text stunnedText("STUNNED", font, 20);
            stunnedText.setFillColor(Color::Red);
            stunnedText.setPosition(baseX + pos.x - stunnedText.getLocalBounds().width / 2,
                baseY + pos.y + 100);
            window.draw(stunnedText);
        }
    }
public:
    MagicalPetsKingdom()
        : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Magical Pets Kingdom"),
        currentState(GameState::MENU), selectedPetIndex(0), playerPetCount(0),
        battleType(BattleType::ONE_VS_ONE), selectedBattlePet(nullptr), selectedItemIndex(-1) {

        srand(static_cast<unsigned>(time(nullptr)));
        if (!loadResources()) {
            cerr << "Failed to load some resources\n";
            window.close();
        }

        playerInventory.loadFromFile("player_inventory.txt");

        battleSystem.setMessageCallback([](const string& msg) {
            });

        initializePets();
        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            obstacles[i].setSize(Vector2f(50, 50));
            obstacles[i].setFillColor(Color(128, 0, 0)); // Dark red
            obstacles[i].setPosition(obstaclePositions[i]);
        }

        initMenu();
    }

    ~MagicalPetsKingdom() {
        backgroundMusic.stop();
        playerInventory.saveToFile("player_inventory.txt");
    }

    void run() {
        Clock clock;
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            handleEvents();
            update(deltaTime);
            render();
        }
    }
};
//this is the main menu.
int main() {
    MagicalPetsKingdom game;
    game.run();
    return 0;
}