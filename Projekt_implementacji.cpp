#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// lista transakcji jest zwracana
std::vector<std::vector<int>> load_data(
    std::string file_name   // nazwa pliku z danymi
) {
    std::cout << "działam" << std::endl;
}

// struktura przechowująca unikalny hash kombinacji pojedynczych produktów tworzących itemset
namespace custom {
    using itemset_map = std::unordered_map<int, std::vector<int>>;  // 1 - hash itemsetu, 2 - zbiór id produktów
    using counter_map = std::unordered_map<int, int>;   // 1 - hasj itemsetu, 2 - obecna wartość supportu
}




// zwracana jest mapa supportów dla podanych kandydatów
custom::counter_map generate_local_counts(
    std::vector<int>& transactions, // D - transakcje przypisane dla danego wątku
    int iteration,  // k - która iteracja algorytmu ma być wykonana
    custom::itemset_map itemsets_candidates = {} // C_k - lista potencjalnych itemsetów
) {
    return custom::counter_map {};
}

// tworzy kandydatów z listy list produktów
custom::itemset_map generate_candidates(
    int iteration,  // k - określa ilość produktów które tworzą itemset
    std::vector<std::vector<int>> previous_frequent_itemsets    // L_k-1 - zbiór itemsetów na bazie których mają być stworzeni nowi kandydaci
) {

}


// tworzy id itemsetu przechowywany w liczniku globalnym
int generate_itemset_id(
    std::vector<int>& itemset   // lista id produktów które tworzą itemset
) {
    return 1;
}


custom::counter_map filter_itemsets(
    custom::counter_map itemsets_counters,  // stan supportów itemsetów kandydujących
    double min_support // próg który musi osiągnąć itemset aby się liczyć
) {
    custom::counter_map map;
    return map;
}



std::vector<custom::itemset_map> get_divided_itemsets(
    custom::itemset_map frequent_itemsets   // L - zbiór wszystkich zakwalifikowanych itemsetów
) {

}



struct AssociationRule {
    int antecedent_itemset_id; // 
    int consequent_itemset_id;
    double confidence;
};

std::vector<AssociationRule> generate_association_rules(
    custom::itemset_map& itemsets,  // L - zbiór badanych itemsetów
    custom::counter_map& global_counters,    // referencja do wspólnego obiektu z którego będą odczytywane wartości supportów dla każdego itemsetu
    double min_confidence   //  Wartość confidence którą musi mieć reguła żeby była w wyniku końcowym funkcji
) {

}


int main() {
    std::vector<std::vector<int>> dataset = load_data("dataset.dat");  // load_data
    std::vector<std::vector<int>> tst1 = {
        {12, 34, 78, 90},
        {1, 109, 34, 90},
        {34, 821, 90, 12}
    };

    custom::itemset_map frequent_itemsets;  // L - zbiór wszystkich itemsetów, na podstawie których na końcu będą tworzone reguły asocjacyjne
    custom::counter_map global_counter; // Globalny licznik wystąpień każdego unikalnego itemsetu


    return 0;
}