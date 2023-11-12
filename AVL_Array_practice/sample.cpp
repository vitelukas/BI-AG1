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

// We use std::vector as a reference to check our implementation.
// It is not available in progtest :)
#include <vector>

template <typename T>
struct Ref {
    bool empty() const { return _data.empty(); }
    size_t size() const { return _data.size(); }

    const T &operator[](size_t index) const { return _data.at(index); }
    T &operator[](size_t index) { return _data.at(index); }

    void insert(size_t index, T value) {
        if (index > _data.size())
            throw std::out_of_range("oops");
        _data.insert(_data.begin() + index, std::move(value));
    }

    T erase(size_t index) {
        T ret = std::move(_data.at(index));
        _data.erase(_data.begin() + index);
        return ret;
    }

    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }

private:
    std::vector<T> _data;
};

#endif

namespace config {
inline constexpr bool PARENT_POINTERS = true;
inline constexpr bool CHECK_DEPTH = true;
} // namespace config

//!                           ========================= START OF CODE =============================
using namespace std;

template <typename T>
struct Array {
private:
    struct TNode {
        T value;
        size_t height = 1;
        int balance = 0;
        TNode *parent = nullptr;
        TNode *left = nullptr;
        TNode *right = nullptr;

        size_t nodes_in_tree = 1;

        TNode() = default;
        TNode(T data) : value(data), height(1), balance(0), parent(nullptr), left(nullptr), right(nullptr) {}

        friend bool operator<(const TNode &a, const TNode &b) {
            return a.value < b.value;
        }
    };

    // Tree struct variables
    TNode *root;
    size_t m_size;

    T &find(TNode *node, size_t index) const {
        size_t left_size = node->left ? node->left->nodes_in_tree : 0;

        if (index < left_size) {
            return find(node->left, index);
        } else if (index > left_size) {
            return find(node->right, index - left_size - 1);
        } else {
            return node->value;
        }

        return node->value;
    }

    TNode *insert(TNode *node, T value, size_t index) {
        // If there is a nullptr in the place where the new node should be
        // -> create a new TNode dynamically
        if (node == nullptr)
            return new TNode(value);

        // Set the size with which we will be searching to the number of nodes in the left sub tree
        // if the node doesn't have a left sub tree, set the size to 0
        size_t left_size = node->left ? node->left->nodes_in_tree : 0;

        if (index <= left_size) {
            node->left = insert(node->left, value, index);
            node->left->parent = node;
        } else {
            node->right = insert(node->right, value, index - left_size - 1);
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

        size_t l_num_of_n = 0;
        size_t r_num_of_n = 0;

        if (node->left) {
            l_height = node->left->height;
            l_num_of_n = node->left->nodes_in_tree;
        }
        if (node->right) {
            r_height = node->right->height;
            r_num_of_n = node->right->nodes_in_tree;
        }

        node->height = 1 + max(l_height, r_height);

        node->balance = r_height - l_height;

        node->nodes_in_tree = r_num_of_n + l_num_of_n + 1;
    }

    // Balances the tree if needed
    TNode *balance(TNode *node) {

        // Left heavy
        if (node->balance < -1) {
            if (node->left->balance > 0) {
                // The tree is unbalanced in 2 different ways
                // -> we have to do the left-right-rotation
                node->left = left_rotation(node->left);
            }
            return right_rotation(node);
        } else if (node->balance > 1) { // Right heavy
            if (node->right->balance < 0) {
                // The tree is unbalanced in 2 different ways
                // -> we have to do the right-left-rotation
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
        return Y;
    }

    TNode *erase(TNode *node, size_t index) {
        size_t left_size = node->left ? node->left->nodes_in_tree : 0;

        if (index < left_size) {
            node->left = erase(node->left, index);
        } else if (index > left_size) {
            node->right = erase(node->right, index - left_size - 1);
        } else { // Node to be deleted is found
            // Case_1: Node with only one child or no child
            if (node->left == nullptr || node->right == nullptr) {
                TNode *temp = node->left ? node->left : node->right;
                if (temp)
                    temp->parent = node->parent; // Update the parent pointer of the new child
                delete node;
                return temp;
            } else { // Case_2: Node with two children
                T successor = find_successor(node->right);
                node->value = successor;
                // Erase the successor from the right subtree
                node->right = erase(node->right, 0);
            }
        }

        if (node != nullptr) {
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

public:
    Array() : root(), m_size(0) {}

    ~Array() {
        // Free the allocated memory of the nodes
        delete_tree(root);
    }

    bool empty() const {
        return m_size == 0;
    }

    size_t size() const {
        return m_size;
    }

    const T &operator[](size_t index) const {
        if (index < 0 || index >= size())
            throw std::out_of_range("Index out of range");

        return find(root, index);
    }

    T &operator[](size_t index) {
        if (index < 0 || index >= size())
            throw std::out_of_range("Index out of range");

        return find(root, index);
    }

    void insert(size_t index, T value) {
        if (index < 0 || index > size())
            throw std::out_of_range("Index out of range");

        // Find the node at the given index
        root = insert(root, value, index);
        m_size++;
    }

    T erase(size_t index) {
        if (index < 0 || index >= size())
            throw std::out_of_range("Index out of range");

        T value = find(root, index);
        root = erase(root, index);
        m_size--;
        return value;
    }

    // Needed to test the structure of the tree.
    // Replace Node with the real type of your nodes
    // and implementations with the ones matching
    // your attributes.
    struct TesterInterface {
        // using Node = TNode
        static const TNode *root(const Array *t) { return t->root; }
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

    size_t size() const {
        bool te = tested.empty();
        size_t r = ref.size();
        size_t t = tested.size();
        if (te != !t)
            throw TestFailed(fmt("Size: size %zu but empty is %s.",
                                 t, te ? "true" : "false"));
        if (r != t)
            throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
        return r;
    }

    const T &operator[](size_t index) const {
        const T &r = ref[index];
        const T &t = tested[index];
        if (r != t)
            throw TestFailed("Op [] const mismatch.");
        return t;
    }

    void assign(size_t index, T x) {
        ref[index] = x;
        tested[index] = std::move(x);
        operator[](index);
    }

    void insert(size_t i, T x, bool check_tree_ = false) {
        ref.insert(i, x);
        tested.insert(i, std::move(x));
        size();
        if (check_tree_)
            check_tree();
    }

    T erase(size_t i, bool check_tree_ = false) {
        T r = ref.erase(i);
        T t = tested.erase(i);
        if (r != t)
            TestFailed(fmt("Erase mismatch at %zu.", i));
        size();
        if (check_tree_)
            check_tree();
        return t;
    }

    void check_tree() const {
        using TI = typename Array<T>::TesterInterface;
        auto ref_it = ref.begin();
        bool check_value_failed = false;
        auto check_value = [&](const T &v) {
            if (check_value_failed)
                return;
            check_value_failed = (ref_it == ref.end() || *ref_it != v);
            if (!check_value_failed)
                ++ref_it;
        };

        size();

        check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);

        if (check_value_failed)
            throw TestFailed(
                "Check tree: element mismatch");
    }

    template <typename Node, typename F>
    int check_node(const Node *n, const Node *p, F &check_value) const {
        if (!n)
            return -1;

        using TI = typename Array<T>::TesterInterface;
        if constexpr (config::PARENT_POINTERS) {
            if (TI::parent(n) != p)
                throw TestFailed("Parent mismatch.");
        }

        auto l_depth = check_node(TI::left(n), n, check_value);
        check_value(TI::value(n));
        auto r_depth = check_node(TI::right(n), n, check_value);

        if (config::CHECK_DEPTH && abs(l_depth - r_depth) > 1)
            throw TestFailed(fmt(
                "Tree is not avl balanced: left depth %i and right depth %i.",
                l_depth, r_depth));

        return std::max(l_depth, r_depth) + 1;
    }

    static void _throw(const char *msg, bool s) {
        throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
    }

    Array<T> tested;
    Ref<T> ref;
};

void test_insert() {
    Tester<int> t;

    for (int i = 0; i < 10; i++)
        t.insert(i, i, true);
    for (int i = 0; i < 10; i++)
        t.insert(i, -i, true);
    for (size_t i = 0; i < t.size(); i++)
        t[i];

    for (int i = 0; i < 5; i++)
        t.insert(15, (1 + i * 7) % 17, true);
    for (int i = 0; i < 10; i++)
        t.assign(2 * i, 3 * t[2 * i]);
    for (size_t i = 0; i < t.size(); i++)
        t[i];
}

void test_erase() {
    Tester<int> t;

    for (int i = 0; i < 10; i++)
        t.insert(i, i, true);
    for (int i = 0; i < 10; i++)
        t.insert(i, -i, true);

    for (size_t i = 3; i < t.size(); i += 2)
        t.erase(i, true);
    for (size_t i = 0; i < t.size(); i++)
        t[i];

    for (int i = 0; i < 5; i++)
        t.insert(3, (1 + i * 7) % 17, true);
    for (size_t i = 1; i < t.size(); i += 3)
        t.erase(i, true);

    for (int i = 0; i < 20; i++)
        t.insert(3, 100 + i, true);

    for (int i = 0; i < 5; i++)
        t.erase(t.size() - 1, true);
    for (int i = 0; i < 5; i++)
        t.erase(0, true);

    for (int i = 0; i < 4; i++)
        t.insert(i, i, true);
    for (size_t i = 0; i < t.size(); i++)
        t[i];
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

    for (size_t i = 0; i < size; i++) {
        size_t pos = seq ? 0 : my_rand() % (i + 1);
        t.insert(pos, my_rand() % (3 * size), check_tree);
    }

    t.check_tree();

    for (size_t i = 0; i < t.size(); i++)
        t[i];

    for (size_t i = 0; i < 30 * size; i++)
        switch (my_rand() % 7) {
        case 1: {
            if (!erase && i % 3 == 0)
                break;
            size_t pos = seq ? 0 : my_rand() % (t.size() + 1);
            t.insert(pos, my_rand() % 1'000'000, check_tree);
            break;
        }
        case 2:
            if (erase)
                t.erase(my_rand() % t.size(), check_tree);
            break;
        case 3:
            t.assign(my_rand() % t.size(), 155 + i);
            break;
        default:
            t[my_rand() % t.size()];
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

        std::cout << "Bigger random test..." << std::endl;
        test_random(5'000);

        std::cout << "Bigger sequential test..." << std::endl;
        test_random(5'000, SEQ);

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &e) {
        std::cout << "Test failed: " << e.what() << std::endl;
    }
}

#endif
