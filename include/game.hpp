#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <string>

#define NUMBER_OF_RESOURCES 4
#define EXPECTED_ARG_COUNT 2
#define MAXIMUM_NUMBER_OF_ROOMS 255
#define MINIMUM_NUMBER_OF_ROOMS 1

extern const int RESOURCE_VALUES[NUMBER_OF_RESOURCES];
extern const std::string RESOURCE_NAMES[NUMBER_OF_RESOURCES];

struct Room {
    int id;
    int resources[NUMBER_OF_RESOURCES];
    bool collected[NUMBER_OF_RESOURCES];
    bool firstCollected;
    std::vector<int> neighbors;
    
    Room();
};

struct GameState {
    std::vector<Room> rooms;
    int N;
    double food;
    int targetResource;
    int currentRoom;
    int collected[NUMBER_OF_RESOURCES];
    int totalValue;
    std::set<int> visited;
    
    GameState();
};

bool parseNeighbors(const std::string& s, std::vector<int>& result, int N);
int getResourceType(const std::string& s);
void printState(const GameState& state, int roomId, std::ofstream& outFile);
void outputGo(int roomId, std::ofstream& outFile);
void outputCollect(int resourceType, std::ofstream& outFile);
int getBestResourceInRoom(const Room& room, int targetResource);
void collectResource(GameState& state, int roomId, int resourceType, std::ofstream& outFile);
void moveTo(GameState& state, int roomId, std::ofstream& outFile, bool printStateAfterMove = true);
std::vector<int> bfsPath(const GameState& state, int start, int target);
std::vector<int> bfsToNearestUnvisited(const GameState& state, int start);
void printResult(const GameState& state, std::ofstream& outFile);
void returnPhase(GameState& state, std::ofstream& outFile);
void explorePhase(GameState& state, std::ofstream& outFile);

#endif