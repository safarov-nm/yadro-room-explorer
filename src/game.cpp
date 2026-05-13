#include "../include/game.hpp"

const int RESOURCE_VALUES[NUMBER_OF_RESOURCES] = {7, 11, 23, 1};
const std::string RESOURCE_NAMES[NUMBER_OF_RESOURCES] = {"iron", "gold", "gems", "exp"};

Room::Room() {
    id = 0;
    firstCollected = false;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        resources[i] = 0;
        collected[i] = false;
    }
}

GameState::GameState() {
    N = 0;
    food = 0.0;
    targetResource = 0;
    currentRoom = 0;
    totalValue = 0;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        collected[i] = 0;
    }
}

bool parseNeighbors(const std::string& s, std::vector<int>& result, int N) {
    std::string current;
    for (char c : s) {
        if (c == ',') {
            if (current.empty()) return false;
            int value = stoi(current);
            if (value < 0 || value > N) return false;
            result.push_back(value);
            current.clear();
        } else if (isdigit(c)) {
            current += c;
        } else return false;
    }
    if (current.empty()) return false;
    int value = stoi(current);
    if (value < 0 || value > N) return false;
    result.push_back(value);
    return true;
}

int getResourceType(const std::string& s) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        if (RESOURCE_NAMES[i] == s) return i;
    }
    return -1;
}

void printState(const GameState& state, int roomId, std::ofstream& outFile) {
    const Room& room = state.rooms[roomId];
    outFile << "state " << roomId;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        outFile << " ";
        if (room.collected[i]) outFile << "_";
        else outFile << room.resources[i];
    }
    outFile << "\n";
}

void outputGo(int roomId, std::ofstream& outFile) {
    outFile << "go " << roomId << "\n";
}

void outputCollect(int resourceType, std::ofstream& outFile) {
    outFile << "collect " << RESOURCE_NAMES[resourceType] << "\n";
}

int getBestResourceInRoom(const Room& room, int targetResource) {
    int bestType = -1;
    int bestValue = -1;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        if (room.collected[i]) continue;
        if (room.resources[i] == 0) continue;
        int value = RESOURCE_VALUES[i];
        if (i == targetResource) value *= 2;
        int total = room.resources[i] * value;
        if (total > bestValue) {
            bestValue = total;
            bestType = i;
        }
    }
    return bestType;
}

void collectResource(GameState& state, int roomId, int resourceType, std::ofstream& outFile) {
    if (resourceType == -1) return;

    Room& room = state.rooms[roomId];

    outputCollect(resourceType, outFile);

    if (room.firstCollected) {
        state.food--;
    } else {
        room.firstCollected = true;
    }

    room.collected[resourceType] = true;

    int amount = room.resources[resourceType];
    state.collected[resourceType] += amount;

    int value = RESOURCE_VALUES[resourceType];
    if (resourceType == state.targetResource) value *= 2;

    state.totalValue += amount * value;

    printState(state, roomId, outFile);
}

void moveTo(GameState& state, int roomId, std::ofstream& outFile, bool printStateAfterMove) {
    state.food -= 1.0;
    state.currentRoom = roomId;
    state.visited.insert(roomId);
    outputGo(roomId, outFile);
    if (printStateAfterMove) printState(state, roomId, outFile);
}

std::vector<int> bfsPath(const GameState& state, int start, int target) {
    std::vector<bool> used(state.N + 1, false);
    std::vector<int> parent(state.N + 1, -1);
    std::queue<int> q;
    q.push(start);
    used[start] = true;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (v == target) break;
        std::vector<int> next = state.rooms[v].neighbors;
        sort(next.begin(), next.end());
        for (int to : next) {
            if (used[to]) continue;
            if (state.visited.find(to) == state.visited.end()) continue;
            used[to] = true;
            parent[to] = v;
            q.push(to);
        }
    }
    if (!used[target]) return {};
    std::vector<int> path;
    int current = target;
    while (current != start) {
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    return path;
}

std::vector<int> bfsToNearestUnvisited(const GameState& state, int start) {
    std::vector<bool> used(state.N + 1, false);
    std::vector<int> parent(state.N + 1, -1);
    std::queue<int> q;
    q.push(start);
    used[start] = true;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        std::vector<int> next = state.rooms[v].neighbors;
        sort(next.begin(), next.end());
        for (int to : next) {
            if (used[to]) continue;
            used[to] = true;
            parent[to] = v;
            if (state.visited.find(to) == state.visited.end()) {
                std::vector<int> path;
                int current = to;
                while (current != start) {
                    path.push_back(current);
                    current = parent[current];
                }
                reverse(path.begin(), path.end());
                return path;
            }
            q.push(to);
        }
    }
    return {};
}

void printResult(const GameState& state, std::ofstream& outFile) {
    outFile << "result ";
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        outFile << state.collected[i] << " ";
    }
    outFile << state.totalValue << "\n";
}

void returnPhase(GameState& state, std::ofstream& outFile) {
    std::vector<int> path = bfsPath(state, state.currentRoom, 0);
    int requiredFood = (int)path.size();
    int extraFood = state.food - requiredFood;

    for (int to : path) {
        bool finalMove = (to == 0);
        moveTo(state, to, outFile, !finalMove);

        if (to != 0) {
            while (extraFood > 0) {
                int best = getBestResourceInRoom(state.rooms[to], state.targetResource);
                if (best == -1) break;
                collectResource(state, to, best, outFile);
                extraFood--;
            }
        }
    }

    printResult(state, outFile);
}

void explorePhase(GameState& state, std::ofstream& outFile) {
    double exploreFood = state.food / 2.0;

    while (exploreFood > 0.0) {
        std::vector<int> unvisitedNeighbors;

        for (int to : state.rooms[state.currentRoom].neighbors) {
            if (state.visited.find(to) == state.visited.end()) {
                unvisitedNeighbors.push_back(to);
            }
        }

        sort(unvisitedNeighbors.begin(), unvisitedNeighbors.end());

        if (!unvisitedNeighbors.empty()) {
            int nextRoom = unvisitedNeighbors[0];
            moveTo(state, nextRoom, outFile);
            exploreFood -= 1.0;
        } else {
            std::vector<int> path = bfsToNearestUnvisited(state, state.currentRoom);
            if (path.empty()) break;

            for (int to : path) {
                if (exploreFood == 0.0) break;
                moveTo(state, to, outFile);
                exploreFood--;
            }
        }

        int best = getBestResourceInRoom(state.rooms[state.currentRoom], state.targetResource);
        collectResource(state, state.currentRoom, best, outFile);
    }

    returnPhase(state, outFile);
}