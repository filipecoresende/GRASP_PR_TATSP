#include "parser.hpp"
#include "construction.hpp"
#include "datatypes.hpp"

void calculateTATSPcost(vector<int>& solution_tour, vector<Arc>& all_arcs, Graph& graph, double& solution_cost);

int main(int argc, char **argv) {

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    Graph graph;
    vector<Arc> all_arcs;
    vector<int> solution_tour;

    parser(filename, graph, all_arcs);


    PerturbationType pert_type = PerturbationType::ADDITIVE;

    double pert_parameter = 0.1;

    constructiveHeuristic(graph, all_arcs, solution_tour, pert_type, pert_parameter);

    double solution_cost;
    calculateTATSPcost(solution_tour, all_arcs, graph, solution_cost);

    cout << "the solution cost is " << solution_cost << endl;

    return 0;

}

void calculateTATSPcost(vector<int>& solution_tour, vector<Arc>& all_arcs, Graph& graph, double& solution_cost){

    double total_cost = 0;
    size_t n = solution_tour.size();
    for (size_t i = 0; i < n - 1; ++i){

        int from = solution_tour[i]; int to = solution_tour[i+1];
        int arc_idx = graph.adj[from][to];
        Arc current_arc = all_arcs[arc_idx];

        total_cost += current_arc.current_cost;

        for (auto& [target_idx, target_cost]: current_arc.targets)
            all_arcs[target_idx].current_cost = target_cost; 

    }

    //final arc

    int from = solution_tour[n-1]; int to = 0;
    int arc_idx = graph.adj[from][to];
    Arc current_arc = all_arcs[arc_idx];

    total_cost += current_arc.current_cost;

    for (Arc& arc: all_arcs)
            arc.current_cost = arc.original_cost;

    solution_cost = total_cost;

}

