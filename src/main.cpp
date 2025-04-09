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
    std::vector<long double> average_cluster_size;

    auto simulate_lattice = [&](const std::string &lattice_name)
    {
        std::cout << "Lattice type is " << lattice_name << "\n";

        Lattice lattice(lattice_name);
        double initial_concentration = CFG.get<double>("simulation.initial_concentration");
        double final_concentration = CFG.get<double>("simulation.final_concentration");
        double concentration_step = CFG.get<double>("simulation.concentration_step");
        int num_configuration = CFG.get<int>("simulation.num_configurations");
        int lattice_volume = CFG.lattice_volume();

        for (double concentration = initial_concentration; concentration <= final_concentration; concentration += concentration_step)
        {
            int non_magnetic_count = std::ceil((1 - concentration) * lattice_volume);
            long total_clusters = 0;
            long total_clusters_size = 0;

            for (int configuration = 0; configuration < num_configuration; ++configuration)
            {
                std::cout << "\rConcentration: " << concentration << "\t|\tConfig:" << configuration + 1 << "/" << num_configuration << std::flush;

                lattice.initialize();
                lattice.replace_random_spins(non_magnetic_count);
                auto clusters = lattice.find_clusters();

                total_clusters += clusters.size();
                for (const auto &cluster : clusters)
                    total_clusters_size += cluster.size();
            }
            std::cout << "\n";
            average_cluster_counts.push_back(static_cast<long double>(total_clusters) / num_configuration);
            average_cluster_size.push_back(static_cast<long double>(total_clusters_size) / (total_clusters ? total_clusters : 1));
        }

        std::ofstream output_file("../data/output/clusters_" + lattice_name + ".txt");
        if (output_file.is_open())
        {
            for (int i = 0; i < average_cluster_counts.size(); ++i)
                output_file << average_cluster_counts[i] << "\t" << average_cluster_size[i] << "\n";
            output_file.close();
            std::cout << "Data saved" << std::endl;
        }
        else
            std::cerr << "Failed to open file" << std::endl;
        std::cout << std::endl;

        average_cluster_counts.clear();
        average_cluster_size.clear();
    };

    if (lattice_type == "SC" || lattice_type == "ALL")
        simulate_lattice("SC");
    if (lattice_type == "BCC" || lattice_type == "ALL")
        simulate_lattice("BCC");
    if (lattice_type == "FCC" || lattice_type == "ALL")
        simulate_lattice("FCC");

    return 0;
}