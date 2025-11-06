#include "parser.hpp"

void parser(const string& filename, Graph& graph, vector<Arc>& all_arcs) {
    
    // --- Open the file ---
    ifstream infile(filename);

    // --- Check if file opened successfully ---
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return; 
    }

    int num_nodes, num_arcs, num_relations;

    // 1. Read the Header Line (from file)
    infile >> num_nodes >> num_arcs >> num_relations; 

    // 2. Initialize Your Objects
    graph.adj.resize(num_nodes, vector<int>(num_nodes, -1));

    all_arcs.resize(num_arcs);

    // 3. Read the |A| Arc Lines (from file)
    for (int i = 0; i < num_arcs; ++i) {
        int arc_idx, from_node, to_node;
        double cost;
        
        infile >> arc_idx >> from_node >> to_node >> cost;

        all_arcs[arc_idx].from = from_node;
        all_arcs[arc_idx].to = to_node;
        all_arcs[arc_idx].original_cost = cost;
        all_arcs[arc_idx].current_cost = cost; 
        
        graph.adj[from_node][to_node] = arc_idx;
    }

    // 4. Read the |R| Relation Lines (from file)
    for (int i = 0; i < num_relations; ++i) {
        int rel_idx;
        int trigger_arc_idx, trigger_from, trigger_to;
        int target_arc_idx, target_from, target_to;
        double new_cost;
        
        infile >> rel_idx 
               >> trigger_arc_idx >> trigger_from >> trigger_to
               >> target_arc_idx >> target_from >> target_to
               >> new_cost;
            
        all_arcs[trigger_arc_idx].targets.push_back({target_arc_idx, new_cost});
        all_arcs[target_arc_idx].triggers.push_back(trigger_arc_idx);
    }

    // --- Good practice: close the file when done ---
    infile.close();

    cout << "Successfully read " << num_nodes << " nodes, "
         << num_arcs << " arcs, and "
         << num_relations << " relations from " << filename << "." << endl;

    
    return;
}