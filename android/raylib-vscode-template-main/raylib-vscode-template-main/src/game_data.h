#pragma once

namespace wayworn_data
{
enum ItemKind
{
    WEAPON,
    ARMOR,
    FOOD,
    MEDICINE,
    MATERIAL,
    TOOL
};

enum EquipmentSlot
{
    SLOT_NONE,
    SLOT_WEAPON,
    SLOT_HELMET,
    SLOT_BODY,
    SLOT_GLOVES,
    SLOT_BOOTS
};

struct Item
{
    const char *name;
    const char *kind;
    const char *description;
    ItemKind effect;
    int amount;
    int price;
    float weight;
    int shelfLife;
    EquipmentSlot slot;
    int benefit;
    int drawback;
    const char *pros;
    const char *cons;

    Item(const char *itemName, const char *itemKind, const char *itemDescription,
         ItemKind itemEffect, int itemAmount, int itemPrice, float itemWeight, int itemShelfLife,
         EquipmentSlot equipmentSlot = SLOT_NONE, int itemBenefit = 0, int itemDrawback = 0,
         const char *itemPros = "", const char *itemCons = "")
        : name(itemName), kind(itemKind), description(itemDescription), effect(itemEffect),
          amount(itemAmount), price(itemPrice), weight(itemWeight), shelfLife(itemShelfLife),
          slot(equipmentSlot), benefit(itemBenefit), drawback(itemDrawback),
          pros(itemPros), cons(itemCons) {}
};

static const Item ITEMS[] = {
    {"Rusted Shortsword", "Weapon", "A serviceable blade with a tired edge.", WEAPON, 5, 3, 3.0f, -1, SLOT_WEAPON, 0, 0, "Reliable damage", "Rust catches on a clean parry"},
    {"Traveler's Knife", "Weapon", "Light in the hand; made for close work.", WEAPON, 4, 2, 1.0f, -1, SLOT_WEAPON, 2, 0, "Quick, accurate strikes", "Short reach"},
    {"Gnarled Staff", "Weapon", "A walking stick that can answer an argument.", WEAPON, 3, 2, 2.0f, -1, SLOT_WEAPON, 1, 0, "Steady guard", "Low cutting power"},
    {"Scavenged Leather", "Body Armor", "Mismatched scraps. Better than a bare coat.", ARMOR, 2, 3, 4.0f, -1, SLOT_BODY, 0, 1, "Basic body protection", "Rubbed seams tire you on long marches"},
    {"Dented Buckler", "Body Armor", "A small shield with old dents and a good grip.", ARMOR, 2, 3, 3.0f, -1, SLOT_BODY, 1, 0, "Helps turn aside blows", "Occupies a hand; slow to reposition"},
    {"Ration", "Food", "Hard bread, dried meat, and a pinch of salt.", FOOD, 38, 1, 0.7f, 9},
    {"Smoked Sausage", "Food", "Peppered meat wrapped in waxed cloth.", FOOD, 34, 2, 0.6f, 18},
    {"Healing Herb", "Medicine", "Bitter leaves. Chew them slowly.", MEDICINE, 8, 2, 0.1f, 30},
    {"Bandage", "Medicine", "Clean linen for cuts and scrapes.", MEDICINE, 6, 2, 0.2f, -1},
    {"Antiseptic Salve", "Medicine", "A sharp-smelling balm for a bad wound.", MEDICINE, 10, 3, 0.3f, 60},
    {"Dried Mushrooms", "Food", "A handful of earthy caps, safe if properly dried.", FOOD, 22, 1, 0.2f, 16},
    {"Bitterroot", "Material", "A sharp root used in simple wound remedies.", MATERIAL, 0, 1, 0.2f, -1},
    {"Clean Cloth", "Material", "Washed linen, useful for bindings and repairs.", MATERIAL, 0, 1, 0.3f, -1},
    {"Flint and Steel", "Tool", "A small kit for coaxing fire from damp tinder.", TOOL, 0, 3, 0.4f, -1},
    {"Trail Cloak", "Body Armor", "A waxed cloak that turns wind and rain.", ARMOR, 1, 4, 2.0f, -1, SLOT_BODY, 1, 0, "Light protection from exposure", "Little defense against a direct strike"},
    {"Boiled Eggs", "Food", "A pair of eggs wrapped in a leaf.", FOOD, 18, 1, 0.3f, 4},
    {"Smoked Eel", "Food", "Salty fish that keeps well on a long road.", FOOD, 30, 2, 0.5f, 12},
    {"Dried Apples", "Food", "Tart slices from the last orchard harvest.", FOOD, 20, 1, 0.3f, 30},
    {"Hard Cheese", "Food", "A sharp wheel wrapped for the road.", FOOD, 27, 2, 0.6f, 30},
    {"Honeyed Cider Cake", "Food", "A dense oat cake sweetened with pressed apples.", FOOD, 23, 1, 0.5f, 10},
    {"Willow Bark", "Medicine", "Chewed for aches and a little relief.", MEDICINE, 5, 2, 0.2f, 45},
    {"Wound Poultice", "Medicine", "A packed leaf dressing that eases a fresh wound.", MEDICINE, 7, 3, 0.4f, 12},
    {"Ash Salts", "Medicine", "A sharp-smelling pinch that steadies a sick stomach.", MEDICINE, 4, 2, 0.1f, 90},
    {"Iron Nails", "Material", "A handful of bent nails, useful for repairs.", MATERIAL, 0, 1, 0.4f, -1},
    {"Waxed Thread", "Material", "Strong thread protected from rain.", MATERIAL, 0, 2, 0.1f, -1},
    {"Tinder Bundle", "Tool", "Dry bark and grass tied beneath oiled cloth.", TOOL, 0, 2, 0.3f, -1},
    {"Fishing Line", "Tool", "A coil of gut line with a crude bone hook.", TOOL, 0, 3, 0.2f, -1},
    {"Woodcutter's Axe", "Weapon", "A heavy working axe, slow but dependable.", WEAPON, 6, 5, 4.0f, -1, SLOT_WEAPON, 0, 1, "Hard-hitting chop", "Heavy and slow to aim"},
    {"Pilgrim's Coat", "Body Armor", "A patched coat lined against the mountain wind.", ARMOR, 2, 5, 3.0f, -1, SLOT_BODY, 1, 0, "Warm and dependable", "Bulky sleeves catch on brush"},
    {"Sling and Stones", "Weapon", "A simple sling with a pouch of smooth stones.", WEAPON, 4, 3, 0.8f, -1, SLOT_WEAPON, 2, 0, "Fast, accurate at range", "Low impact against heavy armor"},
    {"Copper Ring", "Material", "A worn ring, worth more as a keepsake than metal.", MATERIAL, 0, 3, 0.1f, -1},
    {"Honey Cakes", "Food", "Dense little cakes wrapped in waxed paper.", FOOD, 26, 2, 0.4f, 14},
    {"Herb Dumplings", "Food", "Small dumplings filled with bitter greens.", FOOD, 20, 1, 0.4f, 8},
    {"Reed Mat", "Tool", "A folded mat that keeps damp from soaking through.", TOOL, 0, 2, 1.2f, -1},
    {"Iron Longknife", "Weapon", "A balanced blade made for close, fast work.", WEAPON, 6, 6, 1.4f, -1, SLOT_WEAPON, 2, 0, "Accurate and light", "Cannot reach a distant foe"},
    {"Hunter's Spear", "Weapon", "A long ash shaft tipped with a leaf-shaped head.", WEAPON, 7, 7, 2.8f, -1, SLOT_WEAPON, 1, 0, "Keeps a target at reach", "Awkward in close quarters"},
    {"Ashwood Bow", "Weapon", "A springy bow with a short bundle of arrows.", WEAPON, 6, 8, 2.0f, -1, SLOT_WEAPON, 3, 1, "Precise opening shots", "Slow to ready under pressure"},
    {"Blacksmith Hammer", "Weapon", "A compact hammer with a weighted iron head.", WEAPON, 7, 7, 3.5f, -1, SLOT_WEAPON, 0, 1, "Crushes through hard hides", "Heavy head slows aim"},
    {"Warden's Blade", "Weapon", "A well-kept sword marked with an old road seal.", WEAPON, 8, 12, 2.7f, -1, SLOT_WEAPON, 2, 0, "Strong, balanced edge", "A costly blade to replace"},
    {"Bone-Handled Cleaver", "Weapon", "A butcher's cleaver sharpened for the wilds.", WEAPON, 8, 9, 3.2f, -1, SLOT_WEAPON, 0, 1, "Brutal damage", "Wide swing leaves an opening"},
    {"Boiled Leather Vest", "Body Armor", "Layered leather that turns aside glancing blows.", ARMOR, 3, 8, 4.0f, -1, SLOT_BODY, 1, 1, "Good protection for its weight", "Restricts a long stride"},
    {"Ringmail Shirt", "Body Armor", "A heavy shirt of interlocked iron rings.", ARMOR, 4, 12, 6.0f, -1, SLOT_BODY, 0, 2, "Excellent cut protection", "Heavy rings exhaust the wearer"},
    {"Warden's Shield", "Body Armor", "A road-warden's shield, scarred but sound.", ARMOR, 4, 11, 4.5f, -1, SLOT_BODY, 1, 1, "Strong guard and coverage", "Clumsy while travelling"},
    {"Fur Mantle", "Body Armor", "A thick mantle sewn from winter pelts.", ARMOR, 2, 7, 2.5f, -1, SLOT_BODY, 2, 0, "Warm without much weight", "Leaves the torso exposed"},
    {"Smoked Jerky", "Food", "Dry strips of seasoned meat that keep for weeks.", FOOD, 32, 2, 0.5f, 24},
    {"Oat Porridge", "Food", "A wrapped oat cake that softens in a little broth.", FOOD, 26, 1, 0.5f, 12},
    {"Field Dressing", "Medicine", "A padded wrap for a deep cut or torn muscle.", MEDICINE, 9, 4, 0.4f, -1},
    {"Sharpening Stone", "Tool", "A fine stone for keeping an edge true.", TOOL, 0, 4, 0.7f, -1},
    {"Beast Pelt", "Material", "A cured hide prized by tanners and armorers.", MATERIAL, 0, 6, 1.5f, -1},
    {"Wolf Fang", "Material", "A clean fang with a little value to a charm-maker.", MATERIAL, 0, 3, 0.1f, -1},
    {"Iron Token", "Material", "A stamped toll token taken from a road bandit.", MATERIAL, 0, 7, 0.2f, -1},
    {"Mire Scale", "Material", "A thick, iridescent scale from a marsh hunter.", MATERIAL, 0, 5, 0.3f, -1},
    {"Wight Ash", "Material", "Cold grey ash sealed in a small glass vial.", MATERIAL, 0, 8, 0.2f, -1},
    {"Reaver's Hook", "Material", "A sailor's hooked blade, still worth more than scrap.", MATERIAL, 0, 9, 1.0f, -1},
    {"Ridgeback Horn", "Material", "A curved horn valued by carvers and traders.", MATERIAL, 0, 4, 0.5f, -1},
    {"Boar Tusks", "Material", "Two sharp tusks, cleaned and bound together.", MATERIAL, 0, 5, 0.4f, -1},
    {"Snapper Shell", "Material", "A plated shell that can be polished into a buckler.", MATERIAL, 0, 4, 1.2f, -1},
    {"Shade Glass", "Material", "A dark shard left where a ruin shade faded.", MATERIAL, 0, 8, 0.2f, -1},
    {"Troll Knuckle", "Material", "A dense bone relic from a mountain brute.", MATERIAL, 0, 10, 1.0f, -1},
    {"Thorncat Claw", "Material", "A hooked claw sought by leatherworkers.", MATERIAL, 0, 6, 0.2f, -1},
    {"Drowned Signet", "Material", "A salt-eaten signet recovered from drowned mail.", MATERIAL, 0, 9, 0.1f, -1},
    {"Sentinel Cog", "Material", "A brass gear still turning from an ancient guardian.", MATERIAL, 0, 11, 0.8f, -1},
    {"Borderland Falchion", "Weapon", "A single-edged sword forged to clear brush and break a guard.", WEAPON, 3, 5, 2.2f, -1, SLOT_WEAPON, 1, 0, "Accurate against unarmored foes", "Shorter reach than a longsword"},
    {"Marsh-Reed Glaive", "Weapon", "A hooked polearm with a haft sealed against wet weather.", WEAPON, 5, 12, 4.2f, -1, SLOT_WEAPON, 0, 1, "Excellent reach and hard hits", "Bulky and slow to bring around"},
    {"Orchard Hatchet", "Weapon", "A light chopping blade balanced for quick work.", WEAPON, 3, 6, 1.8f, -1, SLOT_WEAPON, 2, 0, "Quick, nimble attacks", "Little weight behind a heavy blow"},
    {"Northpass Greatsword", "Weapon", "A broad two-handed blade made for the wind-cut pass.", WEAPON, 8, 31, 5.8f, -1, SLOT_WEAPON, 0, 2, "Devastating heavy strikes", "Slow, tiring, and difficult to aim"},
    {"Copperhead Rapier", "Weapon", "A narrow thrusting blade with a copper guard.", WEAPON, 4, 14, 1.5f, -1, SLOT_WEAPON, 3, 0, "Very precise thrusts", "Poor at forcing a blocked guard"},
    {"Grave-Iron Mace", "Weapon", "A dark iron head mounted on a short oak haft.", WEAPON, 6, 18, 3.4f, -1, SLOT_WEAPON, 0, 1, "Crushes bone and armor", "Slow recovery after a swing"},
    {"Fen Hunter's Trident", "Weapon", "Three narrow prongs shaped for close marsh hunting.", WEAPON, 5, 16, 3.0f, -1, SLOT_WEAPON, 1, 0, "Reliable control at reach", "Catches easily in thick brush"},
    {"Amberwood Longbow", "Weapon", "A tall bow of amberwood with a horn-reinforced grip.", WEAPON, 5, 22, 2.4f, -1, SLOT_WEAPON, 3, 1, "Accurate opening attacks", "Slow to draw under pressure"},
    {"Riveted War Pick", "Weapon", "A steel point and counterweight made to pierce mail.", WEAPON, 6, 20, 2.9f, -1, SLOT_WEAPON, 1, 1, "Punches through hard defense", "Unbalanced for quick feints"},
    {"Hearthguard Flail", "Weapon", "A short chain links the iron ball to a stout grip.", WEAPON, 7, 24, 3.8f, -1, SLOT_WEAPON, 0, 2, "Heavy impact can overwhelm a guard", "Wide, unpredictable swing"},
    {"Coastline Cutlass", "Weapon", "A salt-resistant curved blade made for close decks.", WEAPON, 4, 13, 2.0f, -1, SLOT_WEAPON, 2, 0, "Fast and steady in close combat", "Lacks reach on open ground"},
    {"Pilgrim's Ash Spear", "Weapon", "A plain ash spear with a steel point and leather wrap.", WEAPON, 4, 11, 2.5f, -1, SLOT_WEAPON, 2, 0, "Long reach with accurate jabs", "Cannot deliver a crushing blow"},
    {"Blackfen Recurve", "Weapon", "A compact bow laminated for damp forest trails.", WEAPON, 4, 19, 1.8f, -1, SLOT_WEAPON, 3, 1, "Quick, precise first shot", "Light arrows struggle against armor"},
    {"Stonebreaker Maul", "Weapon", "A two-handed iron maul with a stone-set head.", WEAPON, 8, 35, 7.0f, -1, SLOT_WEAPON, 0, 3, "The hardest-hitting weapon", "Extremely slow and exhausting"},
    {"Twin-Ridge Hatchets", "Weapon", "A matched pair of short axes with rawhide grips.", WEAPON, 5, 21, 2.7f, -1, SLOT_WEAPON, 2, 1, "Fast follow-up strikes", "Requires a committed, open stance"},
    {"Silvered Thornblade", "Weapon", "A slender sword etched with pale runes for shades.", WEAPON, 6, 29, 2.1f, -1, SLOT_WEAPON, 2, 0, "Accurate; bites into unnatural foes", "Delicate silver edge is costly"},
    {"Miller's Crossbow", "Weapon", "A compact windlass bow that hits hard when readied.", WEAPON, 7, 27, 4.1f, -1, SLOT_WEAPON, 2, 2, "Powerful, steady bolt", "Slow reload and heavy frame"},
    {"Redoubt Arming Sword", "Weapon", "A stout, hand-and-a-half sword built for the shield wall.", WEAPON, 6, 25, 3.1f, -1, SLOT_WEAPON, 1, 0, "Dependable balance and reach", "No special advantage at range"},
    {"Marrowbone Knife", "Weapon", "A keen bone-handled knife that never rusts.", WEAPON, 2, 8, 0.7f, -1, SLOT_WEAPON, 3, 0, "Very fast and easy to aim", "Low damage against armor"},
    {"Waykeeper's Longsword", "Weapon", "A master-balanced road sword bearing an old keeper's mark.", WEAPON, 7, 38, 2.8f, -1, SLOT_WEAPON, 3, 0, "Elite damage, reach, and accuracy", "Rare and expensive to maintain"},
    {"Padded Coif", "Helmet", "A quilted cap that cushions glancing blows.", ARMOR, 1, 4, 0.6f, -1, SLOT_HELMET, 0, 0, "Comfortable basic head cover", "Minimal protection from a hard strike"},
    {"Boarhide Skullcap", "Helmet", "Thick hide stiffened with horn strips.", ARMOR, 2, 8, 1.4f, -1, SLOT_HELMET, 0, 0, "Light, dependable head armor", "Restricts hearing in the brush"},
    {"Iron Nasal Helm", "Helmet", "A bowl helm with a narrow bar over the nose.", ARMOR, 3, 14, 2.3f, -1, SLOT_HELMET, 0, 1, "Strong forehead protection", "The brow bar narrows your view"},
    {"Warden's Open Helm", "Helmet", "A road warden's helm cut open at the cheeks.", ARMOR, 2, 13, 1.8f, -1, SLOT_HELMET, 1, 0, "Clear sight and solid defense", "Leaves the jaw exposed"},
    {"Ridgewalker Hood", "Helmet", "A waxed hood with a fur-lined brow.", ARMOR, 1, 9, 0.8f, -1, SLOT_HELMET, 1, 0, "Helps shed cold rain", "Little protection against weapons"},
    {"Ringed Sallet", "Helmet", "A close-fitting sallet reinforced with iron rings.", ARMOR, 3, 19, 2.6f, -1, SLOT_HELMET, 0, 1, "Reliable all-around coverage", "Heavy around the neck"},
    {"Brass Lantern Mask", "Helmet", "A slotted brass face guard used in dark ruins.", ARMOR, 2, 17, 2.1f, -1, SLOT_HELMET, 1, 0, "Bright metal turns aside glancing blows", "Slits dim peripheral vision"},
    {"Fen-Reed Hat", "Helmet", "A layered reed hat lacquered against marsh rain.", ARMOR, 1, 7, 0.5f, -1, SLOT_HELMET, 1, 0, "Excellent rain cover for little weight", "Poor defense against a direct hit"},
    {"Antler-Crested Cap", "Helmet", "A leather cap framed by polished antler points.", ARMOR, 2, 12, 1.7f, -1, SLOT_HELMET, 0, 0, "Good cheek and temple cover", "Antlers snag on low branches"},
    {"Blacksmith's Pot Helm", "Helmet", "A thick iron pot helm with a padded lining.", ARMOR, 4, 25, 3.8f, -1, SLOT_HELMET, 0, 2, "Exceptional head protection", "Very heavy and exhausting"},
    {"Pilgrim's Star Hood", "Helmet", "A wool hood stitched with a small brass star.", ARMOR, 1, 10, 0.7f, -1, SLOT_HELMET, 2, 0, "Warm, light, and weatherproof", "Offers little physical defense"},
    {"Tideward Helm", "Helmet", "A rounded helm sealed against salt spray.", ARMOR, 3, 22, 2.9f, -1, SLOT_HELMET, 1, 1, "Excellent defense in foul weather", "Salt seal makes it cumbersome"},
    {"Grave-Iron Visor", "Helmet", "A dark visor hammered from old grave iron.", ARMOR, 4, 29, 3.4f, -1, SLOT_HELMET, 0, 2, "Heavy plate wards the skull", "Dim sight and stiff neck"},
    {"Foxfur Warband", "Helmet", "A fur brow-band hiding a thin plate of steel.", ARMOR, 2, 15, 1.0f, -1, SLOT_HELMET, 1, 0, "Warm ears and clear vision", "Open crown leaves the top exposed"},
    {"Copper Rivet Barbute", "Helmet", "A flared barbute edged with copper rivets.", ARMOR, 3, 21, 2.5f, -1, SLOT_HELMET, 1, 1, "Good face and neck protection", "Weight pulls at the shoulders"},
    {"Ashen Scout's Cowl", "Helmet", "A charcoal cowl over a flexible brow plate.", ARMOR, 2, 18, 1.2f, -1, SLOT_HELMET, 2, 0, "Quiet, warm, and easy to move in", "Soft sides leave the temples exposed"},
    {"Cairn-Keeper's Crown", "Helmet", "A rune-cut iron crown worn by ruin wardens.", ARMOR, 4, 34, 3.1f, -1, SLOT_HELMET, 1, 2, "Excellent defense and cold resistance", "Heavy rim presses on the brow"},
    {"Stormwatch Sallet", "Helmet", "A visor helm with a drain channel for driving rain.", ARMOR, 3, 27, 2.7f, -1, SLOT_HELMET, 2, 1, "Strong cover with a weather seal", "Rain guard restricts upward vision"},
    {"Wayfarer's Riveted Hood", "Helmet", "A travel hood with small plates sewn beneath the cloth.", ARMOR, 2, 16, 1.3f, -1, SLOT_HELMET, 1, 0, "Comfortable protection on long roads", "Rivets can catch on straps"},
    {"Dawnsteel Greathelm", "Helmet", "A polished full helm made for the Marches' finest guard.", ARMOR, 5, 48, 4.2f, -1, SLOT_HELMET, 2, 3, "The best protection for the head", "Very heavy with severely limited sight"},
    {"Quilted Gambeson", "Body Armor", "A layered linen coat that softens cuts and bruises.", ARMOR, 1, 5, 2.0f, -1, SLOT_BODY, 1, 0, "Warm, quiet, and comfortable", "Cannot stop a direct heavy blow"},
    {"Tanner's Scale Vest", "Body Armor", "Overlapping leather scales tied with waxed cord.", ARMOR, 2, 10, 3.8f, -1, SLOT_BODY, 1, 1, "Flexible protection across the ribs", "Lacing strains during a long march"},
    {"Miller's Ring Jacket", "Body Armor", "Iron rings sewn onto a thick workman's jacket.", ARMOR, 3, 17, 5.5f, -1, SLOT_BODY, 0, 2, "Good defense against cuts", "Heavy rings wear down stamina"},
    {"Amber Compact Brigandine", "Body Armor", "Small plates hidden beneath a merchant's dark coat.", ARMOR, 4, 28, 6.0f, -1, SLOT_BODY, 0, 2, "Excellent protection without a metal shell", "Weight slows travel and recovery"},
    {"Fenwalker Waxcoat", "Body Armor", "A long waxed coat with a high split collar.", ARMOR, 1, 9, 2.5f, -1, SLOT_BODY, 2, 0, "Excellent rain and cold protection", "Thin cloth offers little armor"},
    {"Boarhide Cuirass", "Body Armor", "Cured boarhide layered over a padded vest.", ARMOR, 3, 20, 5.0f, -1, SLOT_BODY, 1, 1, "Resilient against claws and teeth", "Stiff hide tires the wearer"},
    {"Warden's Lamellar", "Body Armor", "Lacquered iron plates laced over a wool coat.", ARMOR, 4, 32, 6.8f, -1, SLOT_BODY, 1, 2, "Excellent defense in a disciplined stance", "Heavy plates impede a fast retreat"},
    {"Coast Reaver's Jack", "Body Armor", "A tarred leather jack reinforced with scavenged rings.", ARMOR, 3, 23, 4.8f, -1, SLOT_BODY, 1, 1, "Blocks cuts and resists wet weather", "Tarred layers run hot on the road"},
    {"Pilgrim's Wool Mantle", "Body Armor", "A thick mantle woven for high passes and cold nights.", ARMOR, 2, 12, 2.8f, -1, SLOT_BODY, 3, 0, "Warmth and good freedom of movement", "Leaves the shoulders lightly guarded"},
    {"Grave-Iron Hauberk", "Body Armor", "A black chain hauberk recovered from an old barrow.", ARMOR, 5, 42, 8.4f, -1, SLOT_BODY, 0, 3, "Outstanding protection from weapons", "Extremely heavy and tiring"},
    {"Tidepool Shellcoat", "Body Armor", "Small polished shells overlap across a padded coat.", ARMOR, 3, 19, 4.4f, -1, SLOT_BODY, 1, 1, "Hard plates resist bites", "Shell edges catch on clothing"},
    {"Ridgeback Scale Mail", "Body Armor", "Mountain scales are riveted to a leather backing.", ARMOR, 4, 35, 7.0f, -1, SLOT_BODY, 1, 2, "Hard to pierce and sheds cold rain", "Heavy scales exhaust the wearer"},
    {"Thorncat Hidecoat", "Body Armor", "A close-woven hide jacket with thorn-resistant sleeves.", ARMOR, 3, 26, 4.1f, -1, SLOT_BODY, 1, 0, "Light, flexible, and hard to tear", "Less protection at the waist"},
    {"Redoubt Half-Plate", "Body Armor", "A steel breastplate fitted over a short mail skirt.", ARMOR, 5, 45, 8.0f, -1, SLOT_BODY, 0, 3, "Exceptional torso protection", "Heavy, hot, and hard to climb in"},
    {"Mossguard Scale Coat", "Body Armor", "Green-stained scales blend into forest and marsh.", ARMOR, 3, 25, 4.7f, -1, SLOT_BODY, 2, 1, "Weather resistant with good mobility", "Uneven scales leave gaps"},
    {"Lantern Rest Laminate", "Body Armor", "Laminated ash plates under a traveler's quilt.", ARMOR, 2, 15, 3.4f, -1, SLOT_BODY, 2, 0, "Warm, light, and moderately protective", "Wooden plates can split under a maul"},
    {"Blackfen Chain Shirt", "Body Armor", "Fine dark links cover the heart and upper ribs.", ARMOR, 4, 37, 5.7f, -1, SLOT_BODY, 0, 2, "Strong protection with a compact fit", "Metal links drain warmth and stamina"},
    {"Waykeeper's Harness", "Body Armor", "A leather harness with steel splints and travel straps.", ARMOR, 3, 30, 3.6f, -1, SLOT_BODY, 2, 0, "Light, balanced, and weatherwise", "Open gaps expose the lower torso"},
    {"Dawnsteel Warcoat", "Body Armor", "A master-fitted coat backed by bright steel scales.", ARMOR, 5, 55, 6.4f, -1, SLOT_BODY, 2, 1, "Top-tier defense without full plate weight", "Costly and stiff at the shoulders"},
    {"Mender's Quilted Vest", "Body Armor", "A reinforced vest designed to protect without hindering work.", ARMOR, 2, 13, 2.6f, -1, SLOT_BODY, 1, 0, "Comfortable and easy to repair", "Limited protection against blades"},
    {"Patchwork Gauntlets", "Gloves", "Leather palms and linen cuffs stitched from scraps.", ARMOR, 1, 3, 0.4f, -1, SLOT_GLOVES, 0, 0, "Keeps grip steady in wet weather", "Little protection for the fingers"},
    {"Tanner's Grip Gloves", "Gloves", "Soft hide gloves with a ridged gripping palm.", ARMOR, 1, 7, 0.5f, -1, SLOT_GLOVES, 1, 0, "Improves weapon control", "Thin across the knuckles"},
    {"Iron-Rim Bracers", "Gloves", "Leather bracers capped with narrow iron strips.", ARMOR, 2, 13, 1.2f, -1, SLOT_GLOVES, 0, 0, "Protects wrists from glancing blows", "Iron rims catch on sleeves"},
    {"Warden's Riding Gloves", "Gloves", "Close-fitting gloves with stitched steel knuckle pads.", ARMOR, 2, 16, 0.8f, -1, SLOT_GLOVES, 1, 0, "Good grip and hand protection", "Tight seams reduce finger warmth"},
    {"Fenweave Mitts", "Gloves", "Waxed reed cloth keeps the palms dry and warm.", ARMOR, 1, 9, 0.4f, -1, SLOT_GLOVES, 1, 0, "Dry grip in rain and marsh fog", "Poor defense against a hard strike"},
    {"Blacksmith's Half-Gauntlets", "Gloves", "Riveted cuffs guard the wrist while leaving fingers free.", ARMOR, 3, 21, 1.7f, -1, SLOT_GLOVES, 0, 1, "Strong wrist and knuckle armor", "Weight slows a precise strike"},
    {"Pilgrim's Palm Wraps", "Gloves", "Wool and leather wraps protect the palms on long walks.", ARMOR, 1, 8, 0.3f, -1, SLOT_GLOVES, 2, 0, "Excellent comfort and steady grip", "Almost no protection from blades"},
    {"Coast Reaver's Hooks", "Gloves", "Salt-stiff gloves with reinforced hooked fingers.", ARMOR, 2, 14, 0.7f, -1, SLOT_GLOVES, 1, 0, "Secure grip on rope and weapon", "Hooks snag on cloth and brush"},
    {"Boarhide Knuckle Guards", "Gloves", "Thick hide covers the knuckles and heel of the hand.", ARMOR, 2, 15, 0.9f, -1, SLOT_GLOVES, 0, 0, "Reliable protection in a brawl", "Stiff hide limits fine movement"},
    {"Grave-Iron Fists", "Gloves", "Dark plates shield the hands beneath a chain cuff.", ARMOR, 4, 31, 2.2f, -1, SLOT_GLOVES, 0, 2, "Exceptional hand protection", "Very heavy and poor for delicate work"},
    {"Ashford Archer's Bracer", "Gloves", "A smooth leather bracer protects the bow hand.", ARMOR, 1, 12, 0.5f, -1, SLOT_GLOVES, 2, 0, "Improves aim and bow control", "Only one side is well protected"},
    {"Ringmail Mitts", "Gloves", "Fine links sewn over padded wool fingers.", ARMOR, 3, 23, 1.6f, -1, SLOT_GLOVES, 1, 1, "Good coverage and steady grip", "Links tire the hand"},
    {"Thorncat Clawguards", "Gloves", "Flexible leather guards stitched with claw-resistant plates.", ARMOR, 2, 19, 0.8f, -1, SLOT_GLOVES, 2, 0, "Quick, sure grip in close combat", "Short cuffs leave the wrist exposed"},
    {"Tideward Scale Gloves", "Gloves", "Small shell scales cover fingers beneath waxed cuffs.", ARMOR, 3, 24, 1.3f, -1, SLOT_GLOVES, 1, 1, "Hard shell guards the fingers", "Stiff scales dull fine grip"},
    {"Redoubt Steel Vambraces", "Gloves", "Long steel vambraces protect the hand to the elbow.", ARMOR, 4, 34, 2.5f, -1, SLOT_GLOVES, 0, 2, "Outstanding arm and hand defense", "Heavy arms slow attacks"},
    {"Foxfur Grip Mitts", "Gloves", "Fur-lined mitts split at the palm for a weapon grip.", ARMOR, 1, 11, 0.7f, -1, SLOT_GLOVES, 1, 0, "Warmth with a secure grip", "Poor finger dexterity"},
    {"Lantern Rest Fingerplates", "Gloves", "Thin brass scales guard the fingers without closing the palm.", ARMOR, 2, 18, 0.6f, -1, SLOT_GLOVES, 2, 0, "Precise, light, and protective", "Brass bends under a heavy blow"},
    {"Waykeeper's Gauntlets", "Gloves", "Balanced steel-and-leather gauntlets for the road patrol.", ARMOR, 3, 29, 1.5f, -1, SLOT_GLOVES, 2, 0, "Excellent control and solid coverage", "Costly to replace"},
    {"Dawnsteel Handguards", "Gloves", "Master-made articulated plates over a fine leather glove.", ARMOR, 5, 46, 2.0f, -1, SLOT_GLOVES, 2, 1, "Elite protection with nimble joints", "Expensive and tiring over time"},
    {"Mender's Soft Gloves", "Gloves", "Clean linen gloves with padded leather fingertips.", ARMOR, 1, 10, 0.4f, -1, SLOT_GLOVES, 2, 0, "Excellent dexterity and comfort", "Not made for heavy blows"},
    {"Worn Road Boots", "Boots", "Soft leather boots with one patched sole.", ARMOR, 1, 3, 1.1f, -1, SLOT_BOOTS, 0, 0, "Light and quiet on the trail", "Thin sole gives little protection"},
    {"Tanner's Trail Boots", "Boots", "Double-stitched boots with a tough leather sole.", ARMOR, 1, 8, 1.5f, -1, SLOT_BOOTS, 1, 0, "Reduces fatigue on long journeys", "Worn leather leaks in deep water"},
    {"Ridgewalker Greaves", "Boots", "High boots wrapped with leather shin guards.", ARMOR, 2, 14, 2.4f, -1, SLOT_BOOTS, 0, 1, "Protects ankles on rocky ground", "Heavy steps tire you faster"},
    {"Warden's March Boots", "Boots", "Road-warden boots with a shock-absorbing sole.", ARMOR, 2, 18, 2.0f, -1, SLOT_BOOTS, 2, 0, "Excellent travel comfort and ankle cover", "Stiff leather needs breaking in"},
    {"Fenwalker Reed Boots", "Boots", "Waxed boots packed with reed-fiber insulation.", ARMOR, 1, 11, 1.7f, -1, SLOT_BOOTS, 2, 0, "Keeps feet warm and dry", "Soft soles wear quickly on stone"},
    {"Iron-Toed Work Boots", "Boots", "A thick work boot capped with a small iron plate.", ARMOR, 3, 17, 2.8f, -1, SLOT_BOOTS, 0, 1, "Strong toe and foot protection", "Iron cap adds weight"},
    {"Pilgrim's Felt Boots", "Boots", "Layered felt soles cushion a long walk.", ARMOR, 1, 10, 1.2f, -1, SLOT_BOOTS, 2, 0, "Very comfortable and warm", "Offers little protection from a stomp"},
    {"Boarhide Treaders", "Boots", "Thick boarhide boots with a deep-grip sole.", ARMOR, 2, 16, 2.1f, -1, SLOT_BOOTS, 1, 0, "Secure footing on mud and grass", "Hide dries stiff after rain"},
    {"Grave-Iron Sabatons", "Boots", "Dark metal plates guard the shin and instep.", ARMOR, 4, 32, 4.5f, -1, SLOT_BOOTS, 0, 2, "Excellent lower-leg defense", "Very heavy on a long march"},
    {"Ashford Stable Boots", "Boots", "Flexible boots built for farm work and stable paths.", ARMOR, 1, 7, 1.4f, -1, SLOT_BOOTS, 1, 0, "Easy to move and comfortable", "Limited protection on rough ground"},
    {"Tidepool Scale Greaves", "Boots", "Shell scales overlap the shin above a sealed boot.", ARMOR, 3, 23, 3.1f, -1, SLOT_BOOTS, 1, 1, "Hard lower-leg protection", "Scale edges catch while climbing"},
    {"Thorncat Stalkers", "Boots", "Quiet-soled boots patterned after the forest hunter.", ARMOR, 1, 20, 1.3f, -1, SLOT_BOOTS, 3, 0, "Very light, quiet, and low-fatigue", "Thin soles wear down quickly"},
    {"Redoubt Greaves", "Boots", "Knee-high greaves fitted over reinforced road boots.", ARMOR, 4, 36, 4.0f, -1, SLOT_BOOTS, 0, 2, "High defense for the legs", "Restricts a quick retreat"},
    {"Miller's Mudboots", "Boots", "Tall boots sealed with wax for flooded tracks.", ARMOR, 1, 12, 2.0f, -1, SLOT_BOOTS, 1, 0, "Keeps feet dry in marsh and rain", "Warm and clumsy in summer"},
    {"Blackfen Chain Boots", "Boots", "Fine chain protects the ankle under a leather shell.", ARMOR, 3, 27, 3.5f, -1, SLOT_BOOTS, 1, 1, "Strong ankle coverage", "Links add weight and noise"},
    {"Lantern Rest Walkers", "Boots", "Soft-soled boots with a springy ashwood heel.", ARMOR, 2, 19, 1.8f, -1, SLOT_BOOTS, 2, 0, "Reduces travel fatigue and cushions stone", "Heel can crack under heavy loads"},
    {"Foxfur Winter Boots", "Boots", "Fur-lined boots made for bitter winter roads.", ARMOR, 2, 22, 2.6f, -1, SLOT_BOOTS, 2, 0, "Excellent warmth with good comfort", "Bulky in mud and warm weather"},
    {"Waykeeper's Spurs", "Boots", "Fitted patrol boots with a braced heel and steel toe.", ARMOR, 3, 31, 2.9f, -1, SLOT_BOOTS, 2, 1, "Good protection and steady pace", "Spurs snag in dense undergrowth"},
    {"Dawnsteel Sabatons", "Boots", "Articulated steel plates fitted to a master boot.", ARMOR, 5, 49, 4.8f, -1, SLOT_BOOTS, 1, 3, "The finest protection for the feet", "Very heavy and exhausting"},
    {"Mender's Softstep Boots", "Boots", "Padded boots with a flexible sole and stitched ankle wrap.", ARMOR, 1, 15, 1.4f, -1, SLOT_BOOTS, 3, 0, "Extremely comfortable over distance", "Little defense against a crushing blow"},
    {"Bogglass Eye", "Material", "A cloudy lens left in the nest of a fen basilisk.", MATERIAL, 0, 12, 0.2f, -1},
    {"Rimewolf Pelt", "Material", "A silver-grey pelt thick enough to turn the mountain wind.", MATERIAL, 0, 11, 1.8f, -1},
    {"Manticore Quill", "Material", "A barbed quill whose venom has long since dried.", MATERIAL, 0, 15, 0.2f, -1},
    {"Grave Moth Wing", "Material", "A powdery wing found near cold barrows.", MATERIAL, 0, 9, 0.1f, -1},
    {"Thunder Roc Feather", "Material", "A broad feather scorched at the tip by stormlight.", MATERIAL, 0, 14, 0.2f, -1},
    {"Cavebear Claw", "Material", "A hooked claw from a bear that sleeps beneath the ridge.", MATERIAL, 0, 13, 0.4f, -1},
    {"Briarling Heartwood", "Material", "A knotted seed-heart still smelling of wet earth.", MATERIAL, 0, 10, 0.6f, -1},
    {"Wyrm Scale", "Material", "A dark scale warm to the touch long after the beast is gone.", MATERIAL, 0, 18, 0.5f, -1},
    {"Salt Wraith Chain", "Material", "A link of corroded chain that never quite dries.", MATERIAL, 0, 13, 0.4f, -1},
    {"Sunken Crown Shard", "Material", "A bright fragment from a relic guarded below the coast.", MATERIAL, 0, 20, 0.3f, -1}
};
static const int ITEM_COUNT = sizeof(ITEMS) / sizeof(ITEMS[0]);
static const int FIRST_NEW_WEAPON = 62;
static const int FIRST_NEW_HELMET = 82;
static const int FIRST_NEW_BODY_ARMOR = 102;
static const int FIRST_NEW_GLOVES = 122;
static const int FIRST_NEW_BOOTS = 142;
static const int MONSTER_LOOT_FIRST_ITEM = 48;
static const int MONSTER_LOOT_NEW_FIRST_ITEM = 162;
static const int MONSTER_LOOT_NEW_COUNT = 10;

struct Creature
{
    const char *name;
    const char *terrain;
    const char *personality;
    const char *ability;
    const char *temper;
    const char *warning;
    int hp;
    int attack;
    int defense;
    int morale;
    int faction;
};

static const Creature CREATURES[] = {
    {"Road Outlaw", "old road", "Cautious and greedy; tests your nerve before committing.", "Feint: a false opening makes the next strike harder to avoid.", "CUNNING", "The knife stays low; the outlaw keeps glancing at your pack.", 19, 5, 11, 7, 0},
    {"Gaunt Wolf", "forest", "A patient hunter. It circles until you turn your back.", "Pounce: a sudden rush deals a heavier blow.", "PATIENT", "Its ribs show. It snarls, but watches for an easier meal.", 16, 6, 12, 5, -1},
    {"Hill Marauder", "mountains", "Loud, proud, and quick to anger when challenged.", "Crushing swing: a slow attack with punishing force.", "FIERCE", "The heavy axe rises slowly; each breath sounds ragged.", 23, 7, 10, 8, 1},
    {"Reed Stalker", "marsh", "Silent and watchful; it waits in the reeds for weakness.", "Snare: tangled roots leave you weakened for a turn.", "WATCHFUL", "Reeds bend around a still shape; the mud is full of footprints.", 18, 5, 12, 6, -1},
    {"Feral Dog", "farmland", "Nervous and hungry; it snaps, then looks for an escape.", "Pack rush: a second bite follows its first attack.", "SKITTISH", "A thin dog bares its teeth, then backs toward the hedgerow.", 14, 4, 11, 3, -1},
    {"Barrow Wight", "ruins", "An old guardian bound to a promise it no longer understands.", "Grave chill: the cold steals strength from an unwary foe.", "BOUND", "The grave soil stirs; a blue light gutters beneath the stone.", 21, 6, 12, 6, -1},
    {"Coast Reaver", "coast", "A hard sailor who would rather bargain than drown far from shore.", "Hook and haul: pulls you off balance before striking.", "WARY", "Salt-crusted mail and a hooked blade; one leg is badly wounded.", 20, 6, 11, 4, 0},
    {"Mire Boar", "marsh", "Territorial and easily startled, but not a hunter of people.", "Rush: a wounded charge can knock you prone.", "TERRITORIAL", "The mud trembles. A bristled shape roots at the ground.", 22, 7, 10, 5, -1},
    {"Compact Tollguard", "old road", "Strict, tired, and willing to hear a credible explanation.", "Shield wall: holds ground and punishes reckless attacks.", "DUTIFUL", "The painted shield bears the Amber Compact's mark.", 21, 5, 13, 8, 0},
    {"Pale Marsh Heron", "marsh", "A wary bird that only attacks when cornered.", "Wing-blind: flurries of feathers obscure a clear strike.", "SKITTISH", "A wounded wing drags in the shallows; it has nowhere to fly.", 12, 4, 12, 2, -1},
    {"Grassland Prowler", "open country", "A lean hunter that shadows the grassline, testing every traveler.", "Grass rush: it darts through cover and gains force on its first strike.", "CIRCLING", "The grass parts in a long, low line; bright eyes follow your pace.", 17, 5, 12, 4, -1},
    {"Ridgeback Goat", "mountains", "A stubborn animal defending its ledge, more frightened than cruel.", "Stonefall: a hoof strike can send loose rock tumbling.", "STUBBORN", "A horned silhouette blocks the narrow ledge above a scatter of fresh stones.", 18, 5, 12, 3, -1},
    {"Fen Crocodile", "marsh", "A patient ambusher that would rather wait than chase.", "Silt drag: a sudden pull can leave a traveler prone.", "PATIENT", "Only its eyes break the dark water; ripples widen against the current.", 20, 6, 11, 5, -1},
    {"Ruin Shade", "ruins", "A broken oath given shape, drawn toward noise and living warmth.", "Cold grasp: its touch saps heat and courage.", "HOLLOW", "A shadow stands where no wall can cast one; the air tastes of old iron.", 19, 6, 12, 6, -1},
    {"Tidepool Snapper", "coast", "A territorial shell-backed scavenger guarding its stretch of shore.", "Crushing clamp: a slow bite can leave a hand numb.", "TERRITORIAL", "A plated shape shifts among the tide pools, claws clicking on stone.", 16, 5, 12, 3, -1},
    {"Hedgerow Boar", "farmland", "A hungry sow protecting the ditch where her piglets hide.", "Hedge charge: it bursts through thorn and can knock you down.", "PROTECTIVE", "The hedge shivers. A bristled back rises between you and the field.", 21, 6, 11, 5, -1},
    {"Thorncat", "forest", "A solitary ambusher that guards its hunting ground.", "Briar rake: hooked claws open a bleeding wound.", "STALKING", "A pair of amber eyes follows you from inside a wall of thorn.", 18, 6, 12, 5, -1},
    {"Slate Troll", "mountains", "A slow, stubborn brute that treats every trail as its own.", "Stone fist: a crushing blow can leave you stunned.", "UNYIELDING", "Freshly split rock and huge footprints lead toward a hulking shape.", 27, 8, 10, 9, -1},
    {"Drowned Deckhand", "coast", "A drowned sailor driven by the last order they heard.", "Anchor drag: a chain pulls you off balance.", "DRIVEN", "A salt-stiff uniform walks against the tide, chain scraping stone.", 23, 7, 11, 7, -1},
    {"Ironbound Sentinel", "ruins", "A brass guardian still obeying a command from a vanished house.", "Hammer pulse: an iron strike can stun and break guard.", "WATCHFUL", "A gear turns in the wall; a plated figure blocks the old arch.", 25, 7, 13, 8, -1},
    {"Thicket Stag", "farmland", "A wounded stag defending its young with desperate force.", "Antler rush: a charge can knock you prone.", "DEFENSIVE", "The hedge parts around a bleeding stag, head lowered toward the road.", 19, 7, 11, 4, -1},
    {"Ashen Shade", "ruins", "A cold echo that gathers where old promises were broken.", "Dread chill: its touch saps warmth and courage.", "HOLLOW", "The lantern flame gutters as a second shadow detaches from the wall.", 22, 6, 13, 7, -1},
    {"Fen Basilisk", "marsh", "A patient reptile that watches through reeds and glassy water.", "Petrifying stare: fear slows your next strike.", "STILL", "A scaled head rises above the black water; its unblinking eyes catch the light.", 24, 7, 13, 7, -1},
    {"Rimewolf", "mountains", "A pack leader that tests a traveler before calling the others.", "Whiteout rush: a sudden lunge leaves you shaken.", "PACKWISE", "Pale tracks circle your camp. One broad pawprint stops beside your bedroll.", 25, 7, 13, 7, -1},
    {"Manticore", "open country", "A wounded, proud hunter that guards the high grass with its tail.", "Barbed lash: a venomous sting worsens an old wound.", "PROUD", "A spined tail cuts through the grass before the beast shows its face.", 27, 8, 12, 8, -1},
    {"Grave Moth Swarm", "ruins", "A cloud of pale wings drawn to warmth and buried names.", "Dust veil: its wings steal breath and obscure your guard.", "RESTLESS", "White wings lift from the stones; beneath them, something has just disturbed the grave dust.", 20, 6, 12, 5, -1},
    {"Thunder Roc", "mountains", "A great cliff-nesting bird that strikes only when its nest is threatened.", "Storm dive: a wingbeat knocks you from your footing.", "DEFENSIVE", "A shadow passes over the pass; the mountain answers with a sharp cry.", 28, 8, 12, 6, -1},
    {"Cavebear", "forest", "An old bear that defends its den, then retreats if bloodied.", "Denward crush: a heavy paw can stun.", "PROTECTIVE", "The pines shake. A scarred bear stands between you and a hollow in the roots.", 29, 8, 11, 8, -1},
    {"Briarling", "forest", "A small thorn-bound guardian that lashes out when its grove is harmed.", "Root snare: twisted vines weaken your legs.", "BOUND", "The hedge moves against the wind; thorns gather into a hunched shape.", 21, 6, 13, 6, -1},
    {"Ash Wyrm", "ruins", "A half-grown drake nesting in warm stone and old kiln vents.", "Cinder breath: heat and ash leave you exposed.", "HUNGRY", "A seam in the ruin exhales smoke. Something scales the wall inside.", 30, 8, 12, 8, -1},
    {"Salt Wraith", "coast", "A sailor's shade tethered to a drowned vessel and its last bell.", "Drowning pull: cold chain and fear drag at your strength.", "TETHERED", "A chain moves beneath the tide, though no hand holds its far end.", 26, 7, 13, 7, -1},
    {"Sunken Crown Guardian", "coast", "An ancient armored sentinel that guards a relic below the tide line.", "Undertow sweep: a measured blow can leave you prone.", "VIGILANT", "A crown glints beneath the surf; iron joints rise from the shallows.", 32, 9, 14, 9, -1}
};
static const int CREATURE_COUNT = sizeof(CREATURES) / sizeof(CREATURES[0]);

struct CreatureLoot
{
    int item;
    int bonusItem;
    int bonusChance;
};
static const CreatureLoot CREATURE_LOOT[] = {
    {50, 38, 20}, {48, 49, 15}, {58, 38, 12}, {51, 49, 15},
    {48, 54, 10}, {52, 41, 8}, {53, 38, 16}, {55, 48, 10},
    {50, 42, 12}, {51, 49, 8}, {48, 49, 15}, {54, 35, 10},
    {51, 48, 12}, {57, 52, 15}, {56, 42, 10}, {55, 48, 12},
    {59, 48, 14}, {58, 41, 10}, {60, 53, 14}, {61, 42, 12},
    {55, 48, 10}, {57, 52, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 0, 53, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 1, 48, 14},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 2, 54, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 3, 52, 10},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 4, 55, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 5, 50, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 6, 48, 10},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 7, 59, 14},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 8, 56, 12},
    {MONSTER_LOOT_NEW_FIRST_ITEM + 9, 60, 16}
};
static const int MONSTER_ENCOUNTER_VARIANTS_PER_LAYER = 10;
static const int MONSTER_ENCOUNTER_COUNT =
    MONSTER_ENCOUNTER_VARIANTS_PER_LAYER * MONSTER_ENCOUNTER_VARIANTS_PER_LAYER;
static const char *MONSTER_ENCOUNTER_SIGNS[] = {
    "Fresh tracks cross the path and vanish beneath",
    "A low growl rolls out from",
    "The birds fall silent near",
    "Something has torn through the brush beside",
    "A sharp scent of blood hangs over",
    "The ground trembles once beneath",
    "A warning cry carries from beyond",
    "The road narrows where",
    "The wind changes as you approach",
    "A single set of eyes watches from"
};
static const char *MONSTER_ENCOUNTER_TURNS[] = {
    "It has seen you, but has not yet chosen to close.",
    "It guards its ground, watching for a careless step.",
    "The creature is wounded; pain has made it dangerous.",
    "It is feeding and may be driven off or left alone.",
    "It circles downwind, testing whether you are an easy meal.",
    "A narrow path offers cover if you would rather slip away.",
    "Its warning is clear: one more step may begin a fight.",
    "It pauses at the edge of the light, wary of your resolve.",
    "The animal turns aside, though it keeps you in sight.",
    "You have a moment to retreat before it commits."
};

enum EventKind
{
    EVENT_SUPPLIES,
    EVENT_RUMOR,
    EVENT_WEATHER,
    EVENT_HAZARD,
    EVENT_HEALING,
    EVENT_NPC,
    EVENT_FESTIVAL,
    EVENT_CELEBRATION,
    EVENT_DILEMMA,
    EVENT_QUIET,
    EVENT_TRADE,
    EVENT_FACTION,
    EVENT_STORY
};

struct RoadEvent
{
    const char *title;
    const char *description;
    const char *outcome;
    EventKind kind;
    int biome;
    int weather;
    int season;
    int weight;
    int storyId;
    int storyStage;
    int faction;
};

// biome/weather/season/faction use -1 as a wildcard. Weights are relative within eligible events.
static const RoadEvent ROAD_EVENTS[] = {
    {"The dropped satchel", "A weather-stained satchel lies half-hidden in the grass.", "A ration and a wrapped meal.", EVENT_SUPPLIES, -1, -1, -1, 5, 0, 0, -1},
    {"Black rain", "Clouds break without warning. The road turns slick and cold.", "The wet miles leave you tired and chilled.", EVENT_WEATHER, -1, -1, -1, 4, 0, 0, -1},
    {"A hunter's snare", "A wire loop catches your boot at the edge of the track.", "You wrench free, but the wire draws blood.", EVENT_HAZARD, -1, -1, -1, 3, 0, 0, -1},
    {"A fallen shrine", "A little stone shrine leans beneath a tree, candles long cold.", "A quiet moment steadies your hand.", EVENT_HEALING, 1, -1, -1, 3, 0, 0, -1},
    {"A lost traveler", "A weary traveler shares the road for a few miles.", "They share a warning about the next stretch, then turn west.", EVENT_QUIET, -1, -1, -1, 5, 0, 0, -1},
    {"The miller's warning", "A miller has heard wolves calling near the north track.", "The warning names a safer trail through the farms.", EVENT_RUMOR, 5, -1, -1, 5, 0, 0, -1},
    {"Lantern festival", "Paper lanterns drift above the town square. Someone offers you a warm cup.", "The small kindness eases your fatigue.", EVENT_FESTIVAL, -1, -1, -1, 4, 0, 0, -1},
    {"A child's birthday", "A shy child asks if you have seen their older brother on the road.", "A name and a blue scarf become a promise to remember.", EVENT_CELEBRATION, -1, -1, -1, 3, 1, 0, -1},
    {"The wedding road", "A wedding party blocks the bridge with ribbons, fiddles, and bread.", "The newlyweds share a meal and tell you of a hidden spring.", EVENT_CELEBRATION, 5, -1, -1, 3, 2, 0, -1},
    {"The ferryman's choice", "A ferryman asks you to carry medicine to a fevered hamlet, unpaid.", "Your answer will be remembered by the river folk.", EVENT_DILEMMA, 6, -1, -1, 3, 3, 0, 2},
    {"A peddler's old map", "An old peddler shows a smudged map of stones beneath the forest.", "You learn where a forgotten waymarker lies.", EVENT_RUMOR, 1, -1, -1, 4, 0, 0, -1},
    {"The hungry watch", "A watch guard asks for a ration, eyes fixed on your pack.", "Give food, refuse, or ask what happened to their patrol.", EVENT_NPC, -1, -1, -1, 4, 4, 0, 1},
    {"A warm hearth", "A farmstead lets you sit by the hearth while the rain passes.", "The warmth eases exposure and fatigue.", EVENT_HEALING, 5, 1, -1, 4, 0, 0, -1},
    {"Tracks in the ash", "Fresh bootprints cross an old campfire, then vanish among stones.", "Someone passed recently. A small keepsake remains.", EVENT_STORY, 4, -1, -1, 4, 5, 0, -1},
    {"The stranger's song", "A lone singer knows the same verse you heard at Lantern Rest.", "The last line hints at a face waiting in the coast town.", EVENT_STORY, -1, -1, -1, 4, 6, 0, -1},
    {"A salt-worn letter", "A sealed letter is wedged in the rocks above the tide.", "Its unfinished apology names a house in Calder Keep.", EVENT_STORY, 6, -1, -1, 3, 7, 0, -1},
    {"The broken cart", "A cart has split an axle on the old road; its driver is unhurt.", "Help repair it, trade supplies, or leave them to the road.", EVENT_NPC, 7, -1, -1, 5, 8, 0, -1},
    {"A borrowed name", "A tired traveler claims to be a local you helped last season.", "Their story is wrong, but the detail they know is true.", EVENT_NPC, -1, -1, -1, 3, 9, 0, -1},
    {"A small kindness", "A stranger offers you half a loaf without asking your name.", "The kindness is more filling than the bread.", EVENT_QUIET, -1, -1, -1, 6, 0, 0, -1},
    {"Crows at the cairn", "Crows gather on a cairn where no road is marked.", "Under the top stone lies a dry tinderbox.", EVENT_SUPPLIES, 2, -1, -1, 4, 0, 0, -1},
    {"A muddied boot", "One boot lies beside the path, still warm from its owner.", "A nearby trail leads toward a hidden camp.", EVENT_STORY, 3, -1, -1, 4, 10, 0, -1},
    {"The quiet mile", "Wind brushes the grass flat. No footsteps answer yours.", "Nothing happens. For now, that is enough.", EVENT_QUIET, 0, -1, -1, 8, 0, 0, -1},
    {"A marsh light", "A pale flame bobs above the reeds, keeping pace with you.", "Follow it toward herbs or turn back to firm ground.", EVENT_NPC, 3, 3, -1, 3, 11, 0, -1},
    {"Old quarry bargain", "A quarry hand offers flint for a strip of clean cloth.", "A fair trade leaves both of you a little better equipped.", EVENT_TRADE, 2, -1, -1, 4, 0, 0, -1},
    {"A shepherd's lamb", "A lamb is caught in thornbush while wolves call beyond the ridge.", "Free it at some risk, or save your strength.", EVENT_DILEMMA, 5, -1, -1, 4, 12, 0, -1},
    {"The watcher's favor", "A March warden recognizes your help from an earlier road.", "A favor is repaid with a warning and a bandage.", EVENT_FACTION, 7, -1, -1, 4, 0, 0, 1},
    {"A debt recalled", "A Compact factor remembers your unpaid promise.", "Pay, bargain for time, or refuse and earn a grudge.", EVENT_FACTION, -1, -1, -1, 3, 13, 0, 0},
    {"Frost in the pass", "A wind like a drawn blade spills through the mountain pass.", "Shelter before night, or press on through the cold.", EVENT_WEATHER, 2, 2, 2, 4, 0, 0, -1},
    {"The spring thaw", "Meltwater reveals a narrow trail and last year's lost pack.", "A little food survived under the waxed canvas.", EVENT_SUPPLIES, 2, -1, 0, 4, 0, 0, -1},
    {"Autumn gathering", "Farmers share apples and news after the harvest carts come in.", "A meal is offered, and a rumor travels with it.", EVENT_FESTIVAL, 5, -1, 2, 5, 0, 0, -1},
    {"The coast remembers", "A fisher names a ship that never came home.", "The story points to a wreck hidden below the western cliffs.", EVENT_STORY, 6, -1, -1, 4, 14, 0, -1},
    {"A veiled merchant", "A covered wagon offers a dry cloak for a fair price.", "Trade coin for protection against the weather.", EVENT_TRADE, -1, -1, -1, 5, 0, 0, 0},
    {"The saint's birthday", "A shrine keeper gives away soup in memory of a vanished healer.", "A bowl warms you; a whispered clue sends you to the ruins.", EVENT_CELEBRATION, 4, -1, -1, 3, 15, 0, -1},
    {"The abandoned nursery", "A cradle rocks in a windless room at the edge of the ruins.", "A name scratched beneath it joins a tale you heard before.", EVENT_STORY, 4, -1, -1, 2, 1, 1, -1},
    {"Brother at the bridge", "The birthday child's brother is keeping watch at a broken bridge.", "He asks you to carry a token home when you next pass.", EVENT_STORY, 7, -1, -1, 2, 1, 2, -1},
    {"The ferryman's answer", "The hamlet's sick have recovered. The ferryman has not forgotten.", "The river folk offer passage without charge.", EVENT_STORY, 6, -1, -1, 2, 3, 1, 2},
    {"A grave with your name", "A stone bears the name of one who walked this road before you.", "The grave holds a rumor and a trace of their old equipment.", EVENT_STORY, -1, -1, -1, 1, 16, 0, -1},
    {"The after-wedding dance", "A fiddler plays the newlyweds' tune on a road far from home.", "The melody recalls the hidden spring and grants a little courage.", EVENT_STORY, -1, -1, -1, 2, 2, 1, -1},
    {"A rook's warning", "A rook calls three times from the battlements, then falls silent.", "Someone watches the road from the ruins.", EVENT_RUMOR, 4, 3, -1, 4, 0, 0, -1},
    {"A lost harvest child", "A child has wandered from the harvest carts into the wood.", "Walk them home or mark the trail for the searchers.", EVENT_DILEMMA, 1, -1, 2, 3, 17, 0, -1},
    {"A tired patrol", "A warden patrol limps toward town after a night in the rain.", "Share supplies to earn trust, or keep your pack closed.", EVENT_NPC, 7, 1, -1, 4, 18, 0, 1},
    {"The candle seller", "An old woman sells candles and asks who you hope to find.", "Your answer earns either a blessing or a knowing silence.", EVENT_NPC, -1, -1, -1, 4, 19, 0, -1},
    {"The rain's last drop", "A leaf holds one bright drop after the storm moves east.", "The quiet moment steadies you; the road can wait a breath.", EVENT_QUIET, 1, 1, -1, 7, 0, 0, -1},
    {"A fox in the granary", "A fox steals an apple, then watches you from the rafters.", "Leave it be and find a little fruit the farmers set aside.", EVENT_SUPPLIES, 5, -1, -1, 4, 0, 0, -1},
    {"The old oath", "Two Compact traders dispute a boundary marked by a broken stone.", "Choose a side, mediate, or refuse to be drawn in.", EVENT_DILEMMA, 7, -1, -1, 4, 20, 0, 0},
    {"The coast wedding", "A shipwright and a net-mender marry beneath a sailcloth arch.", "They ask you to carry a sealed note to the keeper at the harbor.", EVENT_CELEBRATION, 6, -1, 1, 3, 21, 0, -1},
    {"The sealed cask", "A cask washes ashore with a faction mark beneath the salt.", "Opening it may bring a useful find, trouble, or a debt owed.", EVENT_DILEMMA, 6, 3, -1, 3, 22, 0, -1},
    {"The kettle on the stile", "A farmer has left a kettle of nettle broth beside the stile.", "A warm cup eases the chill; the farmer asks no payment.", EVENT_QUIET, 5, 2, -1, 4, 0, 0, -1},
    {"A wheel in the ditch", "A cart wheel has sunk deep in the muddy track.", "The driver offers a meal if you lend your shoulder.", EVENT_NPC, 5, 2, -1, 3, 0, 0, -1},
    {"The empty rookery", "Every nest in the old tower is empty, though the birds circle above.", "A strip of blue cloth points toward a nearby ruin.", EVENT_RUMOR, 7, -1, -1, 3, 0, 0, -1},
    {"A fisher's lantern", "A lantern bobs offshore where no boat should be.", "The keeper names a safe path around the cliff at low tide.", EVENT_RUMOR, 6, 3, -1, 4, 0, 0, -1},
    {"The rain barrel", "A split barrel rests beneath the eaves of an abandoned shed.", "You find a wrapped meal and dry tinder beneath its lid.", EVENT_SUPPLIES, 5, 2, -1, 4, 0, 0, -1},
    {"A hedgerow feast", "Neighbors pass bread and apples over a low hedge.", "They share a small meal and a story about the old road.", EVENT_FESTIVAL, 5, -1, 2, 4, 0, 0, -1},
    {"The charcoal burner", "A soot-blackened worker watches the tree line instead of the kiln.", "A warning of wolves earns you a pouch of charcoal and a safer route.", EVENT_NPC, 1, -1, -1, 3, 0, 0, -1},
    {"A boot full of rain", "A single boot sits upright on a milestone, brimful of rain.", "The strange little sight makes you laugh despite yourself.", EVENT_QUIET, 0, 2, -1, 5, 0, 0, -1},
    {"The bell with no rope", "A shrine bell sounds once in the wind, though its rope is gone.", "A dry alcove beneath the shrine offers brief shelter.", EVENT_HEALING, 7, 2, -1, 3, 0, 0, -1},
    {"A bargain in salt", "A coast trader offers smoked fish for a fair handful of coin.", "The trader remembers whether you paid without haggling.", EVENT_TRADE, 6, -1, -1, 4, 0, 0, 0},
    {"The buried milestone", "A storm has uncovered a mile-marker carved with unfamiliar names.", "One name matches the old song heard at Lantern Rest.", EVENT_STORY, 0, 2, -1, 3, 0, 0, -1},
    {"A borrowed umbrella", "A child lends you a patchwork umbrella, then runs back to the farm.", "The rain feels less cruel beneath the mismatched cloth.", EVENT_WEATHER, 5, 2, -1, 4, 0, 0, -1},
    {"The hound's collar", "A leather collar hangs from a branch above fresh pawprints.", "A shepherd thanks you for returning it and gives you a little food.", EVENT_NPC, 5, -1, -1, 3, 0, 0, -1},
    {"A mountain cairn", "Someone has rebuilt the cairn with careful, patient hands.", "A stone tucked inside bears a warning about unstable snow.", EVENT_RUMOR, 2, 0, 3, 3, 0, 0, -1},
    {"The frost-bitten peddler", "A peddler's fingers have gone pale in the mountain wind.", "A little warmth and help earn a favor with the traveling guild.", EVENT_FACTION, 2, 0, 3, 2, 0, 0, 0},
    {"A market-day drum", "A hand drum calls the village to its small market square.", "A shared dance lifts tired spirits before the road resumes.", EVENT_FESTIVAL, 4, -1, -1, 4, 0, 0, -1},
    {"The silted well", "A hamlet's well has filled with silt after a night of hard rain.", "Help clear it and the villagers remember your kindness.", EVENT_DILEMMA, 5, 2, -1, 3, 0, 0, 1},
    {"The lantern-maker's apprentice", "An apprentice is trying to mend a paper lantern in the wind.", "A patient hand earns a clue about the coast road.", EVENT_NPC, 4, -1, -1, 3, 0, 0, -1},
    {"The last apple", "One red apple remains on a low branch after the harvest.", "You leave it for the birds, and the road is quiet for a while.", EVENT_QUIET, 5, -1, 2, 5, 0, 0, -1},
    {"A bridge of roots", "Roots have twisted together across a narrow marsh channel.", "A careful crossing reveals dry ground on the far side.", EVENT_STORY, 3, -1, -1, 3, 0, 0, -1}
};
static const int EVENT_COUNT = sizeof(ROAD_EVENTS) / sizeof(ROAD_EVENTS[0]);

struct QuestTemplate
{
    int type;
    int target;
    int goal;
    int difficulty;
    const char *title;
    const char *description;
};

static const QuestTemplate QUEST_TEMPLATES[] = {
    {0, 1, 2, 1, "Map the pine track", "Cross the forest twice and mark a safe path for a charcoal burner."},
    {0, 1, 4, 2, "Find the lost forester", "Travel through the forest four times; an old forester has not returned."},
    {0, 2, 2, 1, "Survey the high trail", "Cross mountain country twice and note where the wind turns."},
    {0, 2, 4, 3, "Trace the pass", "Cross the mountains four times to find a route safe for pack animals."},
    {0, 3, 2, 2, "Sound the reed beds", "Travel through the marsh twice and report where the ground gives way."},
    {0, 3, 4, 3, "Mark the dry causeway", "Cross the marsh four times and find a path that holds in rain."},
    {0, 4, 2, 1, "Carry word between farms", "Pass through farmland twice with a message for the neighbors."},
    {0, 4, 4, 2, "Count the harvest roads", "Travel through farmland four times and find which road the raiders avoid."},
    {0, 5, 2, 2, "Search the old stones", "Cross the ruins twice and look for a missing shrine mark."},
    {0, 5, 4, 3, "Chart the fallen halls", "Make four journeys through the ruins and map a safe approach."},
    {0, 6, 2, 2, "Walk the tide line", "Travel along the coast twice and note where the tide turns."},
    {0, 6, 4, 3, "Find the wreck path", "Cross the coast four times and search for a way to the stranded ship."},
    {0, 7, 2, 1, "Follow the old road", "Cross the old road twice and check the mile-markers."},
    {0, 7, 4, 2, "Repair the waymarks", "Travel the old road four times and find its broken markers."},
    {0, 0, 2, 1, "Read the open country", "Cross the open country twice and find a sheltered camp site."},
    {0, 0, 4, 2, "Find the shepherd's route", "Travel the open country four times to trace a safe path for the flock."},
    {1, 0, 1, 1, "Drive off the road outlaw", "Defeat the outlaw who has been shaking down travelers."},
    {1, 1, 1, 2, "Guard the sheepfold", "Defeat a gaunt wolf before it returns to the farm."},
    {1, 2, 1, 3, "End the hill raids", "Defeat the hill marauder threatening the mountain pass."},
    {1, 3, 1, 2, "Clear the reed path", "Defeat the reed stalker haunting the marsh causeway."},
    {1, 4, 1, 1, "Protect the grain cart", "Drive off the feral dog stalking the harvest road."},
    {1, 5, 1, 3, "Lay the barrow wight to rest", "Defeat the bound guardian disturbing the ruin-side graves."},
    {1, 6, 1, 2, "Recover the fisher's net", "Defeat the coast reaver who stole a net from the harbor."},
    {1, 7, 1, 3, "Turn aside the mire boar", "Defeat the territorial boar before it reaches the cottages."},
    {1, 8, 1, 2, "Settle the toll dispute", "Defeat or drive off the Compact tollguard blocking the road."},
    {1, 9, 1, 1, "Free the wounded heron", "Defeat the dangerous marsh bird without letting it suffer longer."},
    {1, 0, 2, 2, "Make the crossroads safe", "Defeat two road outlaws and give travelers room to pass."},
    {1, 1, 2, 2, "Watch the northern flock", "Drive off two gaunt wolves stalking the sheep."},
    {1, 4, 2, 2, "Protect the harvest wagons", "Defeat two feral dogs before the next market day."},
    {1, 7, 2, 2, "Keep the road open", "Defeat two Compact tollguards who have taken more than their due."},
    {1, 16, 1, 2, "Thin the thorncat's range", "Drive the thorncat away from the forest charcoal burners."},
    {1, 17, 1, 3, "Break the slate troll's hold", "Defeat the brute blocking the mountain pass."},
    {1, 18, 1, 3, "Silence the drowned chain", "Defeat the dead sailor haunting the coast road."},
    {1, 19, 1, 3, "Disable the iron sentinel", "Defeat the old guardian before it reaches the village road."},
    {1, 20, 1, 2, "Guard the thicket's edge", "Drive the wounded stag away from the farmstead."},
    {1, 21, 1, 3, "Lay the ashen shade to rest", "Defeat the cold echo gathering in the ruins."},
    {2, 5, 2, 1, "Gather healing herbs", "Find two healing herbs for the village healer."},
    {2, 5, 4, 2, "Stock the winter pantry", "Find four rations to help a family through the cold month."},
    {2, 6, 2, 1, "Stock the travelers' pantry", "Find two portions of smoked sausage for the gate watch."},
    {2, 7, 2, 2, "Bring bitter medicine", "Find two healing herbs for a fevered child."},
    {2, 8, 2, 2, "Prepare clean bandages", "Find two bandages for the warden's patrol."},
    {2, 10, 3, 1, "Collect marsh mushrooms", "Gather three dried mushroom bundles for the cook."},
    {2, 11, 2, 1, "Bring bitterroot", "Find two bitterroots for the village remedy shelf."},
    {2, 12, 2, 1, "Find clean cloth", "Gather two pieces of cloth for the traveling mender."},
    {2, 15, 3, 2, "Provision the bridge watch", "Find three boiled eggs for the guards keeping the bridge open."},
    {2, 18, 3, 2, "Provision the quarry", "Find three portions of hard cheese for the workers in the dry hills."},
    {2, 7, 3, 2, "Supply the field healer", "Gather three healing herbs for the farmstead healer."},
    {2, 8, 3, 2, "Patch the patrol's wounds", "Bring three bandages to the road wardens."},
    {2, 16, 3, 2, "Fill the storehouse jar", "Find three smoked eels for the village pantry before the rains begin."},
    {2, 11, 3, 2, "Gather roots for the apothecary", "Find three bitterroots for a batch of wound salve."},
    {2, 20, 3, 2, "Stock the apothecary", "Find three pieces of willow bark for the village apothecary."},
    {3, -1, 2, 1, "Carry the miller's warning", "Ask locals about the road twice and carry the warning to another settlement."},
    {3, -1, 3, 2, "Learn who watches the pass", "Ask three locals about the road; one has seen the watcher."},
    {3, -1, 3, 2, "Follow the lantern song", "Ask three locals about the road and find who remembers the final verse."},
    {3, -1, 4, 3, "Uncover the harbor rumor", "Ask four locals about the road to learn why the ship never returned."},
    {3, -1, 4, 3, "Trace the old family debt", "Ask four locals about the road and find who still remembers the debt."}
};
static const int QUEST_TEMPLATE_COUNT = sizeof(QUEST_TEMPLATES) / sizeof(QUEST_TEMPLATES[0]);

struct Settlement
{
    int x;
    int y;
    const char *name;
    const char *kind;
    int faction;
};

static const Settlement SETTLEMENTS[] = {
    {2, 2, "Calder Keep", "CITY", 0},
    {8, 2, "Millbrook", "TOWN", 1},
    {5, 5, "Lantern Rest", "CAMP", 2},
    {9, 8, "Brayhold", "CITY", 1},
    {3, 9, "Ashford", "TOWN", 0},
    {7, 10, "Mudlark Camp", "CAMP", 2},
    {0, 6, "Rimewatch", "TOWN", 3},
    {10, 5, "Greenhollow", "TOWN", 4},
    {6, 1, "Saltmere", "CITY", 5},
    {11, 11, "Cinderfast", "TOWN", 6},
    {0, 10, "Greybridge", "CITY", 7}
};
static const int SETTLEMENT_COUNT = sizeof(SETTLEMENTS) / sizeof(SETTLEMENTS[0]);

struct Kingdom
{
    const char *name;
    const char *motto;
};
static const Kingdom KINGDOMS[] = {
    {"Calder Crownlands", "Trade keeps the roads open."},
    {"The Marchward", "A promise is a border."},
    {"The Hollow Reach", "No throne owns the wild."},
    {"Rimevale", "Endure what the mountain keeps."},
    {"Greenfold", "The harvest belongs to those who tend it."},
    {"Saltmere", "The tide carries every debt."},
    {"Cinderhold", "From ruin, a harder steel."},
    {"Greywater", "The bridge is common ground."}
};
static const int KINGDOM_COUNT = sizeof(KINGDOMS) / sizeof(KINGDOMS[0]);

struct Faction
{
    const char *name;
    const char *blurb;
    int kingdom;
};
static const Faction FACTIONS[] = {
    {"Amber Compact", "Merchants, lenders, and road factors.", 0},
    {"Warden's March", "Keepers of roads, bridges, and old promises.", 1},
    {"Hollow Circle", "Camp folk, smugglers, and the people between towns.", 2},
    {"Rimewatch Clans", "Pass-keepers and hardy traders of the north.", 3},
    {"Greenfold Commons", "Farmsteads bound by shared fields and water.", 4},
    {"Saltmere League", "Harbor houses, ferrymen, and coastal crews.", 5},
    {"Cinder Synod", "Craft halls rebuilding around the old kiln roads.", 6},
    {"Greybridge Compact", "River towns that keep the crossings neutral.", 7}
};
static const int FACTION_COUNT = sizeof(FACTIONS) / sizeof(FACTIONS[0]);

struct Background
{
    const char *name;
    const char *description;
};
static const Background BACKGROUNDS[] = {
    {"Sellsword", "A working blade, armor, and enough food to keep moving."},
    {"Runaway", "A quick knife, light pack, and a reason not to look back."},
    {"Wayherb", "A staff, healing plants, and a practiced eye for the wild."}
};
static const int BACKGROUND_COUNT = sizeof(BACKGROUNDS) / sizeof(BACKGROUNDS[0]);

static const char *SKILL_NAMES[] = {"Combat", "Survival", "Awareness", "Rapport", "Trailcraft"};
static const int SKILL_COUNT = sizeof(SKILL_NAMES) / sizeof(SKILL_NAMES[0]);
static const char *TRAIT_NAMES[] = {
    "Scarred", "Merciful", "Promise-Keeper", "Gravebound", "Harbor Friend", "Roadwise"
};
static const int TRAIT_COUNT = sizeof(TRAIT_NAMES) / sizeof(TRAIT_NAMES[0]);

struct NPC
{
    const char *name;
    const char *role;
    const char *personality;
    const char *story;
    int faction;
};
static const NPC NPCS[] = {
    {"Mara Venn", "ferryman", "Careful with promises, generous with directions.", "Her brother vanished on a crossing in the spring flood.", 2},
    {"Orren Pike", "road warden", "Blunt, fair, and slow to trust a stranger.", "He is trying to find who cut the mile-marker ropes.", 1},
    {"Talla Reed", "herbalist", "Quietly funny, with a sharp eye for a fever.", "She keeps a list of patients the towns refuse to name.", 2},
    {"Joss Calder", "factor", "Patient until a debt is denied.", "A sealed ledger connects his family to the old coast wreck.", 0},
    {"Nim", "stablehand", "Restless, bright, and hungry for a life beyond the mill.", "They know a child who has been sleeping beneath the granary.", 1},
    {"Sister Elwen", "shrine keeper", "Gentle, but unwilling to lie for comfort.", "She tends a grave whose name was deliberately scraped away.", 2},
    {"Bram Holt", "fisher", "Dry-witted and superstitious about clear skies.", "He saw lanterns beneath the waves on the night the ship went missing.", 2},
    {"Veya Thorn", "carter", "Practical and kind when no one is watching.", "She carries a letter she has not found the courage to deliver.", 0},
    {"Harl Moss", "watch guard", "Tired, proud, and deeply ashamed of one bad choice.", "He left a patrol partner behind during a night raid.", 1},
    {"Old Fen", "peddler", "Cheerful until someone asks where the map came from.", "He sells copies of a route that should not exist.", 0},
    {"Sella Vorn", "pass guide", "Patient with the lost, suspicious of polished boots.", "She marks safe ledges after each hard winter.", 3},
    {"Dain Rowan", "field reeve", "Plain-spoken and careful with the common grain.", "He is hiding seed stores from a tax collector.", 4},
    {"Edda Coil", "harbor pilot", "Quick to laugh, quicker to read a change in tide.", "She once guided a ship through fog with its lights out.", 5},
    {"Kerr Ashhand", "kiln smith", "Proud of good work and wary of easy promises.", "He is restoring a bell cast before the old kingdom fell.", 6},
    {"Merrit Grey", "bridge keeper", "Even-handed, observant, and hard to hurry.", "They keep two copies of every toll ledger, for a reason.", 7}
};
static const int NPC_COUNT = sizeof(NPCS) / sizeof(NPCS[0]);

static const char *DIALOGUE_OPENERS[] = {
    "Keep your voice low;", "I heard this at first light;", "Between ourselves;",
    "The road has been saying;", "My neighbor swears;", "Since the last market;",
    "A thing worth knowing;", "Don't repeat this at the gate, but;",
    "I have seen enough to say;", "If you have a moment;"
};
static const char *DIALOGUE_OBSERVATIONS[] = {
    "the old boundary stones are being moved.", "grain is dearer near the river.",
    "the watch changed its signal after dusk.", "someone has been buying every length of rope.",
    "the northern track is quieter than it should be.", "three new faces arrived before sunrise.",
    "the ferries are running late this week.", "the crows have left the eastern fields.",
    "the town gates stay open longer when the roads are safe.", "the banners changed, but the debts did not."
};
static const char *DIALOGUE_TAILS[] = {
    "It may be nothing, yet folk are watching.", "Ask again after the next market day.",
    "I would take the long road until it settles.", "Someone will know more by tomorrow.",
    "That is all I can say without naming names.", "The truth usually travels with the carts.",
    "Keep it in mind if your path turns east.", "No one agrees on what it means.",
    "The old families have begun to notice.", "Best decide what you believe before nightfall."
};
static const int DIALOGUE_VARIATION_COUNT =
    sizeof(DIALOGUE_OPENERS) / sizeof(DIALOGUE_OPENERS[0]) *
    sizeof(DIALOGUE_OBSERVATIONS) / sizeof(DIALOGUE_OBSERVATIONS[0]) *
    sizeof(DIALOGUE_TAILS) / sizeof(DIALOGUE_TAILS[0]);
static const int LEGACY_NPC_COUNT = 10;

} // namespace wayworn_data
