#include "path_relinking.hpp"
#include "local_search.hpp"
#include "construction.hpp"

#include <algorithm>
#include <random>

void applyPathRelinkingMove(Tour& current_solution, Tour& guiding_solution, Graph& graph, vector<Arc>& all_arcs) {

    pair<int, int> bestSwap(-1, -1);
    pair<int, int> emergencySwap(-1, -1);

    double bestSwapCost = numeric_limits<double>::infinity();
    int n = current_solution.tour.size();

    size_t depot1_idx = current_solution.depot_idx;
    size_t depot2_idx = guiding_solution.depot_idx;

    Tour temp;
    
    for (int i = 0; i < n - 1; i++){
        for (int j = i + 1; j < n; j++){

            size_t i1 = (depot1_idx + i) % n ; size_t i2 = (depot2_idx + i) % n; 
            size_t j1 = (depot1_idx + j) % n ; size_t j2 = (depot2_idx + j) % n; 

            if (current_solution.tour[i1] == guiding_solution.tour[j2] || current_solution.tour[j1] == guiding_solution.tour[i2]){
                emergencySwap = make_pair(i1, j1);
                temp = applySwapTwoMove(current_solution, i1, j1);
                if (checkTourFeasibility(temp, graph)) {
                    calculateTATSPcost(temp, all_arcs, graph);
                    if (temp.tour_cost < bestSwapCost) {
                        bestSwap = make_pair(i1,j1);
                        bestSwapCost = temp.tour_cost;
                    }
                }
            }
        }
    }

    if (bestSwap != make_pair(-1, -1)){
        current_solution = applySwapTwoMove(current_solution, bestSwap.first, bestSwap.second);
        current_solution.tour_cost = bestSwapCost;
    }
    else {
        current_solution = applySwapTwoMove(current_solution, emergencySwap.first, emergencySwap.second);
    }
    
}

Tour mixedPathRelinking(Tour solution1, Tour solution2, Graph& graph, vector<Arc>& all_arcs) {

    Tour best_solution = (solution1.tour_cost < solution2.tour_cost) ? solution1 : solution2;

    while (calculateHammingDistance(solution1, solution2) > 0) {
        applyPathRelinkingMove(solution1, solution2, graph, all_arcs);

        if (checkTourFeasibility(solution1, graph))
            if (solution1.tour_cost < best_solution.tour_cost)
                best_solution = solution1;
        
        swap(solution1, solution2);
    }

    return best_solution;
        
}

int calculateHammingDistance(Tour& solution1, Tour& solution2) {

    size_t n = solution1.tour.size();
    int distance = 0;
    for (size_t i = 1; i < n; ++i) {
        size_t i1 = (solution1.depot_idx + i) % n; 
        size_t i2 = (solution2.depot_idx + i) % n;
        if (solution1.tour[i1] != solution2.tour[i2])
            ++distance;
    }

    return distance;
}

int selectGuidingSolution(Tour& initial_solution, std::vector<Tour>& pool, mt19937& gen_pr) {

    // Step 1: compute weights
    std::vector<double> weights(pool.size());
    for (size_t i = 0; i < pool.size(); ++i) {
        weights[i] = calculateHammingDistance(initial_solution, pool[i]);
    }

    // Step 2: compute total weight
    double totalWeight = accumulate(weights.begin(), weights.end(), 0.0);

    // Step 3: generate random number in [0, totalWeight)
    uniform_real_distribution<> dist(0.0, totalWeight);
    double r = dist(gen_pr);

    // Step 4: select based on cumulative weight
    double cumulative = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
        cumulative += weights[i];
        if (r < cumulative)
            return i;
    }

    return pool.size() - 1; // fallback, should rarely happen due to floating-point precision
}

int selectMostSimilarTour(Tour& new_solution, vector<Tour>& pool) {

    int idx = 0;
    int bestDistance = new_solution.tour.size() + 1;

    for (size_t i = 0; i < pool.size(); i++){
        int distance = calculateHammingDistance(new_solution, pool[i]);
        if (distance < bestDistance){
            bestDistance = distance;
            idx = i;
        }
    }

    return idx;
    
}

int selectSimilarWorseTour(Tour& new_solution, vector<Tour>& pool) {
    int idx_to_remove = -1;
    int bestDistance = numeric_limits<int>::max();

    for (size_t i = 0; i < pool.size(); i++){
        // Check if the pool's solution is strictly worse (higher cost)
        if (pool[i].tour_cost > new_solution.tour_cost) {
            int distance = calculateHammingDistance(new_solution, pool[i]);
            if (distance < bestDistance){
                bestDistance = distance;
                idx_to_remove = i;
            }
        }
    }
    return idx_to_remove; // Will be -1 if no tour is strictly worse
}

void updatePool(Tour& current_solution, vector<Tour>& pool) {

    double maxCost = 0; 
    double minCost = numeric_limits<double>::infinity();

    for (size_t i = 0; i < pool.size(); i++){
        double current_cost = pool[i].tour_cost;
        if (current_cost > maxCost) {
            maxCost = current_cost;
        }
        if (current_cost < minCost) {
            minCost = current_cost;
        }
    }

    // Case 1: New solution is a "new best" (better than the best in the pool)
    if (current_solution.tour_cost < minCost) {
        // Find the most similar tour to replace. Since all are worse,
        // we can just use your original `selectMostSimilarTour`.
        int idx = selectMostSimilarTour(current_solution, pool);
        pool[idx] = current_solution;
        return;
    }

    // Case 2: New solution is an "improving" solution (but not a new best)
    if (current_solution.tour_cost < maxCost) {
        
        // Your diversity check (this is a good, valid strategy)
        int n = current_solution.tour.size();
        int threshold = ceil(0.6*n);
        for (size_t i = 0; i < pool.size(); i++){
            if (calculateHammingDistance(current_solution, pool[i]) < threshold)
                return; // Too similar, don't add
        }
        
        int idx_to_replace = selectSimilarWorseTour(current_solution, pool);

        pool[idx_to_replace] = current_solution;
    }

}