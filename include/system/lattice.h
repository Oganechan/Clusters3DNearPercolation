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
    Lattice(std::string crystal_type,
            std::vector<int> initial_spin_values_vec = {},
            std::vector<int> initial_ferro_indices_vec = {})
        : lattice_size_(CFG.lattice_size()),
          lattice_area_(CFG.lattice_area()),
          lattice_volume_(CFG.lattice_volume()),
          n_layers_(CFG.n_layers()),
          crystal_type_string_(crystal_type),
          boundary_conditions_(CFG.get<std::string>("lattice.boundary_conditions") == "hard"),
          spin_values_vec_(initial_spin_values_vec),
          ferro_indices_vec_(initial_ferro_indices_vec) {}

    void initialize();
    void replace_spin_via_index(int index);
    void flip_spin_via_index(int index);
    void replace_random_spins(int non_magnetic_count);

    std::array<int, 3> get_coordinates_via_index(int index) const;
    std::array<int, 3> get_random_coordinates() const;

    const std::vector<std::vector<int>> &neighbors() const
    {
        if (cached_neighbors_.empty())
            cached_neighbors_ = generate_neighbors();
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

    mutable std::mt19937 rng_{std::random_device{}()};
    mutable std::vector<std::vector<int>> cached_neighbors_;

    std::vector<std::vector<int>> generate_neighbors() const;

    int find(int index);
    void union_clusters(int a, int b);
};

#endif