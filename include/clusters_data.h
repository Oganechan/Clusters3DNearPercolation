#ifndef CLUSTERS_DATA_H
#define CLUSTERS_DATA_H

#include <vector>
#include <cstdint>

struct ClustersData
{
    double total_clusters;
    double total_up_clusters;
    double total_down_clusters;
    double total_perc_clusters;

    double total_cluster_size;
    double total_up_cluster_size;
    double total_down_cluster_size;
    double total_perc_cluster_size;

    void update(const std::vector<std::vector<uint32_t>> &all,
                const std::vector<std::vector<uint32_t>> &up,
                const std::vector<std::vector<uint32_t>> &down,
                const std::vector<std::vector<uint32_t>> &percolating)
    {
        total_clusters += all.size();
        total_up_clusters += up.size();
        total_up_clusters += up.size();
        total_perc_clusters += percolating.size();

        for (const auto &c : all)
            total_cluster_size += c.size();
        for (const auto &c : up)
            total_up_cluster_size += c.size();
        for (const auto &c : down)
            total_down_cluster_size += c.size();
        for (const auto &c : percolating)
            total_perc_cluster_size += c.size();
    }

    double average_cluster_size() const
    {
        return total_clusters ? total_cluster_size / total_clusters : 0.0;
    }
    double average_cluster_up_size() const
    {
        return total_up_clusters ? total_up_cluster_size / total_up_clusters : 0.0;
    }
    double average_cluster_down_size() const
    {
        return total_down_clusters ? total_down_cluster_size / total_down_clusters : 0.0;
    }
    double average_percolating_cluster_size() const
    {
        return total_perc_clusters ? total_perc_cluster_size / total_perc_cluster_size : 0.0;
    }
};

#endif