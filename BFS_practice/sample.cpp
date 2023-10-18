/*
 * # BFS
 *
 * Your task is to implement the function `size_t bfs(
 * const Graph& G, Vertex u, std::vector<Vertex>& P,
 * std::vector<size_t>& D)` which implements the breadth-first
 * search. Its arguments are:
 *
 * - `Graph G`: The graph to search. See description of `Graph` below.
 * - `Vertex u`: The starting vertex.
 * - `std::vector<Vertex>& P`: The array of predecessors on the shortest
 *   path from `u`. It has the correct size and it is initialized with
 *   `NO_VERTEX` before calling `bfs`.
 * - `std::vector<size_t>& D`: The array of distances from `u`. It has
 *    the correct size and it is initialized with `NO_VERTEX` before
 *    calling `bfs`.
 *
 * The return value is the number of visited vertices and the arrays
 * `P` and `D` must be filled appropriately. The predecessor of the
 * starting vertex should be set to `ROOT`. Your implementation should
 * run in the time linear in the size of the visited part of the graph.
 *
 * The class `Graph` represents an undirected or directed graph. Important
 * methods are:
 *
 * - `vertices()`: The number of vertices.
 * - `operator[](Vertex v)`: A list of neighbors (resp. successors in the
 *   directed case) of `v`. Vertices are integers starting with 0.
 * - Methods `begin()` and `end()` allow iteration over all vertices and
 *   using `Graph` in range-for like `for (Vertex v : G) ...`.
 *
 * The time limit is 5 seconds for the small and 3.5 seconds for the large
 * test.
 *
 */

#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <type_traits>
#include <vector>

struct TestFailed : std::runtime_error {
    using std::runtime_error::runtime_error;
};

std::string fmt(const char *f, ...) {
    va_list args1;
    va_list args2;
    va_start(args1, f);
    va_copy(args2, args1);

    std::string buf(vsnprintf(nullptr, 0, f, args1), '\0');
    va_end(args1);

    vsnprintf(buf.data(), buf.size() + 1, f, args2);
    va_end(args2);

    return buf;
}

#define CHECK(succ, ...)                        \
    do {                                        \
        if (!(succ))                            \
            throw TestFailed(fmt(__VA_ARGS__)); \
    } while (0)

enum Vertex : size_t {
    NO_VERTEX = -size_t(1),
    ROOT = -size_t(2)
};

enum : size_t { NO_DISTANCE = -size_t(1) };

struct Graph {
    Graph() : Graph(false, 0) {}
    Graph(bool directed, size_t vertices) : _dir(directed), _adj(vertices) {}
    Graph(bool directed, const std::vector<std::vector<size_t>> &adj)
        : Graph(directed, adj.size()) {
        for (size_t i = 0; i < adj.size(); i++)
            for (size_t v : adj[i])
                add_edge(Vertex{i}, Vertex{v});
    }

    bool is_directed() const { return _dir; }
    size_t vertices() const { return _adj.size(); }

    void add_edge(Vertex u, Vertex v) {
        _adj[u].push_back(v);
        if (!_dir)
            _adj[v].push_back(u);
    }

    const std::vector<Vertex> &operator[](Vertex v) const {
        CHECK(size_t(v) < _adj.size(),
              "Graph: index %zu out of range [0..%zu).", size_t(v), _adj.size());

        if (!_seen.empty()) {
            CHECK(!_seen[v], "Graph: vertex %zu examined second time", size_t(v));
            _seen[v] = true;
        }

        return _adj[v];
    }

    struct Iterator {
        Iterator() = default;

        Iterator &operator++() {
            _v++;
            return *this;
        }
        Vertex operator*() const { return Vertex{_v}; }

        friend bool operator==(Iterator a, Iterator b) { return a._v == b._v; }
        friend bool operator!=(Iterator a, Iterator b) { return !(a == b); }

    private:
        friend struct Graph;
        Iterator(size_t v) : _v(v) {}

        size_t _v = NO_VERTEX;
    };

    Iterator begin() const { return {0}; }
    Iterator end() const { return {vertices()}; }

    void bfs_debug_begin() const { _seen.assign(_adj.size(), false); }
    void bfs_debug_end() const { _seen.assign(0, false); }

private:
    bool _dir;
    std::vector<std::vector<Vertex>> _adj;
    mutable std::vector<bool> _seen;
};

std::ostream &operator<<(std::ostream &out, const Graph &G) {
    out << "{ " << (G.is_directed() ? "true" : "false") << ", { ";
    for (Vertex v : G) {
        out << "{";
        for (Vertex w : G[v])
            out << w << ",";
        out << "}, ";
    }
    return out << "} }";
}

#endif

//!                             ============================== START OF MY CODE =============================
using namespace std;

// TODO implement
// - Arrays P and D have the correct size and are set to NO_VERTEX resp. NO_DISTANCE
//   before calling bfs.
// - Function bfs must set predecesor of u to ROOT.
// - Return value is the number of visited vertices.
size_t bfs(const Graph &G, Vertex u, std::vector<Vertex> &P, std::vector<size_t> &D) {
    size_t visited_nodes = 0;
    queue<Vertex> que;
    vector<bool> visited(G.vertices(), false);

    que.push(u);
    visited[u] = true;
    D[u] = 0;
    P[u] = ROOT;

    while (!que.empty()) {
        Vertex v = que.front();
        que.pop();

        visited_nodes++;

        size_t cur_dist = D[v];

        for (const auto &neigh : G[v]) {
            if (!visited[neigh]) {
                visited[neigh] = true;
                D[neigh] = cur_dist + 1;
                P[neigh] = v;
                que.push(neigh);
            }
        }
    }

    return visited_nodes;
}

//!                             ============================== END OF MY CODE =============================

#ifndef __PROGTEST__

const Graph SMALL_GRAPHS[] = {
    {false, {{1}, {2}, {3}, {4}, {}}},
    {false, {{1}, {2}, {3}, {4}, {0}}},
    {false, {{1}, {2, 4}, {3}, {4}, {}, {}}},
    {false, {{1}, {2, 5}, {3}, {4}, {}, {}}},
    {false, {{1}, {2, 5}, {3}, {4}, {0}, {4}}},
    {true, {{1}, {2}, {3}, {4}, {}}},
    {true, {{1}, {2}, {3}, {4}, {0}}},
    {true, {{1}, {2, 4}, {3}, {4}, {}, {}}},
    {true, {{1}, {2, 5}, {3}, {4}, {}, {}}},
    {true, {{1}, {2, 5}, {3}, {4}, {0}, {4}}},
};

struct RandomGraphGenerator {
    RandomGraphGenerator(uint32_t seed) : my_rand(seed) {}

    uint32_t num(uint32_t max) { return my_rand() % max; }
    Vertex vertex(const Graph &G) { return Vertex{num(G.vertices())}; }

    Graph graph1(uint32_t s, size_t edges, bool directed = true) {
        Graph G(directed, s);

        while (edges--) {
            auto u = vertex(G);
            auto v = vertex(G);
            G.add_edge(u, v);
        }

        return G;
    }

    Graph graph2(uint32_t s, double density, bool directed = true) {
        Graph G(directed, s);

        for (Vertex u : G)
            for (Vertex v : G)
                if (num(1'000'000'000) < 1'000'000'000 * density)
                    G.add_edge(u, v);

        return G;
    }

private:
    std::mt19937 my_rand;
};

void test_bfs_inner(const Graph &G, Vertex u) {
    std::vector<Vertex> P(G.vertices(), NO_VERTEX);
    std::vector<size_t> D(G.vertices(), NO_DISTANCE);

    G.bfs_debug_begin();
    size_t seen_t = bfs(G, u, P, D);
    G.bfs_debug_end();

    std::vector<bool> pred_ok(G.vertices(), false);

    CHECK(P[u] == ROOT, "P[u] != ROOT but %zu.", size_t(P[u]));
    CHECK(D[u] == 0, "D[u] != 0 but %zu.", D[u]);
    pred_ok[u] = true;

    for (Vertex v : G) {
        if (P[v] == NO_VERTEX) {
            CHECK(D[v] == NO_DISTANCE, "P[%zu] == NO_VERTEX but D[%u] == %zu not NO_DISTANCE.",
                  size_t(v), size_t(v), D[v]);

            pred_ok[v] = true;
            continue;
        }

        if (v != u) {
            CHECK(P[v] < G.vertices(),
                  "P[%zu] == %zu >= %zu (# of vertices).", size_t(v), size_t(P[v]), G.vertices());
            CHECK(P[P[v]] != NO_VERTEX,
                  "P[%zu] == %zu but P[%zu] == NO_VERTEX.", size_t(v), size_t(P[v]), size_t(P[v]));
        }

        for (Vertex w : G[v]) {
            CHECK(D[w] <= D[v] + 1, "D[%zu] == %zu but its neighbor has D[%zu] == %zu.",
                  size_t(w), D[w], size_t(v), D[v]);

            if (P[w] != v)
                continue;

            CHECK(D[w] == D[v] + 1, "P[%zu] == %zu but D[%zu] == %zu != D[%zu] + 1 == %zu",
                  size_t(w), size_t(v), size_t(w), D[w], size_t(v), 1 + D[v]);
            pred_ok[w] = true;
        }
    }

    size_t seen_r = 0;
    for (Vertex v : G) {
        CHECK(pred_ok[v], "P[%zu] == %zu but there is no edge.", size_t(v), size_t(P[v]));
        seen_r += (P[v] != NO_VERTEX);
    }

    CHECK(seen_r == seen_t,
          "Reported size of component is %zu but it should be %zu.", seen_t, seen_r);
}

void test_bfs(const Graph &G, Vertex u) {
    try {
        test_bfs_inner(G, u);
    } catch (const TestFailed &e) {
        G.bfs_debug_end();
        std::cout << "Test failed: v = " << u << ", G = " << G << "\n"
                  << e.what() << std::endl;
        throw;
    }
}

void run_tests() {
    std::cout << "Hardcoded graphs..." << std::endl;
    for (const Graph &G : SMALL_GRAPHS)
        for (Vertex u : G)
            test_bfs(G, u);

    RandomGraphGenerator rgg(53323);
    std::cout << "Small random graphs..." << std::endl;
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph1(10 + i, 4 * (10 + i));
        Vertex u = rgg.vertex(G);
        test_bfs(G, u);
    }
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph2(10 + i, 0.7);
        Vertex u = rgg.vertex(G);
        test_bfs(G, u);
    }

    std::cout << "Big random graphs..." << std::endl;
    for (size_t i = 0; i < 20; i++) {
        Graph G = rgg.graph1(30'000 + 50 * i, 150'000 + 300 * i);
        Vertex u = rgg.vertex(G);
        test_bfs(G, u);
    }
    for (size_t i = 0; i < 20; i++) {
        Graph G = rgg.graph2(900 + i, 0.7);
        Vertex u = rgg.vertex(G);
        test_bfs(G, u);
    }
}

int main() {
    try {
        run_tests();

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &) {
    }
}

#endif
