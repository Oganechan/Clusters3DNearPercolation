#include "include/config/config.h"

void Config::load(const std::string &path)
{
    std::lock_guard<std::mutex> lock(load_mutex_);
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cant open config file" + path);
    config_ = json::parse(file);
    is_loaded_ = true;
    path_cache_.clear();
}

template <typename T>
T Config::get(const std::string &key, const T &default_value = T()) const
{
    if (!is_loaded_)
        throw std::runtime_error("Config not loaded");

    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = path_cache_.fing(key);
    if (it != path_cache_.end())
        return it->second.get<T>();

    try
    {
        json current = config_;
        size_t pos = 0;
        std::string k = key;
        while ((pos = k.find('.')) != std::string::npos)
        {
            current = current.at(k.substr(0, pos));
            k = k.substr(pos + 1);
        }
        T value = current.value(k, default_value);

        std::lock_guard<std::mutex> lock(cache_mutex_);
        path_cache_[key] = value;

        return value;
    }
    catch (const json::exception &e)
    {
        throw std::runtime_error("Config error at '" + key + "': " e.what());
    }
}