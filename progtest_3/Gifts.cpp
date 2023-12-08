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

using Price = unsigned long long;
using Employee = size_t;
inline constexpr Employee NO_EMPLOYEE = -1;
using Gift = size_t;

#endif

//!        ===================================================== START OF CODE =====================================================
inline constexpr Gift NO_GIFT = -1;
using namespace std;

//| ----------------------------------------------------------------------------------------------------------

struct S_Gift {
    S_Gift(Price price, Gift id) : g_price(price), g_id(id) {}
    S_Gift() = default;

    Price g_price = 0;
    Gift g_id = NO_GIFT;
};

struct S_Employee {
    S_Employee() = default;

    Employee id = 0;
    Employee boss = NO_EMPLOYEE;
    vector<S_Employee *> subordinates;
    //?   id   subtre_price
    //?   |      |
    pair<Gift, size_t> gift_1 = {NO_GIFT, SIZE_MAX};
    pair<Gift, size_t> gift_2 = {NO_GIFT, SIZE_MAX};
};

//| ----------------------------------------------------------------------------------------------------------

void traverse_graph(Employee node, vector<S_Employee> &employees_dbs, const vector<S_Gift> &sorted_gifts);

void assign_gifts(vector<Employee> &q, const vector<S_Employee> &employees_dbs, Price &sum_of_gift_prices, vector<Gift> &gift_per_employee);

//| ----------------------------------------------------------------------------------------------------------

//?  sum of gift prices     vector of gifts(_ID_ of a gift) given to the employee on the indes _i_ in the vector
//?         |                  |
std::pair<Price, std::vector<Gift>> optimize_gifts(const std::vector<Employee> &boss, const std::vector<Price> &gift_price) {
    //?                                                                 |                                 |
    //?                                        vector of empolyees - vector[Employee_ID] = Boss_ID      vector of prices of gifts -> vector[Gift_ID] = price
    //? supreme boss has no boss

    Price sum_of_gift_prices = 0;
    vector<Gift> gift_per_employee(boss.size(), NO_GIFT);

    //~ Create _q_ of supreme bosses and set the data for each employee
    //    -> for each employee set his boss
    //    -> for each boss set the list of his subordinates
    vector<S_Employee> employees_dbs(boss.size());
    std::vector<Employee> q;
    for (Employee i = 0; i < boss.size(); ++i) {
        employees_dbs[i].id = i;
        if (boss[i] == NO_EMPLOYEE) {
            q.push_back(i);
            continue;
        }
        employees_dbs[i].boss = boss[i];
        employees_dbs[boss[i]].subordinates.emplace_back(&employees_dbs[i]);
    }

    //~ Create a list of sorted gifts
    vector<S_Gift> sorted_gifts;
    for (Gift i = 0; i < gift_price.size(); ++i) {
        sorted_gifts.emplace_back(gift_price[i], i);
    }
    // Sort gifts based on their price (ascendingly)
    sort(sorted_gifts.begin(), sorted_gifts.end(), [](const S_Gift &a, const S_Gift &b) {
        return a.g_price < b.g_price;
    });

    //~ Iterate in post-order over all the employees, strating from the supreme bosses in the _q_
    for (auto &cur_employee : q)
        traverse_graph(cur_employee, employees_dbs, sorted_gifts);

    //~ Use DFS to give the optimal gifts to the employees
    assign_gifts(q, employees_dbs, sum_of_gift_prices, gift_per_employee);

    return {sum_of_gift_prices, gift_per_employee};
}

// Calculate the best two gifts for the current employee
void calc_best_gifts(Employee node, vector<S_Employee> &employees_dbs, const vector<S_Gift> &sorted_gifts) {
    S_Employee &cur_employee = employees_dbs[node];
    // If the employee has no subordinates --> he is a leaf, he gets the two cheapest gifts
    if (cur_employee.subordinates.empty()) {
        cur_employee.gift_1 = {sorted_gifts[0].g_id, sorted_gifts[0].g_price};
        cur_employee.gift_2 = {sorted_gifts[1].g_id, sorted_gifts[1].g_price};
    }
    // The employee is a boss --> has at least one subordinate
    // --> he gets the two most optimal (gifts which will result in the cheapest overall price sum) gifts that have not yet been given to his subordinates
    else {
        // For each gift from the list
        for (auto &gift : sorted_gifts) {
            Gift cg_id = gift.g_id; // current gift id
            size_t cost_sum = 0;    // sum of the prices of of the gifts if we give the current gift to the boss
            cost_sum += gift.g_price;

            // Check all the subordinates of the current employee
            // and calculate the sum of the prices and the id of the gift would be given to the boss
            for (auto &sub : cur_employee.subordinates) {
                // Check if the subordinate has already been given the gift with the current id
                if (sub->gift_1.first != cg_id)
                    cost_sum += sub->gift_1.second;
                else
                    cost_sum += sub->gift_2.second;
            }

            // Compare the price sum with the current best two gifts of the boss
            if (cost_sum < cur_employee.gift_1.second) {
                // If the current gift option is cheaper than the first best gift
                // --> swap the two gifts and set the current gift as the first best gift
                swap(cur_employee.gift_1, cur_employee.gift_2);

                cur_employee.gift_1.first = cg_id;
                cur_employee.gift_1.second = cost_sum;
            } else if (cost_sum < cur_employee.gift_2.second) {
                cur_employee.gift_2.first = cg_id;
                cur_employee.gift_2.second = cost_sum;
            }
        }
    }
}

// Iteratively traverse the pseudo-tree in post-order manner
void traverse_graph(Employee root, vector<S_Employee> &employees_dbs, const vector<S_Gift> &sorted_gifts) {
    stack<Employee> s1, s2;
    s1.push(root);

    //~ Traverse the tree iteratively from the root to the leaves
    // Store the nodes in s2 in post-order manner
    while (!s1.empty()) {
        Employee node = s1.top();
        s1.pop();
        s2.push(node);

        // Add the subordinates of the current node to the stack _s1_ which will be processed later
        for (auto &subordinate : employees_dbs[node].subordinates)
            s1.push(subordinate->id);
    }

    //~ Calculate the best two gifts for each employee (starting from the leaves and going up to the root)
    while (!s2.empty()) {
        Employee node = s2.top();
        s2.pop();
        calc_best_gifts(node, employees_dbs, sorted_gifts);
    }
}

// Assign the gifts to the employees by traversing the pseudo-tree with DFS
void assign_gifts(vector<Employee> &q, const vector<S_Employee> &employees_dbs, Price &sum_of_gift_prices, vector<Gift> &gift_per_employee) {
    stack<Employee> s;
    // Push the supreme bosses in the stack
    for (auto &b : q) {
        s.push(b);
        sum_of_gift_prices += employees_dbs[b].gift_1.second;
    }

    while (!s.empty()) {
        Employee node = s.top();
        s.pop();

        const S_Employee &cur_employee = employees_dbs[node];
        Gift boss_gift_id = cur_employee.boss == NO_EMPLOYEE ? NO_GIFT : gift_per_employee[cur_employee.boss];

        // Check if the boss of the current employee has already been given the first best gift
        // If not --> give the first best gift to the current employee
        // If yes --> give the second best gift to the current employee
        if (cur_employee.gift_1.first != boss_gift_id)
            gift_per_employee[node] = boss_gift_id = cur_employee.gift_1.first;
        else
            gift_per_employee[node] = boss_gift_id = cur_employee.gift_2.first;

        // Add the subordinates of the current employee to the stack
        for (auto &sub : cur_employee.subordinates) {
            s.push(sub->id);
        }
    }
}

//| ----------------------------------------------------------------------------------------------------------

//!                   ==================================== END OF CODE ====================================

#ifndef __PROGTEST__

const std::tuple<Price, std::vector<Employee>, std::vector<Price>> EXAMPLES[] = {
    {17, {1, 2, 3, 4, NO_EMPLOYEE}, {25, 4, 18, 3}},
    {16, {4, 4, 4, 4, NO_EMPLOYEE}, {25, 4, 18, 3}},
    {17, {4, 4, 3, 4, NO_EMPLOYEE}, {25, 4, 18, 3}},
    {24, {4, 4, 3, 4, NO_EMPLOYEE, 3, 3}, {25, 4, 18, 3}},
};

#define CHECK(cond, ...)                     \
    do {                                     \
        if (cond)                            \
            break;                           \
        printf("Test failed: " __VA_ARGS__); \
        printf("\n");                        \
        return false;                        \
    } while (0)

bool test(Price p, const std::vector<Employee> &boss, const std::vector<Price> &gp) {
    auto &&[sol_p, sol_g] = optimize_gifts(boss, gp);
    CHECK(sol_g.size() == boss.size(),
          "Size of the solution: expected %zu but got %zu.", boss.size(), sol_g.size());

    Price real_p = 0;
    for (Gift g : sol_g)
        real_p += gp[g];
    CHECK(real_p == sol_p, "Sum of gift prices is %llu but reported price is %llu.", real_p, sol_p);

    if (0) {
        for (Employee e = 0; e < boss.size(); e++)
            printf(" (%zu)%zu", e, sol_g[e]);
        printf("\n");
    }

    for (Employee e = 0; e < boss.size(); e++)
        CHECK(boss[e] == NO_EMPLOYEE || sol_g[boss[e]] != sol_g[e],
              "Employee %zu and their boss %zu has same gift %zu.", e, boss[e], sol_g[e]);

    CHECK(p == sol_p, "Wrong price: expected %llu got %llu.", p, sol_p);

    return true;
}
#undef CHECK

int main() {
    int ok = 0, fail = 0;
    for (auto &&[p, b, gp] : EXAMPLES)
        (test(p, b, gp) ? ok : fail)++;

    if (!fail)
        printf("Passed all %d tests!\n", ok);
    else
        printf("Failed %d of %d tests.", fail, fail + ok);
}

#endif