#ifndef HASH_TREE_H
#define HASH_TREE_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <functional>


struct Node {
    std::vector<std::shared_ptr<Node>> children;    // wskaźniki do węzłów niżej
    int hash_value; // na podstawie tej wartosci wiadomo czy sprawdzac dalej dla itemu
    std::vector<int> candidates;    // zawiera id kandydatów do sprawdzenia -> jak None to znaczy, że to węzeł a nie liść
};


class HashTree {
private:
    int hash_pivot;
    int bucket_size;
    std::shared_ptr<Node> head;
    std::vector<std::vector<int>> candidate_itemsets;

public:
    // Konstruktory
    HashTree(int hash_pivot, int bucket_size, std::vector<std::vector<int>> &candidate_itemsets);

    // metody
    void generate();

    void insert(std::shared_ptr<Node> current_node, int &itemset_id, int current_k);

    void traverse(const std::vector<int> &transaction, int startPos, int depth, int k, const std::shared_ptr<Node> &current_node, std::vector<int> &candidates_id, std::vector<char> &seen) const;

    std::vector<int> search_candidates(const std::vector<int> &transaction) const;

    void display();
};

#endif  // HASH_TREE_H