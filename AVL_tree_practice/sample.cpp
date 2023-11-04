#ifndef __PROGTEST__
#include <array>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <type_traits>

// We use std::set as a reference to check our implementation.
// It is not available in progtest :)
#include <set>

template <typename T>
struct Ref {
    size_t size() const { return _data.size(); }
    const T *find(const T &value) const {
        auto it = _data.find(value);
        if (it == _data.end())
            return nullptr;
        return &*it;
    }
    bool insert(const T &value) { return _data.insert(value).second; }
    bool erase(const T &value) { return _data.erase(value); }

    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }

private:
    std::set<T> _data;
};

#endif

namespace config {
// Enable to check that the tree is AVL balanced.
inline constexpr bool CHECK_DEPTH = true;

// Disable if your implementation does not have parent pointers
inline constexpr bool PARENT_POINTERS = true;
} // namespace config

//!                           ========================= START OF CODE =============================
using namespace std;

template <typename T>
struct Tree {
private:
    struct TNode {
        T value;
        size_t height = 1;
        int balance = 0;
        TNode *parent = nullptr;
        TNode *left = nullptr;
        TNode *right = nullptr;

        TNode() = default;
        TNode(T data) : value(data), height(1), balance(0), parent(nullptr), left(nullptr), right(nullptr) {}

        friend bool operator<(const TNode &a, const TNode &b) {
            return a.value < b.value;
        }
    };

    // Tree struct variables
    TNode *root;
    size_t m_size;

    TNode *find(TNode *node, const T &value) const {
        if (node == nullptr)
            return nullptr;

        if (node->value == value)
            return node;

        if (value < node->value) {
            return find(node->left, value);
        } else if (value > node->value) {
            return find(node->right, value);
        }

        return nullptr; // Value not found in the tree
    }

    TNode *insert(TNode *node, T value) {
        if (node == nullptr)
            return new TNode(value); // Create a new TNode dynamically

        if (value < node->value) {
            node->left = insert(node->left, value);
            node->left->parent = node;
        } else if (value > node->value) {
            node->right = insert(node->right, value);
            node->right->parent = node;
        }

        update(node);

        // Rebalance the tree
        return balance(node);
    }

    // Update balance and height
    void update(TNode *node) {
        if (node == nullptr)
            return;

        int l_height = 0;
        int r_height = 0;

        if (node->left)
            l_height = node->left->height;
        if (node->right)
            r_height = node->right->height;

        node->height = 1 + max(l_height, r_height);

        node->balance = r_height - l_height;
    }

    // Balances the tree if needed
    TNode *balance(TNode *node) {

        // Left heavy
        if (node->balance < -1) {
            if (node->left->balance > 0) {
                node->left = left_rotation(node->left);
            }
            return right_rotation(node);
        } else if (node->balance > 1) { // Right heavy
            if (node->right->balance < 0) {
                node->right = right_rotation(node->right);
            }
            return left_rotation(node);
        }

        // The node has balance of -1, 0, or 1 => we don't need to balance it
        return node;
    }

    TNode *left_rotation(TNode *X) {
        TNode *P = X->parent;
        TNode *Y = X->right;
        X->right = Y->left;
        if (Y->left)
            Y->left->parent = X;
        Y->left = X;
        X->parent = Y;
        Y->parent = P;

        // Update son of the parent
        if (P != nullptr) {
            if (P->left == X)
                P->left = Y;
            else
                P->right = Y;
        }
        update(X);
        update(Y);
        update(P);
        return Y;
    }

    TNode *right_rotation(TNode *X) {
        TNode *P = X->parent;
        TNode *Y = X->left;
        X->left = Y->right;
        if (Y->right)
            Y->right->parent = X;
        Y->right = X;
        X->parent = Y;
        Y->parent = P;

        // Update son of the parent
        if (P != nullptr) {
            if (P->left == X)
                P->left = Y;
            else
                P->right = Y;
        }
        update(X);
        update(Y);
        update(P);
        return Y;
    }

    TNode *erase(TNode *node, T value) {
        if (node == nullptr)
            return nullptr; // Value not found in the tree

        if (value < node->value) {
            node->left = erase(node->left, value);
        } else if (value > node->value) {
            node->right = erase(node->right, value);
        } else { // Node to be deleted is found
            // Case_1: Node with only one child or no child
            if (node->left == nullptr || node->right == nullptr) {
                TNode *temp = node->left ? node->left : node->right;
                if (temp)
                    temp->parent = node->parent; // Update the parent pointer of the new child
                delete node;
                return temp;
            } else { // Case_2: Node with two children
                // Find the successor of the node and replace the node with it
                T succ_value = find_successor(node->right);
                node->value = succ_value;
                // Delete the successor to remove the duplicate
                node->right = erase(node->right, succ_value);
            }
        }

        if (node) {
            update(node);
            return balance(node); // Rebalance the tree
        }

        return nullptr;
    }

    T find_successor(TNode *node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node->value;
    }

    void delete_tree(TNode *node) {
        if (node == nullptr)
            return;

        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }

    //~  		############ FUNCTION IMPLEMENTATIONS FOR PROGTEST ############
public:
    Tree() : root(), m_size(0) {}
    ~Tree() {
        // Free the allocated memory of the nodes
        delete_tree(root);
    }

    size_t size() const {
        return m_size;
    }

    const T *find(const T &value) const {
        TNode *node = find(root, value);
        return &node->value;
    }

    // TODO - optimize by not searching the node twice -> only search it in the find function
    bool insert(T value) {
        // Return false if the value already exists in the tree
        if (find(root, value))
            return false;

        root = insert(root, value);
        m_size++;
        return true;
    }

    // TODO - optimize by not searching the node twice -> only search it in the erase function
    bool erase(const T &value) {
        // Return false if the value is not in the tree
        if (!find(root, value))
            return false;

        root = erase(root, value);
        m_size--;
        return true;
    }

    // Needed to test the structure of the tree.
    // Replace Node with the real type of your nodes
    // and implementations with the ones matching
    // your attributes.
    struct TesterInterface {
        // using Node = TNode
        static const TNode *root(const Tree *t) { return t->root; }
        // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
        static const TNode *parent(const TNode *n) { return n->parent; }
        static const TNode *right(const TNode *n) { return n->right; }
        static const TNode *left(const TNode *n) { return n->left; }
        static const T &value(const TNode *n) { return n->value; }
    };
};

//!                           ========================= END OF CODE =============================
#ifndef __PROGTEST__

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

template <typename T>
struct Tester {
    Tester() = default;

    void size() const {
        size_t r = ref.size();
        size_t t = tested.size();
        if (r != t)
            throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
    }

    void find(const T &x) const {
        auto r = ref.find(x);
        auto t = tested.find(x);
        bool found_r = r != nullptr;
        bool found_t = t != nullptr;

        if (found_r != found_t)
            _throw("Find mismatch", found_r);
        if (found_r && *t != x)
            throw TestFailed("Find: found different value");
    }

    void insert(const T &x, bool check_tree_ = false) {
        auto succ_r = ref.insert(x);
        auto succ_t = tested.insert(x);
        if (succ_r != succ_t)
            _throw("Insert mismatch", succ_r);
        size();
        if (check_tree_)
            check_tree();
    }

    void erase(const T &x, bool check_tree_ = false) {
        bool succ_r = ref.erase(x);
        auto succ_t = tested.erase(x);
        if (succ_r != succ_t)
            _throw("Erase mismatch", succ_r);
        size();
        if (check_tree_)
            check_tree();
    }

    struct NodeCheckResult {
        const T *min = nullptr;
        const T *max = nullptr;
        int depth = -1;
        size_t size = 0;
    };

    void check_tree() const {
        using TI = typename Tree<T>::TesterInterface;
        auto ref_it = ref.begin();
        bool check_value_failed = false;
        auto check_value = [&](const T &v) {
            if (check_value_failed)
                return;
            check_value_failed = (ref_it == ref.end() || *ref_it != v);
            if (!check_value_failed)
                ++ref_it;
        };

        auto r = check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);
        size_t t_size = tested.size();

        if (t_size != r.size)
            throw TestFailed(
                fmt("Check tree: size() reports %zu but expected %zu.", t_size, r.size));

        if (check_value_failed)
            throw TestFailed(
                "Check tree: element mismatch");

        size();
    }

    template <typename Node, typename F>
    NodeCheckResult check_node(const Node *n, const Node *p, F &check_value) const {
        if (!n)
            return {};

        using TI = typename Tree<T>::TesterInterface;
        if constexpr (config::PARENT_POINTERS) {
            if (TI::parent(n) != p)
                throw TestFailed("Parent mismatch.");
        }

        auto l = check_node(TI::left(n), n, check_value);
        check_value(TI::value(n));
        auto r = check_node(TI::right(n), n, check_value);

        if (l.max && !(*l.max < TI::value(n)))
            throw TestFailed("Max of left subtree is too big.");
        if (r.min && !(TI::value(n) < *r.min))
            throw TestFailed("Min of right subtree is too small.");

        if (config::CHECK_DEPTH && abs(l.depth - r.depth) > 1)
            throw TestFailed(fmt(
                "Tree is not avl balanced: left depth %i and right depth %i.",
                l.depth, r.depth));

        return {
            l.min ? l.min : &TI::value(n),
            r.max ? r.max : &TI::value(n),
            std::max(l.depth, r.depth) + 1, 1 + l.size + r.size};
    }

    static void _throw(const char *msg, bool s) {
        throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
    }

    Tree<T> tested;
    Ref<T> ref;
};

void test_insert() {
    Tester<int> t;

    for (int i = 0; i < 10; i++)
        t.insert(i, true);
    for (int i = -10; i < 20; i++)
        t.find(i);

    for (int i = 0; i < 10; i++)
        t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++)
        t.find(i);
}

void test_erase() {
    Tester<int> t;

    for (int i = 0; i < 10; i++)
        t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++)
        t.find(i);

    for (int i = 3; i < 22; i += 2)
        t.erase(i, true);
    for (int i = -10; i < 20; i++)
        t.find(i);

    for (int i = 0; i < 10; i++)
        t.insert((1 + i * 13) % 17 - 8, true);
    for (int i = -10; i < 20; i++)
        t.find(i);

    for (int i = -4; i < 10; i++)
        t.erase(i, true);
    for (int i = -10; i < 20; i++)
        t.find(i);
}

enum RandomTestFlags : unsigned {
    SEQ = 1,
    NO_ERASE = 2,
    CHECK_TREE = 4
};

void test_random(size_t size, unsigned flags = 0) {
    Tester<size_t> t;
    std::mt19937 my_rand(24707 + size);

    bool seq = flags & SEQ;
    bool erase = !(flags & NO_ERASE);
    bool check_tree = flags & CHECK_TREE;

    for (size_t i = 0; i < size; i++)
        t.insert(seq ? 2 * i : my_rand() % (3 * size), check_tree);

    t.check_tree();

    for (size_t i = 0; i < 3 * size + 1; i++)
        t.find(i);

    for (size_t i = 0; i < 30 * size; i++)
        switch (my_rand() % 5) {
        case 1:
            t.insert(my_rand() % (3 * size), check_tree);
            break;
        case 2:
            if (erase)
                t.erase(my_rand() % (3 * size), check_tree);
            break;
        default:
            t.find(my_rand() % (3 * size));
        }

    t.check_tree();
}

int main() {
    try {
        std::cout << "Insert test..." << std::endl;
        test_insert();

        std::cout << "Erase test..." << std::endl;
        test_erase();

        std::cout << "Tiny random test..." << std::endl;
        test_random(20, CHECK_TREE);

        std::cout << "Small random test..." << std::endl;
        test_random(200, CHECK_TREE);

        std::cout << "Big random test..." << std::endl;
        test_random(50'000);

        std::cout << "Big sequential test..." << std::endl;
        test_random(50'000, SEQ);

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &e) {
        std::cout << "Test failed: " << e.what() << std::endl;
    }
}

#endif
