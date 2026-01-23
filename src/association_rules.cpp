#include "association_rules.hpp"




std::vector<AssociationRule> calculate_rules(const std::vector<ItemSet> &L_k, const std::unordered_map<std::string, int> &supports_map, int start, int end, double min_confidence) {
    std::vector<AssociationRule> rules;

    for (int index = start;index <= end;++index) {
        auto current_itemset = L_k.at(index).itemset;
        const int current_itemset_size = current_itemset.size();

        auto row = supports_map.find(generate_key(current_itemset));
        const int support = row->second;    // support obecnie badanego itemsetu

        const int allMask = (1 << current_itemset_size) - 1;    // definicja ilości masek które zostaną sprawdzone -> O(2^current_itemset_size)

        std::vector<int> left;
        std::vector<int> right;
        left.reserve(current_itemset_size);
        right.reserve(current_itemset_size);

        // wersja bez pruning - przejście po wszystkich mozliwych kominacjach - ewentualnie TODO do optymalizacji
        for (int mask = 1;mask < allMask;++mask) {    // pomijanie mask = 0 i mask = allMask żeby left i right nie były puste
            left.clear();
            right.clear();
            for (int bit = 0;bit < current_itemset_size;++bit) {
                if (mask & (1 << bit)) {
                    left.push_back(current_itemset.at(bit));    // dla bitów = 1 (elementów z itemset)
                } else {
                    right.push_back(current_itemset.at(bit));   // dla bitów = 0 (elementów z itemset)
                }
            }

            auto row_left = supports_map.find(generate_key(left));
            if (row_left == supports_map.end()) {
                continue;
            }

            const int support_left = row_left->second;

            double confidence = static_cast<double>(support) / static_cast<double>(support_left); // liczenie confidence dla danej zasady
            if (confidence < min_confidence) {
                continue;
            }

            AssociationRule r{confidence, static_cast<double>(support), left, right};
            rules.push_back(std::move(r));
        }
    }

    return rules;
}

 
std::vector<AssociationRule> generate_association_rules(std::vector<std::vector<ItemSet>> &L, int threads_amount, double min_confidence, int min_size) {
    // wygenerowanie mapy id: support do wyszukiwania O(1)
    std::unordered_map<std::string, int> supports_map;
    std::vector<AssociationRule> association_rules;

    for (auto &L_k : L) {
        for (auto &itemset : L_k) {
            supports_map[generate_key(itemset.itemset)] = itemset.support;
        }
    }
    L.erase(L.begin());     // usuwanie frequent itemsetów jednoelementowych - one nie tworzą zasad a mapowanie supportów już jest policzone


    for (std::vector<ItemSet> &L_k : L) {
        if (L_k.size() > min_size) {    // równolegle bo może być uzysk
            std::vector<std::future<std::vector<AssociationRule>>> future_rules;
            future_rules.reserve(threads_amount); // każdy wektor zostanie odebrany z innego wątku

            int step = (L_k.size() + threads_amount - 1) / threads_amount;
            int start, end = 0;
            for (size_t i = 0; i < L_k.size();i += step) {
                start = i;
                if (i + step >= L_k.size()) {
                    end = L_k.size() - 1;
                } else {
                    end = i + step - 1;
                }
                future_rules.emplace_back(
                    std::async(std::launch::async, calculate_rules, std::cref(L_k), std::cref(supports_map), start, end, min_confidence)
                );
            }

            for (auto &future_rule : future_rules) {    // zebranie zasad ze wszystkich wątków
                auto rules = future_rule.get();
                association_rules.insert(association_rules.end(), rules.begin(), rules.end());
            }


        } else {    // synchronicznie
            auto rules = calculate_rules(L_k, supports_map, 0, L_k.size() - 1, min_confidence);
            association_rules.insert(association_rules.end(), rules.begin(), rules.end());
        }
    }
    
    return association_rules;
}