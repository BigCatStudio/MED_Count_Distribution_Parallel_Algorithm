#include "count_distribution.hpp"


// zlicza supporty lokalne w pierwszej iteracji 
std::vector<int> get_first_iteration_counters(const std::vector<std::vector<int>> &transactions, int start, int end, int size) {
    std::vector<int> counters(size, 0); // od razu alokacja liczników dla wszystkich możliwych itemów
    for (size_t i = start; i <= end;i++) {
        for (const int &item : transactions.at(i)) {
            counters.at(item)++;    // zliczanie ilości wystąpień pojedynczych itemów 
        }
    }
    return counters;
}


// generuje frequent itemy dla k = 1 -> L_k=1
std::vector<ItemSet> first_iteration_data(std::vector<std::vector<int>> &transactions_mapped, int unique_items_amount, double min_support, int threads_amount) {
    std::vector<std::future<std::vector<int>>> future_k1_counters;
    future_k1_counters.reserve(threads_amount); // każdy wektor zostanie odebrany z innego wątku

    int step = (transactions_mapped.size() + threads_amount - 1) / threads_amount;
    int start, end = 0;
    for (size_t i = 0; i < transactions_mapped.size();i += step) {
        start = i;
        if (i + step >= transactions_mapped.size()) {
            end = transactions_mapped.size() - 1;
        } else {
            end = i + step - 1;
        }
        future_k1_counters.emplace_back(
            std::async(std::launch::async, get_first_iteration_counters, std::cref(transactions_mapped), start, end, unique_items_amount)
        );
    }

    std::vector<int> global_counter(unique_items_amount, 0);    // C_k=1
    for (auto &future_counter : future_k1_counters) {
        auto local_counter = future_counter.get();
        for (size_t i = 0;i < unique_items_amount;i++) {
            global_counter[i] += local_counter[i];
        }
    }

    // Tworzenie L_k=1
    std::vector<ItemSet> L_1;   // frequent itemy dla k = 1
    int min_support_amount = static_cast<int>(std::ceil(min_support * transactions_mapped.size())); // ilość wystąpień spełniająca minsup
    for (int i = 0;i < global_counter.size();i++) {
        if (global_counter[i] >= min_support_amount) {
            L_1.emplace_back(ItemSet{{i}, global_counter[i]}); 
        }
    }

    return L_1;
}


std::string generate_key(std::vector<int> &itemset) {
    std::string key;
    for (const auto &item : itemset) {
        key.append(std::to_string(item));
        key.append(",");
    }
    return key;
}


// weryfikuje czy wszystkie podzbiory są zawarte w kluczu
bool check_sub_itemsets(std::unordered_set<std::string> &keys, std::vector<int> &itemset, int k) {
    for (int pivot = 0;pivot < k;pivot++) {
        std::vector<int> sub_itemset;
        sub_itemset.reserve(itemset.size() - 1);

        sub_itemset.insert(sub_itemset.end(), itemset.begin(), itemset.begin() + pivot);
        sub_itemset.insert(sub_itemset.end(), itemset.begin() + pivot + 1, itemset.end());

        if (keys.find(generate_key(sub_itemset)) == keys.end()) {
            return false;
        }
    }
    return true;
}


// tworzy kandydatów C_k
std::vector<std::vector<int>> generate_candidates(std::vector<ItemSet> L_k_1, int k) {
    std::vector<std::vector<int>> candidates;
    bool check_k_elements = true;

    std::unordered_set<std::string> keys;
    for (ItemSet &itemset : L_k_1) {
        keys.insert(generate_key(itemset.itemset));
    }

    // wygenerowac wszystkich możliwych kandydatów
    for (int i = 0;i < L_k_1.size();i++) {
        std::vector<int> itemset_1 = L_k_1.at(i).itemset;
        for (int j = i + 1;j < L_k_1.size();j++) {
            check_k_elements = true;
            std::vector<int> itemset_2 = L_k_1.at(j).itemset;
            for (int k_temp = 0;k_temp < k - 2;k_temp++) {
                if (itemset_1.at(k_temp) != itemset_2.at(k_temp)) {
                    check_k_elements = false;  // itemsety mają różne elementy z zbioru k - 1, to wynika z faktu że itemy są posortowane
                    break;
                }
            }

            if (!check_k_elements) {
                continue;   // itemsety nie mają podobnego prefixu więc nie będą dalej analizowane
            }

            itemset_2.push_back(itemset_1.at(k - 2));   // nowy kandydat
            std::sort(itemset_2.begin(), itemset_2.end());  // zachowuje kolejność itemów w itemsecie

            if (check_sub_itemsets(keys, itemset_2, k)) {   // kandydat jest brany pod uwagę tylko gdy wszystkie jego podzbiory k - 1 są zawarte w frequent items dla k - 1
                candidates.push_back(itemset_2);
            }
        }
    }

    return candidates;
}


std::vector<int> get_next_iterations_counters(const std::vector<std::vector<int>> &transactions, const std::vector<std::vector<int>> &candidates_C_k, const HashTree &hash_tree, int start, int end) {
    std::vector<int> counters(candidates_C_k.size(), 0); // od razu alokacja liczników dla wszystkich kandydatów
    for (size_t i = start; i <= end;i++) {
        std::vector<int> candidates_id = hash_tree.search_candidates(transactions.at(i));
        for (int id : candidates_id) {
            counters.at(id) += 1;
        }
    }
    return counters;
}


std::vector<ItemSet> next_iteration_data(const std::vector<std::vector<int>> &transactions_mapped, const std::vector<std::vector<int>> &candidates_C_k, const HashTree &hash_tree, int k, double min_support, int threads_amount) {
    std::vector<std::future<std::vector<int>>> future_counters;
    future_counters.reserve(threads_amount); // każdy wektor zostanie odebrany z innego wątku

    int step = (transactions_mapped.size() + threads_amount - 1) / threads_amount;
    int start, end = 0;
    for (size_t i = 0; i < transactions_mapped.size();i += step) {
        start = i;
        if (i + step >= transactions_mapped.size()) {
            end = transactions_mapped.size() - 1;
        } else {
            end = i + step - 1;
        }
        future_counters.emplace_back(
            std::async(std::launch::async, get_next_iterations_counters, std::cref(transactions_mapped), candidates_C_k, hash_tree, start, end)
        );
    }

    std::vector<int> global_counter(candidates_C_k.size(), 0);
    for (auto &future_counter : future_counters) {
        auto local_counter = future_counter.get();
        for (size_t i = 0;i < candidates_C_k.size();i++) {
            global_counter.at(i) += local_counter.at(i);
        }
    }

    // Tworzenie L_k
    std::vector<ItemSet> L_k;   // frequent itemy dla k = 1
    int min_support_amount = static_cast<int>(std::ceil(min_support * transactions_mapped.size())); // ilość wystąpień spełniająca minsup
    for (int i = 0;i < global_counter.size();i++) {
        if (global_counter[i] >= min_support_amount) {
            L_k.emplace_back(ItemSet{candidates_C_k.at(i), global_counter.at(i)});
        }
    }

    return L_k;
}


std::vector<std::vector<ItemSet>> count_distribution(std::vector<std::vector<int>> &transactions_mapped, int unique_items_amount, double min_support, double min_confidence, int threads_amount, int hash_pivot, int bucket_size) {
    std::vector<std::vector<ItemSet>> L;   // zawiera wszystkie itemsety dla każdego k
    int k = 1;  // oznacza obecną iterację

    // pierwsza iteracja k = 1  - bez hash tree itp, po prostu zliczanie
    std::vector<ItemSet> L_1 = first_iteration_data(transactions_mapped, unique_items_amount, min_support, threads_amount);
    L.push_back(L_1);
    k += 1;


    // kolejne iteracje k > 1
    while(true) {
        std::vector<std::vector<int>> candidates_C_k = generate_candidates(L.at(L.size() - 1), k);  // generowanie kandydatów na podstawie L_k-1
        if (candidates_C_k.empty()) {   // jak nie ma żadnych kandydatów to też koniec
            break;
        }

        HashTree hash_tree(hash_pivot, bucket_size, candidates_C_k);
        hash_tree.generate();   // tworzy drzewo gdzie węzły wewnętrzne określają jaki hash się znajduje na danym miejscu, a liście zawierają tablicę kandydatów osiągalnych daną ścieżką

        std::vector<ItemSet> L_k = next_iteration_data(transactions_mapped, candidates_C_k, hash_tree, k, min_support, threads_amount); // generowanie frequent itemsetów dla k

        if (L_k.empty()) {  // nie trzeba już dalej szukać kandydatów
            break;
        }
        L.push_back(L_k);   // jeżeli L_k nie jest puste to dodajemy do do L i sprawdzamy kandydatów dla k + 1
        k += 1;
    }

    return L;
}