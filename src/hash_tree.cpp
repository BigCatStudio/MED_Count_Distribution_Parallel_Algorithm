#include "hash_tree.hpp"


// konstruktor
HashTree::HashTree(int hash_pivot, int bucket_size, std::vector<std::vector<int>> &candidate_itemsets) 
    : hash_pivot{hash_pivot}, bucket_size{bucket_size}, candidate_itemsets{candidate_itemsets} {}


void HashTree::insert(std::shared_ptr<Node> current_node, int &itemset_id, int current_depth) {
    if (!current_node->children.empty() || current_node->hash_value == -1) {   // jesteśmy w węźle wewnętrznym
        int hash = candidate_itemsets.at(itemset_id).at(current_depth) % hash_pivot;
        for (auto child : current_node->children) { // istnieje dziecko obsługujące dany hash
            if (child->hash_value == hash) {
                insert(child, itemset_id, current_depth + 1);
                return;
            }
        }
        // nie istnieje dziecko obsługujące dany hash
        std::shared_ptr<Node> new_node = std::make_shared<Node>();
        new_node->children.clear();
        new_node->hash_value = hash;
        new_node->candidates.push_back(itemset_id);
        current_node->children.push_back(new_node);
        return;
    }

    // jesteśmy w liściu
    current_node->candidates.push_back(itemset_id);

    if (current_depth < candidate_itemsets.at(0).size() && current_node->candidates.size() > bucket_size) {  // można wciąż tworzyć liście na większej głębokości
        // tworzenie nowego liścia
        int found = false;
        for (int candidate_id : current_node->candidates) {
            int hash = candidate_itemsets.at(candidate_id).at(current_depth) % hash_pivot;
            for (auto child : current_node->children) {
                if (child->hash_value == hash) {
                    insert(child, candidate_id, current_depth + 1);
                    found = true;
                    break;
                }
            }

            if (!found) {   // nie ma jeszcze liscia dla danego hasza
                std::shared_ptr<Node> new_node = std::make_shared<Node>();
                new_node->hash_value = hash;
                new_node->candidates.push_back(candidate_id);
                new_node->children.clear();
                current_node->children.push_back(new_node);
            }
            found = false;
        }
        current_node->candidates.clear();
    }
} 


// metody
void HashTree::generate() {
    head = std::make_shared<Node>();  // inicjowanie węzła początkowego - ma tylko waskaźniki na niższe węzły
    head->hash_value = -1;
    head->children.clear();
    head->candidates.clear();

    int depth = 0;
    std::shared_ptr<Node> current_node = head;

    for (int i = 0;i < candidate_itemsets.size();i++) {
        // węzeł ma dzieci
        insert(head, i, depth);
    }
}


// metoda weryfikacja obecności podzbioru w zbiorze zakłądając że oba kontenery są posortowane
bool check_candidate(const std::vector<int>& candidate, const std::vector<int>& transaction) {
    size_t i = 0, j = 0;
    while (i < candidate.size() && j < transaction.size()) {
        if (candidate[i] == transaction[j]) { 
            ++i; 
            ++j; 
        }
        else if (candidate[i] > transaction[j]) { 
            ++j; 
        }
        else { // candidate[i] < transaction[j]
            return false; 
        }
    }
    return i == candidate.size();   // znaleziono wszystkie elementy kandydata w transakcji
}


void HashTree::traverse(const std::vector<int> &transaction, int start, int depth, int k, const std::shared_ptr<Node> &current_node, std::vector<int> &candidates_id, std::vector<char> &seen) const {
    if (!current_node) {
        return;
    }

    if (transaction.size() - start < (k - depth)) {  // nie ma już elementów żeby dobrać
        return;
    }

    if (current_node->children.empty()) {   // jesteśmy w liściu
        for (int candidate_id : current_node->candidates) {
            if (candidate_id >= candidate_itemsets.size()) {
                continue;
            }
            if (!seen.at(candidate_id) && check_candidate(candidate_itemsets.at(candidate_id), transaction)) {
                seen.at(candidate_id) = 1;
                candidates_id.push_back(candidate_id);
            }
        }
        return;
    }

    // ograniczenie górnej granicy żeby nie brać itemów który już nie stworzą kandydata o porządanej długości k
    const int end = transaction.size() - (k - depth);
    for (int current_item_id = start; current_item_id <= end; ++current_item_id) {
        int hash = transaction.at(current_item_id) % hash_pivot;
        if (hash < 0) hash += hash_pivot;

        std::shared_ptr<Node> node_next = nullptr;
        for (const auto &child : current_node->children) {
            if (child && child->hash_value == hash) {   // przeszukiwanie niższych węzłów czy którys pasuje hashem
                node_next = child;
                break; 
            }
        }

        if (!node_next) {
            continue;
        }

        traverse(transaction, current_item_id + 1, depth + 1, k, node_next, candidates_id, seen);   // kolejne pozycje do weryfikacji tylko dla itemów po obecnym - bo są posortowane
    }
}

std::vector<int> HashTree::search_candidates(const std::vector<int> &transaction) const {
    std::vector<int> candidates_id;

    const int k = candidate_itemsets.at(0).size();

    std::vector<char> seen(candidate_itemsets.size(), 0);   // do weryfikacji czy dany kandydat już nie był uwzględniony - TODO do weryfikacji czy potrzebne
    traverse(transaction, 0, 0, k, head, candidates_id, seen);  // przeszukiwanie od korzenia i pierwszego elementu transakcji

    return candidates_id;
}


// tylko pod debug
void HashTree::display() {
    auto indent = [](int depth) {
        return std::string(depth * 2, ' ');
    };

    std::function<void(const std::shared_ptr<Node>&, int)> dfs;
    dfs = [&](const std::shared_ptr<Node>& node, int depth) {
        if (!node) return;

        const bool is_leaf = node->children.empty();

        std::cout << indent(depth)
                  << (is_leaf ? "[LISC]" : "[WEZEL]")
                  << " depth=" << depth
                  << " hash_value=" << node->hash_value;

        // kandydaci
        std::cout << " candidates={";
        for (size_t i = 0; i < node->candidates.size(); ++i) {
            std::cout << node->candidates[i];
            if (i + 1 < node->candidates.size()) std::cout << ",";
        }
        std::cout << "}";

        // dzieci
        std::cout << " children=" << node->children.size() << "\n";

        if (!is_leaf) {
            for (size_t i = 0; i < node->children.size(); ++i) {
                auto child = node->children[i];
                if (!child) {
                    std::cout << indent(depth + 1)
                              << "- child[" << i << "] = nullptr\n";
                } else {
                    std::cout << indent(depth + 1)
                              << "- child[" << i << "] -> hash_value=" << child->hash_value
                              << "\n";
                    dfs(child, depth + 2);
                }
            }
        }
    };

    std::cout << "========== HASH TREE ==========\n";
    dfs(head, 0);
    std::cout << "================================\n";
}
