# Finding 3D Clusters In Diluted Magnets

[![Conference Paper](https://img.shields.io/badge/Conference-Regional%20Scientific%20Conference%202025-blue.svg)](https://www.dvfu.ru/upload/medialibrary/d9c/3xq7x2qlnvvq405fixx2ss58wj9vwg9f/%D1%81%D0%B1%D0%BE%D1%80%D0%BD%D0%B8%D0%BA%20%D0%A0%D0%9D%D0%9F%D0%9A%202025.pdf)

## Description

Numerical investigation of cluster distribution near the percolation transition in dilute magnetic systems. The study focuses on concentration phase transitions in cubic crystal lattices with randomly distributed magnetic spin moments.

The research examines how the number of magnetic clusters and cluster order parameter depend on magnetic atom concentration in three types of cubic crystal lattices:
- **Simple Cubic (SC)** 
- **Body-Centered Cubic (BCC)**  
- **Face-Centered Cubic (FCC)**

The simulation uses Wolf Monte Carlo algorithm to eliminate thermal fluctuations ($k_BT \ll J$) and Hoshen-Kopelman algorithm for cluster identification. Systems with linear size $L = 30$ ($N = L^3$ nodes) were studied with magnetic concentration varying from 0 to 1 in steps of $\Delta p = 0.01$.

## Key Features

- **Multi-lattice Support**: Comparative analysis across SC, BCC, and FCC crystal structures
- **Percolation Detection**: Identification of system-spanning clusters
- **Cluster Statistics**: Comprehensive tracking of:
  - Total cluster counts and sizes
  - Spin-up and spin-down clusters  
  - Percolating cluster formation
  - Cluster order parameter evolution

- **Monte Carlo Methods**: Implementation of Wolff cluster algorithm
- **Cluster find algorithm**: Implementation of Hoshen-Kopelman algorithm with union-find optimization
- **Physical Modeling**: Exchange interaction energy $E = -J\sum_{\{i,j\}} s_i s_j$ with $J = 1$
- **Configurable Parameters**: Flexible simulation settings via JSON configuration

## Build Instructions
1. Clone the repository:
```bash
git clone https://github.com/Oganechan/concentation_phase_transition
cd Clusters3DNearPercolation
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
- **Nikita Legkikh** (formerly Timofeev) ([@oganechan](https://github.com/Oganechan)) - Implementation and development

## Conference Publication

This work was presented at the **Regional Scientific-Practical Conference of Students, Postgraduates and Young Scientists in Natural Sciences** (Vladivostok, April 15-30, 2025):

[Full Conference Proceedings](https://www.dvfu.ru/upload/medialibrary/d9c/3xq7x2qlnvvq405fixx2ss58wj9vwg9f/%D1%81%D0%B1%D0%BE%D1%80%D0%BD%D0%B8%D0%BA%20%D0%A0%D0%9D%D0%9F%D0%9A%202025.pdf) (see page 402)

[Conference Abstract](ТЕОРЕТИЧЕСКАЯ_Тимофеев.docx) - Timofeev N.V. "Cluster Distribution Near Percolation Transition"
