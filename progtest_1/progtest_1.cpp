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

struct Tnode {
    Tnode() = default;

    bool n_visited = 0;
    size_t distance = 0;
    size_t n_indegree = 0;
    vector<Path> n_children;
    Path parent_path = {0, 0, 0};
};

void construct_graph(const vector<Path> &all_paths, stack<size_t> &starting_nodes, vector<Tnode> &nodesData);
void bfs(vector<Path> &longest_path, stack<size_t> &que, vector<Tnode> &nodesData);
vector<Path> create_path(vector<Tnode> &nodesData, size_t node);

vector<Path> longest_track(size_t points, const vector<Path> &all_paths) {
    vector<Path> longest_path; // final path returned by the functions
    if (all_paths.empty())
        return longest_path;

    stack<size_t> que; // queue of nodes
    vector<Tnode> nodesData(points);
    nodesData.reserve(points);

    // topologically sort the graph to get the starting nodes,
    // which will be placed into the que
    construct_graph(all_paths, que, nodesData);
    bfs(longest_path, que, nodesData);

    return longest_path;
}

void construct_graph(const vector<Path> &all_paths, stack<size_t> &starting_nodes, vector<Tnode> &nodesData) {
    for (const auto &path : all_paths) {
        Tnode &node_from = nodesData[(size_t)path.from]; // create the parent node in the map so that we can later check which nodes have indegree == 0
        Tnode &node_to = nodesData[(size_t)path.to];
        node_from.n_children.emplace_back(path);
        node_from.parent_path.from = path.from;
        node_to.n_indegree++; // increase the indegree value of TO crossroad
    }

    //? queue of crossroad_indegree "on the top" (with 0 indegrees)
    for (auto &n : nodesData) {
        if (n.n_indegree == 0) {
            size_t node_itself = n.parent_path.from;
            n.parent_path = Path(node_itself, node_itself, 0); // Set the parent of the node to itself, since it is a starting node
            starting_nodes.emplace(node_itself);
        }
    }
}

void bfs(vector<Path> &longest_path, stack<size_t> &que, vector<Tnode> &nodesData) {
    size_t cur_longest_path = 0;
    size_t cur_longest_node = SIZE_MAX;

    while (!que.empty()) {
        Tnode &node_from = nodesData[que.top()];
        que.pop();
        node_from.n_visited = true;

        // If the crossroad doesn't have any children -> skip it
        if (node_from.n_children.empty())
            continue;

        // Check all paths poiting from this crossroad (check the crossroad's children)
        for (const auto &path : node_from.n_children) {
            Tnode &node_to = nodesData[(size_t)path.to];
            if (node_to.n_visited && (node_to.distance >= (node_from.distance + path.length)))
                continue;

            node_to.n_visited = true;
            // If the parent for the given node was already created, erase the old one and update it
            node_to.parent_path = path;                          //? set the parent of the current node
            node_to.distance = node_from.distance + path.length; //? set the acutal distance of the current node

            que.push(path.to);

            if (node_to.distance > cur_longest_path) {
                cur_longest_path = node_to.distance;
                cur_longest_node = path.to;
            }
        }
    }

    longest_path = create_path(nodesData, cur_longest_node);
}

vector<Path> create_path(vector<Tnode> &nodesData, size_t node) {
    vector<Path> result;

    // Backtrace the path from the source node == node with the biggest distance
    while ((size_t)nodesData[node].parent_path.from != node) {
        // Insert the Path to the start of the vector,
        // so that the result path will be automatically build in reverse (from the TOP node)
        const Path &p = nodesData[node].parent_path;
        result.insert(result.begin(), p);
        node = p.from;
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
    // !!!!!!!!!!!!!!!!!! CUSTOM ASSERTS !!!!!!!!!!!!!!!!!!
    {39, 13, {{7, 11, 9}, {10, 12, 11}, {0, 2, 4}, {2, 8, 2}, {0, 3, 9}, {9, 10, 9}, {3, 7, 10}, {1, 5, 5}, {5, 3, 6}, {5, 6, 4}, {6, 9, 10}, {3, 4, 5}, {4, 10, 7}, {10, 11, 6}, {2, 3, 5}, {8, 7, 7}}},
    {100, 15, {{9, 10, 9}, {10, 11, 6}, {13, 14, 100}, {7, 11, 9}, {0, 3, 9}, {3, 7, 10}, {3, 4, 5}, {10, 12, 11}, {1, 5, 5}, {0, 2, 4}, {2, 8, 2}, {2, 3, 5}, {8, 7, 7}, {6, 9, 10}, {5, 3, 6}, {5, 6, 4}, {4, 10, 7}}},
    // !!!!!!!!!!!!!!!!!! CUSTOM ASSERTS !!!!!!!!!!!!!!!!!!
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