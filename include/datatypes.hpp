#pragma once

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
