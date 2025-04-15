#ifndef LATTICE_H
#define LATTICE_H

#include <vector>
#include <array>
#include <cstdint>
#include <random>
#include <unordered_map>
#include "include/config.h"

class Lattice
{
public:
    Lattice(std::string crystal_type_string)
        : lattice_size_(static_cast<uint16_t>(CFG.lattice_size())),
          n_layers_(static_cast<uint16_t>(CFG.n_layers())),
          lattice_area_(lattice_size_ * lattice_size_),
          lattice_volume_(lattice_area_ * n_layers_),
          crystal_type_string_(crystal_type_string),
          boundary_conditions_(CFG.get<std::string>("lattice.boundary_conditions") == "hard")
    {
        cached_neighbors_ = generate_neighbors();
    }

    void initialize();
    void replace_random_spins(uint32_t non_magnetic_count);

    std::array<uint16_t, 3> get_coordinates_via_index(uint32_t index) const;

    const std::vector<std::vector<uint32_t>> &neighbors() const
    {
        return cached_neighbors_;
    }

    std::array<std::vector<std::vector<uint32_t>>, 4> find_clusters();
    void wolf(double temperature);

private:
    const uint16_t lattice_size_;
    const uint16_t n_layers_;
    const uint32_t lattice_area_;
    const uint32_t lattice_volume_;
    const std::string crystal_type_string_;
    const bool boundary_conditions_;

    std::vector<int8_t> spin_values_vec_;
    std::vector<uint32_t> ferro_indices_vec_;

    std::vector<uint32_t> parent_;
    std::vector<uint32_t> rank_;

    mutable std::vector<std::vector<uint32_t>> cached_neighbors_;

    static std::mt19937 &get_rng()
    {
        thread_local std::mt19937 rng(std::random_device{}());
        return rng;
    }

    std::vector<std::vector<uint32_t>> generate_neighbors() const;

    uint32_t find(uint32_t index);
    void union_clusters(uint32_t a, uint32_t b);
    bool is_cluster_percolation(const std::vector<uint32_t> &cluster);
};

#endif