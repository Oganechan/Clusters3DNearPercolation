#include "include/system/lattice.h"

// Creates a system completely filled with magnetic (1) spins
void Lattice::initialize()
{
    spin_values_vec_.clear();
    ferro_indices_vec_.clear();
    parent_.clear();
    rank_.clear();

    std::bernoulli_distribution dist(0.5);
    for (uint32_t i = 0; i < lattice_volume_; ++i)
    {
        spin_values_vec_.push_back(dist(get_rng()) ? 1 : -1);
        ferro_indices_vec_.push_back(i);
    }
}

// Replaces random magnetic spins with non-magnetic (0) spins
void Lattice::replace_random_spins(uint32_t non_magnetic_count)
{
    if (non_magnetic_count == 0)
        return;

    ferro_indices_vec_.clear();
    ferro_indices_vec_.reserve(lattice_volume_);
    for (uint32_t i = 0; i < spin_values_vec_.size(); ++i)
        if (spin_values_vec_[i] != 0)
            ferro_indices_vec_.push_back(i);

    if (ferro_indices_vec_.empty())
        return;

    for (uint32_t i = 0; i < non_magnetic_count && !ferro_indices_vec_.empty(); ++i)
    {
        std::uniform_int_distribution<uint32_t> dist(0, ferro_indices_vec_.size() - 1);
        uint32_t random_index = dist(get_rng());
        spin_values_vec_[ferro_indices_vec_[random_index]] = 0;
        ferro_indices_vec_.erase(ferro_indices_vec_.begin() + random_index);
    }
}

// Converts 1D index to 3D coordinates
std::array<uint16_t, 3> Lattice::get_coordinates_via_index(uint32_t index) const
{
    uint16_t z = index / lattice_area_;
    uint16_t y = (index - z * lattice_area_) / lattice_size_;
    uint16_t x = index - y * lattice_size_ - z * lattice_area_;

    return {x, y, z};
}

std::vector<std::vector<uint32_t>> Lattice::generate_neighbors() const
{
    std::vector<std::vector<uint32_t>> joosky_neighbors_vec(lattice_volume_);
    std::vector<std::array<int8_t, 3>> neighbor_offsets_vec;

    if (crystal_type_string_ == "SC" || crystal_type_string_ == "sc")
    {
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
    }
    else if (crystal_type_string_ == "BCC" || crystal_type_string_ == "bcc")
    {
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 1}, {-1, -1, -1}};
    }
    else if (crystal_type_string_ == "FCC" || crystal_type_string_ == "fcc")
    {
        neighbor_offsets_vec = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {1, 1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, -1}, {0, 0, 1}, {0, 0, -1}};
    }
    else
    {
        throw std::invalid_argument("unknown type of crystal");
    }

    for (uint32_t index = 0; index < lattice_volume_; ++index)
    {
        auto [i, j, k] = get_coordinates_via_index(index);

        for (const auto &offset : neighbor_offsets_vec)
        {
            int16_t ni, nj, nk;
            if (boundary_conditions_)
            {
                ni = i + offset[0];
                nj = j + offset[1];
                nk = k + offset[2];
                if (ni < 0 || ni >= lattice_size_ || nj < 0 || nj >= lattice_size_ || nk < 0 || nk >= n_layers_)
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

uint32_t Lattice::find(uint32_t index)
{
    while(parent_[index] != index)
    {
        parent_[index] = parent_[parent_[index]];
        index = parent_[index];
    }
    
    return index;
}

void Lattice::union_clusters(uint32_t a, uint32_t b)
{
    uint32_t parent_a = find(a);
    uint32_t parent_b = find(b);

    if (parent_a == parent_b)
        return;

    if (rank_[parent_a] < rank_[parent_b])
    {
        parent_[parent_a] = parent_b;
    }
    else if (rank_[parent_a > rank_[parent_b]])
    {
        parent_[parent_b] = parent_a;
    }
    else
    {
        parent_[parent_b] = parent_a;
        ++rank_[parent_a];
    }
}

std::vector<std::vector<uint32_t>> Lattice::find_clusters()
{
    parent_.clear();
    parent_.resize(lattice_volume_);
    rank_.clear();
    rank_.resize(lattice_volume_);

    for (const auto &index : ferro_indices_vec_)
    {
        parent_[index] = index;
        rank_[index] = 1;
    }

    const auto &neighbors_vec = neighbors();

    for (const auto &index : ferro_indices_vec_)
        for (const auto &neighbor_index : neighbors_vec[index])
            if (spin_values_vec_[neighbor_index] != 0 && spin_values_vec_[index] == spin_values_vec_[neighbor_index])
                union_clusters(index, neighbor_index);

    std::unordered_map<uint32_t, std::vector<uint32_t>> cluster_map;
    for (const auto &index : ferro_indices_vec_)
    {
        uint32_t parent = find(index);
        cluster_map[parent].push_back(index);
    }

    std::vector<std::vector<uint32_t>> clusters;
    for (auto &[parent, indices] : cluster_map)
        clusters.push_back(std::move(indices));

    return clusters;
}