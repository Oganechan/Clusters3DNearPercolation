#include <iostream>
#include <fstream>
#include <argparse/argparse.hpp>
#include "include/config.h"
#include "include/lattice.h"
#include "include/clusters_data.h"

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
        std::cerr << err.what() << "\n";
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

    auto simulate_lattice = [&](const std::string &lattice_name)
    {
        std::cout << "Lattice type is " << lattice_name << std::endl;

        Lattice lattice(lattice_name);
        const int lattice_volume = CFG.lattice_volume();

        const double initial_concentration = CFG.get<double>("simulation.initial_concentration");
        const double final_concentration = CFG.get<double>("simulation.final_concentration");
        const double concentration_step = CFG.get<double>("simulation.concentration_step");

        const int num_configuration = CFG.get<int>("simulation.num_configurations");
        const int num_mc_steps = CFG.get<int>("simulation.num_mc_steps");
        const int progres_step = num_configuration / 100;

        const double temperature = CFG.get<double>("parameters.temperature");

        ClustersData data;
        std::ofstream output_file("../data/output_txt/clusters_" + lattice_name + ".txt");

        for (double concentration = initial_concentration; concentration <= final_concentration + 1e-3; concentration += concentration_step)
        {
            int non_magnetic_count = std::ceil((1 - concentration) * lattice_volume);
            data.reset();

            for (int configuration = 0; configuration < num_configuration; ++configuration)
            {
                if (configuration % progres_step == 0 || configuration == num_configuration - 1)
                {
                    int persent = (configuration * 100) / (num_configuration - 1);
                    std::cout << "\rConcentration: " << concentration
                              << " | Completed: " << persent << "%   " << std::flush;
                }

                lattice.initialize();
                lattice.replace_random_spins(non_magnetic_count);

                for (int mc_step = 0; mc_step < num_mc_steps; ++mc_step)
                {
                    lattice.wolf(temperature);
                }

                auto [clusters_all, clusters_up, clusters_down, clusters_percolation] = lattice.find_clusters();
                data.update(clusters_all, clusters_up, clusters_down, clusters_percolation);
            }

            output_file << concentration << "\t"
                        << data.total_clusters / (double)num_configuration << "\t"
                        << data.average_cluster_size() / lattice_volume << "\t"
                        << data.total_up_clusters / (double)num_configuration << "\t"
                        << data.average_cluster_up_size() / lattice_volume << "\t"
                        << data.total_down_clusters / (double)num_configuration << "\t"
                        << data.average_cluster_down_size() / lattice_volume << "\t"
                        << data.total_perc_clusters / (double)num_configuration << "\t"
                        << data.average_percolating_cluster_size() / lattice_volume << "\n";
        }
        std::cout << "\nData saved for " << lattice_name << " lattice!\n"
                  << std::endl;
    };

    if (lattice_type == "SC" || lattice_type == "ALL")
        simulate_lattice("SC");
    if (lattice_type == "BCC" || lattice_type == "ALL")
        simulate_lattice("BCC");
    if (lattice_type == "FCC" || lattice_type == "ALL")
        simulate_lattice("FCC");

    return 0;
}