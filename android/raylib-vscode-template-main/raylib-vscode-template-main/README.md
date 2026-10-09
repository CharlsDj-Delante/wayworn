# Wayworn

Wayworn is a raylib text-based fantasy survival RPG. Its content tables live in
`src/game_data.h`; game rules and presentation live in `src/main.cpp`.

## Build and run on Windows

From this project directory:

```powershell
& 'C:\raylib\w64devkit\bin\mingw32-make.exe' `
  'RAYLIB_PATH=C:/raylib/raylib' 'PROJECT_NAME=game' 'OBJS=src/*.cpp' 'BUILD_MODE=DEBUG'
.\game.exe
```

The same build is available from the VS Code **build debug** task.

## Play

- Forge a name, choose one of three backgrounds, and allocate 5 attribute
  points; every stat starts at 10.
- Combat, survival, awareness, rapport, and trailcraft skills improve through
  repeated use. Events can earn lasting traits with small mechanical benefits;
  skills and earned traits are shown in the HUD and journey record.
- Use the **Map** tab's eight-way compass to travel. Travel spends time and
  food and fatigue; hunger is the only tracked consumption need. Unknown tiles
  and hidden points of interest are revealed as you explore.
- The **Quests** tab tracks local tasks, objective progress, and difficulty-based
  coin and item rewards. Active objectives have a red map pin for their
  destination; hunt targets appear at their marked ground, and gathered quest
  items must be delivered at the marked settlement. Ask people in settlements
  for work.
- The world clock begins at 6 AM and moves in six-hour steps; a map movement
  advances the clock by six hours. The parchment UI darkens slightly at night.
- Main-page choices are local actions. Combat, conversations, and event
  decisions lock the other tabs until resolved.
- Gold comes only from quest rewards or selling monster trophies to settlement
  traders. There is no story-telling payout or thirst/water survival meter.
- Taverns sell warm meals; talking with locals is for rumors, stories, favors,
  and quests rather than coin.
- In combat, read the enemy warning and choose to strike, guard, use a supply,
  flee, bargain, use terrain, or sneak away.
- Equip weapons, helmets, body armor, gloves, and boots independently from the
  Items ledger. Each piece contributes its listed protection and tradeoffs;
  armor weight affects travel fatigue, weather gear reduces exposure, gloves
  and weapons affect strikes, and boots affect travel pace.
- Browse the armory at settlements to compare each item's description, stats,
  pros, cons, and gold price. Purchases must fit within the strict pack limit.
- The item ledger tracks pack weight and food freshness. Bitterroot and clean
  cloth can be crafted into a bandage.
- The Items page shows only supplies currently in your pack. Use the mouse
  wheel or the on-screen up/down controls to move through long lists; portrait
  layouts keep activity choices reachable with touch-sized buttons.
- Tab changes and choices use brief visual feedback; random events and combat
  encounters have a short parchment-tinted transition.
- **F3** opens the hidden deterministic 1,000-run balance report.

## World, saves, and legacy

The map is seeded once per world and retained for later characters. Regions,
settlements, landmarks, faction influence, NPC relationships, rumor chains,
weather, seasons, and weighted road events contribute to that shared world.
Eight kingdoms and their eight factions hold eleven settlements; faction power,
unrest, kingdom order, and local prosperity shift as in-game days pass. Weekly
border contests can move influence and change local market prices. Market
prices and tavern meal costs also respond to town prosperity, faction unrest,
and your reputation.
The event table contains 67 distinct encounters and story beats. One hundred
additional monster encounter variations combine regional signs with different
creature behaviors, so sightings carry useful warning and tactical context.
The quest book draws from 56 authored task templates.

NPC conversations draw from ten openers, ten observations, and ten follow-ups
to produce 1,000 dialogue variations, then pair those lines with each speaker's
own personality, history, and current encounter. Dialogue appears in road
rumors, favors, quests, and social events.

The game overwrites one version 11 `wayworn_save.dat` autosave in its working directory
after a new journey and meaningful turn/action. The save includes the seeded
random state, faction and kingdom conditions, settlement prosperity, and temporary
local encounters; death is saved as a fallen record,
not a rewind point. A
successor can discover the grave and recover gear that fits their pack.
Survival deaths name the cause, including starvation, exhaustion, cold, blood
loss, or infection. Older saves migrate to the current format while retaining
their known reputation and NPC memories; the expanded world's simulation starts
from the saved random state.

Wildlife appearances are region-specific (wolves, bears, and thorncats in
forest; prowlers and manticores in open country; basilisk and other marsh
creatures in fen; and so on). Ten new monsters have distinct warnings,
personalities, abilities, and trophy drops; the expanded bestiary contains 32
creatures. Their sightings use a hundred data-driven narrative variations.
Defeated monsters drop trophies and sometimes equipment; sell trophies in towns
and cities. The expanded ledger contains 172 item types, including 20
additional weapons and 20 each of helmets, body armor, gloves, and boots.
Weapons and each armor slot have distinct descriptions, numerical effects,
pros, cons, and quality-based prices. The quest book has 56 task
templates. A sighting gives a warning and a
choice to approach it or keep your distance; it may disappear after a few
actions and new creatures return over time. Travelers can appear in settlements or the wilderness, each busy with a
faction- and location-aware task. Their speech reflects personality, trust,
reputation, and prior dealings. Once a traveler has spoken with the player,
they remain at that place and can be found again; unconnected passersby may
still move on. The map carries a live dispatch for border conflict,
celebrations, market strain, and quiet roads.

NPCs and creatures also cross paths in local scenes: two named characters may
argue over a favor, a traveler may be threatened by an outlaw, or someone may
need help while a regional creature prowls nearby. These encounters appear in
the road scene and offer optional actions on desktop and touch layouts. Combat
is never automatic: players can leave, bargain, mediate, or choose a clearly
telegraphed fight. Meaningful intervention can earn trust, faction standing,
and favors; NPCs witnessed but not engaged do not become permanently bound to
the location. Active scenes and quest destinations are saved with the version 11 autosave.

The UI uses an illustrated parchment-and-leather palette with gilded page
edges, alongside narrow-screen and touch-friendly layouts. This project
does not currently configure an Android SDK/package build. Native Android
packaging still needs a raylib Android toolchain and device validation.
