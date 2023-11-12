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
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#endif
//!                      ================================ START OF CODE ================================
using namespace std;

struct TextEditorBackend {
private:
    struct TNode {
        char value;
        // A single node has height of 1 - itself
        size_t height = 1;
        int balance = 0;
        TNode *parent = nullptr;
        TNode *left = nullptr;
        TNode *right = nullptr;

        // Number of nodes in the subtree - including the node itself
        // => one node has 1 node in its subtree (itself)
        size_t nodes_in_tree = 1;
        // Num of newlines in the left subtree (including the current node)
        size_t newlines_in_L_tree = 0;
        // Num of newlines in the right subtree (including the current node)
        size_t newlines_in_R_tree = 0;

        TNode() = default;
        TNode(char data) : value(data) {}
    };

    // Tree struct variables
    TNode *root = nullptr;
    size_t m_size = 0;
    // Number of lines in the whole tree
    // Empty text has 1 line (empty line)
    size_t m_lines = 1;

    TNode *insert(TNode *node, size_t index, char value) {
        // If there is a nullptr in the place where the new node should be
        // -> create a new TNode dynamically
        if (node == nullptr) {
            TNode *new_node = new TNode(value);
            // Set the newlines_in_tree to 1 if the value is '\n'
            if (value == '\n')
                new_node->newlines_in_L_tree = new_node->newlines_in_R_tree = 1;

            return new_node;
        }

        // Set the size with which we will be searching to the number of nodes in the left sub tree
        // if the node doesn't have a left sub tree, set the size to 0
        size_t cur_idx = node->left ? node->left->nodes_in_tree : 0;

        if (index <= cur_idx) {
            node->left = insert(node->left, index, value);
            node->left->parent = node;
        } else {
            node->right = insert(node->right, index - cur_idx - 1, value);
            node->right->parent = node;
        }

        update(node);

        // Rebalance the tree
        return balance(node);
    }

    TNode *erase(TNode *node, size_t index) {
        size_t cur_idx = node->left ? node->left->nodes_in_tree : 0;

        if (index < cur_idx) {
            node->left = erase(node->left, index);
        } else if (index > cur_idx) {
            node->right = erase(node->right, index - cur_idx - 1);
        } else { // Node to be deleted is found
            if (node->value == '\n')
                m_lines--;

            // Case_1: Node with only one child or no child
            if (node->left == nullptr || node->right == nullptr) {
                TNode *temp = node->left ? node->left : node->right;
                if (temp)
                    temp->parent = node->parent; // Update the parent pointer of the new child
                delete node;
                return temp;
            } else { // Case_2: Node with two children
                char successor = find_successor(node->right);
                if (successor == '\n')
                    m_lines++;

                node->value = successor;
                // Erase the successor from the right subtree
                node->right = erase(node->right, 0);
            }
        }

        update(node);
        return balance(node); // Rebalance the tree
    }

    char &find(TNode *node, size_t index) const {
        size_t cur_idx = node->left ? node->left->nodes_in_tree : 0;

        if (index < cur_idx) {
            return find(node->left, index);
        } else if (index > cur_idx) {
            return find(node->right, index - cur_idx - 1);
        }

        return node->value;
    }

    TNode *edit(TNode *node, size_t index, char new_value) {
        size_t cur_idx = node->left ? node->left->nodes_in_tree : 0;

        if (index < cur_idx) {
            node->left = edit(node->left, index, new_value);
        } else if (index > cur_idx) {
            node->right = edit(node->right, index - cur_idx - 1, new_value);
        } else {
            if (node->value == '\n' && new_value != '\n')
                m_lines--;
            else if (node->value != '\n' && new_value == '\n')
                m_lines++;

            node->value = new_value;
        }

        update(node);
        return node;
    }

    // Update balance and height
    void update(TNode *node) {
        int l_height = 0;
        int r_height = 0;

        // Number of nodes in the left and right subtree
        size_t l_num_of_n = 0;
        size_t r_num_of_n = 0;

        // Number of newlines in the left and right subtree
        size_t l_tree_nl = 0;
        size_t r_tree_nl = 0;

        if (node->left) {
            l_height = node->left->height;
            l_num_of_n = node->left->nodes_in_tree;
            l_tree_nl = node->left->newlines_in_L_tree + node->left->newlines_in_R_tree - (node->left->value == '\n');
        }
        if (node->right) {
            r_height = node->right->height;
            r_num_of_n = node->right->nodes_in_tree;
            r_tree_nl = node->right->newlines_in_L_tree + node->right->newlines_in_R_tree - (node->right->value == '\n');
        }

        node->height = 1 + std::max(l_height, r_height);

        node->balance = r_height - l_height;

        node->nodes_in_tree = r_num_of_n + l_num_of_n + 1;

        node->newlines_in_L_tree = l_tree_nl + (node->value == '\n');

        node->newlines_in_R_tree = r_tree_nl + (node->value == '\n');
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
        } // Right heavy
        else if (node->balance > 1) {
            if (node->right->balance < 0) {
                // The tree is unbalanced in 2 different ways
                // -> we have to do the right-left-rotation
                node->right = right_rotation(node->right);
            }
            return left_rotation(node);
        }

        // The node has balance of -1, 0, or 1 => no need to balance it
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

    char find_successor(TNode *node) {
        while (node->left)
            node = node->left;

        return node->value;
    }

    size_t char_to_line(TNode *node, size_t index, size_t nl_passed) const {
        if (node == nullptr)
            return nl_passed;

        size_t cur_idx = node->left ? node->left->nodes_in_tree : 0;

        // => go to the left subtree
        if (index < cur_idx) {
            size_t new_nl_passed = nl_passed - (node->left ? node->left->newlines_in_R_tree : 0);
            return char_to_line(node->left, index, new_nl_passed);
        }
        // => go to the right subtree
        else if (index > cur_idx) {
            size_t new_nl_passed = nl_passed +
                                   (node->value == '\n') +
                                   (node->right ? node->right->newlines_in_L_tree - (node->right->value == '\n') : 0);
            return char_to_line(node->right, index - cur_idx - 1, new_nl_passed);
        }

        // we are in the node with searched index
        return nl_passed;
    }

    // todo - Optimize this bullshit into one function, beacause i know that this is too fckin complicated
    // todo   and these 3 functions can be integrated into one simple function
    //? Returns the index of the first character of line r == returns the starting index of the line r
    //? '\n' is considered to be part of the line
    // Traverse the tree into the right subtree until the nl_count is not equal to r
    // Then return the index of the smallest node in the current subtree
    size_t line_start(TNode *node, size_t r, size_t cur_idx, size_t nl_passed) const {
        if (node == nullptr)
            return cur_idx;

        // -> go to the left subtree
        if (nl_passed > r) {
            size_t new_idx = cur_idx - 1;
            size_t new_nl_passed = nl_passed;
            if (node->left) {
                new_nl_passed = new_nl_passed - node->left->newlines_in_R_tree;
                if (node->left->right)
                    new_idx = new_idx - node->left->right->nodes_in_tree;
            }
            return line_start(node->left, r, new_idx, new_nl_passed);
        }
        // -> go to the right subtree
        else if (nl_passed < r) {
            size_t new_idx = cur_idx + 1;
            size_t new_nl_passed = nl_passed + (node->value == '\n');
            if (node->right) {
                new_nl_passed = new_nl_passed + (node->right->newlines_in_L_tree - (node->right->value == '\n'));
                if (node->right->left)
                    new_idx = new_idx + node->right->left->nodes_in_tree;
            }
            return line_start(node->right, r, new_idx, new_nl_passed);
        }

        // The current number of passed new lines is the number of the line we are looking for
        // -> return the index of the first node in that current row
        nl_passed = node->newlines_in_L_tree - (node->value == '\n');
        return first_on_row(node, cur_idx, nl_passed);
    }

    size_t first_on_row(TNode *node, size_t cur_idx, size_t nl_passed) const {
        // There aren't any nodes with smaller index on the given row
        if (node->left == nullptr)
            return cur_idx;

        // if the left node is on the same line, go into it
        if (nl_passed == (node->left->newlines_in_L_tree - (node->left->value == '\n'))) {
            cur_idx--;
            node = node->left;
            cur_idx = cur_idx - (node->right ? node->right->nodes_in_tree : 0);
            return first_on_row(node, cur_idx, nl_passed);
        }
        // if atleast one node from the right subtree of the left node is on the same row, go into the right subtree of the left node
        else if (nl_passed == (node->left->newlines_in_L_tree + node->left->newlines_in_R_tree - (node->left->value == '\n'))) {
            if (node->left->right == nullptr)
                return cur_idx;

            size_t ref_nl = nl_passed;
            size_t origin_idx = cur_idx;

            // go to the left node first
            node = node->left;
            cur_idx = cur_idx - 1 - (node->right ? node->right->nodes_in_tree : 0);
            nl_passed = node->newlines_in_L_tree;

            // then from there, go to the right node
            node = node->right;
            cur_idx = cur_idx + 1 + (node->left ? node->left->nodes_in_tree : 0);

            return search_lr_subtree(node, cur_idx, nl_passed, ref_nl, origin_idx);
        }

        // The current node is the first node in the row
        return cur_idx;
    }

    // We are already in the right subtree but the node we are looking for is yet in the right subtree
    size_t search_lr_subtree(TNode *node, size_t cur_idx, size_t nl_passed, size_t ref_nl, size_t origin_idx) const {
        // If the current node is a leaf return it's index,
        // or the index of the original node if the leaf node is newline
        if (node->height == 1)
            return node->value == '\n' ? origin_idx : cur_idx;

        // check left node
        if (node->left && (nl_passed + node->left->newlines_in_L_tree - (node->left->value == '\n') == ref_nl)) {
            cur_idx--;
            node = node->left;
            cur_idx = cur_idx - (node->right ? node->right->nodes_in_tree : 0);
            return first_on_row(node, cur_idx, node->newlines_in_L_tree - (node->value == '\n'));
        }
        // check the left->right subtree
        else if (node->left && node->right && (nl_passed + node->left->newlines_in_L_tree + node->left->newlines_in_R_tree - (node->left->value == '\n')) == ref_nl) {
            size_t ref_nl = node->newlines_in_L_tree - (node->value == '\n');
            size_t origin_idx = cur_idx;

            // go to the left node first
            node = node->left;
            cur_idx = cur_idx - 1 - (node->right ? node->right->nodes_in_tree : 0);
            nl_passed = node->newlines_in_L_tree;

            // then from there, go to the right node if it exists
            if (node->right == nullptr)
                return node->value == '\n' ? origin_idx : cur_idx;

            node = node->right;
            cur_idx = cur_idx + 1 + (node->left ? node->left->nodes_in_tree : 0);

            return search_lr_subtree(node, cur_idx, nl_passed, ref_nl, origin_idx);
        }
        // go into the right node
        else if ((node->newlines_in_R_tree != 0) && ((nl_passed + node->newlines_in_L_tree + node->newlines_in_R_tree - (node->value == '\n')) == ref_nl) && node->right != nullptr) {
            nl_passed = nl_passed + node->newlines_in_L_tree;
            cur_idx++;
            node = node->right;
            cur_idx = cur_idx + (node->left ? node->left->nodes_in_tree : 0);
            return search_lr_subtree(node, cur_idx, nl_passed, ref_nl, origin_idx);
        }

        return node->value == '\n' ? origin_idx : cur_idx;
    }

    void delete_tree(TNode *node) {
        if (node == nullptr)
            return;

        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }

public:
    TextEditorBackend() = default;

    TextEditorBackend(const std::string &text) : TextEditorBackend() {
        for (size_t i = 0; i < text.size(); i++)
            insert(i, text[i]);
    }

    ~TextEditorBackend() {
        // Free the allocated memory for nodes
        delete_tree(root);
    }

    size_t size() const {
        return m_size;
    }

    size_t lines() const {
        return m_lines;
    }

    char at(size_t i) const {
        if (i < 0 || i >= size())
            throw std::out_of_range("Try next time man");

        return find(root, i);
    }

    void edit(size_t i, char c) {
        if (i < 0 || i >= size())
            throw std::out_of_range("Try next time man");

        root = edit(root, i, c);
    }

    void insert(size_t i, char c) {
        if (i < 0 || i > size())
            throw std::out_of_range("Try next time man");

        root = insert(root, i, c);
        m_size++;
        if (c == '\n')
            m_lines++;
    }

    void erase(size_t i) {
        if (i < 0 || i >= size())
            throw std::out_of_range("Try next time man");

        root = erase(root, i);
        m_size--;
    }

    //? Returns the index of the first character of line r == returns the starting index of the line r
    //? '\n' is considered to be part of the line
    size_t line_start(size_t r) const {
        if (r < 0 || r >= lines())
            throw std::out_of_range("Try next time man");

        // First index of the line 0 is always 0 ■
        if (r == 0)
            return 0;

        size_t nl_passed = root->newlines_in_L_tree - (root->value == '\n');
        size_t cur_idx = (root->left ? root->left->nodes_in_tree : 0);
        return line_start(root, r, cur_idx, nl_passed);
    }

    //? Returns the length of the line r (including the newline character)
    size_t line_length(size_t r) const {
        if (r < 0 || r >= lines())
            throw std::out_of_range("Index out of range");

        // If the line is the last line, return the length of the whole text - the starting index of the last line
        if (r == lines() - 1)
            return size() - line_start(r);

        // If the line is not the last line, return the difference between the start of the next line and the start of the current line
        return line_start(r + 1) - line_start(r);
    }

    size_t char_to_line(size_t i) const {
        if (i < 0 || i >= size())
            throw std::out_of_range("Index out of range");

        size_t cur_nl_passed = (root ? root->newlines_in_L_tree - (root->value == '\n') : 0);
        return char_to_line(root, i, cur_nl_passed);
    }
};

//!                      ================================ END OF CODE ================================
#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template <typename T>
auto quote(const T &t) { return t; }

std::string quote(const std::string &s) {
    std::string ret = "\"";
    for (char c : s)
        if (c != '\n')
            ret += c;
        else
            ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str)                                   \
    do {                                                             \
        auto _a = (a);                                               \
        decltype(a) _b = (b);                                        \
        if (_a != _b) {                                              \
            std::cout << "Line " << __LINE__ << ": Assertion "       \
                      << a_str << " == " << b_str << " failed!"      \
                      << " (lhs: " << quote(_a) << ")" << std::endl; \
            fail++;                                                  \
        } else                                                       \
            ok++;                                                    \
    } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...)                                                    \
    do {                                                                        \
        std::array _arr = {__VA_ARGS__};                                        \
        for (size_t _i = 0; _i < _arr.size(); _i++)                             \
            CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
    } while (0)

#define CHECK_EX(expr, ex)                                                                                                             \
    do {                                                                                                                               \
        try {                                                                                                                          \
            (expr);                                                                                                                    \
            fail++;                                                                                                                    \
            std::cout << "Line " << __LINE__ << ": Expected " STR(expr) " to throw " #ex " but no exception was raised." << std::endl; \
        } catch (const ex &) {                                                                                                         \
            ok++;                                                                                                                      \
        } catch (...) {                                                                                                                \
            fail++;                                                                                                                    \
            std::cout << "Line " << __LINE__ << ": Expected " STR(expr) " to throw " #ex " but got different exception." << std::endl; \
        }                                                                                                                              \
    } while (0)

////////////////// End of dark magic ////////////////////////

std::string text(const TextEditorBackend &t) {
    std::string ret;
    for (size_t i = 0; i < t.size(); i++)
        ret.push_back(t.at(i));
    return ret;
}

void test1(int &ok, int &fail) {
    TextEditorBackend s("123\n456\n789");
    CHECK(s.size(), 11);
    CHECK(text(s), "123\n456\n789");
    CHECK(s.lines(), 3);
    CHECK_ALL(s.char_to_line, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2);
    CHECK_ALL(s.line_start, 0, 4, 8);
    CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int &ok, int &fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK(t.size(), 12);
    CHECK(text(t), "123\n456\n789\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.char_to_line, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2);
    CHECK_ALL(t.line_start, 0, 4, 8, 12);
    CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int &ok, int &fail) {
    TextEditorBackend t("asdfasdfasdf");

    CHECK(t.size(), 12);
    CHECK(text(t), "asdfasdfasdf");
    CHECK(t.lines(), 1);
    CHECK_ALL(t.char_to_line, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    CHECK(t.line_start(0), 0);
    CHECK(t.line_length(0), 12);

    t.insert(0, '\n');
    CHECK(t.size(), 13);
    CHECK(text(t), "\nasdfasdfasdf");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);

    t.insert(4, '\n');
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasdfasdf");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.insert(t.size(), '\n');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdf\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.line_start, 0, 1, 5, 15);

    t.edit(t.size() - 1, 'H');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(8);
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(4);
    CHECK(t.size(), 13);
    CHECK(text(t), "\nasdfasfasdfH");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int &ok, int &fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK_EX(t.at(12), std::out_of_range);

    CHECK_EX(t.insert(13, 'a'), std::out_of_range);
    CHECK_EX(t.edit(12, 'x'), std::out_of_range);
    CHECK_EX(t.erase(12), std::out_of_range);

    CHECK_EX(t.line_start(4), std::out_of_range);
    CHECK_EX(t.line_start(40), std::out_of_range);
    CHECK_EX(t.line_length(4), std::out_of_range);
    CHECK_EX(t.line_length(6), std::out_of_range);
    CHECK_EX(t.char_to_line(12), std::out_of_range);
    CHECK_EX(t.char_to_line(25), std::out_of_range);
}

int main() {
    int ok = 0, fail = 0;
    if (!fail)
        test1(ok, fail);
    if (!fail)
        test2(ok, fail);
    if (!fail)
        test3(ok, fail);
    if (!fail)
        test_ex(ok, fail);

    if (!fail)
        std::cout
            << "Passed all " << ok << " tests" << std::endl;
    else
        std::cout
            << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif