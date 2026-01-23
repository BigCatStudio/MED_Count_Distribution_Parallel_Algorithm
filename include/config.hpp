#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>

std::unordered_map<std::string, std::string> load_config(const std::string& filename);


#endif  // CONFIG_H