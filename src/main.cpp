#include "parallel_apriori.hpp"
#include "config.hpp"
#include <iostream>
#include <chrono>


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "[ERROR]: podaj sciezke do configu" << std::endl;
    }
    auto config = load_config(argv[1]);

    using clock = std::chrono::steady_clock;
    auto t0 = clock::now();
    std::vector<AssociationRule> association_rules = get_association_rules(config["data_path"], std::stod(config["minsup"]), std::stod(config["minconf"]), std::stoi(config["threads"]), std::stoi(config["min_size"]), std::stoi(config["hash_pivot"]), std::stoi(config["bucket_size"]));
    auto t1 = clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "Czas: " << ms << " ms\n";


    // for (AssociationRule &rule : association_rules) {
    //     std::cout << "Rule (";
    //     for (auto &item_id : rule.left_part) {
    //         std::cout << item_id << ", ";
    //     }
    //     std::cout << ") -> (";
    //     for (auto &item_id : rule.right_part) {
    //         std::cout << item_id << ", "; 
    //     }
    //     std::cout << ") | support: " << rule.support << " | confidence: " << rule.confidence << "\n";
    // }
}
