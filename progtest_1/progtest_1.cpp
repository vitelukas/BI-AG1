#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Point : size_t {};

struct Path {
    Point from, to;
    unsigned length;

    Path(size_t f, size_t t, unsigned l) : from{f}, to{t}, length{l} {}

    friend bool operator==(const Path &a, const Path &b) {
        return std::tie(a.from, a.to, a.length) == std::tie(b.from, b.to, b.length);
    }

    friend bool operator!=(const Path &a, const Path &b) { return !(a == b); }
};

#endif
//!                             -------------------------------------------- START OF MY CODE -------------------------------------------
using namespace std;
void construct_graph(const vector<Path> &all_paths, stack<size_t> &starting_nodes, unordered_map<size_t, vector<Path>> &croads_children);
void bfs(const vector<Path> &all_paths, vector<Path> &longest_path, stack<size_t> &que, const unordered_map<size_t, vector<Path>> &croads_children);
vector<Path> create_path(const unordered_map<size_t, Path> &crossroads, size_t source_node);

//~  ########## DELETE ##########
void print_path(const vector<Path> &longest_path) {
    // cout << endl
    //  << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "Longest path: " << endl;
    for (const auto &p : longest_path) {
        cout << p.from << " -> " << p.to << "  # " << p.length << endl;
    }
}

vector<Path> longest_track(size_t points, const vector<Path> &all_paths) {
    vector<Path> longest_path; // final path returned by the functions
    if (all_paths.empty())
        return longest_path;

    stack<size_t> que; // queue of nodes
    unordered_map<size_t, vector<Path>> croads_children;

    // topologically sort the graph to get the starting nodes,
    // which will be placed into the que
    construct_graph(all_paths, que, croads_children);

    bfs(all_paths, longest_path, que, croads_children);

    // print_path(longest_path); //~  ########## DELETE ##########

    return longest_path;
}

void construct_graph(const vector<Path> &all_paths, stack<size_t> &starting_nodes, unordered_map<size_t, vector<Path>> &croads_children) {
    unordered_map<size_t, size_t> crossroad_indegree;
    crossroad_indegree.reserve(all_paths.size());

    for (const auto &path : all_paths) {
        croads_children[path.from].emplace_back(path);

        crossroad_indegree.emplace(path.from, 0); // create the parent node in the map so that we can later check which nodes have indegree == 0
        crossroad_indegree[path.to]++;            // increase the indegree value of TO crossroad
    }

    //? queue of crossroad_indegree "on the top" (with 0 indegrees)
    // cout << "Indegrees of vertices" << endl; //~  ########## DELETE ##########
    for (const auto &c : crossroad_indegree) {
        if (c.second == 0)
            starting_nodes.emplace(c.first);

        // cout << c.first << ": " << c.second << endl; //~  ########## DELETE ##########
    }
}

void bfs(const vector<Path> &all_paths, vector<Path> &longest_path, stack<size_t> &que, const unordered_map<size_t, vector<Path>> &croads_children) {
    size_t cur_longest_path = 0;
    size_t cur_longest_node = SIZE_MAX;
    unordered_map<size_t, bool> visited;
    unordered_map<size_t, Path> parents;
    unordered_map<size_t, size_t> distances;
    //?   |            |        |
    //? point(node)   parent   distance

    // Set the distance of all starting vertices to 0 and set their parents to INF
    stack<size_t> temp_que = que;
    while (!temp_que.empty()) {
        size_t node = temp_que.top(); // Get the front element
        temp_que.pop();               // Remove the front element

        distances[node] = 0;
        parents.emplace(node, Path(node, node, 0)); // Set the parent of the node to itself, since it is a starting node
    }

    while (!que.empty()) {
        size_t node = que.top();
        visited[node] = true;
        que.pop();

        if (croads_children.find(node) == croads_children.end())
            continue;

        // cout << "____checking children of node: " << node << "____" << endl; //~  ########## DELETE ##########
        for (const auto &path : croads_children.at(node)) {
            if (visited[path.to] || (distances[path.to] >= (distances[path.from] + path.length)))
                continue;

            visited[path.to] = true;
            // If the parent for the given was already created, erase the old one and update it
            if (parents.find(path.to) != parents.end()) {
                parents.erase(path.to);
            }
            parents.emplace(path.to, path);                          //? set the parent of the current node
            distances[path.to] = distances[path.from] + path.length; //? set the acutal distance of the current node

            // cout << "pushing node: " << path.to << "into the queue" << endl; //~  ########## DELETE ##########
            que.push(path.to);

            if (distances[path.to] > cur_longest_path) {
                cur_longest_path = distances[path.to];
                cur_longest_node = path.to;
            }
        }
    }

    longest_path = create_path(parents, cur_longest_node);
}

vector<Path> create_path(const unordered_map<size_t, Path> &crossroads, size_t node) {
    vector<Path> result;

    // Backtrace the path from the source node == node with the biggest distance
    while (crossroads.at(node).from != node) {
        // Reverse the vector so that it will start in the top node
        result.insert(result.begin(), crossroads.at(node));
        node = crossroads.at(node).from;
    }

    return result;
}

//!                             -------------------------------------------- END OF MY CODE ---------------------------------------------
#ifndef __PROGTEST__

struct Test {
    unsigned longest_track;
    size_t points;
    std::vector<Path> all_paths;
};

inline const Test TESTS[] = {
    {13, 5, {{3, 2, 10}, {3, 0, 9}, {0, 2, 3}, {2, 4, 1}}},
    {11, 5, {{3, 2, 10}, {3, 1, 4}, {1, 2, 3}, {2, 4, 1}}},
    {16, 8, {{3, 2, 10}, {3, 1, 1}, {1, 2, 3}, {1, 4, 15}}},
};

#define CHECK(cond, ...)              \
    do {                              \
        if (cond)                     \
            break;                    \
        printf("Fail: " __VA_ARGS__); \
        printf("\n");                 \
        return false;                 \
    } while (0)

bool run_test(const Test &t) {
    auto sol = longest_track(t.points, t.all_paths);

    unsigned length = 0;
    for (auto [_, __, l] : sol)
        length += l;

    CHECK(t.longest_track == length,
          "Wrong length: got %u but expected %u", length, t.longest_track);

    for (size_t i = 0; i < sol.size(); i++) {
        CHECK(std::count(t.all_paths.begin(), t.all_paths.end(), sol[i]),
              "Solution contains non-existent path: %zu -> %zu (%u)",
              sol[i].from, sol[i].to, sol[i].length);

        if (i > 0)
            CHECK(sol[i].from == sol[i - 1].to,
                  "Paths are not consecutive: %zu != %zu", sol[i - 1].to, sol[i].from);
    }

    return true;
}
#undef CHECK

int main() {
    int ok = 0, fail = 0;

    for (auto &&t : TESTS)
        (run_test(t) ? ok : fail)++;

    if (!fail)
        printf("Passed all %i tests!\n", ok);
    else
        printf("Failed %u of %u tests.\n", fail, fail + ok);
}

#endif