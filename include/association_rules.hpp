#ifndef ASSOCIATION_RULES_H
#define ASSOCIATION_RULES_H

#include <vector>
#include "count_distribution.hpp"

struct AssociationRule {
    double confidence;
    double support;
    std::vector<int> left_part;
    std::vector<int> right_part;
};


std::vector<AssociationRule> generate_association_rules(std::vector<std::vector<ItemSet>> &L, int threads_amount, double min_confidence, int min_size);


#endif  // ASSOCIATION_RULES_H