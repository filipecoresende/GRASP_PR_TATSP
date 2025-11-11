

#include "construction.hpp"

void additivePerturbation(vector<Arc>& all_arcs, double alpha, mt19937& gen){
    uniform_real_distribution<> dist(-1.0, 1.0);
    for (Arc& arc: all_arcs){
        double factor = alpha * dist(gen);
        arc.current_cost += factor;
    }
}

void multiplicativePerturbation(vector<Arc>& all_arcs, double beta, mt19937& gen){
    uniform_real_distribution<> dist(0.0, 1.0);
    for (Arc& arc: all_arcs){
        double factor = beta * dist(gen);
        arc.current_cost *= factor;
    }
}

int solveTSP(const Graph& graph, const vector<Arc>& all_arcs, Tour& solution_tour) {

    // Get the number of nodes
    int n = graph.adj.size();
    if (n == 0) {
        cerr << "Error: Graph is empty." << endl;
        return -1;
    }

    // Get the number of arcs
    int num_arcs = all_arcs.size();

    // --- 1. Pre-processing ---
    vector<vector<int>> incoming_arcs(n);
    map<pair<int, int>, int> arc_map;

    for (int k = 0; k < num_arcs; ++k) {
        const Arc& arc = all_arcs[k];
        incoming_arcs[arc.to].push_back(k);
        arc_map[{arc.from, arc.to}] = k;
    }

    try {
        // --- 2. Model Setup ---
        GRBEnv env(true);  // Create environment in "empty" state — no automatic output
        env.set(GRB_IntParam_OutputFlag, 0); // Disable all Gurobi output
        env.set(GRB_IntParam_Threads, 1);    // Determinism
        env.set(GRB_IntParam_Seed, 42);      // Determinism
        env.start(); 

        // --- MODIFICATION 1: Enforce Determinism ---
        // Force Gurobi to use only one thread
        env.set(GRB_IntParam_Threads, 1);
        // Set Gurobi's internal random seed
        env.set(GRB_IntParam_Seed, 42); 

        env.start();
        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "TSP");

        // Set a 2-second time limit
        model.getEnv().set(GRB_DoubleParam_TimeLimit, 2.0);

        // --- Create Variables ---
        vector<GRBVar> x(num_arcs);
        for (int k = 0; k < num_arcs; ++k) {
            string var_name = "x_" + to_string(all_arcs[k].from) + "_" + to_string(all_arcs[k].to);
            x[k] = model.addVar(0.0, 1.0, all_arcs[k].current_cost, GRB_BINARY, var_name);
        }

        vector<GRBVar> u(n);
        u[0] = model.addVar(0.0, 0.0, 0.0, GRB_CONTINUOUS, "u_0");
        for (int i = 1; i < n; ++i) {
            u[i] = model.addVar(1.0, (double)(n - 1), 0.0, GRB_CONTINUOUS, "u_" + to_string(i));
        }

        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

        // --- 3. Add Constraints ---
        for (int i = 0; i < n; ++i) {
            GRBLinExpr expr = 0;
            for (int k : graph.adj[i]) {
                if (k != -1) 
                    expr += x[k];
            }
            model.addConstr(expr == 1, "leave_" + to_string(i));
        }

        for (int j = 0; j < n; ++j) {
            GRBLinExpr expr = 0;
            for (int k : incoming_arcs[j]) {
                expr += x[k];
            }
            model.addConstr(expr == 1, "enter_" + to_string(j));
        }

        for (int i = 1; i < n; ++i) {
            for (int j = 1; j < n; ++j) {
                if (i == j) continue;
                auto it = arc_map.find({i, j});
                if (it != arc_map.end()) {
                    int k = it->second;
                    model.addConstr(u[i] - u[j] + (n - 1) * x[k] <= (n - 2),
                                    "mtz_" + to_string(i) + "_" + to_string(j));
                }
            }
        }

        // --- 4. Solve Model ---
        model.optimize();

        // --- 5. Solution Retrieval ---
        // --- MODIFICATION 2: Check for ANY solution, not just OPTIMAL ---
        if (model.get(GRB_IntAttr_SolCount) > 0) {
            
            // Clear the solution tour vector
            solution_tour.tour.clear();

            // Reconstruct the tour successor map
            map<int, int> successor;
            for (int k = 0; k < num_arcs; ++k) {
                if (x[k].get(GRB_DoubleAttr_X) > 0.5) {
                    successor[all_arcs[k].from] = all_arcs[k].to;
                }
            }

            // Build the tour vector starting from node 0
            int current_node = 0;
            solution_tour.tour.reserve(n);
            for (int i = 0; i < n; ++i) { // n nodes in the tour
                solution_tour.tour.push_back(current_node);
                current_node = successor[current_node];
            }

        } else {
            // This now means no feasible solution was found in the time limit
            cerr << "No feasible solution found by Gurobi. Status: "
                 << model.get(GRB_IntAttr_Status) << endl;
            return -1;
        }

    } catch (GRBException e) {
        cerr << "Gurobi Error code = " << e.getErrorCode() << endl;
        cerr << e.getMessage() << endl;
        return -1;
    } catch (...) {
        cerr << "An unknown error occurred." << endl;
        return -1;
    }

    return 0; // Success
}

void constructiveHeuristic(Graph& graph, vector<Arc>& all_arcs, Tour& solution_tour, 
                           PerturbationType pert_type, double pert_parameter, mt19937& gen) {
    switch (pert_type){
    case PerturbationType::ADDITIVE:
        additivePerturbation(all_arcs, pert_parameter, gen);
        break;
    case PerturbationType::MULTIPLICATIVE:
        multiplicativePerturbation(all_arcs, pert_parameter, gen);
        break;
    case PerturbationType::NONE:
        break;
    }

    solveTSP(graph, all_arcs, solution_tour);

    for (Arc& arc: all_arcs)
            arc.current_cost = arc.original_cost;

    calculateTATSPcost(solution_tour, all_arcs, graph);

    
}

void calculateTATSPcost(Tour& solution_tour, vector<Arc>& all_arcs, Graph& graph){

    double total_cost = 0;

    size_t n = solution_tour.tour.size();

    int i = solution_tour.depot_idx;
   
    do {

        int from = solution_tour.tour[i]; int to = solution_tour.tour[(i+1) % n];
        int arc_idx = graph.adj[from][to];


        Arc current_arc = all_arcs[arc_idx];



        total_cost += current_arc.current_cost;

        for (auto& [target_idx, target_cost]: current_arc.targets)
            all_arcs[target_idx].current_cost = target_cost;

        i = (i + 1) % n;


    }  while (solution_tour.tour[i] != 0);

    for (Arc& arc: all_arcs)
            arc.current_cost = arc.original_cost;

    solution_tour.tour_cost = total_cost;

}

int modified_mod(int a, int b) {
    int r = a % b;
    if (r < 0) r += b;
    return r;
}

