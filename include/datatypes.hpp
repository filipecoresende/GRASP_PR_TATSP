#pragma once

#include <cstdlib>

#include <vector>
using namespace std;

struct Arc {
    int from;
    int to;
    double original_cost;
    double current_cost;
    vector<pair<int, double>> targets; //(target index, new cost for the target)
    vector<int> triggers; //trigger for the arc

};

struct Graph {
    vector<vector<int>> adj; //for each node there is a vector of arc indexes
};

struct Tour {
    vector<int> tour;
    double tour_cost;
    size_t depot_idx = 0;

    bool operator==(const Tour& other) const {
        const auto& a = tour;
        const auto& b = other.tour;

        if (tour_cost != other.tour_cost) return false;
        if (a.size() != b.size()) return false;
        if (a.empty()) return true;

        size_t n = a.size();

        bool same = true;
        for (size_t i = 0; i < n; ++i) {
            if (a[(depot_idx + i) % n] != b[(other.depot_idx + i) % n]){
                same = false;
                break;
            }
        }

        if (same) return true;
        
        return false;
    }

    bool operator!=(const Tour& other) const {
        return !(*this == other);
    }

};