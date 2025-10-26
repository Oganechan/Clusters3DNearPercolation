# Finding Clusters in Dilute Magnets


## Description

This C++ project simulates magnetic phase transitions in dilute magnetic systems using Monte Carlo methods. It studies cluster formation across different crystal lattice types (SC, BCC, FCC) as a function of magnetic concentration.

The program performs Wolff cluster algorithm simulations to analyze:
- Cluster counts and sizes
- Percolating clusters
- Spin-up and spin-down clusters
- Phase transition behavior

## Key Features
- Supports Simple Cubic (SC), Body-Centered Cubic (BCC), and Face-Centered Cubic (FCC) lattices
- Configurable simulation parameters via JSON files
- Parallel-ready architecture with thread-safe configuration
- Efficient union-find algorithm for cluster identification
- Wolf cluster Monte Carlo updates
- Automatic percolation detection
- Data export for visualization

## Build Instructions
1. Clone the repository:
```bash
git clone https://github.com/Oganechan/concentation_phase_transition
cd finding_clusters_in_dilute_magnets
```
2. Configure with CMake:
```bash
mkdir build && cd build
cmake ..
```
3. Build the project:
```bash
cmake --build .
```
The build system automatically fetches the required dependencies (nlohmann/json and argparse).

## Usage
Basic usage:
```bash
./finding_clusters_in_dilute_magnets
```
With custom configuration:
```bash
./finding_clusters_in_dilute_magnets -c ../data/configs/test.json -l SC
```
Command line options:
```bash
-c, --config    Path to configuration file (default: ../data/configs/default.json)
-l, --lattice   Lattice type: SC, BCC, FCC, or ALL (default: ALL)
```

## Configuration Files
Example configuration (default.json):
```json
{
    "lattice": {
        "size": 40,
        "n_layers": 40,
        "boundary_conditions": "hard"
    },
    "simulation": {
        "initial_concentration": 0.0,
        "final_concentration": 1.0,
        "concentration_step": 0.01,
        "num_configurations": 500,
        "num_mc_steps": 5000
    },
    "parameters": {
        "temperature": 0.01
    }
}
```

## Output
The program generates text files in data/output_txt/ with the naming pattern:
clusters_<LATTICE_TYPE>.txt

Output columns (tab-separated):
1. Magnetic concentration
2. Average total clusters
3. Average cluster size (normalized)
4. Average spin-up clusters
5. Average spin-up cluster size (normalized)
6. Average spin-down clusters
7. Average spin-down cluster size (normalized)
8. Average percolating clusters
9. Average percolating cluster size (normalized)

## Visualization

Use the provided Python script to generate plots:
```bash
python scripts/make_plots.py
```

This creates:
- clusters_count.png: Number of clusters vs concentration
- cluster_size.png: Order parameter vs concentration

## Authors

- **Leonid Afremov** - Scientific supervisor
- **Nikita Legkikh** ([@oganechan](https://github.com/Oganechan)) - Implementation and development

## References
- Wolff cluster algorithm for Monte Carlo simulations
- Percolation theory in magnetic systems
- Phase transitions in dilute magnets
