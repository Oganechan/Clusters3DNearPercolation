#include "include/config.h"

void Config::load(const std::string &path)
{
    std::lock_guard<std::mutex> lock(load_mutex_);
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cant open config file" + path);
    file >> config_;
    is_loaded_ = true;
}