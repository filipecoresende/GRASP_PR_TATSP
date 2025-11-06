#include <random>

#include "construction.hpp"


void additivePerturbation(vector<Arc>& all_arcs, double alpha){
    
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dist(-1.0, 1.0);
    
    for (Arc& arc: all_arcs){
        double factor = alpha * dist(gen);
        arc.current_cost += factor;
    }
};

void multiplicativePerturbation(vector<Arc>& all_arcs, double beta){
    
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);
    
    for (Arc& arc: all_arcs){
        double factor = beta * dist(gen);
        arc.current_cost *= factor;
    }
};

int solveTSP(const Graph& graph, const vector<Arc>& all_arcs, vector<int>& solution_tour) {

    // Get the number of nodes
    int n = graph.adj.size();
    if (n == 0) {
        cerr << "Error: Graph is empty." << endl;
        return -1;
    }

    // Get the number of arcs
    int num_arcs = all_arcs.size();

    // --- 1. Pre-processing ---
    // incoming_arcs[j] = vector of arc indices k that end at node j
    vector<vector<int>> incoming_arcs(n);

    // arc_map[{i, j}] = arc index k for arc (i, j)
    map<pair<int, int>, int> arc_map;

    for (int k = 0; k < num_arcs; ++k) {
        const Arc& arc = all_arcs[k];
        incoming_arcs[arc.to].push_back(k);
        arc_map[{arc.from, arc.to}] = k;
    }

    try {
        // --- 2. Model Setup ---
        GRBEnv env;
        env.set(GRB_IntParam_OutputFlag, 1); // Enable Gurobi output
        env.start();
        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "TSP");

        // --- Create Variables ---

        // x_k: binary variable, 1 if arc k is in the tour, 0 otherwise
        vector<GRBVar> x(num_arcs);
        for (int k = 0; k < num_arcs; ++k) {
            string var_name = "x_" + to_string(all_arcs[k].from) + "_" + to_string(all_arcs[k].to);
            x[k] = model.addVar(0.0, 1.0, all_arcs[k].current_cost, GRB_BINARY, var_name);
        }

        // u_i: continuous variable for node i (MTZ position)
        // We fix node 0 as the depot, so u[0] = 0
        vector<GRBVar> u(n);
        u[0] = model.addVar(0.0, 0.0, 0.0, GRB_CONTINUOUS, "u_0");
        for (int i = 1; i < n; ++i) {
            u[i] = model.addVar(1.0, (double)(n - 1), 0.0, GRB_CONTINUOUS, "u_" + to_string(i));
        }

        // Set objective: Minimize total cost
        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);


        // --- 3. Add Constraints ---

        // Constraint 1: Leave each node exactly once
        for (int i = 0; i < n; ++i) {
            GRBLinExpr expr = 0;
            for (int k : graph.adj[i]) {
                if (k != -1) 
                    expr += x[k];
            }
            model.addConstr(expr == 1, "leave_" + to_string(i));
        }

        // Constraint 2: Enter each node exactly once
        for (int j = 0; j < n; ++j) {
            GRBLinExpr expr = 0;
            for (int k : incoming_arcs[j]) {
                expr += x[k];
            }
            model.addConstr(expr == 1, "enter_" + to_string(j));
        }

        // Constraint 3: MTZ Subtour Elimination
        // u_i - u_j + (n-1) * x_ij <= n-2
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
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            
            // --- MODIFICATION ---
            // Store the optimal cost in the output variable
            double solution_cost = model.get(GRB_DoubleAttr_ObjVal);
            
            // Clear the solution tour vector
            solution_tour.clear();

            // Reconstruct the tour successor map
            map<int, int> successor;
            for (int k = 0; k < num_arcs; ++k) {
                if (x[k].get(GRB_DoubleAttr_X) > 0.5) {
                    successor[all_arcs[k].from] = all_arcs[k].to;
                }
            }

            // Build the tour vector starting from node 0
            int current_node = 0;
            for (int i = 0; i < n; ++i) { // n nodes in the tour
                solution_tour.push_back(current_node);
                current_node = successor[current_node];
            }
            
            // Optional: Print the tour
            cout << "Found optimal solution!" << endl;
            cout << "Total Cost: " << solution_cost << endl;
            cout << "Tour: 0";
            for (size_t i = 1; i < solution_tour.size(); ++i) {
                cout << " -> " << solution_tour[i];
            }
            cout << " -> 0" << endl;
            // --- End Modification ---

        } else {
            cerr << "No optimal solution found. Gurobi status: "
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

void constructiveHeuristic(const Graph& graph, vector<Arc>& all_arcs, vector<int>& solution_tour, PerturbationType pert_type,  double pert_parameter){

    switch (pert_type){
    case PerturbationType::ADDITIVE:
        additivePerturbation(all_arcs, pert_parameter);
        break;
    case PerturbationType::MULTIPLICATIVE:
        multiplicativePerturbation(all_arcs, pert_parameter);
        break;
    case PerturbationType::NONE:
        break;
    }

    solveTSP(graph, all_arcs, solution_tour);

    for (Arc& arc: all_arcs)
            arc.current_cost = arc.original_cost;
}