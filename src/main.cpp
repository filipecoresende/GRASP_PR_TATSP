#include "parser.hpp"
#include "construction.hpp"
#include "datatypes.hpp"
#include "local_search.hpp"
#include "path_relinking.hpp"

#include <chrono>
#include <vector>   // NEW: Include for vector
#include <string>   // NEW: Include for string processing

using namespace std;

const int POOL_SIZE = 10;


int main(int argc, char **argv) {
    // 1. Argument Parsing and Validation

    // --- NEW: Argument parsing logic ---
    bool path_flag = false;
    vector<string> positional_args;

    // Loop through all command-line arguments (starting from 1, skipping executable name)
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-pr") {
            path_flag = true;
        } else {
            // This is not a flag we recognize, assume it's a positional argument
            positional_args.push_back(arg);
        }
    }
    // --- END NEW ---


    // MODIFIED: Check if we have the correct number of POSITIONAL arguments
    if (positional_args.size() < 4) {
        // MODIFIED: Updated usage message
        cout << "Usage: " << argv[0] << " <filename> <pert_type> <pert_param> <time_limit> [-pr]" << endl;
        cout << "Perturbation Types: 0 (NONE), 1 (ADDITIVE), 2 (MULTIPLICATIVE)" << endl;
        cout << "Optional flag: -pr (to enable Path Relinking)" << endl; // NEW
        return 1;
    }

    // MODIFIED: Parse arguments from our new vector
    string filename = positional_args[0];
    int pert_type_int = stoi(positional_args[1]);
    double pert_parameter = stod(positional_args[2]);
    double time_limit = stod(positional_args[3]);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file '" << filename << "'. Check if the file exists and is readable." << endl;
        return 1;
    }
    file.close();

    if (pert_parameter < 0) {
        cerr << "Error: Perturbation parameter must be non-negative." << endl;
        return 1;
    }
    if (time_limit <= 0) {
        cerr << "Error: Time limit must be positive." << endl;
        return 1;
    }

    PerturbationType pert_type;
    string pert_type_str;

    switch (pert_type_int) {
        case 0: 
            pert_type = PerturbationType::NONE;
            pert_type_str = "NONE"; 
            break;
        case 1: 
            pert_type = PerturbationType::ADDITIVE;
            pert_type_str = "ADDITIVE"; 
            break;
        case 2: 
            pert_type = PerturbationType::MULTIPLICATIVE;
            pert_type_str = "MULTIPLICATIVE"; 
            break;
        default:
            cerr << "Error: Invalid perturbation type. Use 0, 1, or 2." << endl;
            return 1;
    }

    // 2. Initialization
    Graph graph;
    vector<Arc> all_arcs;
    parser(filename, graph, all_arcs);

    // vector<Tour> pool;
    // pool.reserve(2000);

    cout << "Starting search for " << time_limit << " seconds..." << endl;
    cout << "Configuration: Type=" << pert_type_str << ", Param=" << pert_parameter << endl;
    cout << "Path Relinking: " << (path_flag ? "ENABLED" : "DISABLED") << endl; // NEW: Report PR status

    // 3. Search Loop
    auto start_time = chrono::steady_clock::now();

    random_device rd;

    mt19937 gen_grasp(rd());
    mt19937 gen_pr(rd());

    
    int best_iteration = 0;

    Tour best_tour;
    best_tour.tour_cost = numeric_limits<double>::infinity();
    int count = 0;
    vector<Tour> pool;
    pool.reserve(POOL_SIZE);
    while (true) {
        // Use duration<double> for precise comparison against the double time_limit
        chrono::duration<double> elapsed = chrono::steady_clock::now() - start_time;
        if (elapsed.count() >= time_limit) {
            break;
        }

        Tour solution_tour;
        // MODIFICATION: Capture the return status from constructiveHeuristic
        int construct_status = constructiveHeuristic(graph, all_arcs, solution_tour, pert_type, pert_parameter, gen_grasp);
        
        // MODIFICATION: If construction failed, skip local search and recording
        if (construct_status == -1) {
            // This iteration failed to find a feasible solution, skip to the next
            count++;
            continue;
        }

        localSearch(solution_tour, all_arcs, graph);

        if (path_flag) {
            if (pool.size() < POOL_SIZE) {
                pool.push_back(solution_tour);
            }
            
            else {
                Tour guidingSolution = pool[selectGuidingSolution(solution_tour, pool, gen_pr)];
                solution_tour = mixedPathRelinking(solution_tour, guidingSolution, graph, all_arcs);
                localSearch(solution_tour, all_arcs, graph);
                updatePool(solution_tour, pool);
            }

        }


        if (solution_tour.tour_cost < best_tour.tour_cost) {
            best_tour = solution_tour;
            best_iteration = count;
        }

        count++;
    }




    // 4. Results Reporting
    if (best_iteration == 0) {
        cout << "No solutions found within time limit." << endl;
        return 0;
    }

    cout << "------------------------------------------------" << endl;
    cout << "Search Finished." << endl;
    cout << "Total Iterations: " << count << endl;
    cout << "Best Tour Cost: " << best_tour.tour_cost << endl;
    cout << "Best Tour Path: ";

    size_t i = 0;
    // MODIFICATION: Add check for non-empty tour before printing
    if (!best_tour.tour.empty()) {
        while (i < best_tour.tour.size() - 1){
            cout << best_tour.tour[i] << ",";
            i++;
        }
        cout << best_tour.tour[i] << endl;
    } else {
        cout << "[Empty Tour]" << endl;
    }
    
    cout << "Best Tour Found at Iteration: " << best_iteration << endl;
    cout << endl;
            
    return 0;
}