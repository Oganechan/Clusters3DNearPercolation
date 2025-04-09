#include <iostream>
#include <fstream>
#include <argparse/argparse.hpp>
#include "include/config/config.h"
#include "include/system/lattice.h"

int main(int argc, char **argv)
{
    argparse::ArgumentParser program("finding_clusters_in_dilute_magnets");
    program.add_argument("-c", "--config")
        .default_value(std::string("../data/configs/default.json"));
    program.add_argument("-l", "--lattice")
        .default_value(std::string("ALL"))
        .choices("SC", "BCC", "FCC", "ALL");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string config_path = program.get<std::string>("--config");
    std::string lattice_type = program.get<std::string>("--lattice");

    try
    {
        CFG.load(config_path);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<long double> average_cluster_counts;

    // Simple Cubic lattice simulation
    if (lattice_type == "SC" || lattice_type == "ALL")
    {
        std::cout << "Lattice type is SC" << std::endl;
        Lattice sc_lattice("SC");
        for (double concentration = CFG.get<double>("simulation.initial_concentration");
             concentration <= CFG.get<double>("simulation.final_concentration");
             concentration += CFG.get<double>("simulation.concentration_step"))
        {
            std::cout << "Current concentration: " << concentration << std::endl;
            int non_magnetic_count = std::ceil((1 - concentration) * CFG.lattice_volume());
            long total_clusters = 0;
            for (int configuration = 0; configuration < CFG.get<int>("simulation.num_configurations"); ++configuration)
            {
                std::cout << "\033[2K\rConfiguration: " << configuration + 1 << " of "
                          << CFG.get<int>("simulation.num_configurations") << std::flush;
                sc_lattice.initialize();
                sc_lattice.replace_random_spins(non_magnetic_count);
                auto clusters = sc_lattice.find_clusters();
                int multi_atom_clusters_count = 0;
                for (const auto &cluster : clusters)
                    if (cluster.size() != 1)
                        ++multi_atom_clusters_count;
                total_clusters += multi_atom_clusters_count;
            }
            std::cout << std::endl;
            average_cluster_counts.push_back(total_clusters / CFG.get<int>("simulation.num_configurations"));
        }
        std::ofstream sc_output_file("../data/output/clusters_sc.txt");
        if (sc_output_file.is_open())
        {
            for (const auto &avg_count : average_cluster_counts)
                sc_output_file << avg_count << "\n";
            sc_output_file.close();
            std::cout << "Data saved" << std::endl;
        }
        else
            std::cerr << "Failed to open file" << std::endl;
        std::cout << std::endl;
    }

    average_cluster_counts.clear();

    // Body-Centered Cubic lattice simulation
    if (lattice_type == "BCC" || lattice_type == "ALL")
    {
        std::cout << "Lattice type is BCC" << std::endl;
        Lattice bcc_lattice("BCC");
        for (double concentration = CFG.get<double>("simulation.initial_concentration");
             concentration <= CFG.get<double>("simulation.final_concentration");
             concentration += CFG.get<double>("simulation.concentration_step"))
        {
            std::cout << "Current concentration: " << concentration << std::endl;
            int non_magnetic_count = std::ceil((1 - concentration) * CFG.lattice_volume());
            long total_clusters = 0;
            for (int configuration = 0; configuration < CFG.get<int>("simulation.num_configurations"); ++configuration)
            {
                std::cout << "\033[2K\rConfiguration: " << configuration + 1 << " of "
                          << CFG.get<int>("simulation.num_configurations") << std::flush;
                bcc_lattice.initialize();
                bcc_lattice.replace_random_spins(non_magnetic_count);
                auto clusters = bcc_lattice.find_clusters();
                int multi_atom_clusters_count = 0;
                for (const auto &cluster : clusters)
                    if (cluster.size() != 1)
                        ++multi_atom_clusters_count;
                total_clusters += multi_atom_clusters_count;
            }
            std::cout << std::endl;
            average_cluster_counts.push_back(total_clusters / CFG.get<int>("simulation.num_configurations"));
        }
        std::ofstream bcc_output_file("../data/output/clusters_bcc.txt");
        if (bcc_output_file.is_open())
        {
            for (const auto &avg_count : average_cluster_counts)
                bcc_output_file << avg_count << "\n";
            bcc_output_file.close();
            std::cout << "Data saved" << std::endl;
        }
        else
            std::cerr << "Failed to open file" << std::endl;
        std::cout << std::endl;
    }

    average_cluster_counts.clear();

    // Face-Centered Cubic lattice simulation
    if (lattice_type == "FCC" || lattice_type == "ALL")
    {
        std::cout << "Lattice type is FCC" << std::endl;
        Lattice fcc_lattice("FCC");
        for (double concentration = CFG.get<double>("simulation.initial_concentration");
             concentration <= CFG.get<double>("simulation.final_concentration");
             concentration += CFG.get<double>("simulation.concentration_step"))
        {
            std::cout << "Current concentration: " << concentration << std::endl;
            int non_magnetic_count = std::ceil((1 - concentration) * CFG.lattice_volume());
            long total_clusters = 0;
            for (int configuration = 0; configuration < CFG.get<int>("simulation.num_configurations"); ++configuration)
            {
                std::cout << "\033[2K\rConfiguration: " << configuration + 1 << " of "
                          << CFG.get<int>("simulation.num_configurations") << std::flush;
                fcc_lattice.initialize();
                fcc_lattice.replace_random_spins(non_magnetic_count);
                auto clusters = fcc_lattice.find_clusters();
                int multi_atom_clusters_count = 0;
                for (const auto &cluster : clusters)
                    if (cluster.size() != 1)
                        ++multi_atom_clusters_count;
                total_clusters += multi_atom_clusters_count;
            }
            std::cout << std::endl;
            average_cluster_counts.push_back(total_clusters / CFG.get<int>("simulation.num_configurations"));
        }
        std::ofstream fcc_output_file("../data/output/clusters_fcc.txt");
        if (fcc_output_file.is_open())
        {
            for (const auto &avg_count : average_cluster_counts)
                fcc_output_file << avg_count << "\n";
            fcc_output_file.close();
            std::cout << "Data saved" << std::endl;
        }
        else
            std::cerr << "Failed to open file" << std::endl;
        std::cout << std::endl;
    }
    return 0;
}