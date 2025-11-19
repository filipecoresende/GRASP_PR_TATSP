# GRASP+PR Metaheuristic for the Trigger Arc Traveling Salesman Problem (TA-TSP)

This repository implements a **Greedy Randomized Adaptive Search Procedure (GRASP)** metaheuristic designed to solve the **Trigger Arc Traveling Salesman Problem (TA-TSP)** based on the work of [Soler & de Lambertye, 2025](https://arxiv.org/abs/2508.08477). I extended their GRASP with **Path Relinking** as an intensification step at the end of each GRASP iteration ([Laguna et al., 2025](https://doi.org/10.1016/j.ejor.2025.02.022)).

The TA-TSP is a variant of the classical TSP in which the cost of traversing certain arcs (*target arcs*) is dynamically modified if a specific prerequisite arc (*trigger arc*) is traversed earlier in the tour.

---

## 🚀 Algorithm Overview

The core algorithm uses a GRASP framework that incorporates the following components:

### MIP-Based Construction
Generates diverse initial solutions by transforming the TA-TSP into a series of perturbed classical TSP instances, leveraging a MIP solver’s primal heuristics. I implemented MIP-Based Random Perturbation according to [Soler & de Lambertye, 2025](https://arxiv.org/abs/2508.08477).

### Multi-Neighborhood Local Search
Systematically improves the constructed solution using standard neighborhood operators: **2-Opt**, **Swap**, and **Relocate**, until a local optimum is reached.

### Path Relinking
An intensification step that explores the solution space between the current local optimum and a set of elite solutions (the pool) to find higher-quality solutions. Specifically, I implemented Mixed Path Relinking ([Laguna et al., 2025](https://doi.org/10.1016/j.ejor.2025.02.022)).

---

## 🛠️ Usage

The program is a command-line executable that requires four mandatory arguments to define the instance, the construction heuristic’s perturbation, and the runtime limit. It also supports an optional flag to activate Path Relinking.

### Building the Executable

The project uses a standard `Makefile`. To compile the C/C++ source code and generate the executable, open your terminal in the repository root directory and run:

```bash
make
```
This command will create the executable file named `solver` in the current directory.

### Command Line Syntax

```bash
./solver <filename> <pert_type> <pert_param> <time_limit> [-pr]
```

### Argument Details

| Argument       | Description                                                                  | Required? | Example          |
|----------------|------------------------------------------------------------------------------|-----------|------------------|
| `<filename>`   | Path to the TA-TSP instance file.                                            | Yes       | instance_01.txt  |
| `<pert_type>`  | Defines the perturbation strategy applied to arc costs in construction.      | Yes       | 1                |
| `<pert_param>` | Non-negative parameter (α or β) controlling perturbation magnitude.          | Yes       | 0.1              |
| `<time_limit>` | Maximum runtime (seconds).                                                   | Yes       | 60.0             |
| `-pr`          | Optional flag enabling Path Relinking (elite pool size = 10).                | No        | -pr              |

---

## Perturbation Types

The `<pert_type>` argument must be an integer:

| Value | Name               | Description                                                                                   |
|-------|--------------------|-----------------------------------------------------------------------------------------------|
| 0     | **NONE**           | No perturbation (pure greedy/deterministic construction).                                     |
| 1     | **ADDITIVE**       | Costs modified using additive noise: `c'_{ij} = c_{ij} + α * U(-1,1)`                          |
| 2     | **MULTIPLICATIVE** | Costs scaled by a random factor: `c'_{ij} = c_{ij} * (β * U(0,1))`                            |

---

## Example Execution

To run the algorithm on `large_instance.txt` with Additive Perturbation (type 1), parameter 0.1, 120-second limit, and Path Relinking enabled:

```bash
./solver large_instance.txt 1 0.1 120.0 -pr
```

---

## 📋 Output Format

Upon completion or when the time limit is reached, the program prints the final results:

```
Search Finished.
Total Iterations: 1542
Best Tour Cost: 478.9152
Best Tour Path: 0,15,3,8,1,10,12,5,2,11,4,9,13,6,14,7
Best Tour Found at Iteration: 84
```

## References

- Soler, J. S., & de Lambertye, G. (2025). *A Fast GRASP Metaheuristic for the Trigger Arc TSP with MIP-Based Construction and Multi-Neighborhood Local Search*. arXiv:2508.08477. [https://arxiv.org/abs/2508.08477](https://arxiv.org/abs/2508.08477)

- Laguna, M., Martí, R., Martínez-Gavara, A., Pérez-Peló, S., & Resende, M. G. C. (2025). Greedy Randomized Adaptive Search Procedures with Path Relinking. An analytical review of designs and implementations. European Journal of Operational Research, 327(3), 717–734. https://doi.org/10.1016/j.ejor.2025.02.022