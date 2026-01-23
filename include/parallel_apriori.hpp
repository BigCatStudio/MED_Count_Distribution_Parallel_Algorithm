#ifndef PARALLEL_APRIORI_H
#define PARALLEL_APRIORI_H

#include "count_distribution.hpp"
#include "association_rules.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>


std::vector<AssociationRule> get_association_rules(std::string filename, double min_support, double min_confidence, int threads_amount, int min_size, int hash_pivot, int bucket_size);


#endif  // PARALLEL_APRIORI_H