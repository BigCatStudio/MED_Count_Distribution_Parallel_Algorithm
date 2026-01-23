#include "config.hpp"

std::unordered_map<std::string, std::string> load_config(const std::string& filename) {
    std::unordered_map<std::string, std::string> cfg;
    std::ifstream file(filename);
    std::string line;

    if(!file) {
        std::cerr << "[ERROR]: zla sciezka do pliku" << std::endl;
        return cfg;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find('=');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        cfg[key] = value;
    }
    return cfg;
}