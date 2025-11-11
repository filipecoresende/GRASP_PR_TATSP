#include "parser.hpp"
#include "construction.hpp"
#include "datatypes.hpp"
#include "local_search.hpp"

using namespace std;

int main(int argc, char **argv) {
    // 1. Argument Parsing and Validation
    if (argc < 5) {
        cout << "Usage: " << argv[0] << " <filename> <pert_type> <pert_param> <time_limit>" << endl;
        cout << "Perturbation Types: 0 (NONE), 1 (ADDITIVE), 2 (MULTIPLICATIVE)" << endl;
        return 1;
    }

    string filename = argv[1];
    int pert_type_int = stoi(argv[2]);
    double pert_parameter = stod(argv[3]);
    double time_limit = stod(argv[4]);

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

    // 3. Search Loop
    auto start_time = chrono::steady_clock::now();

    unsigned int seed = 42;
    mt19937 gen(seed);
    
    int best_iteration = -1;
    // double best_cost = numeric_limits<double>::infinity();

    Tour best_tour;
    best_tour.tour_cost = numeric_limits<double>::infinity();
    int count = 0;
    while (true) {
        // Use duration<double> for precise comparison against the double time_limit
        chrono::duration<double> elapsed = chrono::steady_clock::now() - start_time;
        if (elapsed.count() >= time_limit) {
            break;
        }

        Tour solution_tour;
        // MODIFICATION: Capture the return status from constructiveHeuristic
        int construct_status = constructiveHeuristic(graph, all_arcs, solution_tour, pert_type, pert_parameter, gen);
        
        // MODIFICATION: If construction failed, skip local search and recording
        if (construct_status == -1) {
            // This iteration failed to find a feasible solution, skip to the next
            count++;
            continue;
        }

        localSearch(solution_tour, all_arcs, graph);
        
        // pool.push_back(solution_tour);

        if (solution_tour.tour_cost < best_tour.tour_cost) {
            best_tour = solution_tour;
            best_iteration = count;
        }

        count++;
    }

    // 4. Results Reporting
    if (best_iteration == -1) {
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