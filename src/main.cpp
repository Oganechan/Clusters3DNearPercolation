#include <iostream>
#include <fstream>
#include <argparse/argparse.hpp>
#include "include/config.h"
#include "include/lattice.h"

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
        const double initial_concentration = CFG.get<double>("simulation.initial_concentration");
        const double final_concentration = CFG.get<double>("simulation.final_concentration");
        const double concentration_step = CFG.get<double>("simulation.concentration_step");
        const int num_configuration = CFG.get<int>("simulation.num_configurations");
        const int lattice_volume = CFG.lattice_volume();
        const int progres_step = num_configuration / 100;

        for (double concentration = initial_concentration; concentration <= final_concentration + 1e-3; concentration += concentration_step)
        {
            int non_magnetic_count = std::ceil((1 - concentration) * lattice_volume);
            long total_clusters = 0;
            long total_clusters_size = 0;

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
                auto clusters = lattice.find_clusters();

                total_clusters += clusters.size();
                for (const auto &cluster : clusters)
                    total_clusters_size += cluster.size();
            }
        }
        std::cout << std::endl;
    };

    if (lattice_type == "SC" || lattice_type == "ALL")
        simulate_lattice("SC");
    if (lattice_type == "BCC" || lattice_type == "ALL")
        simulate_lattice("BCC");
    if (lattice_type == "FCC" || lattice_type == "ALL")
        simulate_lattice("FCC");

    return 0;
}