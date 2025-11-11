#pragma once

#include "datatypes.hpp"

void localSearch(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph);

bool twoOpt(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph);

Tour applyTwoOptMove(Tour& tour, Graph& graph, int arc1_source, int arc2_source);

Tour applySwapTwoMove(Tour& tour, int idx1, int idx2);

Tour applyRelocateMove(Tour& tour, int idx1, int idx2);

bool swapTwo(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph);

bool relocate(Tour& current_solution, vector<Arc>& all_arcs, Graph& graph);

bool checkTourFeasibility(Tour& tour, const Graph& graph);