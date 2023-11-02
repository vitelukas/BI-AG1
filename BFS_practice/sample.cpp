//~!@!#!@#!@$!@$!@$*!_$)*!)@*$)!@Y$(*!@^$(*!&@{$(%!@$^!@^$(%!*@)$^!@(*$^%)!@$^!@&*$^&#%$@#)&^%&*(@#$!_@$^)}))

/*
 * # TopSort
 *
 * Your task is to implement the function `std::pair<bool, std::vector<Vertex>>
 * topsort(const Graph& G)`. It should return either
 *
 * - `true` and a list of all vertices of `G` in a topological order or
 * - `false` and a list of vertices that form a cycle (there must be an
 *   edge from the last to the first vertex).
 *
 * The class `Graph` represents a directed graph. Important methods are:
 *
 * - `vertices()`: The number of vertices.
 * - `operator[](Vertex v)`: A list of successors of `v`. Vertices are integers starting with 0.
 * - Methods `begin()` and `end()` allow iteration over all vertices and
 *   using `Graph` in range-for like `for (Vertex v : G) ...`.
 * - `reversed()`: Returns a new graph created by flipping the direction of all edges.
 *
 * The time limit is 5 seconds for the small and 3 seconds for the large
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

struct Graph {
    Graph() : Graph(0) {}
    explicit Graph(size_t vertices) : _adj(vertices) {}
    Graph(const std::vector<std::vector<size_t>> &adj) : Graph(adj.size()) {
        for (size_t i = 0; i < adj.size(); i++)
            for (size_t v : adj[i])
                add_edge(Vertex{i}, Vertex{v});
    }

    size_t vertices() const { return _adj.size(); }

    void add_edge(Vertex u, Vertex v) {
        _adj[u].push_back(v);
    }

    const std::vector<Vertex> &operator[](Vertex v) const {
        CHECK(size_t(v) < _adj.size(),
              "Graph: index %zu out of range [0..%zu).", size_t(v), _adj.size());
        return _adj[v];
    }

    Graph reversed() const {
        Graph ret(vertices());
        for (Vertex v : *this)
            for (Vertex w : operator[](v))
                ret.add_edge(w, v);
        return ret;
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

private:
    std::vector<std::vector<Vertex>> _adj;
};

std::ostream &operator<<(std::ostream &out, const Graph &G) {
    out << "{{ ";
    for (Vertex v : G) {
        out << "{";
        for (Vertex w : G[v])
            out << w << ",";
        out << "}, ";
    }
    return out << "}}";
}

#endif
using namespace std;

//!                     ========================= START OF CODE =========================

void create_path(size_t v, size_t start_node, vector<Vertex> &path, vector<size_t> &P) {
    size_t node = P[start_node];

    while (node != start_node) {
        path.insert(path.begin(), (Vertex)node);
        node = P[node];
    }
}

bool dfs(const Graph &G, queue<Vertex> &que, vector<size_t> &P) {
    deque<Vertex> dque;
    // 0 - not visited      - white
    // 1 - visited visited  - grey
    // 2 - closed           - black
    vector<int> visited(G.vertices(), 0);

    while (!que.empty()) {
        dque.push_front(que.front());
        que.pop();
    }

    while (!dque.empty()) {
        auto v = dque.back();

        if (visited[v] != 1) {
            visited[v] = 1;

            for (const auto &neigh : G[v]) {
                if (visited[neigh] == 0) {
                    dque.push_back(neigh);
                } else if (visited[neigh] == 1) {
                    // create_path(v, neigh, que, P);
                    return false; // == there is a cycle
                }
            }
        } else if (visited[v] == 1) {
            dque.pop_back(); // Remove the node from the stack as it has been explored
            visited[v] = 2;
        }
    }
    return true;
}

// Returns either true and a topological order
// or false and a cycle
std::pair<bool, std::vector<Vertex>> topsort(const Graph &G) {
    pair<bool, vector<Vertex>> result;
    result.first = true;
    vector<size_t> P(G.vertices());
    queue<Vertex> que;
    vector<int> v_indegree(G.vertices(), 0);

    // cout << "\nnum of vertices = " << G.vertices() << endl; // TODO

    // Walk through the graph and set the vertex indegrees
    for (const auto &v : G) {
        // cout << "checking vertex: " << v << endl; // TODO
        for (const auto &neigh : G[v]) {
            // cout << "   checking neighbor: " << neigh << endl; // TODO
            v_indegree[neigh]++;
            P[neigh] = v;
        }
    }

    cout << "que = "; // todo: check neighbor
    // Add to the queue all starting vertices (vertices with indegree 0)
    for (size_t i = 0; i < v_indegree.size(); i++) {
        // cout << i << " has indegree = " << v_indegree[i] << endl; // TODO
        if (v_indegree[i] == 0) {
            que.push((Vertex)i);
            cout << i << ", "; // TODO
        }
        // cout << " P[" << i << "] = " << P[i] << endl; // TODO
    }
    cout << endl; // TODO

    if (que.empty()) {
        que.push((Vertex)0);
    }

    // Walk through the rest of the graph and check if there is a cycle
    while (!que.empty()) {
        Vertex v = que.front();
        que.pop();
        result.second.push_back(v);
        cout << "~~~~~ Puhsing " << v << " into the result" << endl; // TODO

        for (const auto &neigh : G[v]) {
            v_indegree[neigh]--;
            if (v_indegree[neigh] == 0) {
                que.push((Vertex)neigh);
                cout << "~~~~~ Puhsing " << neigh << " into the queue" << endl; // TODO
            } else if (v_indegree[neigh] == -1) {
                result.first = false;
                result.second.clear();

                result.second.push_back((Vertex)neigh);

                // bool is_acyclic = dfs(G, que, P);

                create_path(v, neigh, result.second, P);

                cout << "GRAPH IS CYCLIC" << endl; // TODO
                return result;
            }
        }
        for (size_t i = 0; i < result.second.size(); i++) {
            cout << result.second[i] << ", "; // TODO
        }
        cout << endl;
    }

    cout << "GRAPH IS NON-CYCLIC" << endl; // TODO

    return result;
}
//!                     ========================= END OF CODE =========================
#ifndef __PROGTEST__

const Graph SMALL_DAGS[] = {
    {{{1}, {2}, {3}, {4}, {}}},
    {{{1}, {2, 4}, {3}, {4}, {}, {}}},
    {{{1}, {2, 5}, {3}, {4}, {}, {}}},
};

const Graph SMALL_CYCLIC[] = {
    {{{1}, {2}, {3}, {4}, {0}}},
    {{{1}, {2, 5}, {3}, {4}, {0}, {4}}},
};

struct RandomGraphGenerator {
    RandomGraphGenerator(uint32_t seed) : my_rand(seed) {}

    uint32_t num(uint32_t max) { return my_rand() % max; }
    Vertex vertex(const Graph &G) { return Vertex{num(G.vertices())}; }

    Graph graph1(uint32_t s, size_t edges) {
        Graph G(s);
        double rev_chance = 1.2 / edges;

        while (edges--) {
            auto u = vertex(G);
            auto v = vertex(G);
            if (u == v)
                continue;
            if (u < v)
                std::swap(u, v);
            if (num(1'000'000'000) <= rev_chance * 1'000'000'000)
                std::swap(u, v);
            G.add_edge(u, v);
        }

        return G;
    }

    Graph graph2(uint32_t s, double density) {
        Graph G(s);
        double rev_chance = 0.8 / (s * s / 2);

        for (Vertex u : G)
            for (Vertex v : G) {
                if (u < v) {
                    if (num(1'000'000'000) < 1'000'000'000 * density)
                        G.add_edge(u, v);
                } else {
                    if (num(1'000'000'000) < 1'000'000'000 * rev_chance)
                        G.add_edge(u, v);
                }
            }

        return G;
    }

    Graph cycle(uint32_t n) {
        Graph G(n);
        for (uint32_t i = 0; i < n; i++)
            G.add_edge(Vertex{i}, Vertex{(i + 1) % n});
        return G;
    }

private:
    std::mt19937 my_rand;
};

void verify_toporder(const Graph &G, const std::vector<Vertex> &order) {
    CHECK(order.size() == G.vertices(),
          "Top order has %zu vertices but the graph has %zu vertices.",
          order.size(), G.vertices());

    std::vector<size_t> index(G.vertices());
    for (size_t i = 0; i < order.size(); i++)
        index[order[i]] = i;

    for (Vertex v : G)
        for (Vertex w : G[v])
            CHECK(index[v] < index[w],
                  "Edge %zu --> %zu goes backwards.", size_t(v), size_t(w));
}

void verify_cycle(const Graph &G, const std::vector<Vertex> &cycle) {
    auto has_edge_to = [&](Vertex u, Vertex v) {
        for (Vertex w : G[u])
            if (w == v)
                return true;
        return false;
    };

    CHECK(cycle.size(), "Cycle has length zero.");
    CHECK(has_edge_to(cycle.back(), cycle.front()),
          "Missing edge from last (%zu) to first vertex (%zu).",
          size_t(cycle.back()), size_t(cycle.front()));

    for (size_t i = 1; i < cycle.size(); i++)
        CHECK(has_edge_to(cycle[i - 1], cycle[i]),
              "Missing edge from vertex %zu to vertex %zu.", size_t(cycle[i - 1]), size_t(cycle[i]));
}

void test_topsort_inner(const Graph &G) {
    auto [is_dag, data] = topsort(G);
    // std::cout << is_dag;

    std::vector<bool> seen(G.vertices(), false);
    for (Vertex v : data) {
        CHECK(v < G.vertices(),
              "Vertex %zu >= # of vertices == %zu.", size_t(v), G.vertices());

        CHECK(!seen[v], "Vertex %zu is repeated.", size_t(v));
        seen[v] = true;
    }

    if (is_dag)
        verify_toporder(G, data);
    else
        verify_cycle(G, data);
}

void test_topsort(const Graph &G) {
    try {
        test_topsort_inner(G);
    } catch (const TestFailed &e) {
        std::cout << "Test failed: G = " << G << "\n"
                  << e.what() << std::endl;
        throw;
    }
}

void run_tests() {
    std::cout << "Small DAGs..." << std::endl;
    RandomGraphGenerator rgg(53323);
    for (const Graph &G : SMALL_DAGS)
        test_topsort(G);

    std::cout << "Small cyclic graphs..." << std::endl;
    for (const Graph &G : SMALL_CYCLIC)
        test_topsort(G);

    std::cout << "Small random graphs..." << std::endl;
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph1(20 + i, 14 + i);
        test_topsort(G);
    }
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph2(10 + i, 0.7);
        test_topsort(G);
    }

    std::cout << "Big random graphs..." << std::endl;
    for (size_t i = 0; i < 100; i++) {
        Graph G = rgg.graph1(11'000 + 50 * i, 50'000 + 50 * i);
        test_topsort(G);
    }
    for (size_t i = 0; i < 20; i++) {
        Graph G = rgg.graph2(900 + i, 0.7);
        test_topsort(G);
    }
    std::cout << "Long cycle..." << std::endl;
    test_topsort(rgg.cycle(50'000));
}

int main() {
    try {
        run_tests();

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &) {
    }
}

#endif










//! ~@!)@(&~)(!#(*^~!#*(^~)(!&@)(&~!#%^(*)&~!#*( (*~G! #*(~!#*((*^(*^(*^$*(&^@*&$*#%&*#@&(*%&(#*%&(*983720984790287)@(&#$&@$&)(*^@#*&$%)(*!@#%^&#%^&*!^$))))))))))))))
/*
 * # TopSort
 *
 * Your task is to implement the function `std::pair<bool, std::vector<Vertex>>
 * topsort(const Graph& G)`. It should return either
 *
 * - `true` and a list of all vertices of `G` in a topological order or
 * - `false` and a list of vertices that form a cycle (there must be an
 *   edge from the last to the first vertex).
 *
 * The class `Graph` represents a directed graph. Important methods are:
 *
 * - `vertices()`: The number of vertices.
 * - `operator[](Vertex v)`: A list of successors of `v`. Vertices are integers starting with 0.
 * - Methods `begin()` and `end()` allow iteration over all vertices and
 *   using `Graph` in range-for like `for (Vertex v : G) ...`.
 * - `reversed()`: Returns a new graph created by flipping the direction of all edges.
 *
 * The time limit is 5 seconds for the small and 3 seconds for the large
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

struct Graph {
    Graph() : Graph(0) {}
    explicit Graph(size_t vertices) : _adj(vertices) {}
    Graph(const std::vector<std::vector<size_t>> &adj) : Graph(adj.size()) {
        for (size_t i = 0; i < adj.size(); i++)
            for (size_t v : adj[i])
                add_edge(Vertex{i}, Vertex{v});
    }

    size_t vertices() const { return _adj.size(); }

    void add_edge(Vertex u, Vertex v) {
        _adj[u].push_back(v);
    }

    const std::vector<Vertex> &operator[](Vertex v) const {
        CHECK(size_t(v) < _adj.size(),
              "Graph: index %zu out of range [0..%zu).", size_t(v), _adj.size());
        return _adj[v];
    }

    Graph reversed() const {
        Graph ret(vertices());
        for (Vertex v : *this)
            for (Vertex w : operator[](v))
                ret.add_edge(w, v);
        return ret;
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

private:
    std::vector<std::vector<Vertex>> _adj;
};

std::ostream &operator<<(std::ostream &out, const Graph &G) {
    out << "{{ ";
    for (Vertex v : G) {
        out << "{";
        for (Vertex w : G[v])
            out << w << ",";
        out << "}, ";
    }
    return out << "}}";
}

#endif
using namespace std;

//!                     ========================= START OF CODE =========================

void create_path(size_t v, size_t start_node, vector<Vertex> &path, vector<size_t> &P) {
    size_t node = P[start_node];

    while (node != start_node) {
        path.insert(path.begin(), (Vertex)node);
        node = P[node];
    }
}

bool dfs(const Graph &G, deque<Vertex> &dque, vector<size_t> &P, vector<Vertex> &path) {
    // 0 - not visited      - white
    // 1 - visiting         - grey
    // 2 - closed           - black
    vector<int> visited(G.vertices(), 0);

    while (!dque.empty()) {
        auto v = dque.back();

        if (visited[v] != 1) {
            visited[v] = 1;

            for (const auto &neigh : G[v]) {
                if (visited[neigh] == 0) {
                    dque.push_back(neigh);
                } else if (visited[neigh] == 1) {
                    path.clear();
                    create_path(v, neigh, path, P);
                    path.push_back(neigh);
                    return false; // == there is a cycle
                }
            }
        } else if (visited[v] == 1) {
            path.insert(path.begin(), dque.back());
            dque.pop_back(); // Remove the node from the stack as it has been explored
            visited[v] = 2;
        }
    }
    return true;
}

// Returns either true and a topological order
// or false and a cycle
// - true == acyclic
// - false == there is a cycle
std::pair<bool, std::vector<Vertex>> topsort(const Graph &G) {
    pair<bool, vector<Vertex>> result;
    result.first = true;
    // Parents vector
    vector<size_t> P(G.vertices());
    deque<Vertex> dque;
    vector<int> v_indegree(G.vertices(), 0);

    // cout << "\nnum of vertices = " << G.vertices() << endl; // TODO

    // Walk through the graph and set the vertex indegrees
    for (const auto &v : G) {
        // cout << "checking vertex: " << v << endl; // TODO
        for (const auto &neigh : G[v]) {
            // cout << "   checking neighbor: " << neigh << endl; // TODO
            v_indegree[neigh]++;
            P[neigh] = v;
        }
    }

    cout << "dque = "; // todo
    // Add to the queue all starting vertices (vertices with indegree 0)
    for (size_t i = 0; i < v_indegree.size(); i++) {
        // cout << i << " has indegree = " << v_indegree[i] << endl; // TODO
        if (v_indegree[i] == 0) {
            dque.push_back((Vertex)i);
            cout << i << ", "; // TODO
        }
        // cout << " P[" << i << "] = " << P[i] << endl; // TODO
    }
    cout << endl; // TODO

    if (dque.empty()) {
        dque.push_back((Vertex)0);
    }

    result.first = dfs(G, dque, P, result.second);

    // TODO
    for (size_t i = 0; i < result.second.size(); i++) {
        cout << result.second[i] << ", ";
    }
    cout << endl;

    cout << "GRAPH IS: " << (result.first ? " ACYCLIC" : "CYCLIC") << endl; // TODO

    return result;
}
//!                     ========================= END OF CODE =========================
#ifndef __PROGTEST__

const Graph SMALL_DAGS[] = {
    {{{1}, {2}, {3}, {4}, {}}},
    {{{1}, {2, 4}, {3}, {4}, {}, {}}},
    {{{1}, {2, 5}, {3}, {4}, {}, {}}},
};

const Graph SMALL_CYCLIC[] = {
    {{{1}, {2}, {3}, {4}, {0}}},
    {{{1}, {2, 5}, {3}, {4}, {0}, {4}}},
};

struct RandomGraphGenerator {
    RandomGraphGenerator(uint32_t seed) : my_rand(seed) {}

    uint32_t num(uint32_t max) { return my_rand() % max; }
    Vertex vertex(const Graph &G) { return Vertex{num(G.vertices())}; }

    Graph graph1(uint32_t s, size_t edges) {
        Graph G(s);
        double rev_chance = 1.2 / edges;

        while (edges--) {
            auto u = vertex(G);
            auto v = vertex(G);
            if (u == v)
                continue;
            if (u < v)
                std::swap(u, v);
            if (num(1'000'000'000) <= rev_chance * 1'000'000'000)
                std::swap(u, v);
            G.add_edge(u, v);
        }

        return G;
    }

    Graph graph2(uint32_t s, double density) {
        Graph G(s);
        double rev_chance = 0.8 / (s * s / 2);

        for (Vertex u : G)
            for (Vertex v : G) {
                if (u < v) {
                    if (num(1'000'000'000) < 1'000'000'000 * density)
                        G.add_edge(u, v);
                } else {
                    if (num(1'000'000'000) < 1'000'000'000 * rev_chance)
                        G.add_edge(u, v);
                }
            }

        return G;
    }

    Graph cycle(uint32_t n) {
        Graph G(n);
        for (uint32_t i = 0; i < n; i++)
            G.add_edge(Vertex{i}, Vertex{(i + 1) % n});
        return G;
    }

private:
    std::mt19937 my_rand;
};

void verify_toporder(const Graph &G, const std::vector<Vertex> &order) {
    CHECK(order.size() == G.vertices(),
          "Top order has %zu vertices but the graph has %zu vertices.",
          order.size(), G.vertices());

    std::vector<size_t> index(G.vertices());
    for (size_t i = 0; i < order.size(); i++)
        index[order[i]] = i;

    for (Vertex v : G)
        for (Vertex w : G[v])
            CHECK(index[v] < index[w],
                  "Edge %zu --> %zu goes backwards.", size_t(v), size_t(w));
}

void verify_cycle(const Graph &G, const std::vector<Vertex> &cycle) {
    auto has_edge_to = [&](Vertex u, Vertex v) {
        for (Vertex w : G[u])
            if (w == v)
                return true;
        return false;
    };

    CHECK(cycle.size(), "Cycle has length zero.");
    CHECK(has_edge_to(cycle.back(), cycle.front()),
          "Missing edge from last (%zu) to first vertex (%zu).",
          size_t(cycle.back()), size_t(cycle.front()));

    for (size_t i = 1; i < cycle.size(); i++)
        CHECK(has_edge_to(cycle[i - 1], cycle[i]),
              "Missing edge from vertex %zu to vertex %zu.", size_t(cycle[i - 1]), size_t(cycle[i]));
}

void test_topsort_inner(const Graph &G) {
    auto [is_dag, data] = topsort(G);
    // std::cout << is_dag;

    std::vector<bool> seen(G.vertices(), false);
    for (Vertex v : data) {
        CHECK(v < G.vertices(),
              "Vertex %zu >= # of vertices == %zu.", size_t(v), G.vertices());

        CHECK(!seen[v], "Vertex %zu is repeated.", size_t(v));
        seen[v] = true;
    }

    if (is_dag)
        verify_toporder(G, data);
    else
        verify_cycle(G, data);
}

void test_topsort(const Graph &G) {
    try {
        test_topsort_inner(G);
    } catch (const TestFailed &e) {
        std::cout << "Test failed: G = " << G << "\n"
                  << e.what() << std::endl;
        throw;
    }
}

void run_tests() {
    std::cout << "Small DAGs..." << std::endl;
    RandomGraphGenerator rgg(53323);
    for (const Graph &G : SMALL_DAGS)
        test_topsort(G);

    std::cout << "Small cyclic graphs..." << std::endl;
    for (const Graph &G : SMALL_CYCLIC)
        test_topsort(G);

    std::cout << "Small random graphs..." << std::endl;
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph1(20 + i, 14 + i);
        test_topsort(G);
    }
    for (size_t i = 0; i < 30; i++) {
        Graph G = rgg.graph2(10 + i, 0.7);
        test_topsort(G);
    }

    std::cout << "Big random graphs..." << std::endl;
    for (size_t i = 0; i < 100; i++) {
        Graph G = rgg.graph1(11'000 + 50 * i, 50'000 + 50 * i);
        test_topsort(G);
    }
    for (size_t i = 0; i < 20; i++) {
        Graph G = rgg.graph2(900 + i, 0.7);
        test_topsort(G);
    }
    std::cout << "Long cycle..." << std::endl;
    test_topsort(rgg.cycle(50'000));
}

int main() {
    try {
        run_tests();

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &) {
    }
}

#endif
