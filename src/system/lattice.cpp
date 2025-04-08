#include "include/system/lattice.h"

// Creates a system completely filled with magnetic (1) spins
void Lattice::initialize()
{
    spin_values_vec_.assign(lattice_volume_, 1);
    ferro_indices_vec_.clear();
    for (int i = 0; i < lattice_volume_; ++i)
        ferro_indices_vec_.push_back(i);
}

void Lattice::replace_spin_via_index(int index)
{
    if (index < 0 || index >= spin_values_vec_.size())
        throw(std::out_of_range("Invalid spin index"));

    if (spin_values_vec_[index] == 0)
        return;

    for (int i = 0; i < ferro_indices_vec_.size(); ++i)
        if (ferro_indices_vec_[i] == index)
        {
            ferro_indices_vec_[i] = ferro_indices_vec_.back();
            ferro_indices_vec_.pop_back();
            return;
        }
}

void Lattice::flip_spin_via_index(int index)
{
    if (index < 0 || index >= spin_values_vec_.size())
        throw(std::out_of_range("Invalid spin index"));

    if (spin_values_vec_[index] == 0)
        return;

    spin_values_vec_[index] *= -1;
}

// Replaces random magnetic spins with non-magnetic (0) spins
void Lattice::replace_random_spins(int non_magnetic_count)
{
    if (non_magnetic_count <= 0)
        return;

    ferro_indices_vec_.clear();
    for (int i = 0; i < spin_values_vec_.size(); ++i)
        if (spin_values_vec_[i] != 0)
            ferro_indices_vec_.push_back(i);

    if (ferro_indices_vec_.empty())
        return;

    for (int i = 0; i < non_magnetic_count && !ferro_indices_vec_.empty(); ++i)
    {
        std::uniform_int_distribution<size_t> dist(0, ferro_indices_vec_.size() - 1);
        size_t random_index = dist(get_rng());
        spin_values_vec_.at(ferro_indices_vec_.at(random_index)) = 0;
        ferro_indices_vec_.erase(ferro_indices_vec_.begin() + random_index);
    }
}

// Converts 1D index to 3D coordinates
std::array<int, 3> Lattice::get_coordinates_via_index(int index) const
{
    int z = index / lattice_area_;
    int y = (index - z * lattice_area_) / lattice_size_;
    int x = index - y * lattice_size_ - z * lattice_area_;

    return {x, y, z};
}

// Returns random coordinates of a magnetic spin
std::array<int, 3> Lattice::get_random_coordinates() const
{
    if (ferro_indices_vec_.empty())
        throw std::runtime_error("No magnetic spins available");

    std::uniform_int_distribution<size_t> dist(0, ferro_indices_vec_.size() - 1);
    int index = ferro_indices_vec_[dist(get_rng())];

    return get_coordinates_via_index(index);
}

std::vector<std::vector<int>> Lattice::generate_neighbors() const
{
    std::vector<std::vector<int>> joosky_neighbors_vec;
    joosky_neighbors_vec.resize(lattice_volume_);
    std::vector<std::array<int, 3>> neighbor_offsets_vec;
    int crystal_type;
    if (crystal_type_string_ == "SC")
        crystal_type = 0;
    else if (crystal_type_string_ == "BCC")
        crystal_type = 1;
    else if (crystal_type_string_ == "FCC")
        crystal_type = 2;

    switch (crystal_type)
    {
    case 0:
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
        break;
    case 1:
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 1}, {-1, -1, -1}};
        break;
    case 2:
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {1, 1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, -1}, {0, 0, 1}, {0, 0, -1}};
        break;
    default:
        throw std::invalid_argument("unknown type of crystal");
    }

    for (int index = 0; index < lattice_volume_; ++index)
    {
        auto [i, j, k] = get_coordinates_via_index(index);

        for (const auto &offset : neighbor_offsets_vec)
        {
            int ni, nj, nk;
            if (boundary_conditions_)
            {
                ni = i + offset[0];
                nj = j + offset[1];
                nk = k + offset[2];
                if (!((ni >= 0 && ni < lattice_size_) && (nj >= 0 && nj < lattice_size_) && (nk >= 0 && nk < n_layers_)))
                    continue;
            }
            else
            {
                ni = (i + offset[0] + lattice_size_) % lattice_size_;
                nj = (j + offset[1] + lattice_size_) % lattice_size_;
                nk = (k + offset[2] + n_layers_) % n_layers_;
            }
            joosky_neighbors_vec[index].push_back(ni + nj * lattice_size_ + nk * lattice_area_);
        }
    }
    return joosky_neighbors_vec;
}

int Lattice::find(int index)
{
    if (parent_[index] != index)
        parent_[index] = find(parent_[index]);

    return parent_[index];
}

void Lattice::union_clusters(int a, int b)
{
    int parent_a = find(a);
    int parent_b = find(b);
    if (parent_a == parent_b)
        return;

    if (cluster_size_[parent_a] < cluster_size_[parent_b])
    {
        parent_[parent_a] = parent_b;
        cluster_size_[parent_b] += cluster_size_[parent_a];
    }
    else
    {
        parent_[parent_b] = parent_a;
        cluster_size_[parent_a] += cluster_size_[parent_b];
    }
}

std::vector<std::vector<int>> Lattice::find_clusters()
{
    parent_.resize(lattice_volume_);
    cluster_size_.resize(lattice_volume_);

    for (int i = 0; i < lattice_volume_; ++i)
    {
        parent_[i] = i;
        cluster_size_[i] = (spin_values_vec_[i] == 0) ? 0 : 1;
    }

    const auto &neighbors_vec = neighbors();

    for (int index = 0; index < lattice_volume_; ++index)
    {
        if (spin_values_vec_[index] != 0)
            for (const auto &neighbor_index : neighbors_vec[index])
                if (spin_values_vec_[neighbor_index] != 0)
                    union_clusters(index, neighbor_index);
    }

    std::unordered_map<int, std::vector<int>> cluster_map;

    for (int index = 0; index < lattice_volume_; ++index)
        if (spin_values_vec_[index] != 0)
        {
            int parent = find(index);
            cluster_map[parent].push_back(index);
        }

    std::vector<std::vector<int>> clusters;

    for (const auto &[parent, indices] : cluster_map)
        clusters.push_back(indices);

    return clusters;
}