#include "local_search.hpp"
#include "construction.hpp"



void localSearch(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph) {

    bool improved = true;
    
    while (improved){

        improved = false;
        if (twoOpt(current_solution, all_arcs, graph))
            improved = true;
        else if (swapTwo(current_solution, all_arcs, graph))
            improved = true;
        else if (relocate(current_solution, all_arcs, graph))
            improved = true;
    }

    
}

bool checkTourFeasibility(Tour& tour, const Graph& graph) {


    // cout << "tour to be checked: ";
    // for (int i: tour.tour) cout << i << ",";
    // cout << endl;

    size_t n = tour.tour.size();
    
    for (size_t i = 0; i < n - 1; ++i) {

        if (graph.adj[tour.tour[i]][tour.tour[i+1]] == -1){
            // cout << "infeasible\n";
            return false;}
        
    }

    //final arc

    if (graph.adj[tour.tour[n-1]][tour.tour[0]] == -1){
                    // cout << "infeasible\n";
        return false;}

                // cout << "it's feasible\n";


    return true;
}

bool twoOpt(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph ) {

    int n = graph.adj.size(); // number of vertices
    Tour new_tour;
    for (int i = 0; i < n; i++) {
        for (int j = (i + 2) % n; j != modified_mod(i - 1, n); j = (j + 1) % n) {
            new_tour = applyTwoOptMove(current_solution, graph, i, j);
            if (checkTourFeasibility(new_tour, graph)) {
                calculateTATSPcost(new_tour, all_arcs, graph);
                if (new_tour.tour_cost < current_solution.tour_cost) {
                    current_solution = new_tour;
                    return true;
                }
            }
        }  
    }

    return false;

}

Tour applyTwoOptMove(Tour& tour, Graph& graph, int arc1_source, int arc2_source) {

    int n = graph.adj.size(); // number of vertices
    int idx1 = (arc1_source + 1) % n;
    int idx2 = arc2_source;
    //the tour section from idx1 to idx2 has its order reversed

    Tour new_tour(tour);

    int i = idx1, j = idx2;

    while (i != (idx2 + 1) % n){
        new_tour.tour[i] = tour.tour[j];
        if (new_tour.tour[i] == 0)
            new_tour.depot_idx = i;
        i = (i + 1) % n;
        j = modified_mod(j - 1, n);  
    }  

    // cout << new_tour.depot_idx << endl;

    return new_tour;
    
}

Tour applySwapTwoMove(Tour& tour, int idx1, int idx2) {

    Tour new_tour(tour);

    if (new_tour.tour[idx1] == 0)
        new_tour.depot_idx = idx2;
    else if (new_tour.tour[idx2] == 0)
        new_tour.depot_idx = idx1;

    int aux = new_tour.tour[idx1];

    new_tour.tour[idx1] = new_tour.tour[idx2];

    new_tour.tour[idx2] = aux;

    return new_tour;
    
}

bool swapTwo(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph){

    int n = graph.adj.size(); // number of vertices
    Tour new_tour;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            new_tour = applySwapTwoMove(current_solution, i, j);
            if (checkTourFeasibility(new_tour, graph)) {
                calculateTATSPcost(new_tour, all_arcs, graph);
                if (new_tour.tour_cost < current_solution.tour_cost) {
                    current_solution = new_tour;
                    return true;
                }
            }
        }  
    }

    return false;

}


bool relocate(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph){

    int n = graph.adj.size(); // number of vertices
    Tour new_tour;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j - 1 || i == j || i == j + 1) 
                continue;
            new_tour = applyRelocateMove(current_solution, i, j);
            if (checkTourFeasibility(new_tour, graph)) {
                calculateTATSPcost(new_tour, all_arcs, graph);
                if (new_tour.tour_cost < current_solution.tour_cost) {
                    current_solution = new_tour;
                    return true;
                }
            }
        }  
    }

    return false;

}

Tour applyRelocateMove(Tour& tour, int initial_idx, int new_idx){

    Tour new_tour(tour);
    int current_depot = new_tour.depot_idx;

    if (initial_idx < new_idx) {
        int aux = new_tour.tour[initial_idx];
        for (int i = initial_idx; i < new_idx; i++)
            new_tour.tour[i] = new_tour.tour[i+1];
        new_tour.tour[new_idx] = aux;

        if (current_depot == initial_idx)
             new_tour.depot_idx = new_idx; 
        else if (current_depot > initial_idx && current_depot <= new_idx)
             new_tour.depot_idx--;
    }

    else {
        int aux = new_tour.tour[initial_idx];
        for (int i = initial_idx; i > new_idx; i--)
            new_tour.tour[i] = new_tour.tour[i-1];
        new_tour.tour[new_idx] = aux;

        if (current_depot == initial_idx)
             new_tour.depot_idx = new_idx; 
        else if (current_depot >= new_idx && current_depot < initial_idx)
             new_tour.depot_idx++;
    }

    return new_tour;

}
