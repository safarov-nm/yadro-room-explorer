#include "../include/game.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

int main(int argc, char* argv[]) {

    if (argc != EXPECTED_ARG_COUNT) {
        std::cout << "Incorrect number of arguments." << std::endl;
        return 0;
    }

    std::ifstream input(argv[1]);

    if (!input.is_open()) {
        std::cout << "Cannot open input file." << std::endl;
        return 0;
    }

    std::ofstream outFile("result.txt");

    int N;
    if (!(input >> N)) {
        outFile << "N";
        return 0;
    }

    if (N < MINIMUM_NUMBER_OF_ROOMS || N > MAXIMUM_NUMBER_OF_ROOMS) {
        outFile << N;
        return 0;
    }

    std::vector<Room> rooms(N + 1);
    std::string line;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i <= N; ++i) {
        if (!std::getline(input, line)) {
            outFile << "line " << i;
            return 0;
        }

        if (line.empty()) {
            i--;
            continue;
        }

        std::stringstream ss(line);
        Room room;
        std::string neighborsString;

        if (!(ss >> room.id >> neighborsString)) {
            outFile << line;
            return 0;
        }

        if (room.id != i) {
            outFile << line;
            return 0;
        }

        if (!parseNeighbors(neighborsString, room.neighbors, N)) {
            outFile << line;
            return 0;
        }

        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
            if (!(ss >> room.resources[j])) {
                outFile << line;
                return 0;
            }
        }
        
        std::string extra;
        if (ss >> extra) {
            outFile << line;
            return 0;
        }

        rooms[i] = room;
    }

    for (int i = 0; i <= N; ++i) {
        for (int to : rooms[i].neighbors) {
            if (std::find(rooms[to].neighbors.begin(), rooms[to].neighbors.end(), i) == rooms[to].neighbors.end()) {
                rooms[to].neighbors.push_back(i);
            }
        }
    }

    double M;
    std::string targetString;

    while (input.peek() == '\n' || input.peek() == '\r') {
        input.get();
    }

    if (!(input >> M >> targetString)) {
        if (input >> M) {
            outFile << M;
        } else if (!targetString.empty()) {
            outFile << targetString;
        } else {
            outFile << "The amount of food and the target resource could not be read";
        }
        return 0;
    }
    if (M < 2.0 || M > 255.0) {
        outFile << M;
        return 0;
    }

    int targetResource = getResourceType(targetString);

    if (targetResource == -1) {
        outFile << targetString;
        return 0;
    }

    if (M < 1.0) {
        outFile << M;
        return 0;
    }

    GameState state;
    state.N = N;
    state.food = M;
    state.targetResource = targetResource;
    state.currentRoom = 0;
    state.rooms = rooms;
    state.visited.insert(0);

    explorePhase(state, outFile);

    return 0;
}