#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "BattleUnit.h"
#include "../Mod/AlienDeployment.h"

namespace OpenXcom
{

class Tile;
class SavedGame;
class MapDataSet;
class Node;
class BattlescapeState;
class Position;
class Pathfinding;
class TileEngine;
class BattleItem;
class Mod;
class State;
class RuleItem;

/**
 * The battlescape data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like mapdata,
 * soldiers, items, etc.
 */
class SavedBattleGame
{
private:
	BattlescapeState *_battleState;
	int _mapsize_x, _mapsize_y, _mapsize_z;
	std::vector<MapDataSet*> _mapDataSets;
	Tile **_tiles;
	BattleUnit *_selectedUnit, *_lastSelectedUnit;
	std::vector<Node*> _nodes;
	std::vector<BattleUnit*> _units;
	std::vector<BattleItem*> _items, _deleted;
	Pathfinding *_pathfinding;
	TileEngine *_tileEngine;
	std::string _missionType;
	int _globalShade;
	UnitFaction _side;
	int _turn;
	bool _debugMode;
	bool _aborted;
	int _itemId;
	int _objectiveType, _objectivesDestroyed, _objectivesNeeded;
	std::vector<BattleUnit*> _exposedUnits;
	std::list<BattleUnit*> _fallingUnits;
	bool _unitsFalling, _cheating;
	std::vector<Position> _tileSearch, _storageSpace;
	BattleActionType _tuReserved;
	bool _kneelReserved;
	std::vector< std::vector<std::pair<int, int> > > _baseModules;
	int _depth, _ambience;
	double _ambientVolume;
	std::vector<BattleItem*> _recoverGuaranteed, _recoverConditional;
	std::string _music;
	int _turnLimit, _cheatTurn;
	ChronoTrigger _chronoTrigger;
	bool _beforeGame;
	/// Selects a soldier.
	BattleUnit *selectPlayerUnit(int dir, bool checkReselect = false, bool setReselect = false, bool checkInventory = false);
public:
	/// Creates a new battle save, based on the current generic save.
	SavedBattleGame();
	/// Cleans up the saved game.
	~SavedBattleGame();
	/// Loads a saved battle game from YAML.
	void load(const YAML::Node& node, Mod *mod, SavedGame* savedGame);
	/// Saves a saved battle game to YAML.
	YAML::Node save() const;
	/// Sets the dimensions of the map and initializes it.
	void initMap(int mapsize_x, int mapsize_y, int mapsize_z, bool resetTerrain = true);
	/// Initialises the pathfinding and tileengine.
	void initUtilities(Mod *mod);
	/// Gets the game's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Sets the mission type.
	void setMissionType(const std::string &missionType);
	/// Gets the mission type.
	std::string getMissionType() const;
	/// Sets the global shade.
	void setGlobalShade(int shade);
	/// Gets the global shade.
	int getGlobalShade() const;
	/// Gets a pointer to the tiles, a tile is the smallest component of battlescape.
	Tile **getTiles() const;
	/// Gets a pointer to the list of nodes.
	std::vector<Node*> *getNodes();
	/// Gets a pointer to the list of items.
	std::vector<BattleItem*> *getItems();
	/// Gets a pointer to the list of units.
	std::vector<BattleUnit*> *getUnits();
	/// Gets terrain size x.
	int getMapSizeX() const;
	/// Gets terrain size y.
	int getMapSizeY() const;
	/// Gets terrain size z.
	int getMapSizeZ() const;
	/// Gets terrain x*y*z
	int getMapSizeXYZ() const;

	/**
	 * Converts coordinates into a unique index.
	 * getTile() calls this every time, so should be inlined along with it.
	 * @param pos The position to convert.
	 * @return A unique index.
	 */
	inline int getTileIndex(Position pos) const
	{
		return pos.z * _mapsize_y * _mapsize_x + pos.y * _mapsize_x + pos.x;
	}

	/// Converts a tile index to its coordinates.
	void getTileCoords(int index, int *x, int *y, int *z) const;

	/**
	 * Gets the Tile at a given position on the map.
	 * This method is called over 50mil+ times per turn so it seems useful
	 * to inline it.
	 * @param pos Map position.
	 * @return Pointer to the tile at that position.
	 */
	inline Tile *getTile(Position pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.z < 0
			|| pos.x >= _mapsize_x || pos.y >= _mapsize_y || pos.z >= _mapsize_z)
			return 0;

		return _tiles[getTileIndex(pos)];
	}

	/// Gets the currently selected unit.
	BattleUnit *getSelectedUnit() const;
	/// Sets the currently selected unit.
	void setSelectedUnit(BattleUnit *unit);
	/// Selects the previous soldier.
	BattleUnit *selectPreviousPlayerUnit(bool checkReselect = false, bool setReselect = false, bool checkInventory = false);
	/// Selects the next soldier.
	BattleUnit *selectNextPlayerUnit(bool checkReselect = false, bool setReselect = false, bool checkInventory = false);
	/// Selects the unit with position on map.
	BattleUnit *selectUnit(Position pos);
	/// Gets the pathfinding object.
	Pathfinding *getPathfinding() const;
	/// Gets a pointer to the tileengine.
	TileEngine *getTileEngine() const;
	/// Gets the playing side.
	UnitFaction getSide() const;
	/// Gets the turn number.
	int getTurn() const;
	/// Ends the turn.
	void endTurn();
	/// Sets debug mode.
	void setDebugMode();
	/// Gets debug mode.
	bool getDebugMode() const;
	/// Load map resources.
	void loadMapResources(Mod *mod);
	/// Resets tiles units are standing on
	void resetUnitTiles();
	/// Removes an item from the game.
	void removeItem(BattleItem *item);

	/// Converts a unit into a unit of another type.
	BattleUnit *convertUnit(BattleUnit *unit, const SavedGame* saveGame, Mod* mod);

	/// Sets whether the mission was aborted.
	void setAborted(bool flag);
	/// Checks if the mission was aborted.
	bool isAborted() const;
	/// Sets how many objectives need to be destroyed.
	void setObjectiveCount(int counter);
	/// increments the objective counter.
	void addDestroyedObjective();
	/// Checks if all the objectives are destroyed.
	bool allObjectivesDestroyed() const;
	/// Gets the current item ID.
	int *getCurrentItemId();
	/// Gets a spawn node.
	Node *getSpawnNode(int nodeRank, BattleUnit *unit);
	/// Gets a patrol node.
	Node *getPatrolNode(bool scout, BattleUnit *unit, Node *fromNode);
	/// Carries out new turn preparations.
	void prepareNewTurn();
	/// Revives unconscious units (healthcheck).
	void reviveUnconsciousUnits();
	/// Removes the body item that corresponds to the unit.
	void removeUnconsciousBodyItem(BattleUnit *bu);
	/// Sets or tries to set a unit of a certain size on a certain position of the map.
	bool setUnitPosition(BattleUnit *bu, Position position, bool testOnly = false);
	/// Adds this unit to the vector of falling units.
	bool addFallingUnit(BattleUnit* unit);
	/// Gets the vector of falling units.
	std::list<BattleUnit*> *getFallingUnits();
	/// Toggles the switch that says "there are units falling, start the fall state".
	void setUnitsFalling(bool fall);
	/// Checks the status of the switch that says "there are units falling".
	bool getUnitsFalling() const;
	/// Gets a pointer to the BattlescapeState.
	BattlescapeState *getBattleState();
	/// Gets a pointer to the BattlescapeGame.
	BattlescapeGame *getBattleGame();
	/// Sets the pointer to the BattlescapeState.
	void setBattleState(BattlescapeState *bs);
	/// Gets the highest ranked, living XCom unit.
	BattleUnit* getHighestRankedXCom();
	/// Gets the morale modifier for XCom based on the highest ranked, living XCom unit, or the modifier for the unit passed to this function.
	int getMoraleModifier(BattleUnit* unit = 0);
	/// Checks whether a particular faction has eyes on *unit (whether any unit on that faction sees *unit).
	bool eyesOnTarget(UnitFaction faction, BattleUnit* unit);
	/// Attempts to place a unit on or near entryPoint.
	bool placeUnitNearPosition(BattleUnit *unit, const Position& entryPoint, bool largeFriend);
	/// Resets the turn counter.
	void resetTurnCounter();
	/// Resets the visibility of all tiles on the map.
	void resetTiles();
	/// get an 11x11 grid of positions (-10 to +10) to check.
	const std::vector<Position> &getTileSearch() const;
	/// check if the AI has engaged cheat mode.
	bool isCheating() const;
	/// get the reserved fire mode.
	BattleActionType getTUReserved() const;
	/// set the reserved fire mode.
	void setTUReserved(BattleActionType reserved);
	/// get whether we are reserving TUs to kneel.
	bool getKneelReserved() const;
	/// set whether we are reserving TUs to kneel.
	void setKneelReserved(bool reserved);
	/// give me access to the storage tiles vector.
	std::vector<Position> &getStorageSpace();
	/// move all the leftover items to random locations in the storage tiles vector.
	void randomizeItemLocations(Tile *t);
	/// get a reference to the baseModules map.
	std::vector< std::vector<std::pair<int, int> > > &getModuleMap();
	/// calculate the number of map modules remaining
	void calculateModuleMap();
	/// a shortcut to the geoscape save.
	SavedGame *getGeoscapeSave();
	/// get the depth of the battlescape game.
	int getDepth() const;
	/// set the depth of the battlescape game.
	void setDepth(int depth);
	/// uses the depth variable to set a palette.
	void setPaletteByDepth(State *state);
	/// sets the ambient sound effect;
	void setAmbientSound(int sound);
	/// gets the ambient sound effect;
	int getAmbientSound() const;
	/// gets the list of items we're guaranteed.
	std::vector<BattleItem*> *getGuaranteedRecoveredItems();
	/// gets the list of items we MIGHT get.
	std::vector<BattleItem*> *getConditionalRecoveredItems();
	/// Get the name of the music track.
	std::string &getMusic();
	/// Set the name of the music track.
	void setMusic(const std::string& track);
	/// Sets the objective type for this mission.
	void setObjectiveType(int type);
	/// Gets the objective type of this mission.
	SpecialTileType getObjectiveType() const;
	/// sets the ambient sound effect;
	void setAmbientVolume(double volume);
	/// gets the ambient sound effect;
	double getAmbientVolume() const;
	/// Gets the turn limit for this mission.
	int getTurnLimit() const;
	/// Gets the action that triggers when the timer runs out.
	ChronoTrigger getChronoTrigger() const;
	/// Sets the turn limit for this mission.
	void setTurnLimit(int limit);
	/// Sets the action that triggers when the timer runs out.
	void setChronoTrigger(ChronoTrigger trigger);
	/// Sets the turn to start the aliens cheating.
	void setCheatTurn(int turn);
	/// Check whether the battle has actually commenced or not.
	bool isBeforeGame() const;
	/// Checks if an item is usable on this map.
	std::string getItemUsable(BattleItem *item) const;
	/// Checks if an item is usable on this map.
	bool isItemUsable(BattleItem *item) const;
	/// Reset all the unit hit state flags.
	void resetUnitHitStates();
};

}
