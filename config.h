#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>

class config
{
public:
    std::unordered_map<std::string, std::string> read_config(const std::string& config_path);

private:
    std::string config_path;
};