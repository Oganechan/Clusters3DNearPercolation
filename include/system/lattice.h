#ifndef LATTICE_H
#define LATTICE_H

#include <vector>
#include <array>
#include <random>
#include <unordered_map>
#include "include/config/config.h"

class Lattice
{
public:
    Lattice(std::string crystal_type)
        : lattice_size_(CFG.lattice_size()),
          n_layers_(CFG.n_layers()),
          lattice_area_(lattice_size_ * lattice_size_),
          lattice_volume_(lattice_area_ * n_layers_),
          crystal_type_string_(crystal_type),
          boundary_conditions_(CFG.get<std::string>("lattice.boundary_conditions") == "hard") { cached_neighbors_ = generate_neighbors(); }

    void initialize();
    void replace_spin_via_index(int index);
    void flip_spin_via_index(int index);
    void replace_random_spins(int non_magnetic_count);

    std::array<int, 3> get_coordinates_via_index(int index) const;
    std::array<int, 3> get_random_coordinates() const;

    const std::vector<std::vector<int>> &neighbors() const
    {
        return cached_neighbors_;
    }

    std::vector<std::vector<int>> find_clusters();

    const std::vector<int> &spin_values() const { return spin_values_vec_; }
    const std::vector<int> &ferro_indices() const { return ferro_indices_vec_; }

private:
    std::vector<int> spin_values_vec_;
    std::vector<int> ferro_indices_vec_;

    const int lattice_size_;
    const int lattice_area_;
    const int lattice_volume_;
    const int n_layers_;
    const std::string crystal_type_string_;
    const bool boundary_conditions_;

    std::vector<int> parent_;
    std::vector<int> cluster_size_;

    mutable std::vector<std::vector<int>> cached_neighbors_;

    static std::mt19937 &get_rng()
    {
        thread_local std::mt19937 rng(std::random_device{}());
        return rng;
    }

    std::vector<std::vector<int>> generate_neighbors() const;

    int find(int index);
    void union_clusters(int a, int b);
};

#endif