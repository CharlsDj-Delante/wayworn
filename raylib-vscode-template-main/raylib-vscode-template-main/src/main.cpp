#include "raylib.h"
#include "game_data.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace
{
Font UI_FONT = {0};
}

#define DrawText(text, posX, posY, fontSize, tint) \
    DrawTextEx(UI_FONT, text, Vector2{static_cast<float>(posX), static_cast<float>(posY)}, \
               static_cast<float>(fontSize), 0.0f, tint)
#define MeasureText(text, fontSize) \
    static_cast<int>(MeasureTextEx(UI_FONT, text, static_cast<float>(fontSize), 0.0f).x)

namespace
{
using namespace wayworn_data;
const int MAP_SIZE = 12;

const Color INK = {46, 31, 24, 255};
const Color PAPER = {244, 232, 202, 255};
const Color PAPER_RAISED = {235, 220, 181, 255};
const Color PAPER_INSET = {218, 198, 151, 255};
const Color MUTED = {111, 84, 58, 255};
const Color EMBER = {126, 57, 34, 255};
const Color EMBER_BRIGHT = {151, 73, 40, 255};
const Color BLOOD = {119, 39, 34, 255};
const Color BORDER = {169, 131, 77, 255};
const Color GOOD = {69, 91, 57, 255};
const Color NIGHT = {43, 27, 25, 255};

enum Screen
{
    TITLE,
    FORGE,
    GAME,
    EPILOGUE
};

enum Page
{
    ROAD,
    ITEM_LEDGER,
    QUEST_LOG,
    MAP
};

enum QuestType
{
    QUEST_EXPLORE,
    QUEST_HUNT,
    QUEST_GATHER,
    QUEST_NEWS
};

const int QUEST_CAPACITY = 12;

struct Quest
{
    QuestType type;
    int target;
    int destinationX;
    int destinationY;
    int progress;
    int goal;
    int difficulty;
    int rewardGold;
    int rewardItem;
    int rewardQuantity;
    bool complete;
    std::string title;
    std::string description;
};

struct QuestOffer
{
    bool active;
    Quest quest;
};

enum Activity
{
    FREE_ROAM,
    IN_TAVERN,
    TALKING,
    SEARCHING,
    EVENT_CHOICE,
    TRADING
};

enum SocialSceneKind
{
    SOCIAL_NONE,
    SOCIAL_NPC_EXCHANGE,
    SOCIAL_BANDIT_THREAT,
    SOCIAL_CREATURE_THREAT
};

struct Stack
{
    int item;
    int quantity;
};

struct GameState
{
    Screen screen;
    Page page;
    std::string name;
    bool editingName;
    int origin;
    int attributes[6];
    int skills[SKILL_COUNT];
    int skillPractice[SKILL_COUNT];
    bool traits[TRAIT_COUNT];
    int pointsRemaining;
    Activity activity;
    std::string activityText;
    int map[MAP_SIZE][MAP_SIZE];
    int landmarks[MAP_SIZE][MAP_SIZE];
    bool discoveredPOI[MAP_SIZE][MAP_SIZE];
    bool seen[MAP_SIZE][MAP_SIZE];
    int x;
    int y;
    int day;
    int timeOfDay;
    int season;
    int weather;
    int exposure;
    int injuries[4];
    int bleeding;
    int infection;
    int turn;
    int worldSeed;
    int hp;
    int maxHp;
    int hunger;
    int fatigue;
    int gold;
    int equippedWeapon;
    int equippedArmor;
    int equippedHelmet;
    int equippedGloves;
    int equippedBoots;
    std::vector<Stack> inventory;
    std::vector<std::string> journal;
    int discovered[CREATURE_COUNT];
    int enemyIndex;
    int enemyHp;
    int combatTurn;
    int playerAC;
    int enemyMorale;
    int enemyTelegraph;
    int stunnedTurns;
    int proneTurns;
    int frightenedTurns;
    bool guarding;
    bool weakened;
    bool dead;
    bool won;
    int daysSurvived;
    int characterStartDay;
    int creaturesBested;
    int killCount;
    std::string notableKills[10];
    int factionRep[FACTION_COUNT];
    int factionPower[FACTION_COUNT];
    int factionUnrest[FACTION_COUNT];
    int kingdomStability[KINGDOM_COUNT];
    int settlementProsperity[SETTLEMENT_COUNT];
    int lastWorldSimDay;
    int debts;
    int favors;
    int grudges;
    int storyStage[23];
    int eventHistory[EVENT_COUNT];
    int eventHistoryCount;
    int lastEvents[4];
    int eventStreak;
    int pageScroll;
    int roamingCreature;
    int roamingCreatureTurns;
    int wanderingNpc;
    int wanderingNpcTurns;
    int wanderingNpcActivity;
    int pendingEvent;
    int activeNpc;
    int activeFallen;
    int npcTrust[NPC_COUNT];
    bool npcKnown[NPC_COUNT];
    bool npcDead[NPC_COUNT];
    int npcKnownX[NPC_COUNT];
    int npcKnownY[NPC_COUNT];
    SocialSceneKind socialSceneKind;
    int socialSceneNpcA;
    int socialSceneNpcB;
    int socialSceneCreature;
    std::string socialSceneDescription;
    std::string worldEventHeadline;
    int worldEventDay;
    bool worldInitialized;
    bool campBuilt;
    int inventoryAge[ITEM_COUNT];
    Quest quests[QUEST_CAPACITY];
    int questCount;
    QuestOffer questOffer;
    int fallenCount;
    std::string fallenName[8];
    std::string fallenCause[8];
    int fallenX[8];
    int fallenY[8];
    int fallenGear[8][ITEM_COUNT];
    float inventoryWeight;
    bool debugMode;
    std::string debugReport;
    std::string saveError;
    std::string ending;
};

GameState game;
std::mt19937 worldRng;
bool questRewardInProgress = false;
bool pointerPressActive = false;
bool pointerPressDragged = false;
Vector2 pointerPressStart = {0.0f, 0.0f};
Rectangle selectionPulseRect = {0.0f, 0.0f, 0.0f, 0.0f};
float selectionPulseTimer = 0.0f;
float pageTransitionTimer = 0.0f;
float encounterFlashTimer = 0.0f;
void saveGame();
bool loadGame();
const char *terrainName(int terrain);
int itemQuantity(int item);
void spendHour();
int marketPrice(int item);
void initializeWorldSimulation();
void simulateWorldDay();
std::string randomDialogue();
std::string npcDialogue(int npcIndex, const std::string &context);
bool hasQuestRoom();
void updateLocalPresence();
const Settlement *settlementAt(int x, int y);
int npcForFaction(int faction);
int factionAt(int x, int y);
const char *terrainName(int terrain);
void assignQuestDestination(Quest &quest);
void consumeItem(int item);
bool hasSavedJourney()
{
    return FileExists("wayworn_save.dat") || FileExists("wayworn_save.bak");
}
const char *STAT_NAMES[] = {"STR", "DEX", "CON", "INT", "WIS", "CHA"};
const char *STAT_GUIDES[] = {"Melee damage", "Attack accuracy", "Maximum health", "Find useful items", "Forage & search", "Better conversations"};
const char *WEATHER_NAMES[] = {"Clear", "Rain", "Snow", "Fog", "Wind"};
const char *SEASON_NAMES[] = {"Spring", "Summer", "Autumn", "Winter"};
const char *WANDERING_NPC_ACTIVITIES[] = {
    "mending a rain-split coat beneath a tree",
    "counting the last of their supplies beside the road",
    "trying to coax a mule across a muddy track",
    "sketching the ridge line on the back of a meal wrapper",
    "gathering bitterroot from a patch of sheltered ground",
    "warming their hands over a thumb-sized fire",
    "turning a cracked cup over in their hands",
    "following a string of old mile markers",
    "watching the clouds gather over the distant hills",
    "resting beside the path with a half-finished letter"
};

int randomBetween(int low, int high)
{
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(worldRng);
}

int clampValue(int value, int low, int high)
{
    return std::max(low, std::min(value, high));
}

void updateScrollInput(Rectangle viewport, int maximumScroll)
{
    if (CheckCollisionPointRec(GetMousePosition(), viewport))
        game.pageScroll = clampValue(game.pageScroll - static_cast<int>(GetMouseWheelMove() * 48),
                                     0, maximumScroll);
    else
    {
        GetMouseWheelMove();
        game.pageScroll = clampValue(game.pageScroll, 0, maximumScroll);
    }

    if (pointerPressActive && pointerPressDragged &&
        CheckCollisionPointRec(pointerPressStart, viewport) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        game.pageScroll = clampValue(game.pageScroll - static_cast<int>(GetMouseDelta().y),
                                     0, maximumScroll);
    }
}

void drawUiAnimations()
{
    if (selectionPulseTimer > 0.0f)
    {
        float progress = 1.0f - selectionPulseTimer / 0.18f;
        float expansion = 8.0f * progress;
        unsigned char alpha = static_cast<unsigned char>(190.0f * (1.0f - progress));
        Rectangle pulse = {selectionPulseRect.x - expansion, selectionPulseRect.y - expansion,
                           selectionPulseRect.width + expansion * 2.0f,
                           selectionPulseRect.height + expansion * 2.0f};
        DrawRectangleLinesEx(pulse, 2.0f, {EMBER.r, EMBER.g, EMBER.b, alpha});
    }
    if (pageTransitionTimer > 0.0f)
    {
        float progress = pageTransitionTimer / 0.22f;
        unsigned char alpha = static_cast<unsigned char>(30.0f * progress);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {PAPER.r, PAPER.g, PAPER.b, alpha});
    }
    if (encounterFlashTimer > 0.0f)
    {
        float progress = encounterFlashTimer / 0.42f;
        unsigned char alpha = static_cast<unsigned char>(38.0f * progress);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {BLOOD.r, BLOOD.g, BLOOD.b, alpha});
    }
}

void addJournal(const std::string &line)
{
    game.journal.insert(game.journal.begin(), line);
    if (game.journal.size() > 16) game.journal.pop_back();
}

void grantTrait(int trait)
{
    if (trait < 0 || trait >= TRAIT_COUNT || game.traits[trait]) return;
    game.traits[trait] = true;
    addJournal(std::string("A lasting trait takes root: ") + TRAIT_NAMES[trait] + ".");
}

void practiceSkill(int skill)
{
    if (skill < 0 || skill >= SKILL_COUNT || game.skills[skill] >= 5) return;
    ++game.skillPractice[skill];
    if (game.skillPractice[skill] >= 3)
    {
        game.skillPractice[skill] = 0;
        ++game.skills[skill];
        addJournal(std::string("Your ") + SKILL_NAMES[skill] + " skill grows through practice.");
    }
}

std::string skillSummary()
{
    std::string summary = "SK C/S/A/R/T ";
    for (int i = 0; i < SKILL_COUNT; ++i)
    {
        if (i) summary += "/";
        summary += std::to_string(game.skills[i]);
    }
    int traitCount = 0;
    for (int i = 0; i < TRAIT_COUNT; ++i) if (game.traits[i]) ++traitCount;
    summary += "  T:" + std::to_string(traitCount);
    return summary;
}

std::string traitSummary()
{
    std::string summary = "Traits: ";
    bool first = true;
    for (int i = 0; i < TRAIT_COUNT; ++i)
    {
        if (!game.traits[i]) continue;
        if (!first) summary += ", ";
        summary += TRAIT_NAMES[i];
        first = false;
    }
    return first ? summary + "none" : summary;
}

void updateQuestProgress(QuestType type, int target, int amount);

void addItem(int item, int quantity)
{
    if (item < 0 || item >= ITEM_COUNT || quantity <= 0) return;
    float carried = 0.0f;
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
        carried += ITEMS[game.inventory[i].item].weight * game.inventory[i].quantity;
    float capacity = 18.0f + std::max(0, game.attributes[0] - 10) * 1.5f;
    if (carried + ITEMS[item].weight * quantity > capacity)
    {
        addJournal(std::string("You leave ") + ITEMS[item].name + " behind; your pack is at its safe weight.");
        return;
    }
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
    {
        if (game.inventory[i].item == item)
        {
            game.inventory[i].quantity += quantity;
            if (!questRewardInProgress) updateQuestProgress(QUEST_GATHER, item, quantity);
            return;
        }
    }
    Stack stack = {item, quantity};
    game.inventory.push_back(stack);
    game.inventoryAge[item] = 0;
    if (!questRewardInProgress) updateQuestProgress(QUEST_GATHER, item, quantity);
}

int itemQuantity(int item)
{
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
    {
        if (game.inventory[i].item == item) return game.inventory[i].quantity;
    }
    return 0;
}

bool isMonsterLootItem(int item)
{
    return (item >= MONSTER_LOOT_FIRST_ITEM && item < FIRST_NEW_WEAPON) ||
           (item >= MONSTER_LOOT_NEW_FIRST_ITEM &&
            item < MONSTER_LOOT_NEW_FIRST_ITEM + MONSTER_LOOT_NEW_COUNT);
}

int monsterLootValue()
{
    int value = 0;
    for (int item = 0; item < ITEM_COUNT; ++item)
        if (isMonsterLootItem(item)) value += itemQuantity(item) * marketPrice(item);
    return value;
}

void sellMonsterLoot()
{
    const Settlement *place = settlementAt(game.x, game.y);
    if (!place || std::string(place->kind) == "CAMP") return;
    int earnings = monsterLootValue();
    if (earnings <= 0)
    {
        game.activityText = "You have no monster trophies to sell.";
        return;
    }
    std::vector<std::string> sold;
    for (int item = 0; item < ITEM_COUNT; ++item)
    {
        if (!isMonsterLootItem(item)) continue;
        int quantity = itemQuantity(item);
        if (quantity <= 0) continue;
        sold.push_back(std::string(ITEMS[item].name) + " x" + std::to_string(quantity));
        game.inventory.erase(std::remove_if(game.inventory.begin(), game.inventory.end(),
            [item](const Stack &stack) { return stack.item == item; }), game.inventory.end());
        game.inventoryAge[item] = 0;
    }
    game.gold += earnings;
    game.settlementProsperity[static_cast<int>(place - SETTLEMENTS)] =
        clampValue(game.settlementProsperity[static_cast<int>(place - SETTLEMENTS)] + 2, 0, 100);
    game.activityText = "The trader pays " + std::to_string(earnings) + " gold for your trophies.";
    std::string note = "You sell monster loot for " + std::to_string(earnings) + " gold: ";
    for (std::size_t i = 0; i < sold.size(); ++i)
    {
        if (i > 0) note += ", ";
        note += sold[i];
    }
    addJournal(note + ".");
}

int ownedItemCount()
{
    int count = 0;
    for (int i = 0; i < ITEM_COUNT; ++i)
        if (itemQuantity(i) > 0) ++count;
    return count;
}

int equippedItemForSlot(EquipmentSlot slot)
{
    switch (slot)
    {
        case SLOT_WEAPON: return game.equippedWeapon;
        case SLOT_HELMET: return game.equippedHelmet;
        case SLOT_BODY: return game.equippedArmor;
        case SLOT_GLOVES: return game.equippedGloves;
        case SLOT_BOOTS: return game.equippedBoots;
        default: return -1;
    }
}

const char *equipmentSlotName(EquipmentSlot slot)
{
    switch (slot)
    {
        case SLOT_WEAPON: return "Weapon";
        case SLOT_HELMET: return "Helmet";
        case SLOT_BODY: return "Body armor";
        case SLOT_GLOVES: return "Gloves";
        case SLOT_BOOTS: return "Boots";
        default: return "";
    }
}

bool isEquipped(int item)
{
    return item >= 0 && item < ITEM_COUNT && ITEMS[item].slot != SLOT_NONE &&
           equippedItemForSlot(ITEMS[item].slot) == item;
}

void recalculateEquipment()
{
    game.playerAC = 10;
    const int slots[] = {game.equippedHelmet, game.equippedArmor, game.equippedGloves, game.equippedBoots};
    for (std::size_t i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i)
        if (slots[i] >= 0 && slots[i] < ITEM_COUNT) game.playerAC += ITEMS[slots[i]].amount;
}

bool equipItem(int item)
{
    if (item < 0 || item >= ITEM_COUNT || itemQuantity(item) <= 0) return false;
    switch (ITEMS[item].slot)
    {
        case SLOT_WEAPON: game.equippedWeapon = item; break;
        case SLOT_HELMET: game.equippedHelmet = item; break;
        case SLOT_BODY: game.equippedArmor = item; break;
        case SLOT_GLOVES: game.equippedGloves = item; break;
        case SLOT_BOOTS: game.equippedBoots = item; break;
        default: return false;
    }
    recalculateEquipment();
    return true;
}

int equippedArmorLoad()
{
    const int slots[] = {game.equippedHelmet, game.equippedArmor, game.equippedGloves, game.equippedBoots};
    int load = 0;
    for (std::size_t i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i)
        if (slots[i] >= 0 && slots[i] < ITEM_COUNT) load += ITEMS[slots[i]].drawback;
    return load;
}

int equippedExposureProtection()
{
    int protection = 0;
    const int slots[] = {game.equippedHelmet, game.equippedArmor};
    for (std::size_t i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i)
        if (slots[i] >= 0 && slots[i] < ITEM_COUNT) protection += ITEMS[slots[i]].benefit;
    return protection;
}

int equipmentAttackModifier()
{
    int modifier = 0;
    const int slots[] = {game.equippedWeapon, game.equippedGloves};
    for (std::size_t i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i)
        if (slots[i] >= 0 && slots[i] < ITEM_COUNT)
            modifier += ITEMS[slots[i]].benefit - ITEMS[slots[i]].drawback;
    return modifier;
}

int equipmentTravelPace()
{
    return game.equippedBoots >= 0 && game.equippedBoots < ITEM_COUNT ?
        ITEMS[game.equippedBoots].benefit : 0;
}

int equippedWeaponDamageBonus()
{
    if (game.equippedWeapon < 0 || game.equippedWeapon >= ITEM_COUNT) return 0;
    int base = game.equippedWeapon == 0 ? 2 :
               game.equippedWeapon == 2 ? 1 :
               std::max(0, (ITEMS[game.equippedWeapon].amount - 4) / 2);
    return base + ITEMS[game.equippedWeapon].benefit - ITEMS[game.equippedWeapon].drawback;
}

std::string equipmentStatLine(int item)
{
    if (item < 0 || item >= ITEM_COUNT || ITEMS[item].slot == SLOT_NONE) return "";
    const Item &gear = ITEMS[item];
    std::string line;
    if (gear.slot == SLOT_WEAPON)
    {
        int base = item == 0 ? 2 : item == 2 ? 1 : std::max(0, (gear.amount - 4) / 2);
        int damage = base + gear.benefit - gear.drawback;
        line = "damage " + std::string(damage >= 0 ? "+" : "") + std::to_string(damage) +
                ", strike " + (gear.benefit - gear.drawback >= 0 ? "+" : "") +
                std::to_string(gear.benefit - gear.drawback);
    }
    else line = "AC +" + std::to_string(gear.amount) + "  /  ";
    if (gear.slot == SLOT_GLOVES)
        line += "strike " + std::to_string(gear.benefit - gear.drawback);
    else if (gear.slot == SLOT_BOOTS)
        line += "march pace +" + std::to_string(gear.benefit) +
                ", burden +" + std::to_string(gear.drawback);
    else if (gear.slot != SLOT_WEAPON)
        line += "weather cover +" + std::to_string(gear.benefit) +
                ", march burden +" + std::to_string(gear.drawback);
    return line;
}

bool isEquipment(int item)
{
    return item >= 0 && item < ITEM_COUNT && ITEMS[item].slot != SLOT_NONE;
}

void purchaseEquipment(int item)
{
    int price = marketPrice(item);
    if (!isEquipment(item) || itemQuantity(item) > 0 || game.gold < price) return;
    int quantityBefore = itemQuantity(item);
    addItem(item, 1);
    if (itemQuantity(item) != quantityBefore)
    {
        game.gold -= price;
        const Settlement *place = settlementAt(game.x, game.y);
        if (place)
            game.settlementProsperity[static_cast<int>(place - SETTLEMENTS)] =
                clampValue(game.settlementProsperity[static_cast<int>(place - SETTLEMENTS)] + 1, 0, 100);
        game.activityText = std::string("You buy ") + ITEMS[item].name + " for " +
                            std::to_string(price) + " gold.";
        addJournal(game.activityText);
    }
    else
        game.activityText = "Your pack is too full to carry that equipment.";
}

int equipmentItemCount()
{
    int count = 0;
    for (int i = 0; i < ITEM_COUNT; ++i) if (isEquipment(i)) ++count;
    return count;
}

void completeQuest(Quest &quest)
{
    if (quest.complete) return;
    quest.complete = true;
    game.gold += quest.rewardGold;
    int oldQuantity = itemQuantity(quest.rewardItem);
    questRewardInProgress = true;
    addItem(quest.rewardItem, quest.rewardQuantity);
    questRewardInProgress = false;
    const bool receivedItem = itemQuantity(quest.rewardItem) >= oldQuantity + quest.rewardQuantity;
    addJournal("Quest complete: " + quest.title + ". You receive " +
               std::to_string(quest.rewardGold) + " coins" +
               (receivedItem ? " and " + std::to_string(quest.rewardQuantity) + " " +
                               ITEMS[quest.rewardItem].name + "." : ". Your pack cannot carry the item reward."));
}

void updateQuestProgress(QuestType type, int target, int amount)
{
    if (amount <= 0) return;
    for (int i = 0; i < game.questCount; ++i)
    {
        Quest &quest = game.quests[i];
        if (quest.complete || quest.type != type || quest.target != target) continue;
        if (type == QUEST_HUNT &&
            (game.x != quest.destinationX || game.y != quest.destinationY))
            continue;
        if (type == QUEST_NEWS &&
            (game.x != quest.destinationX || game.y != quest.destinationY))
            continue;
        quest.progress = std::min(quest.goal, quest.progress + amount);
        if (quest.progress >= quest.goal && type != QUEST_GATHER) completeQuest(quest);
        else addJournal("Quest progress: " + quest.title + " (" +
                        std::to_string(quest.progress) + "/" + std::to_string(quest.goal) + ").");
    }
}

Quest makeQuestOffer()
{
    Quest quest = {};
    const QuestTemplate &task = QUEST_TEMPLATES[randomBetween(0, QUEST_TEMPLATE_COUNT - 1)];
    quest.type = static_cast<QuestType>(task.type);
    quest.target = task.target;
    assignQuestDestination(quest);
    quest.goal = task.goal;
    quest.difficulty = task.difficulty;
    quest.progress = 0;
    quest.complete = false;
    quest.title = task.title;
    quest.description = task.description;
    quest.rewardGold = 4 + quest.difficulty * 4;
    quest.rewardItem = quest.difficulty == 1 ? 5 : quest.difficulty == 2 ? 7 : 8;
    quest.rewardQuantity = quest.difficulty == 3 ? 2 : 1;
    return quest;
}

void offerQuest()
{
    if (!hasQuestRoom()) return;
    game.questOffer.active = true;
    game.questOffer.quest = makeQuestOffer();
    int speaker = game.activeNpc >= 0 ? game.activeNpc : npcForFaction(factionAt(game.x, game.y));
    game.activeNpc = speaker;
    game.activityText = npcDialogue(speaker, "I have a task for you: " +
                                    game.questOffer.quest.description);
    spendHour();
}

bool hasQuestRoom()
{
    if (game.questCount < QUEST_CAPACITY) return true;
    for (int i = 0; i < game.questCount; ++i)
        if (game.quests[i].complete) return true;
    return false;
}

void acceptQuestOffer()
{
    if (!game.questOffer.active) return;
    if (game.questCount >= QUEST_CAPACITY)
    {
        int completedIndex = -1;
        for (int i = 0; i < game.questCount; ++i)
            if (game.quests[i].complete) { completedIndex = i; break; }
        if (completedIndex < 0) return;
        for (int i = completedIndex + 1; i < game.questCount; ++i)
            game.quests[i - 1] = game.quests[i];
        --game.questCount;
    }
    game.quests[game.questCount++] = game.questOffer.quest;
    game.seen[game.questOffer.quest.destinationY][game.questOffer.quest.destinationX] = true;
    const std::string title = game.questOffer.quest.title;
    game.questOffer.active = false;
    addJournal("Quest accepted: " + title + ".");
    game.activityText = npcDialogue(game.activeNpc, "The task is written in your journal. " +
                                    game.questOffer.quest.description);
    spendHour();
}

void declineQuestOffer()
{
    if (!game.questOffer.active) return;
    game.questOffer.active = false;
    game.activityText = game.activeNpc >= 0 ?
        npcDialogue(game.activeNpc, "No harm done. The road may bring another chance.") :
        "You decline the task for now. The local returns to their work.";
    spendHour();
}

std::string questObjective(const Quest &quest)
{
    if (quest.type == QUEST_EXPLORE)
        return std::string("Travel in ") + terrainName(quest.target);
    if (quest.type == QUEST_HUNT)
        return std::string("Defeat ") + CREATURES[quest.target].name;
    if (quest.type == QUEST_GATHER)
        return std::string("Gather ") + std::to_string(quest.goal) + " " +
               ITEMS[quest.target].name + " and deliver at the marked settlement";
    return "Ask locals about the road";
}

void assignQuestDestination(Quest &quest)
{
    quest.destinationX = game.x;
    quest.destinationY = game.y;
    if (quest.type == QUEST_GATHER || quest.type == QUEST_NEWS)
    {
        int bestDistance = MAP_SIZE * 2;
        for (int i = 0; i < SETTLEMENT_COUNT; ++i)
        {
            const Settlement &place = SETTLEMENTS[i];
            int distance = std::abs(place.x - game.x) + std::abs(place.y - game.y);
            if (distance == 0 && SETTLEMENT_COUNT > 1) continue;
            if (distance < bestDistance)
            {
                bestDistance = distance;
                quest.destinationX = place.x;
                quest.destinationY = place.y;
            }
        }
        return;
    }

    int bestDistance = MAP_SIZE * MAP_SIZE;
    int selectedX = game.x;
    int selectedY = game.y;
    for (int y = 0; y < MAP_SIZE; ++y)
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            bool suitable = false;
            if (quest.type == QUEST_EXPLORE)
                suitable = game.map[y][x] == quest.target;
            else if (quest.type == QUEST_HUNT && quest.target >= 0 && quest.target < CREATURE_COUNT)
                suitable = std::string(CREATURES[quest.target].terrain) ==
                           terrainName(game.map[y][x]) && !settlementAt(x, y);
            if (!suitable) continue;
            int distance = std::abs(x - game.x) + std::abs(y - game.y);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                selectedX = x;
                selectedY = y;
            }
        }
    quest.destinationX = selectedX;
    quest.destinationY = selectedY;
}

bool hasActiveHuntAt(int x, int y, int creature = -1)
{
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        if (!quest.complete && quest.type == QUEST_HUNT &&
            quest.destinationX == x && quest.destinationY == y &&
            (creature < 0 || quest.target == creature) && quest.progress < quest.goal)
            return true;
    }
    return false;
}

void ensureQuestHuntPresence()
{
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        if (quest.complete || quest.type != QUEST_HUNT || quest.progress >= quest.goal ||
            quest.destinationX != game.x || quest.destinationY != game.y)
            continue;
        if (game.roamingCreature != quest.target)
        {
            game.roamingCreature = quest.target;
            game.roamingCreatureTurns = 3;
            addJournal(std::string("At the marked ground, signs of the ") +
                       CREATURES[quest.target].name + " are fresh. " +
                       CREATURES[quest.target].warning);
        }
        return;
    }
}

bool hasDeliverableQuestAt(int x, int y)
{
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        if (!quest.complete && quest.type == QUEST_GATHER && quest.progress >= quest.goal &&
            quest.destinationX == x && quest.destinationY == y &&
            itemQuantity(quest.target) >= quest.goal)
            return true;
    }
    return false;
}

bool hasQuestMarkerAt(int x, int y)
{
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        if (!quest.complete && quest.destinationX == x && quest.destinationY == y)
            return true;
    }
    return false;
}

void drawQuestMapMarker(Rectangle tile, int x, int y)
{
    if (!hasQuestMarkerAt(x, y)) return;
    DrawCircle(static_cast<int>(tile.x + tile.width - 5),
               static_cast<int>(tile.y + 5), 5.0f, PAPER);
    DrawCircle(static_cast<int>(tile.x + tile.width - 5),
               static_cast<int>(tile.y + 5), 3.5f, {183, 38, 34, 255});
}

void deliverQuestItems()
{
    bool delivered = false;
    for (int i = 0; i < game.questCount; ++i)
    {
        Quest &quest = game.quests[i];
        if (quest.complete || quest.type != QUEST_GATHER || quest.progress < quest.goal ||
            quest.destinationX != game.x || quest.destinationY != game.y ||
            itemQuantity(quest.target) < quest.goal)
            continue;
        for (int item = 0; item < quest.goal; ++item) consumeItem(quest.target);
        addJournal("You deliver " + std::to_string(quest.goal) + " " +
                   ITEMS[quest.target].name + " for " + quest.title + ".");
        completeQuest(quest);
        delivered = true;
    }
    if (delivered) spendHour();
}

void consumeItem(int item)
{
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
    {
        if (game.inventory[i].item == item && game.inventory[i].quantity > 0)
        {
            --game.inventory[i].quantity;
            if (game.inventory[i].quantity == 0) game.inventoryAge[item] = 0;
            return;
        }
    }
}

static_assert(sizeof(CREATURE_LOOT) / sizeof(CREATURE_LOOT[0]) == CREATURE_COUNT,
              "Every creature must have a loot entry.");
static_assert(sizeof(MONSTER_ENCOUNTER_SIGNS) / sizeof(MONSTER_ENCOUNTER_SIGNS[0]) ==
                  MONSTER_ENCOUNTER_VARIANTS_PER_LAYER &&
              sizeof(MONSTER_ENCOUNTER_TURNS) / sizeof(MONSTER_ENCOUNTER_TURNS[0]) ==
                  MONSTER_ENCOUNTER_VARIANTS_PER_LAYER,
              "Monster encounter variations require matching sign and behavior tables.");
static_assert(ITEM_COUNT == MONSTER_LOOT_NEW_FIRST_ITEM + MONSTER_LOOT_NEW_COUNT,
              "New monster trophies must remain isolated from armory items.");

const Settlement *settlementAt(int x, int y)
{
    for (int i = 0; i < SETTLEMENT_COUNT; ++i)
    {
        if (SETTLEMENTS[i].x == x && SETTLEMENTS[i].y == y) return &SETTLEMENTS[i];
    }
    return NULL;
}

int marketPrice(int item)
{
    if (item < 0 || item >= ITEM_COUNT) return 0;
    const Settlement *place = settlementAt(game.x, game.y);
    if (!place || std::string(place->kind) == "CAMP") return ITEMS[item].price;
    int settlementIndex = static_cast<int>(place - SETTLEMENTS);
    int owner = place->faction;
    int prosperity = clampValue(game.settlementProsperity[settlementIndex], 0, 100);
    int unrest = owner >= 0 && owner < FACTION_COUNT ? game.factionUnrest[owner] : 0;
    int reputation = owner >= 0 && owner < FACTION_COUNT ? game.factionRep[owner] : 0;
    int marketPercent = clampValue(100 + (50 - prosperity) / 3 + unrest / 8 - reputation * 3, 70, 160);
    return std::max(1, (ITEMS[item].price * marketPercent + 50) / 100);
}

void initializeWorldSimulation()
{
    for (int faction = 0; faction < FACTION_COUNT; ++faction)
    {
        game.factionPower[faction] = randomBetween(35, 65);
        game.factionUnrest[faction] = randomBetween(12, 38);
    }
    for (int kingdom = 0; kingdom < KINGDOM_COUNT; ++kingdom)
        game.kingdomStability[kingdom] = randomBetween(55, 82);
    for (int settlement = 0; settlement < SETTLEMENT_COUNT; ++settlement)
        game.settlementProsperity[settlement] = randomBetween(38, 72);
    game.lastWorldSimDay = game.day;
    game.worldEventHeadline = "The Marches are quiet for now; markets and borders shift with the passing days.";
    game.worldEventDay = game.day;
}

void rememberNpcAtCurrentLocation(int npc)
{
    if (npc < 0 || npc >= NPC_COUNT || game.npcDead[npc]) return;
    game.npcKnown[npc] = true;
    game.npcKnownX[npc] = game.x;
    game.npcKnownY[npc] = game.y;
}

void simulateWorldDay()
{
    if (game.lastWorldSimDay == game.day) return;
    game.lastWorldSimDay = game.day;
    for (int i = 0; i < SETTLEMENT_COUNT; ++i)
    {
        int faction = SETTLEMENTS[i].faction;
        bool nearbyFarmland = false;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
            {
                int x = SETTLEMENTS[i].x + dx, y = SETTLEMENTS[i].y + dy;
                if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE && game.map[y][x] == 5)
                    nearbyFarmland = true;
            }
        int harvest = game.season == 2 && nearbyFarmland ? 2 : 0;
        int supply = randomBetween(-3, 3) + harvest - game.factionUnrest[faction] / 25;
        game.settlementProsperity[i] =
            clampValue(game.settlementProsperity[i] + supply, 5, 95);
    }
    for (int faction = 0; faction < FACTION_COUNT; ++faction)
    {
        int powerDrift = randomBetween(-1, 1) + (game.factionRep[faction] > 2 ? 1 : 0);
        int unrestDrift = randomBetween(-2, 2) + (game.factionPower[faction] < 30 ? 1 : 0);
        game.factionPower[faction] = clampValue(game.factionPower[faction] + powerDrift, 10, 90);
        game.factionUnrest[faction] = clampValue(game.factionUnrest[faction] + unrestDrift, 0, 100);
        int kingdom = FACTIONS[faction].kingdom;
        game.kingdomStability[kingdom] = clampValue(
            game.kingdomStability[kingdom] + (game.factionUnrest[faction] < 25 ? 1 : -1), 0, 100);
    }
    game.worldEventDay = game.day;
    int unsettledFaction = 0;
    for (int faction = 1; faction < FACTION_COUNT; ++faction)
        if (game.factionUnrest[faction] > game.factionUnrest[unsettledFaction])
            unsettledFaction = faction;
    int prosperousTown = randomBetween(0, SETTLEMENT_COUNT - 1);
    for (int tries = 0; tries < SETTLEMENT_COUNT &&
         std::string(SETTLEMENTS[prosperousTown].kind) == "CAMP"; ++tries)
        prosperousTown = (prosperousTown + 1) % SETTLEMENT_COUNT;
    if (game.factionUnrest[unsettledFaction] >= 60)
        game.worldEventHeadline = std::string("UNREST / ") + FACTIONS[unsettledFaction].name +
            " faces rising prices and anxious crowds across " +
            KINGDOMS[FACTIONS[unsettledFaction].kingdom].name + ".";
    else if (game.settlementProsperity[prosperousTown] >= 68 &&
             game.kingdomStability[FACTIONS[SETTLEMENTS[prosperousTown].faction].kingdom] >= 60)
        game.worldEventHeadline = std::string("CELEBRATION / ") +
            SETTLEMENTS[prosperousTown].name +
            " holds a market feast; trade is lively and the streets stay open late.";
    else if (game.settlementProsperity[prosperousTown] <= 30)
        game.worldEventHeadline = std::string("INFLATION / Supplies grow scarce in ") +
            SETTLEMENTS[prosperousTown].name + "; traders raise their prices.";
    else
        game.worldEventHeadline = std::string("ROAD NEWS / ") + SETTLEMENTS[prosperousTown].name +
            " reports a steady market. The Marches keep their breath.";
    if (game.day % 7 != 0) return;
    int regionalTension = 0;
    for (int faction = 0; faction < FACTION_COUNT; ++faction)
        regionalTension = std::max(regionalTension, game.factionUnrest[faction]);
    if (randomBetween(0, 99) >= 18 + regionalTension / 2) return;

    int first = randomBetween(0, FACTION_COUNT - 1);
    int second = (first + randomBetween(1, FACTION_COUNT - 1)) % FACTION_COUNT;
    int firstScore = game.factionPower[first] + game.kingdomStability[FACTIONS[first].kingdom] / 3;
    int secondScore = game.factionPower[second] + game.kingdomStability[FACTIONS[second].kingdom] / 3;
    int victor = firstScore == secondScore ? randomBetween(0, 1) == 0 ? first : second :
                 firstScore > secondScore ? first : second;
    int loser = victor == first ? second : first;
    game.factionPower[victor] = clampValue(game.factionPower[victor] + 3, 10, 90);
    game.factionPower[loser] = clampValue(game.factionPower[loser] - 3, 10, 90);
    game.factionUnrest[victor] = clampValue(game.factionUnrest[victor] - 2, 0, 100);
    game.factionUnrest[loser] = clampValue(game.factionUnrest[loser] + 4, 0, 100);
    game.kingdomStability[FACTIONS[victor].kingdom] =
        clampValue(game.kingdomStability[FACTIONS[victor].kingdom] + 2, 0, 100);
    game.kingdomStability[FACTIONS[loser].kingdom] =
        clampValue(game.kingdomStability[FACTIONS[loser].kingdom] - 3, 0, 100);
    for (int town = 0; town < SETTLEMENT_COUNT; ++town)
    {
        if (SETTLEMENTS[town].faction == victor)
            game.settlementProsperity[town] =
                clampValue(game.settlementProsperity[town] + 2, 5, 95);
        else if (SETTLEMENTS[town].faction == loser)
            game.settlementProsperity[town] =
                clampValue(game.settlementProsperity[town] - 3, 5, 95);
    }
    addJournal(std::string("News travels: ") + FACTIONS[victor].name + " gains ground over " +
               FACTIONS[loser].name + ". Markets shift; old borders grow tense.");
    game.worldEventHeadline = std::string("WAR / ") + FACTIONS[victor].name + " gains ground over " +
        FACTIONS[loser].name + "; border markets and roads are unsettled.";
}

std::string randomDialogue()
{
    const int opener = randomBetween(0, static_cast<int>(sizeof(DIALOGUE_OPENERS) / sizeof(DIALOGUE_OPENERS[0])) - 1);
    const int observation = randomBetween(0, static_cast<int>(sizeof(DIALOGUE_OBSERVATIONS) / sizeof(DIALOGUE_OBSERVATIONS[0])) - 1);
    const int tail = randomBetween(0, static_cast<int>(sizeof(DIALOGUE_TAILS) / sizeof(DIALOGUE_TAILS[0])) - 1);
    return std::string(DIALOGUE_OPENERS[opener]) + " " + DIALOGUE_OBSERVATIONS[observation] +
           " " + DIALOGUE_TAILS[tail];
}

std::string npcDialogue(int npcIndex, const std::string &context)
{
    if (npcIndex < 0 || npcIndex >= NPC_COUNT) npcIndex = randomBetween(0, NPC_COUNT - 1);
    const NPC &npc = NPCS[npcIndex];
    std::string memory;
    if (game.npcDead[npcIndex]) return std::string(npc.name) + " is gone; the road keeps their silence.";
    if (game.npcTrust[npcIndex] >= 3) memory = "They speak with the ease of an old friend. ";
    else if (game.npcTrust[npcIndex] > 0) memory = "They remember your kindness. ";
    else if (game.npcTrust[npcIndex] < 0) memory = "Their reply is guarded after your earlier slight. ";
    if (game.factionRep[npc.faction] >= 3) memory += "Your name is spoken well in their faction. ";
    else if (game.factionRep[npc.faction] <= -3) memory += "They have heard the complaints against you. ";
    return memory + std::string(npc.name) + ", the " + npc.role + ", " + npc.personality +
           " \"" + randomDialogue() + "\" " + context;
}

int npcForFaction(int faction)
{
    std::vector<int> local;
    std::vector<int> newcomers;
    std::vector<int> living;
    for (int i = 0; i < NPC_COUNT; ++i)
    {
        if (game.npcDead[i] || (faction >= 0 && NPCS[i].faction != faction)) continue;
        living.push_back(i);
        if (game.npcKnown[i] && game.npcKnownX[i] == game.x && game.npcKnownY[i] == game.y)
            local.push_back(i);
        else if (!game.npcKnown[i])
            newcomers.push_back(i);
    }
    if (!local.empty()) return local[randomBetween(0, static_cast<int>(local.size()) - 1)];
    if (!newcomers.empty()) return newcomers[randomBetween(0, static_cast<int>(newcomers.size()) - 1)];
    if (faction >= 0)
    {
        for (int i = 0; i < NPC_COUNT; ++i)
            if (!game.npcDead[i] && !game.npcKnown[i]) newcomers.push_back(i);
        if (!newcomers.empty())
            return newcomers[randomBetween(0, static_cast<int>(newcomers.size()) - 1)];
    }
    return living.empty() ? 0 : living[randomBetween(0, static_cast<int>(living.size()) - 1)];
}

void clearSocialScene()
{
    game.socialSceneKind = SOCIAL_NONE;
    game.socialSceneNpcA = -1;
    game.socialSceneNpcB = -1;
    game.socialSceneCreature = -1;
    game.socialSceneDescription.clear();
}

bool createSocialScene()
{
    if (game.socialSceneKind != SOCIAL_NONE || game.enemyIndex >= 0 ||
        game.activity != FREE_ROAM || game.screen != GAME)
        return false;

    const Settlement *place = settlementAt(game.x, game.y);
    if (randomBetween(0, 99) >= (place ? 32 : 22)) return false;

    std::vector<int> livingNpcs;
    for (int i = 0; i < NPC_COUNT; ++i)
        if (!game.npcDead[i]) livingNpcs.push_back(i);
    if (livingNpcs.empty()) return false;

    int sceneRoll = randomBetween(0, 99);
    if (sceneRoll < 42 && livingNpcs.size() > 1)
    {
        int a = livingNpcs[randomBetween(0, static_cast<int>(livingNpcs.size()) - 1)];
        int b = a;
        for (int attempt = 0; attempt < 8 && b == a; ++attempt)
            b = livingNpcs[randomBetween(0, static_cast<int>(livingNpcs.size()) - 1)];
        if (a == b) return false;
        game.socialSceneKind = SOCIAL_NPC_EXCHANGE;
        game.socialSceneNpcA = a;
        game.socialSceneNpcB = b;
        game.socialSceneDescription = std::string(NPCS[a].name) + ", " + NPCS[a].personality +
            ", is in a tense exchange with " + NPCS[b].name + ", " + NPCS[b].personality +
            ". A local favor is at stake; both watch to see who will speak.";
    }
    else if (sceneRoll < 72)
    {
        int npc = livingNpcs[randomBetween(0, static_cast<int>(livingNpcs.size()) - 1)];
        game.socialSceneKind = SOCIAL_BANDIT_THREAT;
        game.socialSceneNpcA = npc;
        game.socialSceneDescription = std::string(NPCS[npc].name) + " is being pressed by a road outlaw. " +
            std::string(CREATURES[0].warning) + " You have room to intervene, but steel may follow.";
    }
    else
    {
        std::vector<int> localCreatures;
        const std::string biome = terrainName(game.map[game.y][game.x]);
        for (int i = 0; i < CREATURE_COUNT; ++i)
            if (biome == CREATURES[i].terrain) localCreatures.push_back(i);
        if (localCreatures.empty()) return false;
        int npc = livingNpcs[randomBetween(0, static_cast<int>(livingNpcs.size()) - 1)];
        int creature = localCreatures[randomBetween(0, static_cast<int>(localCreatures.size()) - 1)];
        game.socialSceneKind = SOCIAL_CREATURE_THREAT;
        game.socialSceneNpcA = npc;
        game.socialSceneCreature = creature;
        game.socialSceneDescription = std::string(NPCS[npc].name) + " is holding still as a " +
            CREATURES[creature].name + " prowls nearby. " + CREATURES[creature].warning +
            " A careless move could turn its attention toward you.";
    }

    addJournal("Nearby, the road carries on without waiting for you: " + game.socialSceneDescription);
    return true;
}

int factionAt(int x, int y)
{
    const Settlement *place = settlementAt(x, y);
    if (place) return place->faction;
    int closest = 1000, owner = -1;
    for (int i = 0; i < SETTLEMENT_COUNT; ++i)
    {
        int distance = std::abs(SETTLEMENTS[i].x - x) + std::abs(SETTLEMENTS[i].y - y);
        if (distance < closest)
        {
            closest = distance;
            owner = SETTLEMENTS[i].faction;
        }
    }
    return owner;
}

std::string politicsAt(int x, int y)
{
    const Settlement *place = settlementAt(x, y);
    int owner = factionAt(x, y);
    if (owner < 0 || owner >= FACTION_COUNT) return "";
    const Faction &faction = FACTIONS[owner];
    int kingdom = faction.kingdom;
    std::string status = game.factionUnrest[owner] >= 60 ? "unrest is rising" :
                         game.factionPower[owner] >= 65 ? "its influence is strong" :
                         game.factionPower[owner] <= 35 ? "its hold is weakening" :
                         "its roads are holding";
    return std::string(place ? "Held by " : "Near ") + KINGDOMS[kingdom].name +
           " / " + faction.name + " / " + status +
           " / order " + std::to_string(game.kingdomStability[kingdom]) + "%";
}

std::string monsterEncounterDescription(int creature, int variant)
{
    variant = (variant % MONSTER_ENCOUNTER_COUNT + MONSTER_ENCOUNTER_COUNT) %
              MONSTER_ENCOUNTER_COUNT;
    const char *sign = MONSTER_ENCOUNTER_SIGNS[variant / MONSTER_ENCOUNTER_VARIANTS_PER_LAYER];
    const char *turn = MONSTER_ENCOUNTER_TURNS[variant % MONSTER_ENCOUNTER_VARIANTS_PER_LAYER];
    return std::string(sign) + " " + CREATURES[creature].name + ". " +
           CREATURES[creature].warning + " " + turn;
}

const char *terrainName(int terrain)
{
    switch (terrain)
    {
        case 1: return "forest";
        case 2: return "mountains";
        case 3: return "marsh";
        case 4: return "old road";
        case 5: return "farmland";
        case 6: return "coast";
        case 7: return "ruins";
        default: return "open country";
    }
}

void updateLocalPresence()
{
    if (game.roamingCreature >= 0 &&
        !hasActiveHuntAt(game.x, game.y, game.roamingCreature) &&
        --game.roamingCreatureTurns <= 0)
    {
        addJournal(std::string(CREATURES[game.roamingCreature].name) +
                   " slips back into its own country before you can follow.");
        game.roamingCreature = -1;
        game.roamingCreatureTurns = 0;
    }
    if (game.wanderingNpc >= 0 && game.npcKnown[game.wanderingNpc] &&
        (game.npcKnownX[game.wanderingNpc] != game.x ||
         game.npcKnownY[game.wanderingNpc] != game.y))
    {
        game.wanderingNpc = -1;
        game.wanderingNpcTurns = 0;
        game.wanderingNpcActivity = -1;
    }
    if (game.wanderingNpc >= 0 && game.npcKnown[game.wanderingNpc])
        game.wanderingNpcTurns = std::max(1, game.wanderingNpcTurns);
    else if (game.wanderingNpc >= 0 && --game.wanderingNpcTurns <= 0)
    {
        addJournal(std::string(NPCS[game.wanderingNpc].name) + " shoulders their things and moves on.");
        game.wanderingNpc = -1;
        game.wanderingNpcTurns = 0;
        game.wanderingNpcActivity = -1;
    }

    const Settlement *place = settlementAt(game.x, game.y);
    int localFaction = factionAt(game.x, game.y);
    int localUnrest = localFaction >= 0 ? game.factionUnrest[localFaction] : 0;
    if (game.wanderingNpc < 0)
    {
        for (int npc = 0; npc < NPC_COUNT; ++npc)
        {
            if (game.npcKnown[npc] && !game.npcDead[npc] &&
                game.npcKnownX[npc] == game.x && game.npcKnownY[npc] == game.y)
            {
                game.wanderingNpc = npc;
                game.wanderingNpcTurns = 1;
                game.wanderingNpcActivity = randomBetween(0, 9);
                break;
            }
        }
    }
    if (!place && game.enemyIndex < 0 && game.activity == FREE_ROAM && game.roamingCreature < 0 &&
        randomBetween(0, 99) < clampValue(16 + localUnrest / 8, 12, 28))
    {
        std::vector<int> localCreatures;
        const std::string biome = terrainName(game.map[game.y][game.x]);
        for (int i = 0; i < CREATURE_COUNT; ++i)
            if (biome == CREATURES[i].terrain) localCreatures.push_back(i);
        if (!localCreatures.empty())
        {
            game.roamingCreature = localCreatures[randomBetween(0, static_cast<int>(localCreatures.size()) - 1)];
            game.roamingCreatureTurns = randomBetween(1, 3);
            addJournal(monsterEncounterDescription(game.roamingCreature,
                randomBetween(0, MONSTER_ENCOUNTER_COUNT - 1)));
        }
    }

    if (game.wanderingNpc < 0 && game.enemyIndex < 0 && game.activity == FREE_ROAM &&
        randomBetween(0, 99) < clampValue(20 + localUnrest / 8, 16, 34))
    {
        std::vector<int> travelers;
        for (int npc = 0; npc < NPC_COUNT; ++npc)
            if (!game.npcDead[npc] && !game.npcKnown[npc] &&
                (localFaction < 0 || NPCS[npc].faction == localFaction))
                travelers.push_back(npc);
        if (travelers.empty())
            for (int npc = 0; npc < NPC_COUNT; ++npc)
                if (!game.npcDead[npc] && !game.npcKnown[npc]) travelers.push_back(npc);
        if (travelers.empty()) return;
        game.wanderingNpc = travelers[randomBetween(0, static_cast<int>(travelers.size()) - 1)];
        game.wanderingNpcTurns = randomBetween(1, 3);
        game.wanderingNpcActivity = randomBetween(0, 9);
        const NPC &npc = NPCS[game.wanderingNpc];
        addJournal(std::string(npc.name) + ", a traveling " + npc.role + ", is " +
                   WANDERING_NPC_ACTIVITIES[game.wanderingNpcActivity] + ".");
    }
}

void drawWrapped(const std::string &text, int x, int y, int maxWidth, int fontSize, Color color, int lineGap = 4)
{
    std::string line;
    std::string word;
    std::vector<std::string> lines;
    for (std::size_t i = 0; i <= text.size(); ++i)
    {
        char c = i < text.size() ? text[i] : ' ';
        if (c == ' ' || c == '\n')
        {
            if (!word.empty())
            {
                std::string proposed = line.empty() ? word : line + " " + word;
                if (!line.empty() && MeasureText(proposed.c_str(), fontSize) > maxWidth)
                {
                    lines.push_back(line);
                    line = word;
                }
                else line = proposed;
                word.clear();
            }
            if (c == '\n')
            {
                lines.push_back(line);
                line.clear();
            }
        }
        else word += c;
    }
    if (!line.empty()) lines.push_back(line);
    for (std::size_t i = 0; i < lines.size(); ++i)
        DrawText(lines[i].c_str(), x, y + static_cast<int>(i) * (fontSize + lineGap), fontSize, color);
}

void drawPanel(Rectangle rect, Color fill = PAPER_RAISED)
{
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2.0f, BORDER);
    Rectangle inner = {rect.x + 4.0f, rect.y + 4.0f,
                       std::max(0.0f, rect.width - 8.0f), std::max(0.0f, rect.height - 8.0f)};
    DrawRectangleLinesEx(inner, 1.0f, PAPER_INSET);
}

bool drawButton(Rectangle rect, const char *label, bool primary = false, bool enabled = true)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    Color fill = primary ? EMBER : PAPER_RAISED;
    Color border = primary ? EMBER : BORDER;
    Color text = primary ? PAPER : INK;
    if (!enabled)
    {
        fill = PAPER_INSET;
        text = MUTED;
    }
    else if (hovered)
    {
        fill = primary ? EMBER_BRIGHT : PAPER_INSET;
    }
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 1.0f, border);
    int fontSize = 18;
    while (fontSize > 12 && MeasureText(label, fontSize) > rect.width - 8.0f) --fontSize;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label, static_cast<int>(rect.x + (rect.width - textWidth) / 2),
             static_cast<int>(rect.y + (rect.height - fontSize - 1) / 2), fontSize, text);
    bool clicked = enabled && hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (clicked)
    {
        selectionPulseRect = rect;
        selectionPulseTimer = 0.18f;
    }
    return clicked;
}

bool drawChoice(Rectangle rect, const char *label, const char *description, bool enabled = true)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    drawPanel(rect, !enabled ? PAPER_INSET : hovered ? PAPER_INSET : PAPER_RAISED);
    DrawText(label, static_cast<int>(rect.x + 14), static_cast<int>(rect.y + 5), 18, enabled ? INK : MUTED);
    DrawText(description, static_cast<int>(rect.x + 14), static_cast<int>(rect.y + 29), 15, MUTED);
    bool clicked = enabled && hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (clicked)
    {
        selectionPulseRect = rect;
        selectionPulseTimer = 0.18f;
    }
    return clicked;
}

void drawBar(const char *label, int value, int maximum, int x, int y, int width, Color color)
{
    DrawText(label, x, y, 15, MUTED);
    Rectangle track = {static_cast<float>(x + 76), static_cast<float>(y + 2), static_cast<float>(width), 13.0f};
    DrawRectangleRec(track, PAPER_INSET);
    Rectangle fill = {track.x, track.y, track.width * clampValue(value, 0, maximum) / maximum, track.height};
    DrawRectangleRec(fill, color);
    DrawRectangleLinesEx(track, 1.0f, BORDER);
    std::string labelValue = std::to_string(value) + "/" + std::to_string(maximum);
    DrawText(labelValue.c_str(), static_cast<int>(track.x + track.width + 8), y, 14, MUTED);
}

void drawFrame(const char *heading, int width, int height, int &left, int &top)
{
    left = (GetScreenWidth() - width) / 2;
    top = (GetScreenHeight() - height) / 2;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), NIGHT);
    Rectangle paper = {static_cast<float>(left), static_cast<float>(top), static_cast<float>(width), static_cast<float>(height)};
    Rectangle cover = {static_cast<float>(left - 8), static_cast<float>(top - 8),
                       static_cast<float>(width + 16), static_cast<float>(height + 16)};
    DrawRectangleRec(cover, EMBER);
    DrawRectangleLinesEx(cover, 2.0f, BORDER);
    drawPanel(paper, PAPER);
    DrawRectangle(left + 18, top + 18, width - 36, 2, BORDER);
    if (heading && heading[0] != '\0')
    {
        DrawText("WAYWORN", left + 38, top + 30, 15, EMBER);
        DrawText(heading, left + 38, top + 52, 27, INK);
    }
}

void buildMap()
{
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            int roll = randomBetween(0, 99);
            int regionalBias = x < 4 ? 6 : x > 8 ? 3 : 0;
            if (y < 3) game.map[y][x] = roll < 48 ? 2 : roll < 68 ? 1 : roll < 78 ? 7 : 0;
            else if (x > 9) game.map[y][x] = roll < 55 ? 6 : roll < 68 ? 3 : 0;
            else if (y > 8) game.map[y][x] = roll < 40 ? 5 : roll < 56 ? 3 : roll < 68 ? 1 : 0;
            else game.map[y][x] = roll < 22 + regionalBias ? 1 : roll < 38 + regionalBias ? 2 :
                                      roll < 52 ? 3 : roll < 64 ? 5 : roll < 73 ? 7 : 0;
            game.landmarks[y][x] = 0;
            game.discoveredPOI[y][x] = false;
            game.seen[y][x] = false;
        }
    }
    for (int i = 0; i < 14; ++i)
    {
        int x = randomBetween(0, MAP_SIZE - 1);
        int y = randomBetween(0, MAP_SIZE - 1);
        if (!settlementAt(x, y)) game.landmarks[y][x] = randomBetween(1, 3);
    }
    for (int i = 0; i < SETTLEMENT_COUNT; ++i)
    {
        game.map[SETTLEMENTS[i].y][SETTLEMENTS[i].x] = 4;
        int x = SETTLEMENTS[i].x;
        int y = SETTLEMENTS[i].y;
        for (int step = 0; step < 5; ++step)
        {
            int tx = 5 + (x < 5 ? -1 : 1) * step;
            int ty = 6 + (y < 6 ? -1 : 1) * step;
            if (tx >= 0 && tx < MAP_SIZE) game.map[6][tx] = 4;
            if (ty >= 0 && ty < MAP_SIZE) game.map[ty][x] = 4;
        }
    }
    game.x = 5;
    game.y = 6;
    game.seen[game.y][game.x] = true;
    game.worldInitialized = true;
}

void revealNearby()
{
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            int x = game.x + dx;
            int y = game.y + dy;
            if (x >= 0 && y >= 0 && x < MAP_SIZE && y < MAP_SIZE)
            {
                game.seen[y][x] = true;
                if (game.landmarks[y][x] > 0 && !game.discoveredPOI[y][x])
                {
                    game.discoveredPOI[y][x] = true;
                    addJournal("You discover a hidden point of interest. Its mark is added to the map.");
                    grantTrait(5);
                }
            }
        }
    }
}

void resetAttributes()
{
    for (int i = 0; i < 6; ++i) game.attributes[i] = 10;
    game.pointsRemaining = 5;
}

void beginGame()
{
    game.screen = GAME;
    game.page = ROAD;
    bool newWorld = !game.worldInitialized;
    if (newWorld) game.day = 1;
    if (!game.worldInitialized) buildMap();
    if (newWorld) game.timeOfDay = 0;
    game.x = 5;
    game.y = 6;
    if (newWorld)
    {
        game.season = game.worldSeed % 4;
        game.weather = randomBetween(0, 4);
        initializeWorldSimulation();
    }
    game.exposure = 0;
    game.bleeding = 0;
    game.infection = 0;
    game.turn = 0;
    int baseHealth = game.origin == 1 ? 18 : game.origin == 2 ? 17 : 20;
    game.maxHp = baseHealth + (game.attributes[2] - 10);
    game.hp = game.maxHp;
    game.hunger = game.fatigue = 100;
    game.gold = 0;
    game.equippedWeapon = game.origin == 0 ? 0 : game.origin == 1 ? 1 : 2;
    game.equippedArmor = game.origin == 0 ? 3 : -1;
    game.equippedHelmet = -1;
    game.equippedGloves = -1;
    game.equippedBoots = -1;
    game.activity = FREE_ROAM;
    game.activityText.clear();
    clearSocialScene();
    game.campBuilt = false;
    game.pendingEvent = -1;
    game.activeNpc = -1;
    game.activeFallen = -1;
    game.inventory.clear();
    game.journal.clear();
    game.questCount = 0;
    game.questOffer.active = false;
    for (int i = 0; i < CREATURE_COUNT; ++i) game.discovered[i] = 0;
    game.enemyIndex = -1;
    game.combatTurn = 0;
    game.guarding = false;
    game.weakened = false;
    game.dead = false;
    game.won = false;
    game.daysSurvived = 0;
    game.characterStartDay = game.day;
    game.creaturesBested = 0;
    for (int i = 0; i < SKILL_COUNT; ++i)
    {
        game.skills[i] = 0;
        game.skillPractice[i] = 0;
    }
    for (int i = 0; i < TRAIT_COUNT; ++i) game.traits[i] = false;
    game.killCount = 0;
    for (int i = 0; i < 10; ++i) game.notableKills[i].clear();
    recalculateEquipment();
    game.enemyMorale = 0;
    game.enemyTelegraph = 0;
    game.stunnedTurns = 0;
    game.proneTurns = 0;
    game.frightenedTurns = 0;
    game.eventHistoryCount = 0;
    game.eventStreak = 0;
    game.pageScroll = 0;
    game.roamingCreature = -1;
    game.roamingCreatureTurns = 0;
    game.wanderingNpc = -1;
    game.wanderingNpcTurns = 0;
    game.wanderingNpcActivity = -1;
    for (int i = 0; i < 4; ++i) game.lastEvents[i] = -1;
    for (int i = 0; i < ITEM_COUNT; ++i) game.inventoryAge[i] = 0;
    revealNearby();
    game.ending.clear();
    if (game.origin == 0)
    {
        addItem(0, 1);
        addItem(3, 1);
        addItem(5, 2);
        addItem(6, 1);
        addItem(8, 1);
    }
    else if (game.origin == 1)
    {
        addItem(1, 1);
        addItem(5, 1);
        addItem(8, 1);
    }
    else
    {
        addItem(2, 1);
        addItem(5, 2);
        addItem(6, 1);
        addItem(7, 2);
        addItem(8, 1);
    }
    for (int fallen = game.fallenCount - 1; fallen >= 0; --fallen)
    {
        if (game.fallenX[fallen] == game.x && game.fallenY[fallen] == game.y &&
            game.landmarks[game.y][game.x] == 4)
        {
            game.pendingEvent = -2;
            game.activeFallen = fallen;
            game.activity = EVENT_CHOICE;
            game.activityText = game.fallenName[fallen] + " rests here. " + game.fallenCause[fallen] +
                                " Their pack may still hold something useful.";
            break;
        }
    }
    addJournal("You shoulder your chosen kit and take the first road.");
    addJournal("Your story begins here. The Marches make no promises.");
    saveGame();
}

void endJourney(const std::string &message, bool victory)
{
    if (!victory && game.screen != EPILOGUE && game.worldInitialized)
    {
        if (game.fallenCount >= 8)
        {
            int oldestX = game.fallenX[0], oldestY = game.fallenY[0];
            if (oldestX >= 0 && oldestX < MAP_SIZE && oldestY >= 0 && oldestY < MAP_SIZE)
                game.landmarks[oldestY][oldestX] = 5;
            for (int i = 1; i < 8; ++i)
            {
                game.fallenName[i - 1] = game.fallenName[i];
                game.fallenCause[i - 1] = game.fallenCause[i];
                game.fallenX[i - 1] = game.fallenX[i];
                game.fallenY[i - 1] = game.fallenY[i];
                for (int item = 0; item < ITEM_COUNT; ++item)
                    game.fallenGear[i - 1][item] = game.fallenGear[i][item];
            }
            game.fallenCount = 7;
        }
        int fallen = game.fallenCount++;
        game.fallenName[fallen] = game.name;
        game.fallenCause[fallen] = message;
        game.fallenX[fallen] = game.x;
        game.fallenY[fallen] = game.y;
        for (int item = 0; item < ITEM_COUNT; ++item) game.fallenGear[fallen][item] = itemQuantity(item);
        game.landmarks[game.y][game.x] = 4;
        game.discoveredPOI[game.y][game.x] = true;
        addJournal(game.name + " is laid to rest here. The world will remember.");
    }
    game.screen = EPILOGUE;
    game.ending = message;
    game.dead = !victory;
    game.won = victory;
    game.daysSurvived = std::max(1, game.day - game.characterStartDay + 1);
    saveGame();
}

void tickNeeds(bool traveling)
{
    int terrain = game.map[game.y][game.x];
    int exertion = traveling ? ((terrain == 2 || terrain == 3) ? 2 : 0) : 0;
    const char *fatalCause = NULL;
    game.hunger = clampValue(game.hunger - (traveling ? 8 + exertion : 3), 0, 100);
    int armorBurden = traveling ? equippedArmorLoad() : 0;
    int bootPace = traveling ? equipmentTravelPace() : 0;
    game.fatigue = clampValue(game.fatigue - std::max(1, traveling ? 7 + exertion + armorBurden - bootPace : 2), 0, 100);
    if (game.hunger == 0)
    {
        --game.hp;
        if (game.hp <= 0) fatalCause = "Starvation finally overcame you.";
    }
    if (game.fatigue == 0)
    {
        --game.hp;
        if (game.hp <= 0 && !fatalCause)
            fatalCause = game.exposure >= 60 ? "Cold and exhaustion finally overcame you." :
                                               "Exhaustion finally overcame you.";
    }
    if (game.weather == 1 || game.weather == 2 || game.weather == 4)
        game.exposure = clampValue(game.exposure +
            std::max(0, (traveling ? 8 : 4) - equippedExposureProtection()), 0, 100);
    else game.exposure = clampValue(game.exposure - 3, 0, 100);
    if (game.exposure >= 60) game.fatigue = clampValue(game.fatigue - 3, 0, 100);
    if (game.bleeding > 0)
    {
        game.hp -= 1;
        --game.bleeding;
        if (game.hp <= 0 && !fatalCause) fatalCause = "Blood loss finally overcame you.";
    }
    if (game.infection > 0)
    {
        game.hp -= 1;
        --game.infection;
        if (game.hp <= 0 && !fatalCause) fatalCause = "An untreated infection finally overcame you.";
    }
    game.hp = clampValue(game.hp, 0, game.maxHp);
    if (game.hp <= 0)
        endJourney(fatalCause ? fatalCause : "Your wounds and empty reserves finally overcame you.", false);
}

void advanceTime()
{
    game.timeOfDay = (game.timeOfDay + 1) % 4;
    ++game.turn;
    updateLocalPresence();
    if (game.timeOfDay == 0)
    {
        ++game.day;
        if (game.day % 7 == 0) game.season = (game.season + 1) % 4;
        game.weather = randomBetween(0, 4);
        for (int i = 0; i < ITEM_COUNT; ++i)
        {
            if (ITEMS[i].shelfLife <= 0 || itemQuantity(i) <= 0) continue;
            ++game.inventoryAge[i];
            if (game.inventoryAge[i] >= ITEMS[i].shelfLife)
            {
                for (std::size_t stack = 0; stack < game.inventory.size(); ++stack)
                    if (game.inventory[stack].item == i) game.inventory[stack].quantity = 0;
                addJournal(std::string(ITEMS[i].name) + " spoils. You discard what cannot be saved.");
            }
        }
        game.inventory.erase(std::remove_if(game.inventory.begin(), game.inventory.end(),
            [](const Stack &stack) { return stack.quantity <= 0; }), game.inventory.end());
        simulateWorldDay();
    }
}

void spendHour()
{
    advanceTime();
    tickNeeds(false);
}

bool eventWasRecent(int index)
{
    for (int i = 0; i < 4; ++i) if (game.lastEvents[i] == index) return true;
    return false;
}

int chooseRoadEvent()
{
    std::vector<int> eligible;
    int totalWeight = 0;
    int biome = game.map[game.y][game.x];
    const auto contextWeight = [](const RoadEvent &event) {
        int weight = event.weight;
        if (game.timeOfDay == 3 && (event.kind == EVENT_HAZARD || event.kind == EVENT_STORY)) weight += 2;
        if (game.timeOfDay == 0 && (event.kind == EVENT_CELEBRATION || event.kind == EVENT_FESTIVAL)) weight += 2;
        if (game.weather == 1 && event.kind == EVENT_WEATHER) weight += 2;
        if (event.faction >= 0 && event.faction < FACTION_COUNT)
            weight += clampValue(game.factionRep[event.faction], -2, 3) +
                      game.factionUnrest[event.faction] / 25;
        if (game.season == 2 && event.kind == EVENT_FESTIVAL) weight += 2;
        return std::max(1, weight);
    };
    for (int i = 0; i < EVENT_COUNT; ++i)
    {
        const RoadEvent &event = ROAD_EVENTS[i];
        if (event.biome >= 0 && event.biome != biome) continue;
        if (event.weather >= 0 && event.weather != game.weather) continue;
        if (event.season >= 0 && event.season != game.season) continue;
        if (event.storyId > 0 && event.storyStage != game.storyStage[event.storyId]) continue;
        if (event.storyId == 16 && game.fallenCount == 0) continue;
        if (event.storyId == 13 && game.debts <= 0) continue;
        if (eventWasRecent(i)) continue;
        eligible.push_back(i);
        totalWeight += contextWeight(event);
    }
    if (eligible.empty())
    {
        for (int i = 0; i < EVENT_COUNT; ++i)
        {
            const RoadEvent &event = ROAD_EVENTS[i];
            if (eventWasRecent(i)) continue;
            if ((event.biome < 0 || event.biome == biome) &&
                (event.storyId == 0 || event.storyStage == game.storyStage[event.storyId]) &&
                (event.storyId != 16 || game.fallenCount > 0) &&
                (event.storyId != 13 || game.debts > 0))
            {
                eligible.push_back(i);
                totalWeight += contextWeight(event);
            }
        }
    }
    if (eligible.empty()) return -1;
    int roll = randomBetween(1, totalWeight);
    for (std::size_t i = 0; i < eligible.size(); ++i)
    {
        roll -= contextWeight(ROAD_EVENTS[eligible[i]]);
        if (roll <= 0) return eligible[i];
    }
    return eligible.back();
}

void triggerEvent(int index)
{
    if (index < 0 || index >= EVENT_COUNT) return;
    encounterFlashTimer = 0.42f;
    const RoadEvent &event = ROAD_EVENTS[index];
    game.lastEvents[3] = game.lastEvents[2];
    game.lastEvents[2] = game.lastEvents[1];
    game.lastEvents[1] = game.lastEvents[0];
    game.lastEvents[0] = index;
    ++game.eventHistoryCount;
    ++game.eventHistory[index];
    addJournal(std::string(event.title) + " — " + event.outcome);
    if (event.kind == EVENT_NPC || event.kind == EVENT_DILEMMA || event.kind == EVENT_TRADE ||
        event.kind == EVENT_FACTION || event.kind == EVENT_FESTIVAL ||
        event.kind == EVENT_CELEBRATION || event.kind == EVENT_RUMOR)
    {
        int faction = event.faction >= 0 && event.faction < FACTION_COUNT ?
                      event.faction : factionAt(game.x, game.y);
        addJournal(npcDialogue(npcForFaction(faction),
                   std::string("The talk around this is: ") + event.outcome));
    }
    if (event.storyId == 16 && game.fallenCount > 0)
    {
        int fallen = game.fallenCount - 1;
        game.seen[game.fallenY[fallen]][game.fallenX[fallen]] = true;
        game.discoveredPOI[game.fallenY[fallen]][game.fallenX[fallen]] = true;
        addJournal("Rumor: " + game.fallenName[fallen] + " fell in the " +
                   terrainName(game.map[game.fallenY[fallen]][game.fallenX[fallen]]) +
                   ". Their grave is marked on the map.");
    }
    if (event.storyId > 0 && event.storyStage == 0)
    {
        game.storyStage[event.storyId] = 1;
        if (event.storyId == 1 || event.storyId == 2 || event.storyId == 3)
            addJournal("You decide whether this is only a passing tale or the beginning of a promise.");
    }
    else if (event.storyId > 0 && event.storyStage > 0)
        game.storyStage[event.storyId] = event.storyStage + 1;
    switch (event.kind)
    {
        case EVENT_SUPPLIES: addItem(5, 1); addItem(6, 1); break;
        case EVENT_RUMOR:
            addJournal("Rumor: old mile-markers, familiar songs, and names left in ledgers may lead somewhere.");
            if (game.storyStage[1] == 1) game.storyStage[1] = 1;
            break;
        case EVENT_WEATHER:
            game.fatigue = clampValue(game.fatigue - 8, 0, 100);
            if (game.weather == 1 || game.weather == 2) game.exposure = clampValue(game.exposure + 12, 0, 100);
            break;
        case EVENT_HAZARD:
            game.hp = clampValue(game.hp - std::max(1, 3 - (game.attributes[1] - 10) / 2), 0, game.maxHp);
            game.bleeding = std::max(game.bleeding, 2);
            {
                int bodyPart = randomBetween(0, 3);
                game.injuries[bodyPart] = clampValue(game.injuries[bodyPart] + 1, 0, 3);
            }
            grantTrait(0);
            addJournal("The snare catches a limb. A lasting injury will make some checks harder.");
            break;
        case EVENT_HEALING: game.hp = clampValue(game.hp + 4, 0, game.maxHp); game.exposure = clampValue(game.exposure - 10, 0, 100); break;
        case EVENT_FESTIVAL:
        case EVENT_CELEBRATION: game.fatigue = clampValue(game.fatigue + 6, 0, 100); break;
        case EVENT_STORY:
        case EVENT_QUIET: break;
        case EVENT_NPC:
        case EVENT_DILEMMA:
        case EVENT_TRADE:
        case EVENT_FACTION:
            game.pendingEvent = index;
            game.activity = EVENT_CHOICE;
            game.activityText = event.description;
            if (event.kind == EVENT_NPC || event.kind == EVENT_FACTION ||
                event.kind == EVENT_DILEMMA || event.kind == EVENT_TRADE)
            {
                int faction = event.faction >= 0 && event.faction < FACTION_COUNT ?
                              event.faction : factionAt(game.x, game.y);
                game.activeNpc = npcForFaction(faction);
                rememberNpcAtCurrentLocation(game.activeNpc);
                game.activityText += " " + npcDialogue(game.activeNpc,
                    std::string("The local stakes: ") + event.outcome);
            }
            game.page = ROAD;
            break;
    }
    if (event.kind == EVENT_SUPPLIES || event.kind == EVENT_RUMOR ||
        event.kind == EVENT_FESTIVAL || event.kind == EVENT_CELEBRATION ||
        event.kind == EVENT_STORY || event.kind == EVENT_QUIET)
        game.eventStreak = 0;
    if (game.hp <= 0) endJourney("A snare on a lonely road ended your journey.", false);
}

void forage()
{
    if (game.enemyIndex >= 0 || game.activity != FREE_ROAM) return;
    advanceTime();
    tickNeeds(false);
    if (game.screen != GAME) return;
    int result = randomBetween(0, 99);
    int forageChance = clampValue(58 + (game.attributes[4] - 10) * 3 + (game.attributes[3] - 10) * 2 +
                                  game.skills[1] * 2 + (game.traits[5] ? 2 : 0), 20, 85);
    if (result < forageChance)
    {
        const int finds[] = {5, 6, 7, 8, 10, 11, 12, 15, 16, 17, 18, 20, 21, 22, 31, 32, 44, 45};
        int found = finds[randomBetween(0, static_cast<int>(sizeof(finds) / sizeof(finds[0])) - 1)];
        addItem(found, 1);
        addJournal(std::string("You forage and find ") + ITEMS[found].name + ".");
    }
    else if (result < 78)
    {
        int damage = randomBetween(1, 3);
        game.hp = clampValue(game.hp - damage, 0, game.maxHp);
        if (randomBetween(1, 20) + game.attributes[4] - 10 - game.injuries[0] < 14)
            game.infection = std::max(game.infection, 2);
        addJournal("A misidentified plant stings your tongue. You lose a little strength and risk infection.");
    }
    else addJournal("You turn up nothing worth carrying.");
    if (game.hp <= 0) endJourney("The wilds took what the road left behind.", false);
    else
    {
        practiceSkill(1);
        game.activity = SEARCHING;
        game.activityText = game.journal.front();
    }
}

void beginActivity(Activity activity, const std::string &message)
{
    if (game.enemyIndex >= 0 || game.activity != FREE_ROAM) return;
    game.activity = activity;
    game.activityText = message;
    game.pageScroll = 0;
    if (activity == TALKING)
    {
        const Settlement *place = settlementAt(game.x, game.y);
        game.activeNpc = game.wanderingNpc >= 0 ? game.wanderingNpc :
                         place ? npcForFaction(place->faction) : randomBetween(0, NPC_COUNT - 1);
        rememberNpcAtCurrentLocation(game.activeNpc);
        const NPC &npc = NPCS[game.activeNpc];
        if (game.wanderingNpcActivity >= 0 && game.wanderingNpcActivity < 10)
            game.activityText = std::string(npc.name) + ", a traveling " + npc.role + ", is " +
                WANDERING_NPC_ACTIVITIES[game.wanderingNpcActivity] + ". " + npc.personality +
                " " + randomDialogue() + " They seem willing to share why they are traveling.";
        else
            game.activityText = npcDialogue(game.activeNpc,
                                            std::string(npc.personality) + " " + randomDialogue());
        game.wanderingNpc = -1;
        game.wanderingNpcTurns = 0;
        game.wanderingNpcActivity = -1;
    }
}

void endActivity()
{
    game.activity = FREE_ROAM;
    game.activityText.clear();
    game.pageScroll = 0;
    game.activeNpc = -1;
    game.activeFallen = -1;
    game.pendingEvent = -1;
}

void lookAround()
{
    if (game.enemyIndex >= 0 || game.activity != FREE_ROAM) return;
    advanceTime();
    tickNeeds(false);
    if (game.screen != GAME) return;

    int roll = randomBetween(0, 99);
    int findChance = clampValue(35 + (game.attributes[4] - 10) * 3 + (game.attributes[3] - 10) * 2 +
                                game.skills[2] * 2 + (game.traits[5] ? 2 : 0), 15, 75);
    if (roll < findChance)
    {
        const int finds[] = {
            5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
            23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            39, 40, 41, 42, 43, 44, 45, 46, 47
        };
        int found = finds[randomBetween(0, static_cast<int>(sizeof(finds) / sizeof(finds[0])) - 1)];
        addItem(found, 1);
        game.activityText = std::string("You find ") + ITEMS[found].name + " tucked away nearby.";
    }
    else if (roll < 48)
    {
        int finds[] = {11, 12, 23, 24, 25};
        int found = finds[randomBetween(0, 4)];
        addItem(found, 1);
        game.activityText = std::string("You find ") + ITEMS[found].name + " tucked away nearby.";
    }
    else
    {
        game.activityText = "You search carefully, but find nothing worth carrying.";
    }
    addJournal(game.activityText);
    practiceSkill(2);
    game.activity = SEARCHING;
}

void askAboutRoad()
{
    if (game.activeNpc < 0) game.activeNpc = randomBetween(0, NPC_COUNT - 1);
    const NPC &npc = NPCS[game.activeNpc];
    const Creature &creature = CREATURES[randomBetween(0, CREATURE_COUNT - 1)];
    game.activityText = npcDialogue(game.activeNpc, std::string("A warning from the trail: ") +
                        creature.warning +
                        (game.npcTrust[game.activeNpc] > 1 ? " I have heard this before; keep to the marked paths." : "") +
                        " " + npc.story);
    game.factionRep[npc.faction] = clampValue(game.factionRep[npc.faction] + 1, -5, 5);
    game.npcTrust[game.activeNpc] = clampValue(game.npcTrust[game.activeNpc] + 1, -5, 5);
    addJournal(std::string(npc.name) + " shared a warning and a piece of their own story.");
    practiceSkill(3);
    updateQuestProgress(QUEST_NEWS, -1, 1);
    spendHour();
}

void shareAnNPCStory()
{
    if (game.activeNpc < 0) return;
    const NPC &npc = NPCS[game.activeNpc];
    int dc = clampValue(15 - game.npcTrust[game.activeNpc] - (game.traits[2] ? 1 : 0), 12, 18);
    int check = randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3] +
                (game.traits[1] ? 1 : 0);
    if (check >= dc)
    {
        game.npcTrust[game.activeNpc] = clampValue(game.npcTrust[game.activeNpc] + 1, -5, 5);
        game.factionRep[npc.faction] = clampValue(game.factionRep[npc.faction] + 1, -5, 5);
        game.favors += 1;
        grantTrait(1);
        grantTrait(2);
        game.activityText = npcDialogue(game.activeNpc, std::string("My own story: ") + npc.story +
                            " Remember it, but do not promise to fix it.");
        addJournal(std::string("You earn ") + npc.name + "'s trust. Their story may return to the road.");
    }
    else
    {
        game.activityText = std::string(npc.name) +
            " changes the subject. \"" + randomDialogue() + "\"";
        addJournal(std::string(npc.name) + " keeps their story private for now.");
    }
    practiceSkill(3);
    spendHour();
}

void offerNPCHelp()
{
    if (game.activeNpc < 0) return;
    const NPC &npc = NPCS[game.activeNpc];
    if (itemQuantity(5) > 0)
    {
        consumeItem(5);
        game.npcTrust[game.activeNpc] = clampValue(game.npcTrust[game.activeNpc] + 2, -5, 5);
        game.factionRep[npc.faction] = clampValue(game.factionRep[npc.faction] + 1, -5, 5);
        game.favors += 1;
        grantTrait(1);
        grantTrait(2);
        game.activityText = std::string("You share a ration. ") +
                            npcDialogue(game.activeNpc, "I will remember who shared their food.");
        addJournal(std::string("You share food with ") + npc.name + ".");
        spendHour();
    }
    else
    {
        game.activityText = "You have nothing spare to offer.";
        addJournal("You have no spare ration to share.");
        spendHour();
        return;
    }
}

void resolvePendingEvent(int choice)
{
    if (game.pendingEvent == -2)
    {
        int fallen = game.activeFallen;
        if (fallen >= 0 && choice == 0)
        {
            grantTrait(3);
            int found = 0;
            for (int item = 0; item < ITEM_COUNT; ++item)
            {
                int quantity = game.fallenGear[fallen][item];
                if (quantity > 0)
                {
                    int before = itemQuantity(item);
                    addItem(item, quantity);
                    found += itemQuantity(item) - before;
                }
            }
            addJournal(found > 0 ? "You recover what the grave and your pack can bear." :
                                   "The old gear is there, but your pack cannot carry it.");
        }
        else addJournal("You leave the grave undisturbed. The name is enough to carry onward.");
        if (game.x >= 0 && game.y >= 0) game.landmarks[game.y][game.x] = 5;
        advanceTime();
        tickNeeds(false);
        endActivity();
        return;
    }
    if (game.pendingEvent < 0 || game.pendingEvent >= EVENT_COUNT) return;
    const RoadEvent &event = ROAD_EVENTS[game.pendingEvent];
    const int faction = event.faction >= 0 ? event.faction : 2;
    if (choice == 0)
    {
        if (event.kind == EVENT_TRADE && game.gold >= 2)
        {
            game.gold -= 2;
            addItem(14, 1);
            addJournal("You buy the trail cloak at the marked price.");
        }
        else
        {
            int dc = 14 + (event.kind == EVENT_DILEMMA ? 2 : 0);
            if (game.gold > 0 && event.storyId == 13)
            {
                --game.gold;
                --game.debts;
            }
            else if (randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3] +
                     (game.traits[1] ? 1 : 0) + (faction == 2 && game.traits[4] ? 1 : 0) >= dc)
            {
                practiceSkill(3);
                game.favors += 1;
                game.factionRep[faction] = clampValue(game.factionRep[faction] + 1, -5, 5);
                grantTrait(1);
                if (event.storyId > 0) grantTrait(2);
                if (event.storyId == 3) grantTrait(4);
                if (event.storyId == 3) game.factionRep[2] = clampValue(game.factionRep[2] + 1, -5, 5);
                if (event.storyId == 12 || event.storyId == 17) addJournal("Someone will remember that you stopped to help.");
                else addJournal("Your help is accepted. A small favor will travel ahead of you.");
                if (event.kind == EVENT_DILEMMA) game.hp = clampValue(game.hp - 1, 0, game.maxHp);
            }
            else
            {
                addJournal("You try to help, but the road's need is larger than your strength today.");
                if (event.kind == EVENT_HAZARD) game.bleeding = std::max(game.bleeding, 1);
            }
        }
    }
    else if (choice == 1)
    {
        int check = randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3] +
                    (game.traits[1] ? 1 : 0) + (faction == 2 && game.traits[4] ? 1 : 0);
        practiceSkill(3);
        if (check >= 16)
        {
            game.favors += 1;
            game.factionRep[faction] = clampValue(game.factionRep[faction] + 1, -5, 5);
            addJournal("A careful bargain leaves both sides willing to meet again.");
        }
        else
        {
            game.debts += 1;
            game.grudges += 1;
            game.factionRep[faction] = clampValue(game.factionRep[faction] - 1, -5, 5);
            addJournal("The bargain goes sour. The other party will remember the slight.");
        }
    }
    else
    {
        game.grudges += event.kind == EVENT_FACTION ? 1 : 0;
        addJournal("You keep your supplies and continue. The people you passed may remember it.");
    }
    if (event.kind == EVENT_FACTION)
        game.factionRep[faction] = clampValue(game.factionRep[faction] + (choice == 0 ? 1 : choice == 2 ? -1 : 0), -5, 5);
    advanceTime();
    tickNeeds(false);
    endActivity();
    if (game.hp <= 0) endJourney("A choice on the road left you too worn to continue.", false);
}

void startEncounter(int creatureIndex = -1)
{
    encounterFlashTimer = 0.42f;
    int biome = game.map[game.y][game.x];
    std::vector<int> likely;
    for (int i = 0; i < CREATURE_COUNT; ++i)
        if (std::string(CREATURES[i].terrain) == terrainName(biome))
            likely.push_back(i);
    if (creatureIndex >= 0 && creatureIndex < CREATURE_COUNT)
        game.enemyIndex = creatureIndex;
    else
        game.enemyIndex = likely.empty() ? randomBetween(0, CREATURE_COUNT - 1) :
            likely[randomBetween(0, static_cast<int>(likely.size()) - 1)];
    game.enemyHp = CREATURES[game.enemyIndex].hp;
    game.enemyMorale = CREATURES[game.enemyIndex].morale;
    game.enemyTelegraph = 0;
    game.combatTurn = 0;
    game.guarding = false;
    game.weakened = false;
    game.discovered[game.enemyIndex] = 1;
    game.roamingCreature = -1;
    game.roamingCreatureTurns = 0;
    const Creature &creature = CREATURES[game.enemyIndex];
    addJournal(std::string(creature.name) + " steps into your path. " + creature.warning);
    addJournal(creature.personality);
    game.page = ROAD;
}

void resolveSocialScene(int choice)
{
    if (game.socialSceneKind == SOCIAL_NONE) return;
    SocialSceneKind kind = game.socialSceneKind;
    int npcA = game.socialSceneNpcA;
    int npcB = game.socialSceneNpcB;
    int creature = game.socialSceneCreature;
    bool leave = (kind == SOCIAL_NPC_EXCHANGE && choice == 2) ||
                 (kind != SOCIAL_NPC_EXCHANGE && choice == 2);
    clearSocialScene();

    if (leave)
    {
        addJournal("You leave the moment to those already living it.");
        advanceTime();
        tickNeeds(false);
        return;
    }

    rememberNpcAtCurrentLocation(npcA);
    if (npcB >= 0) rememberNpcAtCurrentLocation(npcB);

    if (kind == SOCIAL_NPC_EXCHANGE)
    {
        if (choice == 0)
        {
            int check = randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3];
            if (check >= 14)
            {
                game.npcTrust[npcA] = clampValue(game.npcTrust[npcA] + 1, -5, 5);
                game.npcTrust[npcB] = clampValue(game.npcTrust[npcB] + 1, -5, 5);
                game.factionRep[NPCS[npcA].faction] =
                    clampValue(game.factionRep[NPCS[npcA].faction] + 1, -5, 5);
                ++game.favors;
                addJournal(std::string("You find a compromise between ") + NPCS[npcA].name +
                           " and " + NPCS[npcB].name + ". Both leave owing you a small kindness.");
            }
            else
                addJournal("Your words fail to settle the matter, but neither turns the disagreement on you.");
        }
        else
        {
            game.npcTrust[npcA] = clampValue(game.npcTrust[npcA] + 1, -5, 5);
            game.npcTrust[npcB] = clampValue(game.npcTrust[npcB] + 1, -5, 5);
            ++game.favors;
            addJournal(npcDialogue(npcA, std::string("You listen as ") + NPCS[npcB].name +
                                   " answers. The exchange leaves a useful rumor in your keeping."));
        }
    }
    else if (kind == SOCIAL_BANDIT_THREAT)
    {
        if (choice == 0)
        {
            addJournal(std::string("You step between ") + NPCS[npcA].name +
                       " and the outlaw. The outlaw turns on you.");
            startEncounter(0);
        }
        else
        {
            int check = randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3];
            if (check >= 14)
            {
                game.npcTrust[npcA] = clampValue(game.npcTrust[npcA] + 1, -5, 5);
                game.favors++;
                addJournal(std::string("Your diversion gives ") + NPCS[npcA].name +
                           " room to escape. The outlaw breaks off; a favor is promised.");
            }
            else
            {
                addJournal("The outlaw sees through your diversion and advances. The warning was no idle one.");
                startEncounter(0);
            }
        }
    }
    else if (kind == SOCIAL_CREATURE_THREAT)
    {
        if (choice == 1)
        {
            addJournal(std::string("You draw the ") + CREATURES[creature].name +
                       " away from " + NPCS[npcA].name + ".");
            startEncounter(creature);
        }
        else
        {
            int check = randomBetween(1, 20) + (game.attributes[4] - 10) / 2 + game.skills[1];
            if (check >= 14)
            {
                game.npcTrust[npcA] = clampValue(game.npcTrust[npcA] + 1, -5, 5);
                game.favors++;
                addJournal(std::string("You steer the ") + CREATURES[creature].name +
                           " into the brush and help " + NPCS[npcA].name +
                           " reach safety. They will remember it.");
            }
            else
            {
                addJournal(std::string("The ") + CREATURES[creature].name +
                           " wheels toward you. Its warning proved true.");
                startEncounter(creature);
            }
        }
    }

    advanceTime();
    tickNeeds(false);
    if (game.hp <= 0) endJourney("A choice in the Marches left you too worn to continue.", false);
}

void travel(int dx, int dy)
{
    if (game.enemyIndex >= 0 || game.activity != FREE_ROAM) return;
    int nx = game.x + dx;
    int ny = game.y + dy;
    if (nx < 0 || ny < 0 || nx >= MAP_SIZE || ny >= MAP_SIZE) return;
    if (hasActiveHuntAt(game.x, game.y, game.roamingCreature))
    {
        game.roamingCreature = -1;
        game.roamingCreatureTurns = 0;
    }
    clearSocialScene();
    game.x = nx;
    game.y = ny;
    updateQuestProgress(QUEST_EXPLORE, game.map[game.y][game.x], 1);
    game.campBuilt = false;
    revealNearby();
    advanceTime();
    tickNeeds(true);
    if (game.screen != GAME) return;
    practiceSkill(1);
    game.eventStreak = clampValue(game.eventStreak + 1, 0, 5);
    if (game.landmarks[game.y][game.x] == 4 && game.fallenCount > 0)
    {
        int fallen = game.fallenCount - 1;
        int matchingFallen = -1;
        for (int i = 0; i < game.fallenCount; ++i)
            if (game.fallenX[i] == game.x && game.fallenY[i] == game.y &&
                game.landmarks[game.y][game.x] == 4) matchingFallen = i;
        if (matchingFallen < 0) fallen = -1;
        else fallen = matchingFallen;
        if (fallen < 0) return;
        game.pendingEvent = -2;
        game.activeFallen = fallen;
        game.activity = EVENT_CHOICE;
        game.activityText = game.fallenName[fallen] + " rests here. " + game.fallenCause[fallen] +
                            " Their pack may still hold something useful.";
        game.page = ROAD;
        addJournal("You find the grave of " + game.fallenName[fallen] + ".");
        return;
    }
    const Settlement *settlement = settlementAt(game.x, game.y);
    if (settlement)
    {
        game.eventStreak = 0;
        addJournal(std::string("You reach ") + settlement->name + ". " + settlement->kind + " lights break the dusk.");
        game.hp = clampValue(game.hp + 2, 0, game.maxHp);
        createSocialScene();
        return;
    }
    ensureQuestHuntPresence();
    if (game.roamingCreature >= 0 && hasActiveHuntAt(game.x, game.y, game.roamingCreature))
        return;
    int localFaction = factionAt(game.x, game.y);
    int borderTension = localFaction >= 0 ? game.factionUnrest[localFaction] / 18 : 0;
    int combatChance = clampValue(16 + game.eventStreak * 2 + (game.timeOfDay == 3 ? 5 : 0) +
                                 (game.weather == 2 ? 4 : 0) + borderTension -
                                 std::max(0, game.factionRep[1]) * 2, 8, 27);
    if (game.eventStreak >= 3) combatChance = std::max(6, combatChance - 8);
    int roll = randomBetween(0, 99);
    if (roll < combatChance)
    {
        game.eventStreak = 0;
        startEncounter();
    }
    else if (roll < 58)
    {
        int event = chooseRoadEvent();
        if (event >= 0) triggerEvent(event);
    }
    else
    {
        game.eventStreak = 0;
        addJournal(std::string("You cross ") + terrainName(game.map[game.y][game.x]) + ". The wind moves ahead of you.");
    }
    if (game.activity == FREE_ROAM && game.enemyIndex < 0)
        createSocialScene();
    if (game.screen == GAME && game.hp <= 0) endJourney("The road took you when your strength ran out.", false);
}

void useItem(int itemIndex)
{
    if (itemIndex < 0 || itemIndex >= ITEM_COUNT || itemQuantity(itemIndex) == 0) return;
    const Item &item = ITEMS[itemIndex];
    if (item.effect == WEAPON || item.effect == ARMOR) return;
    consumeItem(itemIndex);
    if (item.effect == FOOD) game.hunger = clampValue(game.hunger + item.amount, 0, 100);
    else if (item.effect == MEDICINE)
    {
        game.hp = clampValue(game.hp + item.amount, 0, game.maxHp);
        if (itemIndex == 7) game.infection = std::max(0, game.infection - 1);
        if (itemIndex == 8)
        {
            game.bleeding = 0;
            for (int bodyPart = 0; bodyPart < 4; ++bodyPart)
                if (game.injuries[bodyPart] > 0) { --game.injuries[bodyPart]; break; }
        }
        if (itemIndex == 9) { game.infection = 0; game.injuries[3] = std::max(0, game.injuries[3] - 1); }
    }
    addJournal(std::string("You use ") + item.name + ".");
    if (game.enemyIndex >= 0)
    {
        game.guarding = false;
    }
}

void craftBandage()
{
    if (itemQuantity(11) <= 0 || itemQuantity(12) <= 0)
    {
        addJournal("A bandage needs one bitterroot and one clean cloth.");
        return;
    }
    consumeItem(11);
    consumeItem(12);
    addItem(8, 1);
    addJournal("You bind bitterroot in clean cloth and make a simple bandage.");
}

void enemyTurn()
{
    if (game.enemyIndex < 0 || game.screen != GAME) return;
    const Creature &creature = CREATURES[game.enemyIndex];
    int proneBefore = game.proneTurns;
    int stunnedBefore = game.stunnedTurns;
    ++game.combatTurn;
    int damage = creature.attack + randomBetween(0, 2);
    bool ability = game.combatTurn % 3 == 0;
    if (ability)
    {
        addJournal(std::string(creature.name) + " uses " + creature.ability);
        if (game.enemyIndex == 3) { game.weakened = true; game.injuries[2] = std::max(game.injuries[2], 1); }
        else if (game.enemyIndex == 0) damage += 2;
        else if (game.enemyIndex == 1) damage += 3;
        else if (game.enemyIndex == 2) damage += 4;
        else if (game.enemyIndex == 4) game.stunnedTurns = std::max(game.stunnedTurns, 1);
        else if (game.enemyIndex == 5) { game.exposure = clampValue(game.exposure + 12, 0, 100); game.frightenedTurns = 1; }
        else if (game.enemyIndex == 6 || game.enemyIndex == 8) { game.weakened = true; game.proneTurns = 1; }
        else if (game.enemyIndex == 7) { game.proneTurns = std::max(game.proneTurns, 1); damage += 2; }
        else if (game.enemyIndex == 9) game.bleeding = std::max(game.bleeding, 2);
        else if (game.enemyIndex == 10) damage += 3;
        else if (game.enemyIndex == 11) { game.proneTurns = std::max(game.proneTurns, 1); damage += 2; }
        else if (game.enemyIndex == 12) { game.proneTurns = std::max(game.proneTurns, 1); damage += 2; }
        else if (game.enemyIndex == 13) { game.exposure = clampValue(game.exposure + 10, 0, 100); game.frightenedTurns = 1; }
        else if (game.enemyIndex == 14) game.stunnedTurns = std::max(game.stunnedTurns, 1);
        else if (game.enemyIndex == 15) { game.proneTurns = std::max(game.proneTurns, 1); damage += 3; }
        else if (game.enemyIndex == 16) game.bleeding = std::max(game.bleeding, 2);
        else if (game.enemyIndex == 17) game.stunnedTurns = std::max(game.stunnedTurns, 1);
        else if (game.enemyIndex == 18) { game.proneTurns = std::max(game.proneTurns, 1); damage += 2; }
        else if (game.enemyIndex == 19) { game.stunnedTurns = std::max(game.stunnedTurns, 1); damage += 2; }
        else if (game.enemyIndex == 20) game.proneTurns = std::max(game.proneTurns, 1);
        else if (game.enemyIndex == 21) { game.exposure = clampValue(game.exposure + 12, 0, 100); game.frightenedTurns = 1; }
        else if (game.enemyIndex == 22) game.frightenedTurns = std::max(game.frightenedTurns, 1);
        else if (game.enemyIndex == 23) damage += 3;
        else if (game.enemyIndex == 24) { game.bleeding = std::max(game.bleeding, 2); damage += 1; }
        else if (game.enemyIndex == 25) { game.exposure = clampValue(game.exposure + 5, 0, 100); game.weakened = true; }
        else if (game.enemyIndex == 26) { game.proneTurns = std::max(game.proneTurns, 1); damage += 2; }
        else if (game.enemyIndex == 27) game.stunnedTurns = std::max(game.stunnedTurns, 1);
        else if (game.enemyIndex == 28) { game.weakened = true; game.injuries[2] = std::max(game.injuries[2], 1); }
        else if (game.enemyIndex == 29) { game.exposure = clampValue(game.exposure + 10, 0, 100); damage += 1; }
        else if (game.enemyIndex == 30) { game.exposure = clampValue(game.exposure + 8, 0, 100); game.frightenedTurns = 1; }
        else if (game.enemyIndex == 31) { game.proneTurns = std::max(game.proneTurns, 1); damage += 3; }
        else damage += 2;
    }
    int hit = randomBetween(1, 20);
    bool landed = hit == 20 || (hit != 1 && hit + creature.attack / 2 >=
                                game.playerAC - (game.proneTurns > 0 ? 2 : 0) - game.injuries[3]);
    if (!landed) damage = 0;
    else
    {
        if (hit == 20) damage *= 2;
        if (game.equippedHelmet >= 0 || game.equippedArmor >= 0 ||
            game.equippedGloves >= 0 || game.equippedBoots >= 0 || game.traits[0])
            damage = std::max(1, damage - 1);
        if (game.guarding) damage = std::max(1, damage / 2);
        game.hp = clampValue(game.hp - damage, 0, game.maxHp);
    }
    game.guarding = false;
    if (landed) addJournal(std::string(creature.name) + " strikes for " + std::to_string(damage) + " damage.");
    else addJournal(std::string(creature.name) + " misses; your armor turns the blow aside.");
    if (proneBefore > 0 && game.proneTurns > 0) --game.proneTurns;
    if (stunnedBefore > 0 && game.stunnedTurns > 0) --game.stunnedTurns;
    game.enemyTelegraph = game.combatTurn % 3 == 2 ? 1 : 0;
    if (game.hp <= 0) endJourney("You fell beneath the attack of a " + std::string(creature.name) + ".", false);
    else
    {
        advanceTime();
        tickNeeds(false);
    }
}

bool loseTurnIfStunned()
{
    if (game.stunnedTurns <= 0) return false;
    --game.stunnedTurns;
    addJournal("You are stunned and lose the opening.");
    enemyTurn();
    return true;
}

void tryBargain()
{
    if (game.enemyIndex < 0 || loseTurnIfStunned()) return;
    const Creature &creature = CREATURES[game.enemyIndex];
    int dc = creature.faction >= 0 ? 16 : 19;
    int check = randomBetween(1, 20) + (game.attributes[5] - 10) / 2 + game.skills[3] +
                (game.traits[1] ? 1 : 0) + (creature.faction == 2 && game.traits[4] ? 1 : 0);
    practiceSkill(3);
    if (check >= dc)
    {
        if (creature.faction >= 0)
            game.factionRep[creature.faction] = clampValue(game.factionRep[creature.faction] + 1, -5, 5);
        game.favors += creature.faction >= 0 ? 1 : 0;
        addJournal(std::string("You find a way to make peace with ") + creature.name + ". It withdraws.");
        game.enemyIndex = -1;
        advanceTime();
        tickNeeds(false);
    }
    else
    {
        addJournal("The creature rejects your offer. Its warning turns into motion.");
        enemyTurn();
    }
}

void useTerrain()
{
    if (game.enemyIndex < 0 || loseTurnIfStunned()) return;
    int stat = game.map[game.y][game.x] == 1 || game.map[game.y][game.x] == 7 ? 4 : 3;
    int dc = game.map[game.y][game.x] == 2 || game.map[game.y][game.x] == 3 ? 16 : 14;
    int skill = stat == 4 ? 2 : 4;
    int check = randomBetween(1, 20) + (game.attributes[stat] - 10) / 2 + game.skills[skill] +
                (game.traits[5] ? 1 : 0);
    practiceSkill(skill);
    if (check >= dc)
    {
        game.guarding = true;
        game.enemyMorale = std::max(0, game.enemyMorale - 1);
        addJournal("You use the ground and cover to blunt the next attack.");
    }
    else addJournal("There is no useful cover within reach.");
    enemyTurn();
}

void trySneakAway()
{
    if (game.enemyIndex < 0 || loseTurnIfStunned()) return;
    int check = randomBetween(1, 20) + (game.attributes[1] - 10) / 2 + game.skills[4] -
                game.injuries[2];
    practiceSkill(4);
    if (check >= 16)
    {
        addJournal("You slip behind cover and leave the creature searching the wrong trail.");
        game.enemyIndex = -1;
        advanceTime();
        tickNeeds(true);
    }
    else
    {
        addJournal("A stone shifts beneath your boot. The creature turns toward the sound.");
        enemyTurn();
    }
}

void playerAttack(bool heavy)
{
    if (game.enemyIndex < 0) return;
    const Creature &creature = CREATURES[game.enemyIndex];
    if (loseTurnIfStunned()) return;
    int roll = randomBetween(1, 20);
    int attack = roll + (heavy ? -1 : 2) + (game.attributes[1] - 10) / 2 + game.skills[0] +
                 equipmentAttackModifier() + (game.traits[3] && game.enemyIndex == 5 ? 1 : 0);
    practiceSkill(0);
    if (game.weakened) attack -= 2;
    int strengthBonus = std::max(0, (game.attributes[0] - 10) / 2);
    int weaponBonus = equippedWeaponDamageBonus();
    int damage = (heavy ? randomBetween(5, 9) : randomBetween(3, 7)) +
                 std::max(0, strengthBonus - game.injuries[1]) + weaponBonus;
    if (game.frightenedTurns > 0) { --game.frightenedTurns; attack -= 2; addJournal("Fear shakes your aim."); }
    if (game.injuries[0] > 0) attack -= game.injuries[0];
    if (roll == 20) damage *= 2;
    if (roll == 20 || (roll != 1 && attack >= creature.defense))
    {
        game.enemyHp -= damage;
        addJournal(std::string("Your ") + (heavy ? "hard strike" : "attack") + " hits for " + std::to_string(damage) + ".");
    }
    else addJournal("Your strike finds only air.");
    if (game.weakened)
    {
        game.weakened = false;
        addJournal("The snare's drag loosens from your limbs.");
    }
    if (game.enemyHp <= 0)
    {
        addJournal(std::string(creature.name) + " breaks and flees. The road is yours again.");
        updateQuestProgress(QUEST_HUNT, game.enemyIndex, 1);
        const CreatureLoot &loot = CREATURE_LOOT[game.enemyIndex];
        int lootQuantity = randomBetween(1, 2);
        addItem(loot.item, lootQuantity);
        addJournal(std::string("You recover ") + std::to_string(lootQuantity) + " " +
                   ITEMS[loot.item].name + " from the fallen creature.");
        if (loot.bonusItem >= 0 && randomBetween(1, 100) <= loot.bonusChance)
        {
            addItem(loot.bonusItem, 1);
            addJournal(std::string("Among the remains, you find ") + ITEMS[loot.bonusItem].name + ".");
        }
        ++game.creaturesBested;
        if (game.killCount < 10) game.notableKills[game.killCount++] = creature.name;
        else
        {
            for (int i = 1; i < 10; ++i) game.notableKills[i - 1] = game.notableKills[i];
            game.notableKills[9] = creature.name;
        }
        game.enemyIndex = -1;
        game.hp = clampValue(game.hp + 2, 0, game.maxHp);
        advanceTime();
        tickNeeds(false);
        ensureQuestHuntPresence();
        return;
    }
    if (game.enemyHp * 4 <= creature.hp * creature.morale && game.enemyMorale > 0)
    {
        --game.enemyMorale;
        if (game.enemyMorale <= 0 && randomBetween(1, 20) >= 8)
        {
            addJournal(std::string(creature.name) + " loses its nerve and retreats. You avoid a needless last blow.");
            game.enemyIndex = -1;
            advanceTime();
            tickNeeds(false);
            return;
        }
    }
    enemyTurn();
}

void waitAndRest()
{
    if (game.enemyIndex >= 0) return;
    bool sheltered = settlementAt(game.x, game.y) != NULL;
    bool alreadyCamped = game.campBuilt;
    if (!sheltered && !alreadyCamped)
    {
        game.campBuilt = true;
        addJournal(itemQuantity(13) > 0 ? "You build a low camp and coax a small fire from flint." :
                                         "You build a low shelter from branches and dry grass.");
    }
    advanceTime();
    tickNeeds(false);
    game.fatigue = clampValue(game.fatigue + 18, 0, 100);
    game.hp = clampValue(game.hp + 3, 0, game.maxHp);
    if (sheltered) addJournal("You rest under a roof. The road keeps its silence.");
    else addJournal("You rest inside your camp. The night is watchful.");
    if (!sheltered && randomBetween(1, 100) <= (itemQuantity(13) > 0 ? 6 : 12))
    {
        addJournal("A shape moves beyond the firelight. You are warned before it closes in.");
        startEncounter();
    }
}

void runBalanceSimulation()
{
    std::mt19937 simulation(0x57415957u);
    std::uniform_int_distribution<int> d20(1, 20);
    std::uniform_int_distribution<int> small(0, 99);
    const int runs = 1000;
    long long totalDays = 0;
    int causes[5] = {0, 0, 0, 0, 0};
    int combatCount = 0;
    int eventCount = 0;
    int quietCount = 0;
    for (int run = 0; run < runs; ++run)
    {
        int hp = 20, hunger = 100, fatigue = 100, exposure = 0;
        int turns = 0, consecutiveDanger = 0, cause = -1;
        while (hp > 0 && turns < 480)
        {
            ++turns;
            hunger = std::max(0, hunger - 8);
            fatigue = std::max(0, fatigue - 7);
            if (small(simulation) < 28) exposure = std::min(100, exposure + 5);
            else exposure = std::max(0, exposure - 3);
            if (hunger == 0) --hp;
            if (fatigue == 0) --hp;
            if (exposure >= 60) fatigue = std::max(0, fatigue - 2);

            int combatChance = std::max(8, std::min(24, 16 + consecutiveDanger * 2));
            if (consecutiveDanger >= 3) combatChance = std::max(6, combatChance - 8);
            int roll = small(simulation);
            if (roll < combatChance)
            {
                ++combatCount;
                consecutiveDanger = 0;
                int enemyHp = 18;
                int enemyTurn = 0;
                while (enemyHp > 0 && hp > 0)
                {
                    int attackRoll = d20(simulation);
                    int playerDamage = 3 + small(simulation) % 5 + 2;
                    if (attackRoll == 20) playerDamage *= 2;
                    if (attackRoll == 20 || (attackRoll != 1 && attackRoll + 2 >= 11))
                        enemyHp -= playerDamage;
                    if (enemyHp <= 0) break;
                    ++enemyTurn;
                    int foeRoll = d20(simulation);
                    if (foeRoll == 20 || (foeRoll != 1 && foeRoll + 2 >= 12))
                    {
                        int damage = 4 + small(simulation) % 3;
                        if (enemyTurn % 3 == 0) damage += 2;
                        hp -= std::max(1, damage - 1);
                    }
                    if (enemyHp <= 7 && d20(simulation) >= 13) break;
                }
            }
            else if (roll < 58)
            {
                ++eventCount;
                ++consecutiveDanger;
                int eventRoll = small(simulation);
                if (eventRoll < 8) hp -= 2;
                else if (eventRoll < 20) fatigue = std::max(0, fatigue - 6);
                else if (eventRoll < 35) hunger = std::min(100, hunger + 6);
            }
            else
            {
                ++quietCount;
                ++consecutiveDanger;
            }
            if (hp <= 0)
            {
                if (hunger == 0 && hp > -5) cause = 0;
                else if (exposure >= 60) cause = 2;
                else if (fatigue == 0) cause = 3;
                else cause = 1;
            }
        }
        if (cause < 0 && turns >= 480) cause = 4;
        causes[cause]++;
        totalDays += std::max(1, turns / 4);
    }
    int topCause = 0;
    for (int i = 1; i < 5; ++i) if (causes[i] > causes[topCause]) topCause = i;
    const char *causeNames[] = {"Hunger", "Combat wounds", "Cold / exposure", "Fatigue", "Still walking"};
    std::ostringstream report;
    report << "HIDDEN BALANCE MODE  /  " << runs << " seeded runs\n"
           << "Average days survived: " << (totalDays / static_cast<double>(runs)) << "\n"
           << "Top cause: " << causeNames[topCause] << " (" << causes[topCause] << ")\n"
           << "Event rate: " << (eventCount * 100 / std::max(1, combatCount + eventCount + quietCount)) << "%\n"
           << "Combat rate: " << (combatCount * 100 / std::max(1, combatCount + eventCount + quietCount)) << "%\n"
           << "Quiet travel: " << (quietCount * 100 / std::max(1, combatCount + eventCount + quietCount)) << "%\n"
           << "Death causes  /  hunger " << causes[0] << ", combat " << causes[1]
           << ", exposure " << causes[2] << ", fatigue " << causes[3];
    game.debugReport = report.str();
}

void drawTitle()
{
    if (GetScreenWidth() < 1100 || GetScreenHeight() < 700)
    {
        int width = std::max(260, GetScreenWidth() - 16);
        int height = std::max(440, GetScreenHeight() - 16);
        int left = (GetScreenWidth() - width) / 2, top = (GetScreenHeight() - height) / 2;
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), NIGHT);
        drawPanel({static_cast<float>(left), static_cast<float>(top), static_cast<float>(width), static_cast<float>(height)}, PAPER);
        DrawText("A LIFE ON THE MARCHES", left + 24, top + 32, 15, EMBER);
        DrawText("Wayworn", left + 24, top + 88, 46, INK);
        drawWrapped("An open road. A name. The miles ahead.", left + 24, top + 153, width - 48, 18, MUTED, 4);
        bool hasSave = hasSavedJourney();
        if (hasSave && drawButton({static_cast<float>(left + 20), static_cast<float>(top + height * 0.56f),
                                   static_cast<float>(width - 40), 54.0f}, "Continue Last Journey", true))
        {
            if (!loadGame()) game.saveError = "The autosave could not be loaded; the file was left untouched.";
        }
        if (drawButton({static_cast<float>(left + 20), static_cast<float>(top + height * (hasSave ? 0.70f : 0.62f)),
                        static_cast<float>(width - 40), 56.0f}, "Forge Your Own Story"))
        {
            game.screen = FORGE;
            game.name = "Wayfarer";
            game.origin = 0;
            game.editingName = false;
            resetAttributes();
        }
        DrawText(game.saveError.empty() ? "F3  /  Balance simulator" : game.saveError.c_str(),
                 left + 20, top + height - 30, 13, game.saveError.empty() ? MUTED : BLOOD);
        return;
    }
    int left, top;
    drawFrame("", 720, 540, left, top);
    const char *sub = "A LIFE ON THE MARCHES";
    DrawText(sub, left + (720 - MeasureText(sub, 20)) / 2, top + 157, 20, EMBER);
    DrawText("Wayworn", left + (720 - MeasureText("Wayworn", 64)) / 2, top + 188, 64, INK);
    DrawText("An open road. A name. The miles ahead.", left + (720 - MeasureText("An open road. A name. The miles ahead.", 21)) / 2,
             top + 274, 21, MUTED);
    bool hasSave = hasSavedJourney();
    if (hasSave && drawButton({static_cast<float>(left + 205), static_cast<float>(top + 326), 310, 52},
                              "Continue Last Journey", true))
    {
        if (!loadGame()) game.saveError = "The autosave could not be loaded; the file was left untouched.";
    }
    if (drawButton({static_cast<float>(left + 205), static_cast<float>(top + (hasSave ? 389 : 350)), 310, 58},
                   "Forge Your Own Story"))
    {
        game.screen = FORGE;
        game.name = "Wayfarer";
        game.origin = 0;
        game.editingName = false;
        resetAttributes();
    }
    DrawText(game.saveError.empty() ? "F3  /  Hidden balance simulator" : game.saveError.c_str(),
             left + 40, top + 463, 15, game.saveError.empty() ? MUTED : BLOOD);
}

void drawForge()
{
    if (GetScreenWidth() < 1100 || GetScreenHeight() < 700)
    {
        int width = std::max(260, GetScreenWidth() - 16);
        int height = std::max(440, GetScreenHeight() - 16);
        int left = (GetScreenWidth() - width) / 2, top = (GetScreenHeight() - height) / 2;
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), NIGHT);
        drawPanel({static_cast<float>(left), static_cast<float>(top), static_cast<float>(width), static_cast<float>(height)}, PAPER);
        DrawText("Forge Your Own Story", left + 16, top + 18, 20, INK);
        int contentTop = top + 54, contentBottom = top + height - 64;
        int maxScroll = std::max(0, 430 - (contentBottom - contentTop));
        if (CheckCollisionPointRec(GetMousePosition(),
                {static_cast<float>(left + 8), static_cast<float>(contentTop), static_cast<float>(width - 16),
                 static_cast<float>(contentBottom - contentTop)}))
            game.pageScroll = clampValue(game.pageScroll - static_cast<int>(GetMouseWheelMove() * 42), 0, maxScroll);
        BeginScissorMode(left + 8, contentTop, width - 16, contentBottom - contentTop);
        int y = contentTop - game.pageScroll;
        DrawText("NAME", left + 12, y, 14, MUTED);
        Rectangle nameBox = {static_cast<float>(left + 12), static_cast<float>(y + 20),
                             static_cast<float>(width - 24), 46.0f};
        drawPanel(nameBox, PAPER_INSET);
        DrawText(game.name.c_str(), left + 24, y + 30, 19, INK);
        if (CheckCollisionPointRec(GetMousePosition(), nameBox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            game.editingName = true;
        DrawText("STATS  /  ALL BEGIN AT 10", left + 12, y + 78, 15, EMBER);
        DrawText(TextFormat("%i points left", game.pointsRemaining), left + width - 120, y + 78, 13, INK);
        for (int i = 0; i < 6; ++i)
        {
            int rowY = y + 102 + i * 42;
            DrawText(STAT_NAMES[i], left + 16, rowY + 12, 16, INK);
            if (drawButton({static_cast<float>(left + width - 140), static_cast<float>(rowY), 44.0f, 40.0f}, "-", false,
                           game.attributes[i] > 10))
            {
                --game.attributes[i];
                ++game.pointsRemaining;
            }
            DrawText(TextFormat("%i", game.attributes[i]), left + width - 82, rowY + 10, 17, INK);
            if (drawButton({static_cast<float>(left + width - 56), static_cast<float>(rowY), 44.0f, 40.0f}, "+", false,
                           game.pointsRemaining > 0 && game.attributes[i] < 15))
            {
                ++game.attributes[i];
                --game.pointsRemaining;
            }
        }
        int kitY = y + 366;
        DrawText("BACKGROUND", left + 12, kitY, 15, EMBER);
        int kitWidth = (width - 36) / 3;
        for (int i = 0; i < BACKGROUND_COUNT; ++i)
        {
            Rectangle kit = {static_cast<float>(left + 12 + i * kitWidth), static_cast<float>(kitY + 24),
                             static_cast<float>(kitWidth - 6), 50.0f};
            if (drawButton(kit, BACKGROUNDS[i].name, game.origin == i))
                game.origin = i;
        }
        EndScissorMode();
        if (drawButton({static_cast<float>(left + 12), static_cast<float>(top + height - 58),
                        static_cast<float>(width - 24), 48.0f}, "Begin the Journey", true,
                       game.pointsRemaining == 0))
        {
            game.editingName = false;
            if (game.name.empty()) game.name = "Wayfarer";
            beginGame();
        }
        if (game.editingName)
        {
            if (IsKeyPressed(KEY_BACKSPACE) && !game.name.empty()) game.name.pop_back();
            int key = GetCharPressed();
            while (key > 0)
            {
                if (key >= 32 && key <= 126 && game.name.size() < 22) game.name += static_cast<char>(key);
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_ENTER)) game.editingName = false;
        }
        return;
    }
    int left, top;
    drawFrame("Forge Your Own Story", 1040, 760, left, top);
    int x = left + 42;
    int y = top + 104;
    DrawText("WHAT DO THEY CALL YOU?", x, y, 15, MUTED);
    Rectangle nameBox = {static_cast<float>(x), static_cast<float>(y + 25), 420.0f, 46.0f};
    drawPanel(nameBox, PAPER_INSET);
    DrawText(game.name.c_str(), x + 12, y + 37, 22, INK);
    if (game.editingName) DrawText("|", x + 15 + MeasureText(game.name.c_str(), 22), y + 37, 22, EMBER);
    DrawText("Click the name field, then type. Backspace edits.", x, y + 79, 16, MUTED);
    if (CheckCollisionPointRec(GetMousePosition(), nameBox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        game.editingName = true;

    DrawText("YOUR ATTRIBUTES", x, y + 107, 18, EMBER);
    DrawText(TextFormat("Points left to spend: %i", game.pointsRemaining), x + 660, y + 109, 17, INK);
    DrawText("Every stat begins at 10. Spend your 5 points where you choose.", x, y + 133, 16, MUTED);
    for (int i = 0; i < 6; ++i)
    {
        int rowY = y + 164 + i * 44;
        DrawText(STAT_NAMES[i], x + 6, rowY + 3, 19, INK);
        DrawText(STAT_GUIDES[i], x + 74, rowY + 5, 16, MUTED);
        if (drawButton({static_cast<float>(x + 242), static_cast<float>(rowY - 1), 38.0f, 34.0f}, "-", false,
                       game.attributes[i] > 10))
        {
            --game.attributes[i];
            ++game.pointsRemaining;
        }
        DrawText(TextFormat("%i", game.attributes[i]), x + 299, rowY + 4, 19, INK);
        if (drawButton({static_cast<float>(x + 334), static_cast<float>(rowY - 1), 38.0f, 34.0f}, "+", false,
                       game.pointsRemaining > 0 && game.attributes[i] < 15))
        {
            ++game.attributes[i];
            --game.pointsRemaining;
        }
    }

    int kitY = y + 443;
    DrawText("CHOOSE YOUR BACKGROUND", x, kitY, 18, EMBER);
    for (int i = 0; i < BACKGROUND_COUNT; ++i)
    {
        int cardX = x + i * 316;
        Rectangle card = {static_cast<float>(cardX), static_cast<float>(kitY + 30), 300.0f, 82.0f};
        drawPanel(card, game.origin == i ? PAPER_INSET : PAPER_RAISED);
        DrawRectangle(static_cast<int>(card.x), static_cast<int>(card.y), 4, static_cast<int>(card.height), game.origin == i ? EMBER : BORDER);
        DrawText(BACKGROUNDS[i].name, static_cast<int>(card.x + 15), static_cast<int>(card.y + 12), 19, INK);
        drawWrapped(BACKGROUNDS[i].description, static_cast<int>(card.x + 15), static_cast<int>(card.y + 44), 270, 16, MUTED, 3);
        if (CheckCollisionPointRec(GetMousePosition(), card) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) game.origin = i;
    }
    DrawText("No past is chosen for you. Start with your chosen strengths and kit.", x, top + 690, 16, MUTED);
    if (drawButton({static_cast<float>(left + 42), static_cast<float>(top + 704), 250.0f, 42.0f}, "Begin the Journey", true,
                   game.pointsRemaining == 0))
    {
        game.editingName = false;
        if (game.name.empty()) game.name = "Wayfarer";
        beginGame();
    }
    if (drawButton({static_cast<float>(left + 308), static_cast<float>(top + 704), 150.0f, 42.0f}, "Back"))
    {
        game.editingName = false;
        game.screen = TITLE;
    }
    if (game.editingName)
    {
        if (IsKeyPressed(KEY_BACKSPACE) && !game.name.empty()) game.name.pop_back();
        int key = GetCharPressed();
        while (key > 0)
        {
            if (key >= 32 && key <= 126 && game.name.size() < 22) game.name += static_cast<char>(key);
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ENTER)) game.editingName = false;
    }
}

void drawHeader(int left, int top, int width)
{
    const char *times[] = {"6 AM", "NOON", "6 PM", "MIDNIGHT"};
    DrawText("WAYWORN", left + 34, top + 23, 18, EMBER);
    DrawText(game.name.c_str(), left + 34, top + 49, 23, INK);
    std::string place = std::string("DAY ") + std::to_string(game.day) + "  /  " + times[game.timeOfDay] +
                        "  /  " + (settlementAt(game.x, game.y) ? settlementAt(game.x, game.y)->name : terrainName(game.map[game.y][game.x]));
    DrawText(place.c_str(), left + 280, top + 28, 16, MUTED);
    drawBar("HEALTH", game.hp, game.maxHp, left + 280, top + 54, 150, BLOOD);
    drawBar("HUNGER", game.hunger, 100, left + 570, top + 25, 110, EMBER);
    drawBar("FATIGUE", game.fatigue, 100, left + 570, top + 48, 110, MUTED);
    std::string conditions = std::string(SEASON_NAMES[game.season]) + " / " + WEATHER_NAMES[game.weather] +
                             " / cold " + std::to_string(game.exposure) +
                             " / bleed " + std::to_string(game.bleeding) +
                             " / sick " + std::to_string(game.infection) +
                             " / AC " + std::to_string(game.playerAC);
    DrawText(conditions.c_str(), left + 280, top + 88, 12, MUTED);
    DrawText(skillSummary().c_str(), left + 820, top + 88, 11, EMBER);
    std::string gold = std::to_string(game.gold) + "g";
    DrawText(gold.c_str(), left + width - 88, top + 33, 21, EMBER);
    if (!game.saveError.empty()) DrawText("AUTOSAVE ERROR", left + width - 180, top + 75, 13, BLOOD);
    DrawLine(left + 24, top + 104, left + width - 24, top + 104, BORDER);
}

void drawPageTabs(int left, int top, int width, bool canChangePage)
{
    const char *labels[] = {"MAIN", "ITEMS", "QUESTS", "MAP"};
    const int count = sizeof(labels) / sizeof(labels[0]);
    int gap = 8;
    int btnWidth = (width - 76 - gap * (count - 1)) / count;
    for (int i = 0; i < count; ++i)
    {
        Rectangle r = {static_cast<float>(left + 38 + i * (btnWidth + gap)), static_cast<float>(top + 116),
                       static_cast<float>(btnWidth), 38.0f};
        bool enabled = canChangePage;
        if (drawButton(r, labels[i], game.page == static_cast<Page>(i), enabled))
        {
            Page nextPage = static_cast<Page>(i == 1 ? ITEM_LEDGER : i);
            if (nextPage != game.page)
            {
                game.pageScroll = 0;
                pageTransitionTimer = 0.22f;
            }
            game.page = nextPage;
        }
    }
}

void drawRoadPage(int left, int top, int width)
{
    bool compactConversation = game.activity == TALKING && game.enemyIndex < 0;
    Rectangle scene = {static_cast<float>(left + 34), static_cast<float>(top + 166),
                       static_cast<float>(width - 68), compactConversation ? 100.0f : 178.0f};
    drawPanel(scene);
    if (game.enemyIndex >= 0)
    {
        const Creature &creature = CREATURES[game.enemyIndex];
        DrawText("A ROAD ENCOUNTER", left + 54, top + 183, 15, BLOOD);
        DrawText(creature.name, left + 54, top + 207, 27, INK);
        DrawText(creature.temper, left + width - 210, top + 213, 16, EMBER);
        drawWrapped(creature.personality, left + 54, top + 247, width - 118, 18, MUTED);
        drawWrapped(std::string("Ability: ") + creature.ability, left + 54, top + 278, width - 118, 15, BLOOD, 2);
        std::string combatWarning = std::string("Warning: ") + creature.warning +
                    (game.enemyTelegraph ? " It is preparing its special move; guard or break away." : "");
        if (game.proneTurns > 0) combatWarning += " You are prone.";
        if (game.stunnedTurns > 0) combatWarning += " You are stunned.";
        if (game.frightenedTurns > 0) combatWarning += " Fear clouds your aim.";
        drawWrapped(combatWarning,
                    left + 54, top + 302, width - 118, 14, BLOOD, 2);
        DrawText(TextFormat("CREATURE  %i / %i   YOUR AC  %i", std::max(0, game.enemyHp), creature.hp, game.playerAC),
                 left + 54, top + 330, 15, MUTED);
        Rectangle bar = {static_cast<float>(left + 240), static_cast<float>(top + 332), static_cast<float>(width - 310), 12.0f};
        DrawRectangleRec(bar, PAPER_INSET);
        DrawRectangleRec({bar.x, bar.y, bar.width * clampValue(game.enemyHp, 0, creature.hp) / creature.hp, bar.height}, BLOOD);
        DrawRectangleLinesEx(bar, 1.0f, BORDER);
    }
    else if (game.activity == EVENT_CHOICE && game.pendingEvent >= 0)
    {
        const RoadEvent &event = ROAD_EVENTS[game.pendingEvent];
        DrawText("A CHOICE ON THE ROAD", left + 54, top + 183, 15, EMBER);
        DrawText(event.title, left + 54, top + 207, 27, INK);
        drawWrapped(event.description, left + 54, top + 251, width - 116, 19, MUTED, 6);
    }
    else if (game.activity == EVENT_CHOICE && game.pendingEvent == -2)
    {
        DrawText("A GRAVE FROM ANOTHER ROAD", left + 54, top + 183, 15, BLOOD);
        DrawText(game.activeFallen >= 0 ? game.fallenName[game.activeFallen].c_str() : "A fallen wayfarer",
                 left + 54, top + 207, 27, INK);
        drawWrapped(game.activityText, left + 54, top + 251, width - 116, 19, MUTED, 6);
    }
    else if (game.activity != FREE_ROAM)
    {
        const char *heading = game.activity == IN_TAVERN ? "INSIDE THE TAVERN" :
                              game.activity == TALKING ? "A CONVERSATION" : "SEARCHING THE AREA";
        if (game.activity == TRADING) heading = "SETTLEMENT ARMORY";
        DrawText(heading, left + 54, top + 183, 15, EMBER);
        drawWrapped(game.activityText, left + 54, top + (compactConversation ? 209 : 218),
                    width - 116, 18, INK,
                    compactConversation ? 3 : 7);
        if (!compactConversation)
            DrawText("You are occupied here; finish before opening another page.", left + 54, top + 314, 15, MUTED);
    }
    else
    {
        const Settlement *place = settlementAt(game.x, game.y);
        DrawText(place ? place->kind : "THE OPEN ROAD", left + 54, top + 183, 15, EMBER);
        DrawText(place ? place->name : "The Marches", left + 54, top + 207, 27, INK);
        std::string sceneText = place
            ? "A rough settlement stands against the weather. The locals go about their day; there is time to rest or ask around."
            : std::string("You are crossing ") + terrainName(game.map[game.y][game.x]) +
              ". Hunger rises with every mile. The country ahead is yours to uncover.";
        sceneText += " " + politicsAt(game.x, game.y);
        if (game.roamingCreature >= 0)
            sceneText += " " + std::string(CREATURES[game.roamingCreature].name) + " is nearby: " +
                         CREATURES[game.roamingCreature].warning;
        if (game.wanderingNpc >= 0)
            sceneText += " " + std::string(NPCS[game.wanderingNpc].name) + ", a traveling " +
                         NPCS[game.wanderingNpc].role + ", is " +
                         WANDERING_NPC_ACTIVITIES[game.wanderingNpcActivity] + ".";
        if (game.socialSceneKind != SOCIAL_NONE)
            sceneText = game.socialSceneDescription;
        drawWrapped(sceneText, left + 54, top + 251, width - 112, 19, MUTED, 6);
    }

    if (!compactConversation || game.pageScroll == 0)
        DrawText(game.enemyIndex >= 0 ? "YOUR CHOICES" : "WHAT WILL YOU DO?",
                 left + 36, top + (compactConversation ? 273 : 361), 16, EMBER);
    int buttonY = top + (compactConversation ? 299 : 389) - game.pageScroll;
    const float choiceWidth = 478.0f;
    const float choiceHeight = 50.0f;
    const float choiceGap = 8.0f;
    if (game.activity == TRADING && game.enemyIndex < 0)
    {
        int row = 0;
        for (int i = 0; i < ITEM_COUNT; ++i)
        {
            if (!isEquipment(i)) continue;
            int y = top + 358 + row * 62 - game.pageScroll;
            Rectangle card = {static_cast<float>(left + 36), static_cast<float>(y),
                              choiceWidth, 56.0f};
            bool visible = y >= top + 350 && y + 56 <= top + 705;
            if (visible)
            {
                drawPanel(card, row % 2 == 0 ? PAPER_RAISED : PAPER);
                std::string title = std::string(equipmentSlotName(ITEMS[i].slot)) + ": " + ITEMS[i].name;
                DrawText(title.c_str(), left + 48, y + 5, 14, INK);
                std::string details = std::string(ITEMS[i].description) + "  |  +" +
                    std::to_string(ITEMS[i].amount) + " AC  |  " + ITEMS[i].pros + " / " + ITEMS[i].cons;
                DrawText(details.c_str(), left + 48, y + 26, 11, MUTED);
            }
            Rectangle buy = {static_cast<float>(left + 414), static_cast<float>(y + 6), 92.0f, 42.0f};
            int price = marketPrice(i);
            bool affordable = game.gold >= price;
            std::string label = itemQuantity(i) > 0 ? "OWNED" :
                std::to_string(price) + "g";
            if (drawButton(buy, label.c_str(), false,
                           visible && affordable && itemQuantity(i) == 0))
                purchaseEquipment(i);
            ++row;
        }
    }
    else if (game.enemyIndex >= 0)
    {
        float smallWidth = (choiceWidth - choiceGap) / 2;
        const auto combatChoice = [&](int column, int row, const char *label, const char *description, bool enabled = true) {
            return drawChoice({static_cast<float>(left + 36 + column * (smallWidth + choiceGap)),
                               static_cast<float>(buttonY + row * (choiceHeight + choiceGap)),
                               smallWidth, choiceHeight}, label, description, enabled);
        };
        if (combatChoice(0, 0, "Strike", "Measured attack."))
            playerAttack(false);
        if (combatChoice(1, 0, "Heavy blow", "Harder to land; greater damage."))
            playerAttack(true);
        if (combatChoice(0, 1, "Guard", "Brace against the next blow."))
        {
            if (!loseTurnIfStunned())
            {
                game.guarding = true;
                enemyTurn();
            }
        }
        bool hasCombatSupply = itemQuantity(7) > 0 || itemQuantity(5) > 0 || itemQuantity(6) > 0;
        if (combatChoice(1, 1, "Use a supply", "Use food or medicine.", hasCombatSupply))
        {
            if (!loseTurnIfStunned())
            {
                useItem(itemQuantity(7) ? 7 : itemQuantity(5) ? 5 : 6);
                enemyTurn();
            }
        }
        if (combatChoice(0, 2, "Flee", "DEX check DC 14."))
        {
            if (loseTurnIfStunned()) {}
            else if (randomBetween(1, 20) + (game.attributes[1] - 10) / 2 + game.skills[4] -
                     game.injuries[2] >= 14)
            {
                practiceSkill(4);
                addJournal("You break away and leave the creature behind.");
                game.enemyIndex = -1;
                advanceTime();
                tickNeeds(true);
            }
            else
            {
                addJournal("The creature cuts off your escape.");
                enemyTurn();
            }
        }
        if (combatChoice(1, 2, "Bargain", "CHA check DC 16-19."))
            tryBargain();
        if (combatChoice(0, 3, "Use terrain", "WIS/INT check to blunt a blow."))
            useTerrain();
        if (combatChoice(1, 3, "Sneak away", "DEX check DC 16."))
            trySneakAway();
    }
    else if (game.activity == EVENT_CHOICE)
    {
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY), choiceWidth, choiceHeight},
                       game.pendingEvent == -2 ? "Recover what you can" : "Help / accept the risk",
                       game.pendingEvent == -2 ? "Take the previous character's surviving equipment." : "Spend a supply or attempt a DC 14-18 check."))
            resolvePendingEvent(0);
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + choiceHeight + choiceGap), choiceWidth, choiceHeight},
                       "Bargain / ask for terms", "A Charisma check may improve the deal; failure can create a debt."))
            resolvePendingEvent(1);
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 2 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Decline and move on", "Keep your resources. Some people will remember."))
            resolvePendingEvent(2);
    }
    else if (game.activity == IN_TAVERN)
    {
        int mealPrice = marketPrice(6);
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY), choiceWidth, choiceHeight},
                       "Rent a bed  /  3 gold", "Rest safely and recover health and fatigue.", game.gold >= 3))
        {
            game.gold -= 3;
            waitAndRest();
            game.activityText = "You wake after a few hours of safe sleep. The common room smells of smoke and stew.";
        }
        std::string mealLabel = "Buy a meal  /  " + std::to_string(mealPrice) + " gold";
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + choiceHeight + choiceGap), choiceWidth, choiceHeight},
                       mealLabel.c_str(), "Restore hunger and a little health.", game.gold >= mealPrice))
        {
            game.gold -= mealPrice;
            game.hunger = clampValue(game.hunger + 40, 0, 100);
            game.hp = clampValue(game.hp + 2, 0, game.maxHp);
            game.activityText = "Hot stew, dark bread, and a full belly. The innkeeper clears away your bowl.";
            addJournal("You buy a warm meal at the tavern.");
        }
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 2 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Leave the tavern", "Return to the settlement."))
            endActivity();
    }
    else if (game.activity == TALKING)
    {
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY), choiceWidth, choiceHeight},
                       "Ask about the road", "Hear a warning about a creature in the Marches."))
            askAboutRoad();
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + choiceHeight + choiceGap), choiceWidth, choiceHeight},
                       "Ask for their story", "A DC 15 conversation check may earn lasting trust."))
            shareAnNPCStory();
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 2 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Offer a ration", "Share a meal and earn a favor.", itemQuantity(5) > 0))
            offerNPCHelp();
        if (game.questOffer.active)
        {
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 3 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Accept task", "Add this objective and its promised reward to your quest book."))
                acceptQuestOffer();
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 4 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Decline task", "Leave the offer open for another day."))
                declineQuestOffer();
        }
        else if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + 3 * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Ask for a task", "A local may offer work with a clear objective and reward.", hasQuestRoom()))
            offerQuest();
    }
    else if (game.activity == SEARCHING)
    {
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY), choiceWidth, choiceHeight},
                       "Finish searching", "Return to the area when you are ready."))
            endActivity();
    }
    else
    {
        const Settlement *place = settlementAt(game.x, game.y);
        int row = 0;
        if (hasDeliverableQuestAt(game.x, game.y) &&
            drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)),
                        choiceWidth, choiceHeight}, "Deliver quest supplies",
                       "Hand over gathered items at this marked settlement and claim your reward."))
            deliverQuestItems();
        if (game.socialSceneKind == SOCIAL_NPC_EXCHANGE)
        {
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Mediate the dispute", "CHA check DC 14; success earns trust and a favor."))
                resolveSocialScene(0);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Listen to both", "Hear their exchange and keep a useful rumor."))
                resolveSocialScene(1);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Leave them to it", "Walk on without taking sides."))
                resolveSocialScene(2);
        }
        else if (game.socialSceneKind == SOCIAL_BANDIT_THREAT)
        {
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Confront the outlaw", "Starts combat; the outlaw's warning is shown above."))
                resolveSocialScene(0);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Create a diversion", "CHA check DC 14; failure draws the outlaw to you."))
                resolveSocialScene(1);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Stay out of it", "Leave without risking a fight."))
                resolveSocialScene(2);
        }
        else if (game.socialSceneKind == SOCIAL_CREATURE_THREAT)
        {
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Drive it off", "WIS check DC 14; failure draws the creature to you."))
                resolveSocialScene(0);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Draw it away", "Starts combat with the warned creature."))
                resolveSocialScene(1);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Give them room", "Let the traveler handle the moment without you."))
                resolveSocialScene(2);
        }
        if (place && std::string(place->kind) != "CAMP")
        {
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           "Enter the tavern", "Find a meal, a bed, and a little shelter."))
                beginActivity(IN_TAVERN, "A low fire warms the common room. The keeper looks up from the counter.");
        }
        if (place && std::string(place->kind) != "CAMP" && monsterLootValue() > 0)
        {
            std::string sellLabel = "Sell monster trophies  /  " +
                                    std::to_string(monsterLootValue()) + " gold";
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           sellLabel.c_str(), "Sell carried creature trophies to the local trader."))
                sellMonsterLoot();
        }
        if (place || game.wanderingNpc >= 0)
        {
            std::string talkLabel = game.wanderingNpc >= 0
                ? "Talk to " + std::string(NPCS[game.wanderingNpc].name) : "Talk to someone";
            std::string talkDescription = game.wanderingNpc >= 0
                ? "Ask what brought this traveler here; their story may uncover a larger thread."
                : "Ask for local news, help, or a tale worth remembering.";
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                           talkLabel.c_str(), talkDescription.c_str()))
                beginActivity(TALKING, "A local pauses their work and gives you their attention.");
        }
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Look for something", "Search nearby for useful supplies."))
            lookAround();
        if (!place && drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                                 "Forage", "Search the wilds for food or medicine."))
            forage();
        if (place && drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)),
                                 choiceWidth, choiceHeight}, "Browse the armory",
                                "Compare weapons and protective gear for sale."))
            beginActivity(TRADING, "The armorer lays out steel, leather, and road-worn kit. Your gold: " +
                          std::to_string(game.gold) + ".");
        if (game.roamingCreature >= 0)
        {
            int creature = game.roamingCreature;
            std::string approach = "Approach the " + std::string(CREATURES[creature].name);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)),
                            choiceWidth, choiceHeight}, approach.c_str(),
                           "The warning is plain. Face it now, while it is in sight."))
                startEncounter(creature);
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)),
                            choiceWidth, choiceHeight}, "Let it pass",
                           "Keep your distance; it may vanish before your next turn."))
            {
                addJournal(std::string("You leave the ") + CREATURES[creature].name + " to its own country.");
                game.roamingCreature = -1;
                game.roamingCreatureTurns = 0;
                advanceTime();
                tickNeeds(false);
            }
        }
        if (game.wanderingNpc >= 0 && !game.npcKnown[game.wanderingNpc] && !place)
        {
            std::string leaveLabel = "Leave " + std::string(NPCS[game.wanderingNpc].name) + " be";
            if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row++ * (choiceHeight + choiceGap)),
                            choiceWidth, choiceHeight}, leaveLabel.c_str(),
                           "They have their own road to walk. A chance meeting need not become a promise."))
            {
                addJournal(std::string(NPCS[game.wanderingNpc].name) + " gathers their things and continues onward.");
                game.wanderingNpc = -1;
                game.wanderingNpcTurns = 0;
                game.wanderingNpcActivity = -1;
                advanceTime();
                tickNeeds(false);
            }
        }
        if (drawChoice({static_cast<float>(left + 36), static_cast<float>(buttonY + row * (choiceHeight + choiceGap)), choiceWidth, choiceHeight},
                       "Rest", "Take a short rest and recover a little health."))
            waitAndRest();
    }

    Rectangle notes = {static_cast<float>(left + 536), static_cast<float>(top + 389), static_cast<float>(width - 570), 285.0f};
    drawPanel(notes, PAPER_RAISED);
    DrawText(game.enemyIndex >= 0 ? "THE FIELD CHRONICLE  /  COMBAT" : "THE FIELD CHRONICLE",
             static_cast<int>(notes.x + 16), static_cast<int>(notes.y + 12), 16, EMBER);
    DrawLine(static_cast<int>(notes.x + 16), static_cast<int>(notes.y + 34),
             static_cast<int>(notes.x + notes.width - 16), static_cast<int>(notes.y + 34), BORDER);
    DrawText(TextFormat("DAY %i  /  %s", game.day, SEASON_NAMES[game.season]),
             static_cast<int>(notes.x + 16), static_cast<int>(notes.y + 39), 11, MUTED);
    int notesLimit = std::min(6, static_cast<int>(game.journal.size()));
    for (int i = 0; i < notesLimit; ++i)
    {
        drawWrapped("— " + game.journal[i], static_cast<int>(notes.x + 16),
                    static_cast<int>(notes.y + 58 + i * 34),
                    static_cast<int>(notes.width - 32), 14, i == 0 ? INK : MUTED, 2);
    }
}

void drawItemsPage(int left, int top, int width)
{
    DrawText("YOUR LEDGER  /  ITEMS AND SUPPLIES", left + 38, top + 171, 17, MUTED);
    DrawText("Equip, consume, craft, and watch the weight and age of your supplies.", left + 38, top + 197, 16, MUTED);
    float weight = 0.0f;
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
        weight += ITEMS[game.inventory[i].item].weight * game.inventory[i].quantity;
    float capacity = 18.0f + std::max(0, game.attributes[0] - 10) * 1.5f;
    DrawText(TextFormat("PACK  %.1f / %.1f lb", weight, capacity), left + 680, top + 171, 15,
             weight > capacity * 0.85f ? BLOOD : EMBER);
    if (drawButton({static_cast<float>(left + width - 204), static_cast<float>(top + 190), 164.0f, 34.0f},
                   "Craft bandage", false, itemQuantity(11) > 0 && itemQuantity(12) > 0))
        craftBandage();
    int rowTop = top + 235;
    int visibleRow = 0;
    if (ownedItemCount() == 0)
        DrawText("Your pack is empty. Supplies found on the road will be recorded here.",
                 left + 48, rowTop + 8, 17, MUTED);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        if (itemQuantity(i) <= 0) continue;
        int rowY = rowTop + visibleRow * 80 - game.pageScroll;
        Rectangle row = {static_cast<float>(left + 34), static_cast<float>(rowY), static_cast<float>(width - 68), 74.0f};
        drawPanel(row, visibleRow % 2 == 0 ? PAPER_RAISED : PAPER);
        DrawText(ITEMS[i].name, left + 48, rowY + 4, 16, INK);
        std::string detail = std::string(ITEMS[i].kind) + "  /  " + ITEMS[i].description;
        DrawText(detail.c_str(), left + 48, rowY + 25, 13, MUTED);
        if (isEquipment(i))
        {
            std::string tradeoffs = std::string("Pros: ") + ITEMS[i].pros + "  |  Cons: " + ITEMS[i].cons;
            DrawText(equipmentStatLine(i).c_str(), left + 48, rowY + 43, 11, MUTED);
            DrawText(tradeoffs.c_str(), left + 48, rowY + 57, 10, EMBER);
        }
        std::string quantity = "x" + std::to_string(itemQuantity(i));
        DrawText(quantity.c_str(), left + width - 182, rowY + 7, 15, EMBER);
        if (ITEMS[i].shelfLife > 0 && itemQuantity(i) > 0)
            DrawText(TextFormat("%id", std::max(0, ITEMS[i].shelfLife - game.inventoryAge[i])),
                     left + width - 244, rowY + 7, 14, MUTED);
        bool consumable = !isEquipment(i);
        bool equipped = isEquipped(i);
        const char *action = equipped ? "EQUIPPED" : (consumable ? "USE" : "EQUIP");
        Rectangle actionButton = {static_cast<float>(left + width - 140),
                                  static_cast<float>(rowY + 1), 92.0f, 34.0f};
        bool inView = actionButton.y >= top + 155 && actionButton.y + actionButton.height <= top + 695;
        if (drawButton(actionButton, action, equipped, inView && !equipped && itemQuantity(i) > 0))
        {
            if (consumable) useItem(i);
            else equipItem(i);
        }
        ++visibleRow;
    }
}

void drawQuestsPage(int left, int top, int width)
{
    DrawText("YOUR QUEST BOOK  /  TASKS, PROMISES, AND REWARDS", left + 38, top + 171, 17, MUTED);
    if (game.questCount == 0)
    {
        drawWrapped("No tasks are written yet. Speak with people in settlements and ask if they need help.",
                    left + 44, top + 211, width - 88, 18, MUTED, 4);
        return;
    }
    int rowTop = top + 203 - game.pageScroll;
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        int rowY = rowTop + i * 88;
        Rectangle card = {static_cast<float>(left + 34), static_cast<float>(rowY),
                          static_cast<float>(width - 68), 78.0f};
        drawPanel(card, quest.complete ? PAPER_INSET : PAPER_RAISED);
        DrawRectangle(static_cast<int>(card.x), static_cast<int>(card.y), 4,
                      static_cast<int>(card.height), quest.complete ? GOOD : EMBER);
        DrawText(quest.title.c_str(), left + 50, rowY + 7, 18, INK);
        DrawText(quest.complete ? "FULFILLED" : TextFormat("%s  %i / %i",
                 questObjective(quest).c_str(), quest.progress, quest.goal),
                 left + 50, rowY + 32, 15, quest.complete ? GOOD : EMBER);
        drawWrapped(quest.description, left + 50, rowY + 55, width - 330, 13, MUTED, 2);
        DrawText(TextFormat("REWARD  %ig + %ix", quest.rewardGold, quest.rewardQuantity),
                 left + width - 265, rowY + 14, 14, EMBER);
        DrawText(ITEMS[quest.rewardItem].name, left + width - 265, rowY + 39, 13, MUTED);
        if (!quest.complete)
            DrawText(TextFormat("DEST  %i,%i  /  red map dot", quest.destinationX + 1, quest.destinationY + 1),
                     left + width - 265, rowY + 58, 11, EMBER);
    }
}

Color terrainColor(int terrain)
{
    switch (terrain)
    {
        case 1: return {101, 120, 73, 255};
        case 2: return {137, 127, 114, 255};
        case 3: return {103, 133, 126, 255};
        case 4: return {170, 139, 86, 255};
        case 5: return {173, 152, 92, 255};
        case 6: return {105, 139, 155, 255};
        case 7: return {127, 107, 95, 255};
        default: return {203, 190, 151, 255};
    }
}

void drawMapPage(int left, int top, int width)
{
    DrawText("CURRENT WORLD  /  MAP AND DISPATCHES", left + 38, top + 166, 15, EMBER);
    drawWrapped(game.worldEventHeadline, left + 38, top + 185, width - 76, 14, MUTED, 2);
    DrawText("Red dot: active quest destination. Unmapped country is revealed by travel.",
             left + 38, top + 205, 13, MUTED);
    int cell = 29;
    int mapX = left + 38;
    int mapY = top + 224;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            Rectangle tile = {static_cast<float>(mapX + x * (cell + 3)), static_cast<float>(mapY + y * (cell + 3)),
                              static_cast<float>(cell), static_cast<float>(cell)};
            bool here = x == game.x && y == game.y;
            Color fill = game.seen[y][x] ? terrainColor(game.map[y][x]) : PAPER_INSET;
            DrawRectangleRec(tile, here ? EMBER : fill);
            DrawRectangleLinesEx(tile, 1.0f, BORDER);
            const Settlement *settlement = game.seen[y][x] ? settlementAt(x, y) : NULL;
            const char *symbol = !game.seen[y][x] ? "?" : here ? "@" :
                game.landmarks[y][x] == 4 ? "*" : game.discoveredPOI[y][x] ? "!" :
                settlement ? (std::string(settlement->kind) == "CITY" ? "C" : std::string(settlement->kind) == "CAMP" ? "c" : "T")
                : game.map[y][x] == 4 ? "=" : game.map[y][x] == 1 ? "f" :
                  game.map[y][x] == 2 ? "^" : game.map[y][x] == 3 ? "~" :
                  game.map[y][x] == 5 ? "+" : game.map[y][x] == 6 ? "o" :
                  game.map[y][x] == 7 ? "R" : ".";
            DrawText(symbol, static_cast<int>(tile.x + 10), static_cast<int>(tile.y + 8), 17, here ? PAPER : INK);
            drawQuestMapMarker(tile, x, y);
        }
    }
    int legendX = mapX + MAP_SIZE * (cell + 3) + 32;
    DrawText("MAP KEY", legendX, mapY, 17, EMBER);
    const char *legend[] = {"@ You", "C City  /  T town", "c Camp  /  = road", ". Plains  /  f forest",
                            "^ mountains  /  ~ marsh", "+ farmland  /  o coast", "R ruins", "! point of interest",
                            "* fallen wayfarer", "? unexplored"};
    for (int i = 0; i < 10; ++i) DrawText(legend[i], legendX, mapY + 30 + i * 25, 16, i == 0 ? EMBER : MUTED);

    DrawText("COMPASS", left + 675, top + 326, 17, EMBER);
    int centerX = left + 810;
    int centerY = top + 480;
    DrawCircle(centerX, centerY, 20.0f, PAPER_INSET);
    DrawCircleLines(centerX, centerY, 20.0f, BORDER);
    DrawText("MOVE", centerX - MeasureText("MOVE", 13) / 2, centerY - 7, 13, INK);

    struct Direction
    {
        const char *label;
        int dx;
        int dy;
        int offsetX;
        int offsetY;
    };
    const Direction directions[] = {
        {"NW", -1, -1, -76, -76}, {"N", 0, -1, 0, -108}, {"NE", 1, -1, 76, -76},
        {"W", -1, 0, -108, 0}, {"E", 1, 0, 108, 0},
        {"SW", -1, 1, -76, 76}, {"S", 0, 1, 0, 108}, {"SE", 1, 1, 76, 76}
    };
    for (int i = 0; i < 8; ++i)
    {
        Rectangle button = {
            static_cast<float>(centerX + directions[i].offsetX - 40),
            static_cast<float>(centerY + directions[i].offsetY - 19),
            80.0f,
            38.0f
        };
        if (drawButton(button, directions[i].label, false,
                       game.enemyIndex < 0 && game.activity == FREE_ROAM))
            travel(directions[i].dx, directions[i].dy);
    }
}

void drawMobileBar(const char *label, int value, int x, int y, int width, Color color)
{
    DrawText(label, x, y, 13, MUTED);
    Rectangle track = {static_cast<float>(x), static_cast<float>(y + 17), static_cast<float>(width), 9.0f};
    DrawRectangleRec(track, PAPER_INSET);
    DrawRectangleRec({track.x, track.y, track.width * clampValue(value, 0, 100) / 100.0f, track.height}, color);
    DrawRectangleLinesEx(track, 1.0f, BORDER);
}

bool drawMobileChoice(const char *label, const char *description, int x, int y, int width,
                      int contentTop, int contentBottom, bool enabled = true)
{
    Rectangle row = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), 58.0f};
    bool visible = row.y >= contentTop && row.y + row.height <= contentBottom;
    return drawChoice(row, label, description, enabled && visible);
}

void drawMobileMap(int left, int width, int contentTop, int contentBottom)
{
    int cell = std::max(8, std::min(14, (width - 32) / MAP_SIZE - 2));
    int stride = cell + 2;
    int mapX = left + (width - MAP_SIZE * stride) / 2;
    DrawText("NEWS / RED DOT: QUEST / ?: UNMAPPED", left + 18, contentTop + 4, 12, EMBER);
    drawWrapped(game.worldEventHeadline, left + 18, contentTop + 19, width - 36, 11, MUTED, 1);
    int mapY = contentTop + 48 - game.pageScroll;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            Rectangle tile = {static_cast<float>(mapX + x * stride),
                              static_cast<float>(mapY + y * stride),
                              static_cast<float>(cell), static_cast<float>(cell)};
            bool here = x == game.x && y == game.y;
            Color fill = game.seen[y][x] ? terrainColor(game.map[y][x]) : PAPER_INSET;
            DrawRectangleRec(tile, here ? EMBER : fill);
            DrawRectangleLinesEx(tile, 1.0f, BORDER);
            std::string symbol = !game.seen[y][x] ? "?" : here ? "@" :
                settlementAt(x, y) ? "T" : game.landmarks[y][x] == 4 ? "*" :
                (game.discoveredPOI[y][x] ? "!" : game.map[y][x] == 1 ? "f" :
                 game.map[y][x] == 2 ? "^" : game.map[y][x] == 3 ? "~" :
                 game.map[y][x] == 4 ? "=" : game.map[y][x] == 5 ? "+" :
                 game.map[y][x] == 6 ? "o" : game.map[y][x] == 7 ? "R" : ".");
            DrawText(symbol.c_str(), static_cast<int>(tile.x + cell / 2 - 3),
                     static_cast<int>(tile.y + 1), 10, here ? PAPER : INK);
            drawQuestMapMarker(tile, x, y);
        }
    }
    int centerX = left + width / 2;
    int centerY = mapY + MAP_SIZE * stride + 95;
    DrawCircle(centerX, centerY, 19.0f, PAPER_INSET);
    DrawCircleLines(centerX, centerY, 19.0f, BORDER);
    DrawText("MOVE", centerX - MeasureText("MOVE", 12) / 2, centerY - 6, 12, INK);
    struct Direction { const char *label; int dx; int dy; int ox; int oy; };
    const Direction dirs[] = {
        {"NW",-1,-1,-47,-47},{"N",0,-1,0,-67},{"NE",1,-1,47,-47},
        {"W",-1,0,-67,0},{"E",1,0,67,0},{"SW",-1,1,-47,47},
        {"S",0,1,0,67},{"SE",1,1,47,47}
    };
    for (int i = 0; i < 8; ++i)
    {
        Rectangle button = {static_cast<float>(centerX + dirs[i].ox - 27),
                            static_cast<float>(centerY + dirs[i].oy - 23), 54.0f, 46.0f};
        bool visible = button.y >= contentTop && button.y + button.height <= contentBottom;
        if (drawButton(button, dirs[i].label, false,
                       visible && game.enemyIndex < 0 && game.activity == FREE_ROAM))
            travel(dirs[i].dx, dirs[i].dy);
    }
}

void drawMobileGame()
{
    int width = std::max(260, GetScreenWidth() - 16);
    int height = std::max(440, GetScreenHeight() - 16);
    int left = (GetScreenWidth() - width) / 2;
    int top = (GetScreenHeight() - height) / 2;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), NIGHT);
    drawPanel({static_cast<float>(left), static_cast<float>(top), static_cast<float>(width), static_cast<float>(height)}, PAPER);
    DrawRectangle(left + 10, top + 10, width - 20, 2, BORDER);
    DrawText("WAYWORN", left + 16, top + 14, 13, EMBER);
    DrawText(game.name.c_str(), left + 16, top + 28, 20, INK);
    const char *times[] = {"6 AM", "NOON", "6 PM", "MIDNIGHT"};
    std::string date = "DAY " + std::to_string(game.day) + "  /  " + times[game.timeOfDay];
    DrawText(date.c_str(), left + 16, top + 51, 13, MUTED);
    int barWidth = (width - 48) / 3;
    drawMobileBar("HP", game.hp * 100 / std::max(1, game.maxHp), left + 16, top + 74, barWidth, BLOOD);
    drawMobileBar("FOOD", game.hunger, left + 24 + barWidth, top + 74, barWidth, EMBER);
    drawMobileBar("REST", game.fatigue, left + 32 + 2 * barWidth, top + 74, barWidth, MUTED);
    std::string conditions = std::string(WEATHER_NAMES[game.weather]) + " " + SEASON_NAMES[game.season] +
        " / cold " + std::to_string(game.exposure) + " / bleed " + std::to_string(game.bleeding) +
        " / sick " + std::to_string(game.infection) + " / AC " + std::to_string(game.playerAC);
    DrawText(conditions.c_str(), left + 16, top + 103, 11, MUTED);
    DrawText(skillSummary().c_str(), left + 16, top + 116, 11, EMBER);

    bool canChangePage = game.enemyIndex < 0 && game.activity == FREE_ROAM;
    const char *labels[] = {"MAIN", "ITEMS", "QUESTS", "MAP"};
    int gap = 5, tabWidth = (width - 32 - gap * 3) / 4;
    for (int i = 0; i < 4; ++i)
    {
        Rectangle tab = {static_cast<float>(left + 16 + i * (tabWidth + gap)),
                         static_cast<float>(top + 134), static_cast<float>(tabWidth), 48.0f};
        if (drawButton(tab, labels[i], game.page == static_cast<Page>(i), canChangePage))
        {
            if (game.page != static_cast<Page>(i)) pageTransitionTimer = 0.22f;
            game.page = static_cast<Page>(i == 1 ? ITEM_LEDGER : i);
            game.pageScroll = 0;
        }
    }

    int contentTop = top + 190;
    int contentBottom = top + height - 48;
    int contentHeight = contentBottom - contentTop;
    int count = game.page == ITEM_LEDGER ? ownedItemCount() :
                game.page == QUEST_LOG ? game.questCount : 0;
    int rowHeight = game.page == ITEM_LEDGER ? 58 : 94;
    int maximumScroll = game.page == ITEM_LEDGER ? std::max(0, 56 + count * 96 - contentHeight) :
                        game.page == QUEST_LOG ? std::max(0, 8 + count * rowHeight - contentHeight) :
                        game.page == MAP ? std::max(0, 48 + MAP_SIZE *
                            (std::max(8, std::min(14, (width - 32) / MAP_SIZE - 2)) + 2) +
                            185 - contentHeight) : 0;
    if (game.page == ROAD)
    {
        const Settlement *place = settlementAt(game.x, game.y);
        if (game.activity == TRADING)
            maximumScroll = std::max(0, 126 + equipmentItemCount() * 90 - contentHeight);
        int choiceCount = game.enemyIndex >= 0 ? 8 :
                          game.activity == EVENT_CHOICE ? 3 :
                          game.activity == TALKING ? 4 + (game.questOffer.active ? 1 : 0) :
                          game.activity == TRADING ? 0 :
                          game.activity == IN_TAVERN ? 3 :
                          game.activity == SEARCHING ? 1 :
                          (place ? (std::string(place->kind) == "CAMP" ? 4 : 5 +
                                    (monsterLootValue() > 0 ? 1 : 0)) : 3) +
                              (!place && game.wanderingNpc >= 0 ? 2 : 0) +
                              (game.roamingCreature >= 0 ? 2 : 0);
        if (game.activity == FREE_ROAM && game.enemyIndex < 0)
        {
            choiceCount += hasDeliverableQuestAt(game.x, game.y) ? 1 : 0;
            choiceCount += game.socialSceneKind == SOCIAL_NONE ? 0 : 3;
        }
        if (game.activity != TRADING)
            maximumScroll = std::max(0, 126 + (choiceCount - 1) * 66 + 58 - contentHeight);
    }
    updateScrollInput({static_cast<float>(left + 8), static_cast<float>(contentTop),
                       static_cast<float>(width - 16), static_cast<float>(contentHeight)}, maximumScroll);

    BeginScissorMode(left + 8, contentTop, width - 16, contentHeight);
    if (game.page == ROAD)
    {
        Rectangle scene = {static_cast<float>(left + 12), static_cast<float>(contentTop + 4),
                           static_cast<float>(width - 24), 112.0f};
        drawPanel(scene);
        if (game.enemyIndex >= 0)
        {
            const Creature &creature = CREATURES[game.enemyIndex];
            DrawText(creature.name, left + 24, contentTop + 12, 20, BLOOD);
            std::string warning = creature.warning;
            if (game.enemyTelegraph) warning += " Guard or flee before its special move.";
            if (game.proneTurns > 0) warning += " Prone.";
            if (game.stunnedTurns > 0) warning += " Stunned.";
            if (game.frightenedTurns > 0) warning += " Frightened.";
            drawWrapped(warning, left + 24, contentTop + 40, width - 50, 14, MUTED, 2);
            DrawText(TextFormat("HP %i/%i  AC %i", game.enemyHp, creature.hp, game.playerAC),
                     left + 24, contentTop + 84, 14, INK);
        }
        else if (game.activity == EVENT_CHOICE)
        {
            const char *title = game.pendingEvent == -2 ? "A grave from another road" :
                                game.pendingEvent >= 0 ? ROAD_EVENTS[game.pendingEvent].title : "A choice";
            DrawText(title, left + 24, contentTop + 12, 19, EMBER);
            drawWrapped(game.activityText, left + 24, contentTop + 42, width - 50, 12, INK, 3);
        }
        else if (game.activity != FREE_ROAM)
        {
            DrawText(game.activity == TALKING && game.activeNpc >= 0 ? NPCS[game.activeNpc].name :
                     game.activity == IN_TAVERN ? "The tavern" :
                     game.activity == TRADING ? "The settlement armory" :
                     game.activity == SEARCHING ? "Searching" : "Occupied", left + 24, contentTop + 12, 19, EMBER);
            drawWrapped(game.activityText, left + 24, contentTop + 42, width - 50, 12, INK, 3);
        }
        else
        {
            const Settlement *place = settlementAt(game.x, game.y);
            DrawText(place ? place->name : terrainName(game.map[game.y][game.x]),
                     left + 24, contentTop + 12, 19, EMBER);
            std::string scene = game.socialSceneKind != SOCIAL_NONE ? game.socialSceneDescription :
                place ? "The locals go about their day. Choose how to spend the hour." :
                "Travel, search, forage, or make a small camp. The weather is " +
                std::string(WEATHER_NAMES[game.weather]) + ".";
            if (game.socialSceneKind == SOCIAL_NONE)
            {
                scene += " " + politicsAt(game.x, game.y);
                if (game.roamingCreature >= 0)
                    scene += " " + std::string(CREATURES[game.roamingCreature].name) +
                             " is close: " + CREATURES[game.roamingCreature].warning;
                if (game.wanderingNpc >= 0)
                    scene += " " + std::string(NPCS[game.wanderingNpc].name) + ", a traveling " +
                             NPCS[game.wanderingNpc].role + ", is " +
                             WANDERING_NPC_ACTIVITIES[game.wanderingNpcActivity] + ".";
            }
            drawWrapped(scene, left + 24, contentTop + 42, width - 50, 12, MUTED, 3);
        }

        int y = contentTop + 126 - game.pageScroll;
        int option = 0;
        const int x = left + 12, w = width - 24;
        const int gapY = 66;
        const auto choice = [&](int index, const char *label, const char *description, bool enabled = true) {
            return drawMobileChoice(label, description, x, y + index * gapY, w, contentTop, contentBottom, enabled);
        };
        if (game.activity == TRADING && game.enemyIndex < 0)
        {
            int row = 0;
            for (int i = 0; i < ITEM_COUNT; ++i)
            {
                if (!isEquipment(i)) continue;
                int rowY = contentTop + 126 + row * 90 - game.pageScroll;
                Rectangle card = {static_cast<float>(left + 12), static_cast<float>(rowY),
                                  static_cast<float>(width - 24), 86.0f};
                bool visible = rowY >= contentTop && rowY + 86 <= contentBottom;
                if (visible)
                {
                    drawPanel(card, row % 2 == 0 ? PAPER_RAISED : PAPER);
                    std::string title = std::string(equipmentSlotName(ITEMS[i].slot)) + ": " + ITEMS[i].name;
                    DrawText(title.c_str(), left + 22, rowY + 4, 14, INK);
                    drawWrapped(ITEMS[i].description, left + 22, rowY + 24, width - 116, 11, MUTED, 1);
                    std::string pros = std::string("Pros: ") + ITEMS[i].pros + "  /  Cons: " + ITEMS[i].cons;
                    drawWrapped(pros, left + 22, rowY + 42, width - 116, 10, EMBER, 1);
                    drawWrapped(equipmentStatLine(i), left + 22, rowY + 59, width - 116, 10, MUTED, 1);
                }
                Rectangle buy = {static_cast<float>(left + width - 88), static_cast<float>(rowY + 19), 68.0f, 48.0f};
                int price = marketPrice(i);
                std::string label = itemQuantity(i) > 0 ? "OWNED" :
                    std::to_string(price) + "g";
                if (drawButton(buy, label.c_str(), false,
                               visible && game.gold >= price && itemQuantity(i) == 0))
                    purchaseEquipment(i);
                ++row;
            }
        }
        else if (game.enemyIndex >= 0)
        {
            if (choice(option++, "Strike", "Measured attack against enemy defense.")) playerAttack(false);
            if (choice(option++, "Heavy blow", "Harder to land; greater damage.")) playerAttack(true);
            if (choice(option++, "Guard", "Halve the next landed blow."))
            {
                if (!loseTurnIfStunned()) { game.guarding = true; enemyTurn(); }
            }
            bool supply = itemQuantity(7) > 0 || itemQuantity(5) > 0 || itemQuantity(6) > 0;
            if (choice(option++, "Use a supply", "Use food or medicine; the enemy responds.", supply))
            {
                if (!loseTurnIfStunned())
                {
                    useItem(itemQuantity(7) ? 7 : itemQuantity(5) ? 5 : 6);
                    enemyTurn();
                }
            }
            if (choice(option++, "Flee", "DEX check DC 14; a failed escape draws an attack."))
            {
                if (loseTurnIfStunned()) {}
                else if (randomBetween(1, 20) + (game.attributes[1] - 10) / 2 + game.skills[4] -
                         game.injuries[2] >= 14)
                {
                    practiceSkill(4);
                    game.enemyIndex = -1;
                    addJournal("You break away before the creature can close.");
                    advanceTime();
                    tickNeeds(true);
                }
                else { addJournal("The creature cuts off your escape."); enemyTurn(); }
            }
            if (choice(option++, "Bargain", "CHA check DC 16-19; some creatures will listen."))
                tryBargain();
            if (choice(option++, "Use terrain", "WIS/INT check to blunt the coming blow."))
                useTerrain();
            if (choice(option++, "Sneak away", "DEX check DC 16 to break line of sight."))
                trySneakAway();
        }
        else if (game.activity == EVENT_CHOICE)
        {
            if (choice(option++, game.pendingEvent == -2 ? "Recover old gear" : "Help / accept risk",
                       "Spend a supply or attempt the offered check.")) resolvePendingEvent(0);
            if (choice(option++, "Bargain / ask terms", "Charisma check; a poor deal can create a debt.")) resolvePendingEvent(1);
            if (choice(option++, "Decline and move on", "Keep your resources. Others may remember.")) resolvePendingEvent(2);
        }
        else if (game.activity == IN_TAVERN)
        {
            int mealPrice = marketPrice(6);
            if (choice(option++, "Rent a bed  /  3 gold", "Safe rest and recovery.", game.gold >= 3))
            {
                game.gold -= 3; waitAndRest();
                game.activityText = "A few hours of sleep. The room smells of smoke and stew.";
            }
            std::string mealLabel = "Buy a meal  /  " + std::to_string(mealPrice) + " gold";
            if (choice(option++, mealLabel.c_str(), "Restore hunger and a little health.", game.gold >= mealPrice))
            {
                game.gold -= mealPrice; game.hunger = clampValue(game.hunger + 40, 0, 100);
                game.hp = clampValue(game.hp + 2, 0, game.maxHp); addJournal("You buy a warm meal at the tavern.");
            }
            if (choice(option++, "Leave the tavern", "Return to the settlement.")) endActivity();
        }
        else if (game.activity == TALKING)
        {
            if (choice(option++, "Ask about the road", "Hear a warning tied to a local story.")) askAboutRoad();
            if (choice(option++, "Ask for their story", "A DC 15 check may earn lasting trust.")) shareAnNPCStory();
            if (choice(option++, "Offer a ration", "Share food and earn a favor.", itemQuantity(5) > 0)) offerNPCHelp();
            if (game.questOffer.active)
            {
                if (choice(option++, "Accept task", "Record this objective and its reward.")) acceptQuestOffer();
                if (choice(option++, "Decline task", "Leave the offer for another day.")) declineQuestOffer();
            }
            else if (choice(option++, "Ask for a task", "Take on a local objective for a reward.", hasQuestRoom()))
                offerQuest();
        }
        else if (game.activity == SEARCHING)
        {
            if (choice(option++, "Finish searching", "Return to the area.")) endActivity();
        }
        else
        {
            if (hasDeliverableQuestAt(game.x, game.y) &&
                choice(option++, "Deliver quest supplies",
                       "Hand over gathered items here and claim the quest reward."))
                deliverQuestItems();
            if (game.socialSceneKind == SOCIAL_NPC_EXCHANGE)
            {
                if (choice(option++, "Mediate the dispute", "CHA check DC 14; success earns trust and a favor."))
                    resolveSocialScene(0);
                if (choice(option++, "Listen to both", "Hear their exchange and keep a useful rumor."))
                    resolveSocialScene(1);
                if (choice(option++, "Leave them to it", "Walk on without taking sides."))
                    resolveSocialScene(2);
            }
            else if (game.socialSceneKind == SOCIAL_BANDIT_THREAT)
            {
                if (choice(option++, "Confront the outlaw", "Starts combat; the outlaw's warning is shown above."))
                    resolveSocialScene(0);
                if (choice(option++, "Create a diversion", "CHA check DC 14; failure draws the outlaw to you."))
                    resolveSocialScene(1);
                if (choice(option++, "Stay out of it", "Leave without risking a fight."))
                    resolveSocialScene(2);
            }
            else if (game.socialSceneKind == SOCIAL_CREATURE_THREAT)
            {
                if (choice(option++, "Drive it off", "WIS check DC 14; failure draws the creature to you."))
                    resolveSocialScene(0);
                if (choice(option++, "Draw it away", "Starts combat with the warned creature."))
                    resolveSocialScene(1);
                if (choice(option++, "Give them room", "Let the traveler handle the moment without you."))
                    resolveSocialScene(2);
            }
            const Settlement *place = settlementAt(game.x, game.y);
            if (place && std::string(place->kind) != "CAMP" &&
                choice(option++, "Enter the tavern", "Find food, rest, and shelter."))
                beginActivity(IN_TAVERN, "A low fire warms the common room. The keeper looks up.");
            if (place && std::string(place->kind) != "CAMP" && monsterLootValue() > 0 &&
                choice(option++, TextFormat("Sell monster trophies / %ig", monsterLootValue()),
                       "Trade carried creature trophies for coin."))
                sellMonsterLoot();
            if ((place || game.wanderingNpc >= 0) &&
                choice(option++, game.wanderingNpc >= 0 ?
                           TextFormat("Talk to %s", NPCS[game.wanderingNpc].name) : "Talk to someone",
                       "Hear a road story; a chance meeting may reveal a longer thread."))
                beginActivity(TALKING, "A traveler pauses and gives you their attention.");
            if (game.wanderingNpc >= 0 && !game.npcKnown[game.wanderingNpc] && !place)
            {
                int npc = game.wanderingNpc;
                std::string label = "Leave " + std::string(NPCS[npc].name) + " be";
                if (choice(option++, label.c_str(), "Keep walking; they may be gone next time."))
                {
                    addJournal(std::string(NPCS[npc].name) + " gathers their things and continues onward.");
                    game.wanderingNpc = -1;
                    game.wanderingNpcTurns = 0;
                    game.wanderingNpcActivity = -1;
                    advanceTime();
                    tickNeeds(false);
                }
            }
            if (choice(option++, "Look for something", "Search for supplies, a clue, or a hidden place.")) lookAround();
            if (!place && choice(option++, "Forage", "Find food or useful plants; misidentification carries risk.")) forage();
            if (place && choice(option++, "Browse the armory",
                                "Compare weapons and protective gear for sale."))
                beginActivity(TRADING, "The armorer lays out steel, leather, and road-worn kit. Your gold: " +
                              std::to_string(game.gold) + ".");
            if (game.roamingCreature >= 0)
            {
                int creature = game.roamingCreature;
                std::string label = "Approach " + std::string(CREATURES[creature].name);
                if (choice(option++, label.c_str(), "Face the warning while it is still in sight."))
                    startEncounter(creature);
                if (choice(option++, "Let it pass", "Keep your distance; the creature may vanish."))
                {
                    addJournal(std::string("You leave the ") + CREATURES[creature].name +
                               " to its own country.");
                    game.roamingCreature = -1;
                    game.roamingCreatureTurns = 0;
                    advanceTime();
                    tickNeeds(false);
                }
            }
            if (choice(option++, game.campBuilt ? "Rest at your camp" : "Build camp and rest",
                       "Recover a little. A wilderness camp can draw an ambush.")) waitAndRest();
        }
    }
    else if (game.page == MAP)
    {
        drawMobileMap(left, width, contentTop, contentBottom);
    }
    else if (game.page == ITEM_LEDGER)
    {
        float weight = 0.0f;
        for (std::size_t i = 0; i < game.inventory.size(); ++i)
            weight += ITEMS[game.inventory[i].item].weight * game.inventory[i].quantity;
        float capacity = 18.0f + std::max(0, game.attributes[0] - 10) * 1.5f;
        DrawText(TextFormat("PACK  %.1f / %.1f lb", weight, capacity), left + 14, contentTop + 4, 15, EMBER);
        if (drawButton({static_cast<float>(left + width - 170), static_cast<float>(contentTop),
                        148.0f, 48.0f}, "Craft bandage", false,
                       itemQuantity(11) > 0 && itemQuantity(12) > 0))
            craftBandage();
        if (ownedItemCount() == 0)
            drawWrapped("Your pack is empty. Supplies you find will be recorded here.",
                        left + 18, contentTop + 62, width - 36, 14, MUTED, 3);
        int visibleRow = 0;
        for (int i = 0; i < ITEM_COUNT; ++i)
        {
            if (itemQuantity(i) <= 0) continue;
            int rowY = contentTop + 56 + visibleRow * 96 - game.pageScroll;
            Rectangle row = {static_cast<float>(left + 12), static_cast<float>(rowY),
                             static_cast<float>(width - 24), 90.0f};
            drawPanel(row, visibleRow % 2 == 0 ? PAPER_RAISED : PAPER);
            std::string itemName = std::string(ITEMS[i].name) +
                (isEquipment(i) ? std::string("  /  ") + equipmentSlotName(ITEMS[i].slot) : "");
            DrawText(itemName.c_str(), left + 22, rowY + 5, 16, INK);
            std::string detail = "x" + std::to_string(itemQuantity(i)) + "  /  " +
                                 std::to_string(ITEMS[i].weight) + " lb";
            if (ITEMS[i].shelfLife > 0 && itemQuantity(i) > 0)
                detail += "  /  " + std::to_string(std::max(0, ITEMS[i].shelfLife - game.inventoryAge[i])) + " days";
            DrawText(detail.c_str(), left + 22, rowY + 26, 13, MUTED);
            drawWrapped(ITEMS[i].description, left + 22, rowY + 44, width - 142, 11, MUTED, 1);
            bool consumable = !isEquipment(i);
            bool equipped = isEquipped(i);
            Rectangle button = {static_cast<float>(left + width - 100), static_cast<float>(rowY + 21), 78.0f, 48.0f};
            bool visible = rowY >= contentTop && rowY + 90 <= contentBottom;
            if (drawButton(button, equipped ? "WORN" : consumable ? "USE" : "EQUIP",
                           equipped, visible && !equipped && itemQuantity(i) > 0))
            {
                if (consumable) useItem(i);
                else equipItem(i);
            }
            if (isEquipment(i))
            {
                drawWrapped(std::string("Pros: ") + ITEMS[i].pros + " / Cons: " + ITEMS[i].cons,
                            left + 22, rowY + 58, width - 142, 10, EMBER, 1);
                drawWrapped(equipmentStatLine(i), left + 22, rowY + 73,
                            width - 142, 10, MUTED, 1);
            }
            ++visibleRow;
        }
    }
    else if (game.page == QUEST_LOG)
    {
        if (game.questCount == 0)
        {
            drawWrapped("No tasks yet. Speak with someone in a settlement and ask for work.",
                        left + 18, contentTop + 14, width - 36, 15, MUTED, 3);
        }
        for (int i = 0; i < game.questCount; ++i)
        {
            const Quest &quest = game.quests[i];
            int rowY = contentTop + 8 + i * 94 - game.pageScroll;
            Rectangle row = {static_cast<float>(left + 12), static_cast<float>(rowY),
                             static_cast<float>(width - 24), 86.0f};
            drawPanel(row, quest.complete ? PAPER_INSET : PAPER_RAISED);
            DrawText(quest.title.c_str(), left + 22, rowY + 6, 16, INK);
            DrawText(quest.complete ? "FULFILLED" :
                     TextFormat("%s  %i/%i", questObjective(quest).c_str(), quest.progress, quest.goal),
                     left + 22, rowY + 29, 12, quest.complete ? GOOD : EMBER);
            std::string questDetail = quest.description;
            if (!quest.complete)
                questDetail += " Red map dot: " + std::to_string(quest.destinationX + 1) + "," +
                               std::to_string(quest.destinationY + 1) + ".";
            drawWrapped(questDetail, left + 22, rowY + 48, width - 48, 12, MUTED, 2);
            DrawText(TextFormat("%ig + %ix %s", quest.rewardGold, quest.rewardQuantity,
                                ITEMS[quest.rewardItem].name), left + 22, rowY + 72, 11, EMBER);
        }
    }
    EndScissorMode();
    bool activityFooter = game.activity == TALKING || game.activity == TRADING;
    if (activityFooter)
    {
        if (!game.saveError.empty())
            DrawText("AUTOSAVE FAILED", left + 14, top + height - 62, 11, BLOOD);
        else
            DrawText("Use UP/DOWN or scroll for more.", left + 14, top + height - 62, 11, MUTED);
        const int footerY = top + height - 44;
        if (maximumScroll > 0)
        {
            int pageStep = std::max(60, contentHeight / 2);
            if (drawButton({static_cast<float>(left + 12), static_cast<float>(footerY),
                            44.0f, 36.0f}, "UP", false, game.pageScroll > 0))
                game.pageScroll = clampValue(game.pageScroll - pageStep, 0, maximumScroll);
            if (drawButton({static_cast<float>(left + 60), static_cast<float>(footerY),
                            56.0f, 36.0f}, "DOWN", false, game.pageScroll < maximumScroll))
                game.pageScroll = clampValue(game.pageScroll + pageStep, 0, maximumScroll);
            if (drawButton({static_cast<float>(left + 120), static_cast<float>(footerY),
                            static_cast<float>(width - 132), 36.0f},
                           game.activity == TRADING ? "Leave armory" : "End conversation"))
                endActivity();
        }
        else if (drawButton({static_cast<float>(left + 12), static_cast<float>(footerY),
                            static_cast<float>(width - 24), 36.0f},
                           game.activity == TRADING ? "Leave armory" : "End conversation"))
            endActivity();
    }
    else if (!game.journal.empty())
    {
        int journalWidth = width - 28;
        if (maximumScroll > 0)
        {
            int footerY = top + height - 44;
            int pageStep = std::max(60, contentHeight / 2);
            journalWidth -= 112;
            if (drawButton({static_cast<float>(left + width - 112), static_cast<float>(footerY),
                            48.0f, 36.0f}, "UP", false, game.pageScroll > 0))
                game.pageScroll = clampValue(game.pageScroll - pageStep, 0, maximumScroll);
            if (drawButton({static_cast<float>(left + width - 60), static_cast<float>(footerY),
                            48.0f, 36.0f}, "DOWN", false, game.pageScroll < maximumScroll))
                game.pageScroll = clampValue(game.pageScroll + pageStep, 0, maximumScroll);
        }
        drawWrapped(game.journal.front(), left + 14, contentBottom + 5, journalWidth, 13, MUTED, 2);
    }
    if (!game.saveError.empty() && !activityFooter)
        DrawText("AUTOSAVE FAILED", left + width - 150, top + height - 26, 12, BLOOD);
    if (game.timeOfDay == 2)
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {30, 23, 31, 18});
    else if (game.timeOfDay == 3)
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {25, 20, 36, 38});
    if (IsKeyPressed(KEY_ESCAPE) && canChangePage)
    {
        game.page = ROAD;
        game.pageScroll = 0;
    }
}

void drawGame()
{
    int turnBefore = game.turn;
    int combatBefore = game.combatTurn;
    int enemyBefore = game.enemyIndex;
    int hpBefore = game.hp;
    int goldBefore = game.gold;
    int weaponBefore = game.equippedWeapon;
    int armorBefore = game.equippedArmor;
    int helmetBefore = game.equippedHelmet;
    int glovesBefore = game.equippedGloves;
    int bootsBefore = game.equippedBoots;
    int pageBefore = game.page;
    Activity activityBefore = game.activity;
    std::string activityTextBefore = game.activityText;
    std::string latestNoteBefore = game.journal.empty() ? "" : game.journal.front();
    int quantitiesBefore[ITEM_COUNT];
    for (int i = 0; i < ITEM_COUNT; ++i) quantitiesBefore[i] = itemQuantity(i);
    const auto inventoryChanged = [&]() {
        for (int i = 0; i < ITEM_COUNT; ++i)
            if (itemQuantity(i) != quantitiesBefore[i]) return true;
        return false;
    };
    int left, top;
    if (GetScreenWidth() < 1100 || GetScreenHeight() < 700)
    {
        drawMobileGame();
        if (game.timeOfDay == 2)
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {30, 23, 31, 18});
        else if (game.timeOfDay == 3)
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {25, 20, 36, 38});
        if (game.screen == GAME &&
            (game.turn != turnBefore || game.combatTurn != combatBefore || game.enemyIndex != enemyBefore ||
             game.hp != hpBefore || game.gold != goldBefore || game.page != pageBefore ||
             game.equippedWeapon != weaponBefore || game.equippedArmor != armorBefore ||
             game.equippedHelmet != helmetBefore || game.equippedGloves != glovesBefore ||
             game.equippedBoots != bootsBefore ||
             inventoryChanged() ||
             game.activity != activityBefore || game.activityText != activityTextBefore ||
             (!game.journal.empty() && game.journal.front() != latestNoteBefore)))
            saveGame();
        return;
    }
    int width = std::min(1120, GetScreenWidth() - 36);
    int height = std::min(750, GetScreenHeight() - 32);
    drawFrame("", width, height, left, top);
    drawHeader(left, top, width);
    bool canChangePage = game.enemyIndex < 0 && game.activity == FREE_ROAM;
    drawPageTabs(left, top, width, canChangePage);
    int scrollLimit = 0;
    if (game.page == ITEM_LEDGER) scrollLimit = std::max(0, 310 + ownedItemCount() * 80 - height);
    else if (game.page == QUEST_LOG) scrollLimit = std::max(0, 258 + game.questCount * 88 - height);
    else if (game.page == ROAD && game.activity == TRADING)
        scrollLimit = std::max(0, 358 + equipmentItemCount() * 62 - (height - 60));
    else if (game.page == ROAD && game.activity == TALKING && game.enemyIndex < 0)
    {
        int choices = 4 + (game.questOffer.active ? 1 : 0);
        scrollLimit = std::max(0, 299 + (choices - 1) * 58 + 50 - (height - 60));
    }
    else if (game.page == ROAD && game.activity == FREE_ROAM && game.enemyIndex < 0)
    {
        const Settlement *place = settlementAt(game.x, game.y);
        int choices = (place && std::string(place->kind) != "CAMP" ? 1 : 0) +
                      (place && std::string(place->kind) != "CAMP" && monsterLootValue() > 0 ? 1 : 0) +
                      (place || game.wanderingNpc >= 0 ? 1 : 0) + 1 + (!place ? 1 : 0) +
                      (place ? 1 : 0) + (game.roamingCreature >= 0 ? 2 : 0) +
                      (!place && game.wanderingNpc >= 0 && !game.npcKnown[game.wanderingNpc] ? 1 : 0) +
                      1 + (game.socialSceneKind == SOCIAL_NONE ? 0 : 3) +
                      (hasDeliverableQuestAt(game.x, game.y) ? 1 : 0);
        scrollLimit = std::max(0, 389 + (choices - 1) * 58 + 50 - (height - 60));
    }
    updateScrollInput({static_cast<float>(left + 20), static_cast<float>(top + 155),
                       static_cast<float>(width - 40), static_cast<float>(height - 215)}, scrollLimit);
    BeginScissorMode(left + 20, top + 155, width - 40, height - 215);
    switch (game.page)
    {
        case ROAD: drawRoadPage(left, top, width); break;
        case ITEM_LEDGER: drawItemsPage(left, top, width); break;
        case QUEST_LOG: drawQuestsPage(left, top, width); break;
        case MAP: drawMapPage(left, top, width); break;
    }
    EndScissorMode();
    if (game.screen == GAME)
    {
        DrawLine(left + 24, top + height - 48, left + width - 24, top + height - 48, BORDER);
        bool canChangePage = game.enemyIndex < 0 && game.activity == FREE_ROAM;
        if (game.activity == TALKING || game.activity == TRADING)
        {
            DrawText(game.activity == TRADING ? "Scroll the armory; leave whenever you are ready." :
                     "Scroll or drag choices; exit is always available.",
                     left + 36, top + height - 34, 14, MUTED);
            if (drawButton({static_cast<float>(left + width - 238), static_cast<float>(top + height - 43),
                            204.0f, 32.0f}, game.activity == TRADING ? "Leave armory" : "End conversation"))
                endActivity();
        }
        else
            DrawText(canChangePage ? "ESC  /  Return to main" : "Finish this encounter or activity to change pages.",
                     left + 36, top + height - 34, 15, MUTED);
        if (IsKeyPressed(KEY_ESCAPE) && game.enemyIndex < 0 && game.activity == FREE_ROAM)
        {
            game.page = ROAD;
            game.pageScroll = 0;
        }
    }
    if (game.screen == GAME &&
        (game.turn != turnBefore || game.combatTurn != combatBefore || game.enemyIndex != enemyBefore ||
         game.hp != hpBefore || game.gold != goldBefore || game.page != pageBefore ||
         game.equippedWeapon != weaponBefore || game.equippedArmor != armorBefore ||
         game.equippedHelmet != helmetBefore || game.equippedGloves != glovesBefore ||
         game.equippedBoots != bootsBefore ||
         inventoryChanged() ||
         game.activity != activityBefore || game.activityText != activityTextBefore ||
         (!game.journal.empty() && game.journal.front() != latestNoteBefore)))
        saveGame();
}

void drawEpilogue()
{
    if (GetScreenWidth() < 1100 || GetScreenHeight() < 700)
    {
        int width = std::max(260, GetScreenWidth() - 16);
        int height = std::max(440, GetScreenHeight() - 16);
        int left = (GetScreenWidth() - width) / 2, top = (GetScreenHeight() - height) / 2;
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), NIGHT);
        drawPanel({static_cast<float>(left), static_cast<float>(top), static_cast<float>(width), static_cast<float>(height)}, PAPER);
        DrawText(game.won ? "The Road Remembers" : "Your Journey Ends", left + 18, top + 24, 22, game.won ? GOOD : BLOOD);
        drawWrapped(game.ending, left + 18, top + 67, width - 36, 16, INK, 4);
        DrawText(TextFormat("Days survived: %i", game.daysSurvived), left + 18, top + 160, 15, MUTED);
        DrawText(TextFormat("Creatures bested: %i", game.creaturesBested), left + 18, top + 184, 15, MUTED);
        drawWrapped(skillSummary(), left + 18, top + 210, width - 36, 14, EMBER, 2);
        drawWrapped(traitSummary(), left + 18, top + 232, width - 36, 14, MUTED, 2);
        if (!game.won)
        {
            std::string kills = "Kills: ";
            for (int i = 0; i < game.killCount; ++i)
            {
                if (i) kills += ", ";
                kills += game.notableKills[i];
            }
            if (game.killCount == 0) kills += "none";
            drawWrapped(kills, left + 18, top + 266, width - 36, 14, BLOOD, 3);
            if (game.fallenCount > 0)
                drawWrapped("A grave and surviving gear remain in this world for the next character.",
                            left + 18, top + 310, width - 36, 14, MUTED, 2);
        }
        if (drawButton({static_cast<float>(left + 14), static_cast<float>(top + height - 100),
                        static_cast<float>(width - 28), 42.0f}, "Forge Another Story", true))
        {
            game.screen = FORGE;
            game.origin = 0;
            game.editingName = false;
            resetAttributes();
        }
        if (drawButton({static_cast<float>(left + 14), static_cast<float>(top + height - 52),
                        static_cast<float>(width - 28), 42.0f}, "Title"))
            game.screen = TITLE;
        return;
    }
    int left, top;
    drawFrame(game.won ? "The Road Remembers" : "Your Journey Ends", 760, 550, left, top);
    DrawText(game.won ? "A NAME CARRIED FORWARD" : "THE MARCHES FALL QUIET", left + 46, top + 133, 15, game.won ? GOOD : BLOOD);
    drawWrapped(game.ending, left + 46, top + 171, 660, 21, INK, 8);
    DrawText(TextFormat("DAYS ON THE ROAD  %i", game.daysSurvived), left + 46, top + 267, 16, MUTED);
    DrawText(TextFormat("GOLD CARRIED  %i", game.gold), left + 46, top + 297, 16, MUTED);
    DrawText(TextFormat("CREATURES BESTED  %i", game.creaturesBested), left + 46, top + 327, 16, MUTED);
    if (!game.won)
    {
        drawWrapped("Cause: " + game.ending, left + 46, top + 358, 650, 16, BLOOD, 3);
        std::string kills = "Notable kills: ";
        if (game.killCount == 0) kills += "none";
        for (int i = 0; i < game.killCount; ++i)
        {
            if (i > 0) kills += ", ";
            kills += game.notableKills[i];
        }
        drawWrapped(kills, left + 46, top + 380, 650, 15, MUTED, 2);
        if (game.fallenCount > 0)
            drawWrapped("Fallen record saved. The next character may find the grave and surviving gear.",
                        left + 46, top + 404, 650, 14, EMBER, 1);
    }

    drawWrapped(skillSummary(), left + 46, top + 420, 650, 14, EMBER, 1);
    drawWrapped(traitSummary(), left + 46, top + 438, 650, 14, MUTED, 1);
    if (drawButton({static_cast<float>(left + 46), static_cast<float>(top + 480), 250.0f, 50.0f}, "Forge Another Story", true))
    {
        game.screen = FORGE;
        game.origin = 0;
        game.editingName = false;
        resetAttributes();
    }
    if (drawButton({static_cast<float>(left + 312), static_cast<float>(top + 480), 180.0f, 50.0f}, "Title"))
        game.screen = TITLE;
}

void drawDebugOverlay()
{
    int width = std::min(720, GetScreenWidth() - 24);
    int height = std::min(340, GetScreenHeight() - 24);
    int x = (GetScreenWidth() - width) / 2;
    int y = (GetScreenHeight() - height) / 2;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 165});
    drawPanel({static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)}, PAPER);
    DrawText("HIDDEN BALANCE SIMULATOR", x + 24, y + 20, 20, EMBER);
    drawWrapped(game.debugReport, x + 24, y + 61, width - 48, 17, INK, 8);
    DrawText("F3  /  close", x + 24, y + height - 34, 15, MUTED);
}

void saveGame()
{
    if (game.screen != GAME && game.screen != EPILOGUE) return;
    const char *savePath = "wayworn_save.dat";
    const char *temporaryPath = "wayworn_save.tmp";
    const char *backupPath = "wayworn_save.bak";
    std::ofstream out(temporaryPath, std::ios::out | std::ios::trunc);
    if (!out)
    {
        game.saveError = "Autosave failed: wayworn_save.dat could not be written.";
        return;
    }
    out << "WAYWORN_SAVE 11\n";
    out << static_cast<int>(game.screen) << ' ' << static_cast<int>(game.page) << ' '
        << std::quoted(game.name) << ' ' << game.origin << ' ' << game.editingName << '\n';
    for (int value : game.attributes) out << value << ' ';
    out << '\n';
    for (int i = 0; i < SKILL_COUNT; ++i)
        out << game.skills[i] << ' ' << game.skillPractice[i] << ' ';
    for (int i = 0; i < TRAIT_COUNT; ++i) out << game.traits[i] << ' ';
    out << game.pointsRemaining << ' ' << static_cast<int>(game.activity) << ' '
        << std::quoted(game.activityText) << '\n';
    out << game.worldInitialized << ' ' << game.worldSeed << ' ' << game.x << ' ' << game.y << ' '
        << game.day << ' ' << game.timeOfDay << ' ' << game.season << ' ' << game.weather << ' '
        << game.exposure << ' ' << game.bleeding << ' ' << game.infection << ' ' << game.turn << ' '
        << game.campBuilt << ' ' << game.pageScroll << '\n';
    out << game.roamingCreature << ' ' << game.roamingCreatureTurns << ' '
        << game.wanderingNpc << ' ' << game.wanderingNpcTurns << ' '
        << game.wanderingNpcActivity << '\n';
    for (int i = 0; i < 4; ++i) out << game.injuries[i] << ' ';
    out << '\n';
    for (int y = 0; y < MAP_SIZE; ++y)
        for (int x = 0; x < MAP_SIZE; ++x)
            out << game.map[y][x] << ' ' << game.landmarks[y][x] << ' '
                << game.discoveredPOI[y][x] << ' ' << game.seen[y][x] << '\n';
    out << game.hp << ' ' << game.maxHp << ' ' << game.hunger << ' ' << game.fatigue << ' '
        << game.gold << ' ' << game.equippedWeapon << ' ' << game.equippedArmor << ' '
        << game.equippedHelmet << ' ' << game.equippedGloves << ' ' << game.equippedBoots << '\n';
    out << game.inventory.size() << '\n';
    for (std::size_t i = 0; i < game.inventory.size(); ++i)
        out << game.inventory[i].item << ' ' << game.inventory[i].quantity << '\n';
    out << game.journal.size() << '\n';
    for (std::size_t i = 0; i < game.journal.size(); ++i) out << std::quoted(game.journal[i]) << '\n';
    for (int i = 0; i < CREATURE_COUNT; ++i) out << game.discovered[i] << ' ';
    out << '\n' << game.enemyIndex << ' ' << game.enemyHp << ' ' << game.combatTurn << ' '
        << game.playerAC << ' ' << game.enemyMorale << ' ' << game.enemyTelegraph << ' '
        << game.stunnedTurns << ' ' << game.proneTurns << ' ' << game.frightenedTurns << ' '
        << game.guarding << ' ' << game.weakened << ' ' << game.dead << ' ' << game.won << ' '
        << game.daysSurvived << ' ' << game.characterStartDay << ' ' << game.creaturesBested << ' '
        << game.killCount << '\n';
    for (int i = 0; i < game.killCount; ++i) out << std::quoted(game.notableKills[i]) << '\n';
    out << game.debts << ' ' << game.favors << ' ' << game.grudges << ' ' << game.eventStreak << ' '
        << game.pendingEvent << ' ' << game.activeNpc << ' ' << game.activeFallen << ' ' << game.eventHistoryCount << ' '
        << game.fallenCount << '\n';
    for (int i = 0; i < FACTION_COUNT; ++i) out << game.factionRep[i] << ' ';
    out << '\n';
    for (int i = 0; i < FACTION_COUNT; ++i) out << game.factionPower[i] << ' ';
    out << '\n';
    for (int i = 0; i < FACTION_COUNT; ++i) out << game.factionUnrest[i] << ' ';
    out << '\n';
    for (int i = 0; i < KINGDOM_COUNT; ++i) out << game.kingdomStability[i] << ' ';
    out << '\n';
    for (int i = 0; i < SETTLEMENT_COUNT; ++i) out << game.settlementProsperity[i] << ' ';
    out << '\n' << game.lastWorldSimDay << '\n';
    for (int i = 0; i < NPC_COUNT; ++i) out << game.npcTrust[i] << ' ';
    out << '\n';
    for (int i = 0; i < NPC_COUNT; ++i)
        out << game.npcKnown[i] << ' ' << game.npcDead[i] << ' '
            << game.npcKnownX[i] << ' ' << game.npcKnownY[i] << '\n';
    out << std::quoted(game.worldEventHeadline) << ' ' << game.worldEventDay << '\n';
    for (int i = 0; i < 23; ++i) out << game.storyStage[i] << ' ';
    out << '\n';
    for (int i = 0; i < 4; ++i) out << game.lastEvents[i] << ' ';
    out << '\n';
    for (int i = 0; i < EVENT_COUNT; ++i) out << game.eventHistory[i] << ' ';
    out << '\n';
    for (int i = 0; i < ITEM_COUNT; ++i) out << game.inventoryAge[i] << ' ';
    out << '\n';
    for (int i = 0; i < game.fallenCount; ++i)
    {
        out << std::quoted(game.fallenName[i]) << ' ' << std::quoted(game.fallenCause[i]) << ' '
            << game.fallenX[i] << ' ' << game.fallenY[i] << '\n';
        for (int item = 0; item < ITEM_COUNT; ++item) out << game.fallenGear[i][item] << ' ';
        out << '\n';
    }
    out << std::quoted(game.ending) << '\n';
    std::ostringstream rngText;
    rngText << worldRng;
    out << std::quoted(rngText.str()) << '\n';
    out << game.questCount << '\n';
    for (int i = 0; i < game.questCount; ++i)
    {
        const Quest &quest = game.quests[i];
        out << static_cast<int>(quest.type) << ' ' << quest.target << ' ' << quest.progress << ' '
            << quest.goal << ' ' << quest.difficulty << ' ' << quest.rewardGold << ' '
            << quest.rewardItem << ' ' << quest.rewardQuantity << ' ' << quest.complete << ' '
            << std::quoted(quest.title) << ' ' << std::quoted(quest.description) << ' '
            << quest.destinationX << ' ' << quest.destinationY << '\n';
    }
    out << game.questOffer.active << '\n';
    if (game.questOffer.active)
    {
        const Quest &quest = game.questOffer.quest;
        out << static_cast<int>(quest.type) << ' ' << quest.target << ' ' << quest.progress << ' '
            << quest.goal << ' ' << quest.difficulty << ' ' << quest.rewardGold << ' '
            << quest.rewardItem << ' ' << quest.rewardQuantity << ' ' << quest.complete << ' '
            << std::quoted(quest.title) << ' ' << std::quoted(quest.description) << ' '
            << quest.destinationX << ' ' << quest.destinationY << '\n';
    }
    out << static_cast<int>(game.socialSceneKind) << ' ' << game.socialSceneNpcA << ' '
        << game.socialSceneNpcB << ' ' << game.socialSceneCreature << ' '
        << std::quoted(game.socialSceneDescription) << '\n';
    out.flush();
    out.close();
    if (!out)
    {
        std::remove(temporaryPath);
        game.saveError = "Autosave failed while writing wayworn_save.dat.";
        return;
    }
    std::remove(backupPath);
    bool hadSave = FileExists(savePath);
    if (hadSave && std::rename(savePath, backupPath) != 0)
    {
        std::remove(temporaryPath);
        game.saveError = "Autosave could not preserve the last good save.";
        return;
    }
    if (std::rename(temporaryPath, savePath) != 0)
    {
        if (hadSave) std::rename(backupPath, savePath);
        std::remove(temporaryPath);
        game.saveError = "Autosave could not replace wayworn_save.dat.";
        return;
    }
    if (hadSave) std::remove(backupPath);
    game.saveError.clear();
}

bool loadGame()
{
    if (!FileExists("wayworn_save.dat") && FileExists("wayworn_save.bak"))
        std::rename("wayworn_save.bak", "wayworn_save.dat");
    std::ifstream in("wayworn_save.dat");
    if (!in) return false;
    std::string marker;
    int version = 0;
    if (!(in >> marker >> version) || marker != "WAYWORN_SAVE" ||
        (version < 1 || version > 11))
    {
        game.saveError = "The autosave is missing or has an unsupported format.";
        return false;
    }
    GameState loaded = {};
    loaded.socialSceneNpcA = -1;
    loaded.socialSceneNpcB = -1;
    loaded.socialSceneCreature = -1;
    int screen = 0, page = 0, activity = 0;
    if (!(in >> screen >> page >> std::quoted(loaded.name) >> loaded.origin >> loaded.editingName))
        return false;
    for (int &value : loaded.attributes) if (!(in >> value)) return false;
    if (version >= 2)
    {
        for (int i = 0; i < SKILL_COUNT; ++i)
            if (!(in >> loaded.skills[i] >> loaded.skillPractice[i]) ||
                loaded.skills[i] < 0 || loaded.skills[i] > 5 ||
                loaded.skillPractice[i] < 0 || loaded.skillPractice[i] > 2)
                return false;
        for (int i = 0; i < TRAIT_COUNT; ++i) if (!(in >> loaded.traits[i])) return false;
    }
    if (!(in >> loaded.pointsRemaining >> activity >> std::quoted(loaded.activityText) >>
          loaded.worldInitialized >> loaded.worldSeed >> loaded.x >> loaded.y >> loaded.day >>
          loaded.timeOfDay >> loaded.season >> loaded.weather >> loaded.exposure >> loaded.bleeding >>
          loaded.infection >> loaded.turn >> loaded.campBuilt >> loaded.pageScroll))
        return false;
    if (version >= 5)
    {
        if (!(in >> loaded.roamingCreature >> loaded.roamingCreatureTurns >>
              loaded.wanderingNpc >> loaded.wanderingNpcTurns >> loaded.wanderingNpcActivity))
            return false;
    }
    else
    {
        loaded.roamingCreature = -1;
        loaded.roamingCreatureTurns = 0;
        loaded.wanderingNpc = -1;
        loaded.wanderingNpcTurns = 0;
        loaded.wanderingNpcActivity = -1;
    }
    for (int i = 0; i < 4; ++i) if (!(in >> loaded.injuries[i])) return false;
    for (int y = 0; y < MAP_SIZE; ++y)
        for (int x = 0; x < MAP_SIZE; ++x)
            if (!(in >> loaded.map[y][x] >> loaded.landmarks[y][x] >>
                  loaded.discoveredPOI[y][x] >> loaded.seen[y][x]) ||
                loaded.map[y][x] < 0 || loaded.map[y][x] > 7 ||
                loaded.landmarks[y][x] < 0 || loaded.landmarks[y][x] > 5)
                return false;
    int legacyThirst = 0;
    if (version >= 6)
    {
        if (!(in >> loaded.hp >> loaded.maxHp >> loaded.hunger >> loaded.fatigue >>
              loaded.gold >> loaded.equippedWeapon >> loaded.equippedArmor))
            return false;
    }
    else if (!(in >> loaded.hp >> loaded.maxHp >> loaded.hunger >> legacyThirst >>
               loaded.fatigue >> loaded.gold >> loaded.equippedWeapon >> loaded.equippedArmor))
        return false;
    loaded.equippedHelmet = loaded.equippedGloves = loaded.equippedBoots = -1;
    if (version >= 7 &&
        !(in >> loaded.equippedHelmet >> loaded.equippedGloves >> loaded.equippedBoots))
        return false;
    std::size_t count = 0;
    if (!(in >> count) || count > ITEM_COUNT) return false;
    for (std::size_t i = 0; i < count; ++i)
    {
        Stack stack;
        if (!(in >> stack.item >> stack.quantity) || stack.item < 0 || stack.item >= ITEM_COUNT ||
            stack.quantity <= 0 || stack.quantity > 999)
            return false;
        loaded.inventory.push_back(stack);
    }
    if (!(in >> count) || count > 16) return false;
    for (std::size_t i = 0; i < count; ++i)
    {
        std::string line;
        if (!(in >> std::quoted(line))) return false;
        loaded.journal.push_back(line);
    }
    int creatureSlots = version >= 9 ? CREATURE_COUNT :
                        version >= 6 ? 22 : version >= 5 ? 16 : 10;
    for (int i = 0; i < creatureSlots; ++i)
        if (!(in >> loaded.discovered[i]) || loaded.discovered[i] < 0 || loaded.discovered[i] > 1) return false;
    for (int i = creatureSlots; i < CREATURE_COUNT; ++i) loaded.discovered[i] = 0;
    if (!(in >> loaded.enemyIndex >> loaded.enemyHp >> loaded.combatTurn >> loaded.playerAC >>
          loaded.enemyMorale >> loaded.enemyTelegraph >> loaded.stunnedTurns >> loaded.proneTurns >>
          loaded.frightenedTurns >> loaded.guarding >> loaded.weakened >>
          loaded.dead >> loaded.won >> loaded.daysSurvived >> loaded.characterStartDay >>
          loaded.creaturesBested >> loaded.killCount))
        return false;
    if (loaded.killCount < 0 || loaded.killCount > 10) return false;
    for (int i = 0; i < loaded.killCount; ++i)
        if (!(in >> std::quoted(loaded.notableKills[i]))) return false;
    if (!(in >>
          loaded.debts >> loaded.favors >> loaded.grudges >> loaded.eventStreak >>
          loaded.pendingEvent >> loaded.activeNpc >> loaded.activeFallen >>
          loaded.eventHistoryCount >> loaded.fallenCount))
        return false;
    int savedFactionCount = version >= 8 ? FACTION_COUNT : 3;
    for (int i = 0; i < savedFactionCount; ++i)
        if (!(in >> loaded.factionRep[i]) || loaded.factionRep[i] < -5 || loaded.factionRep[i] > 5)
            return false;
    if (version >= 8)
    {
        for (int i = 0; i < FACTION_COUNT; ++i)
            if (!(in >> loaded.factionPower[i]) || loaded.factionPower[i] < 10 ||
                loaded.factionPower[i] > 90)
                return false;
        for (int i = 0; i < FACTION_COUNT; ++i)
            if (!(in >> loaded.factionUnrest[i]) || loaded.factionUnrest[i] < 0 ||
                loaded.factionUnrest[i] > 100)
                return false;
        for (int i = 0; i < KINGDOM_COUNT; ++i)
            if (!(in >> loaded.kingdomStability[i]) || loaded.kingdomStability[i] < 0 ||
                loaded.kingdomStability[i] > 100)
                return false;
        for (int i = 0; i < SETTLEMENT_COUNT; ++i)
            if (!(in >> loaded.settlementProsperity[i]) || loaded.settlementProsperity[i] < 5 ||
                loaded.settlementProsperity[i] > 95)
                return false;
        if (!(in >> loaded.lastWorldSimDay) || loaded.lastWorldSimDay < 1 ||
            loaded.lastWorldSimDay > loaded.day)
            return false;
    }
    int savedNpcCount = version >= 8 ? NPC_COUNT : LEGACY_NPC_COUNT;
    for (int i = 0; i < savedNpcCount; ++i)
        if (!(in >> loaded.npcTrust[i]) || loaded.npcTrust[i] < -5 || loaded.npcTrust[i] > 5)
            return false;
    if (version >= 9)
    {
        for (int i = 0; i < NPC_COUNT; ++i)
        {
            if (!(in >> loaded.npcKnown[i] >> loaded.npcDead[i] >>
                  loaded.npcKnownX[i] >> loaded.npcKnownY[i]) ||
                loaded.npcKnownX[i] < -1 || loaded.npcKnownX[i] >= MAP_SIZE ||
                loaded.npcKnownY[i] < -1 || loaded.npcKnownY[i] >= MAP_SIZE ||
                (loaded.npcKnown[i] &&
                 (loaded.npcKnownX[i] < 0 || loaded.npcKnownY[i] < 0)))
                return false;
        }
        if (!(in >> std::quoted(loaded.worldEventHeadline) >> loaded.worldEventDay) ||
            loaded.worldEventHeadline.empty() || loaded.worldEventHeadline.size() > 512 ||
            loaded.worldEventDay < 1 || loaded.worldEventDay > loaded.day)
            return false;
    }
    else
    {
        for (int i = 0; i < NPC_COUNT; ++i)
        {
            loaded.npcKnownX[i] = -1;
            loaded.npcKnownY[i] = -1;
            loaded.npcKnown[i] = loaded.npcTrust[i] > 0;
            if (loaded.npcKnown[i])
            {
                loaded.npcKnownX[i] = loaded.x;
                loaded.npcKnownY[i] = loaded.y;
            }
        }
        if (loaded.wanderingNpc >= 0)
        {
            loaded.npcKnown[loaded.wanderingNpc] = true;
            loaded.npcKnownX[loaded.wanderingNpc] = loaded.x;
            loaded.npcKnownY[loaded.wanderingNpc] = loaded.y;
        }
    }
    for (int i = 0; i < 23; ++i) if (!(in >> loaded.storyStage[i])) return false;
    for (int i = 0; i < 23; ++i) if (loaded.storyStage[i] < 0 || loaded.storyStage[i] > 3) return false;
    for (int i = 0; i < 4; ++i)
        if (!(in >> loaded.lastEvents[i]) || loaded.lastEvents[i] < -1 || loaded.lastEvents[i] >= EVENT_COUNT) return false;
    int eventSlots = version >= 4 ? EVENT_COUNT : 47;
    for (int i = 0; i < eventSlots; ++i)
        if (!(in >> loaded.eventHistory[i]) || loaded.eventHistory[i] < 0) return false;
    int itemSlots = version >= 9 ? ITEM_COUNT : version >= 7 ? 162 :
                    version >= 6 ? FIRST_NEW_WEAPON :
                    version >= 4 ? 34 : 15;
    for (int i = 0; i < itemSlots; ++i) if (!(in >> loaded.inventoryAge[i])) return false;
    for (int i = itemSlots; i < ITEM_COUNT; ++i) loaded.inventoryAge[i] = 0;
    if (loaded.fallenCount < 0 || loaded.fallenCount > 8) return false;
    for (int i = 0; i < loaded.fallenCount; ++i)
    {
        if (!(in >> std::quoted(loaded.fallenName[i]) >> std::quoted(loaded.fallenCause[i]) >>
              loaded.fallenX[i] >> loaded.fallenY[i]) || loaded.fallenX[i] < 0 ||
            loaded.fallenX[i] >= MAP_SIZE || loaded.fallenY[i] < 0 || loaded.fallenY[i] >= MAP_SIZE)
            return false;
        for (int item = 0; item < itemSlots; ++item)
            if (!(in >> loaded.fallenGear[i][item]) || loaded.fallenGear[i][item] < 0 ||
                loaded.fallenGear[i][item] > 999) return false;
        for (int item = itemSlots; item < ITEM_COUNT; ++item) loaded.fallenGear[i][item] = 0;
    }
    std::string rngState;
    if (!(in >> std::quoted(loaded.ending) >> std::quoted(rngState)))
        return false;
    if (version < 3)
    {
        if (page == 4) page = MAP;
        else if (page == 2 || page == 3) page = ROAD;
    }
    if (screen < TITLE || screen > EPILOGUE || page < ROAD || page > MAP ||
        activity < FREE_ROAM || activity > TRADING || loaded.x < 0 || loaded.x >= MAP_SIZE ||
        loaded.y < 0 || loaded.y >= MAP_SIZE || loaded.enemyIndex < -1 || loaded.enemyIndex >= CREATURE_COUNT ||
        loaded.activeNpc < -1 || loaded.activeNpc >= NPC_COUNT ||
        loaded.activeFallen < -1 || loaded.activeFallen >= loaded.fallenCount ||
        loaded.origin < 0 || loaded.origin > 2 || loaded.timeOfDay < 0 || loaded.timeOfDay > 3 ||
        loaded.season < 0 || loaded.season > 3 || loaded.weather < 0 || loaded.weather > 4 ||
        loaded.roamingCreature < -1 || loaded.roamingCreature >= CREATURE_COUNT ||
        loaded.roamingCreatureTurns < 0 || loaded.roamingCreatureTurns > 3 ||
        loaded.wanderingNpc < -1 || loaded.wanderingNpc >= NPC_COUNT ||
        loaded.wanderingNpcTurns < 0 || loaded.wanderingNpcTurns > 3 ||
        loaded.wanderingNpcActivity < -1 || loaded.wanderingNpcActivity >= 10 ||
        loaded.maxHp <= 0 || loaded.hp < 0 || loaded.hp > loaded.maxHp ||
        loaded.equippedWeapon < -1 || loaded.equippedWeapon >= ITEM_COUNT ||
        loaded.equippedArmor < -1 || loaded.equippedArmor >= ITEM_COUNT ||
        loaded.equippedHelmet < -1 || loaded.equippedHelmet >= ITEM_COUNT ||
        loaded.equippedGloves < -1 || loaded.equippedGloves >= ITEM_COUNT ||
        loaded.equippedBoots < -1 || loaded.equippedBoots >= ITEM_COUNT ||
        (loaded.equippedWeapon >= 0 && ITEMS[loaded.equippedWeapon].slot != SLOT_WEAPON) ||
        (loaded.equippedArmor >= 0 && ITEMS[loaded.equippedArmor].slot != SLOT_BODY) ||
        (loaded.equippedHelmet >= 0 && ITEMS[loaded.equippedHelmet].slot != SLOT_HELMET) ||
        (loaded.equippedGloves >= 0 && ITEMS[loaded.equippedGloves].slot != SLOT_GLOVES) ||
        (loaded.equippedBoots >= 0 && ITEMS[loaded.equippedBoots].slot != SLOT_BOOTS) ||
        loaded.pendingEvent < -2 || loaded.pendingEvent >= EVENT_COUNT ||
        loaded.pageScroll < 0 || loaded.pageScroll > 10000)
    {
        game.saveError = "The autosave could not be read safely.";
        return false;
    }
    if (version >= 3)
    {
        int questCount = 0;
        if (!(in >> questCount) || questCount < 0 || questCount > QUEST_CAPACITY) return false;
        const auto readQuest = [&in, version](Quest &quest) {
            quest.destinationX = -1;
            quest.destinationY = -1;
            int type = 0;
            if (!(in >> type >> quest.target >> quest.progress >> quest.goal >> quest.difficulty >>
                  quest.rewardGold >> quest.rewardItem >> quest.rewardQuantity >> quest.complete >>
                  std::quoted(quest.title) >> std::quoted(quest.description)) ||
                type < QUEST_EXPLORE || type > QUEST_NEWS ||
                quest.goal < 1 || quest.goal > 10 || quest.progress < 0 || quest.progress > quest.goal ||
                quest.difficulty < 1 || quest.difficulty > 3 || quest.rewardGold < 0 ||
                quest.rewardGold > 10000 || quest.rewardItem < 0 || quest.rewardItem >= ITEM_COUNT ||
                quest.rewardQuantity < 1 || quest.rewardQuantity > 10)
                return false;
            quest.type = static_cast<QuestType>(type);
            if (version >= 11 &&
                (!(in >> quest.destinationX >> quest.destinationY) ||
                 quest.destinationX < 0 || quest.destinationX >= MAP_SIZE ||
                 quest.destinationY < 0 || quest.destinationY >= MAP_SIZE))
                return false;
            if ((quest.type == QUEST_EXPLORE && (quest.target < 0 || quest.target > 7)) ||
                (quest.type == QUEST_HUNT && (quest.target < 0 || quest.target >= CREATURE_COUNT)) ||
                (quest.type == QUEST_GATHER && (quest.target < 0 || quest.target >= ITEM_COUNT)) ||
                (quest.type == QUEST_NEWS && quest.target != -1))
                return false;
            return true;
        };
        loaded.questCount = questCount;
        for (int i = 0; i < loaded.questCount; ++i)
            if (!readQuest(loaded.quests[i])) return false;
        if (!(in >> loaded.questOffer.active)) return false;
        if (loaded.questOffer.active && !readQuest(loaded.questOffer.quest)) return false;
    }
    if (version >= 10)
    {
        int socialSceneKind = SOCIAL_NONE;
        if (!(in >> socialSceneKind >> loaded.socialSceneNpcA >> loaded.socialSceneNpcB >>
              loaded.socialSceneCreature >> std::quoted(loaded.socialSceneDescription)) ||
            socialSceneKind < SOCIAL_NONE || socialSceneKind > SOCIAL_CREATURE_THREAT ||
            loaded.socialSceneNpcA < -1 || loaded.socialSceneNpcA >= NPC_COUNT ||
            loaded.socialSceneNpcB < -1 || loaded.socialSceneNpcB >= NPC_COUNT ||
            loaded.socialSceneCreature < -1 || loaded.socialSceneCreature >= CREATURE_COUNT ||
            loaded.socialSceneDescription.size() > 1024)
            return false;
        loaded.socialSceneKind = static_cast<SocialSceneKind>(socialSceneKind);
        if ((loaded.socialSceneKind == SOCIAL_NONE &&
             (!loaded.socialSceneDescription.empty() || loaded.socialSceneNpcA != -1 ||
              loaded.socialSceneNpcB != -1 || loaded.socialSceneCreature != -1)) ||
            (loaded.socialSceneKind == SOCIAL_NPC_EXCHANGE &&
             (loaded.socialSceneNpcA < 0 || loaded.socialSceneNpcB < 0 ||
              loaded.socialSceneNpcA == loaded.socialSceneNpcB || loaded.socialSceneCreature != -1)) ||
            (loaded.socialSceneKind == SOCIAL_BANDIT_THREAT &&
             (loaded.socialSceneNpcA < 0 || loaded.socialSceneNpcB != -1 ||
              loaded.socialSceneCreature != -1)) ||
            (loaded.socialSceneKind == SOCIAL_CREATURE_THREAT &&
             (loaded.socialSceneNpcA < 0 || loaded.socialSceneCreature < 0 ||
              loaded.socialSceneNpcB != -1)))
            return false;
    }
    std::istringstream rngInput(rngState);
    std::mt19937 restoredRng;
    if (!(rngInput >> restoredRng))
    {
        game.saveError = "The autosave's random state is invalid.";
        return false;
    }
    in.close();
    loaded.screen = static_cast<Screen>(screen);
    loaded.page = static_cast<Page>(page);
    loaded.activity = static_cast<Activity>(activity);
    game = loaded;
    recalculateEquipment();
    worldRng = restoredRng;
    if (version < 11)
    {
        for (int i = 0; i < game.questCount; ++i)
            assignQuestDestination(game.quests[i]);
        if (game.questOffer.active)
            assignQuestDestination(game.questOffer.quest);
    }
    for (int i = 0; i < game.questCount; ++i)
        if (!game.quests[i].complete)
            game.seen[game.quests[i].destinationY][game.quests[i].destinationX] = true;
    if (game.screen == GAME && game.activity == FREE_ROAM && game.enemyIndex < 0)
        ensureQuestHuntPresence();
    if (version < 8) initializeWorldSimulation();
    if (version < 9)
    {
        if (game.worldEventHeadline.empty())
            game.worldEventHeadline = "The Marches are quiet for now; markets and borders shift with the passing days.";
        game.worldEventDay = game.day;
    }
    game.saveError.clear();
    if (version < 11) saveGame();
    return true;
}
} // namespace

int main()
{
    unsigned int seed = static_cast<unsigned int>(std::time(NULL));
    worldRng.seed(seed);
    game.screen = TITLE;
    game.page = ROAD;
    game.name = "Wayfarer";
    game.origin = 0;
    game.worldSeed = static_cast<int>(seed);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1240, 820, "Wayworn");
    SetWindowMinSize(320, 480);
    UI_FONT = GetFontDefault();
    bool customFontLoaded = false;
    const char *fontPath = "C:/Windows/Fonts/segoeui.ttf";
    if (FileExists(fontPath))
    {
        int fontCodepoints[96];
        for (int i = 0; i < 95; ++i) fontCodepoints[i] = 32 + i;
        fontCodepoints[95] = 0x2014;
        Font readableFont = LoadFontEx(fontPath, 32, fontCodepoints, 96);
        if (IsFontValid(readableFont))
        {
            UI_FONT = readableFont;
            customFontLoaded = true;
            SetTextureFilter(UI_FONT.texture, TEXTURE_FILTER_BILINEAR);
        }
    }
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float frameTime = GetFrameTime();
        selectionPulseTimer = std::max(0.0f, selectionPulseTimer - frameTime);
        pageTransitionTimer = std::max(0.0f, pageTransitionTimer - frameTime);
        encounterFlashTimer = std::max(0.0f, encounterFlashTimer - frameTime);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            pointerPressActive = true;
            pointerPressDragged = false;
            pointerPressStart = GetMousePosition();
        }
        else if (pointerPressActive && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            float dx = mouse.x - pointerPressStart.x;
            float dy = mouse.y - pointerPressStart.y;
            if (dx * dx + dy * dy >= 64.0f) pointerPressDragged = true;
        }
        if (IsKeyPressed(KEY_F3))
        {
            game.debugMode = !game.debugMode;
            if (game.debugMode) runBalanceSimulation();
        }
        BeginDrawing();
        ClearBackground(NIGHT);
        switch (game.screen)
        {
            case TITLE: drawTitle(); break;
            case FORGE: drawForge(); break;
            case GAME: drawGame(); break;
            case EPILOGUE: drawEpilogue(); break;
        }
        drawUiAnimations();
        if (game.debugMode) drawDebugOverlay();
        EndDrawing();
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            pointerPressActive = false;
            pointerPressDragged = false;
        }
    }

    if (customFontLoaded) UnloadFont(UI_FONT);
    CloseWindow();
    return 0;
}
