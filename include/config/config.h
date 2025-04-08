#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <fstream>
#include <mutex>

using json = nlohmann::json;

class Config
{
public:
    static Config &getInstance()
    {
        static Config instance;
        return instance;
    }

    void load(const std::string &path);

    template <typename T>
    T get(const std::string &key, const T &default_value = T()) const
    {
        if (!is_loaded_)
            throw std::runtime_error("Config not loaded");
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
            return current.value(k, default_value);
        }
        catch (const json::exception &e)
        {
            throw std::runtime_error("Config error at '" + key + "': " + e.what());
        }
    }

    int lattice_size() const { return get<int>("lattice.size"); }
    int n_layers() const { return get<int>("lattice.n_layers"); }
    int lattice_area() const { return lattice_size() * lattice_size(); }
    int lattice_volume() const { return lattice_area() * n_layers(); }

    const json &data() const { return config_; }

private:
    Config() = default;
    Config(const Config &) = delete;
    void operator=(const Config &) = delete;

    json config_;
    std::mutex load_mutex_;
    bool is_loaded_ = false;
};

#define CFG Config::getInstance()

#endif