#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>



int main() {
    std::ifstream plik("dataset.dat");

    if (!plik.is_open()) {
        std::cerr << "Nie można otworzyć pliku\n";
        return 1;
    }

    std::vector<std::vector<int>> dataset;
    std::string linia;
    while (std::getline(plik, linia)) {
        std::istringstream iss(linia);
        std::vector<int> v;
        int x;

        while (iss >> x) {
            v.push_back(x);
        }

        if (!v.empty()) {                 // pomija puste linie
            dataset.push_back(std::move(v));
        }
    }

    // test: wypisz
    for (size_t i = 0; i < dataset.size(); ++i) {
        std::cout << "Linia " << i << ": ";
        for (int x : dataset[i]) std::cout << x << ' ';
        std::cout << "\n";
    }



    plik.close();

    return 0;
}