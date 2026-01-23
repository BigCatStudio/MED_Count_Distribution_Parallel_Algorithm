#include "parallel_apriori.hpp"


// Wczytuje transakcje z pliku
std::vector<std::vector<int>> get_normalized_transactions(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error(std::string("[ERROR]: Can not open file with data") + filename);
    }

    std::vector<std::vector<int>> transactions;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream stream(line);
        std::string item_id;
        std::vector<int> transaction;
        while (std::getline(stream, item_id, ',')) {
            transaction.push_back(std::stoi(item_id));
        }
        transactions.push_back(std::move(transaction));
    }

    return transactions;
}


// mapuje id transakcji na ciąg liczb w zakresie [0, m] aby przyspieszyć zwiększanie liczników, operacje porównywania w hash drzewach
std::unordered_map<int, int> get_mapping(const std::vector<std::vector<int>> &transactions) {
    int id = 0;
    std::unordered_map<int, int> mapping;   // key - original id, value - new id
    for (const std::vector<int> &transaction : transactions) {
        for (const int &item : transaction) {
            auto iterator = mapping.find(item);
            if (iterator == mapping.end()) {    // nie znalazło klucza w mapie
                mapping[item] = id++;
            }
        }
    }
    return mapping;
}


// zwraca zmapowane wartości itemów w transakcjach, przyjmuje kopie struktury transakcji więc nie napisuje oryginału tylko tworzy nowy
std::vector<std::vector<int>> map_transactions(std::vector<std::vector<int>> transactions, const std::unordered_map<int, int> &mapping) {
    for (std::vector<int> &transaction : transactions) {
        for (int &item : transaction) {
            item = mapping.at(item);
        }
        std::sort(transaction.begin(), transaction.end());  // sortowanie itemów per transakcja - przyspiesza potem obliczenia - std::sort dla zakresu do kilkudziesięciu elelemntów wybierze insertion sort
        transaction.erase(std::unique(transaction.begin(), transaction.end()), transaction.end());  // usuwanie zduplikowanych itemów per transakcja - wydajne po wcześniejszym sortowaniu
    }
    return transactions;
}


std::unordered_map<int, int> invert_map(std::unordered_map<int, int> &mapping) {
    std::unordered_map<int, int> inverted_map;
    inverted_map.reserve(mapping.size());

    for(const auto &[key, value] : mapping) {
        inverted_map[value] = key;
    }
    return inverted_map;
}


std::vector<AssociationRule> get_association_rules(std::string filename, double min_support, double min_confidence, int threads_amount, int min_size, int hash_pivot, int bucket_size) {
    // podstawowe struktury danych
    std::vector<std::vector<int>> transactions_original = get_normalized_transactions(filename);     // struktura przechowująca wszystkie transakcje z posortowanymi itemami
    std::unordered_map<int, int> mapping = get_mapping(transactions_original);  // struktura przechowuje mapowanie oryginalnych id produktów na znormalizowany zakres od 0 do n
    std::vector<std::vector<int>> transactions_mapped = map_transactions(transactions_original, mapping);
    int unique_items_amount = mapping.size();    // przechowuje ile jest unikalnych id itemów

    // etap tworzenia frequent itemsetów spełniających wymagania minsup
    std::vector<std::vector<ItemSet>> L = count_distribution(transactions_mapped, unique_items_amount, min_support, min_confidence, threads_amount, hash_pivot, bucket_size);

    // etap tworzenia reguł asocjacyjnych
    std::vector<AssociationRule> association_rules = generate_association_rules(L, threads_amount, min_confidence, min_size);
    std::cout << "Rules size: " << association_rules.size() << std::endl;

    // etap przywracania oryginalnych id itemów
    std::unordered_map<int, int> inverted_mapping = invert_map(mapping);

    for (AssociationRule &rule : association_rules) {
        for (auto &item_id : rule.left_part) {
            item_id = inverted_mapping[item_id];
        }
        for (auto &item_id : rule.right_part) {
            item_id = inverted_mapping[item_id];
        }
        rule.support = rule.support / transactions_mapped.size();
    }

    return association_rules;
}