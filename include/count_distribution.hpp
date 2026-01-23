#ifndef COUNT_DISTRIBUTION_H
#define COUNT_DISTRIBUTION_H

#include "hash_tree.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <exception>
#include <future>
#include <unordered_map>
#include <cmath>
#include <utility>
#include <unordered_set>

// podstawowa struktura przechowująca dane o itemsecie
struct ItemSet {
    std::vector<int> itemset;
    int support;
};

std::vector<std::vector<ItemSet>> count_distribution(std::vector<std::vector<int>> &transactions_mapped, int unique_items_amount, double min_support, double min_confidence, int threads_amount, int hash_pivot, int bucket_size);

std::string generate_key(std::vector<int> &itemset);

#endif  // COUNT_DISTRIBUTION_H