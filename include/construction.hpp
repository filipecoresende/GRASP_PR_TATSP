#pragma once

#include "datatypes.hpp"
#include "gurobi_c++.h"
#include <iostream>
#include <map>
#include <string>

enum class PerturbationType {
    NONE,           
    ADDITIVE,
    MULTIPLICATIVE
};

// alpha is the perturbation parameter for additive perturbation
void additivePerturbation(vector<Arc>& all_arcs, double alpha);

// beta is the perturbation parameter for multiplicative perturbation
void multiplicativePerturbation(vector<Arc>& all_arcs, double beta);

//solves asymmetric TSP and stores the solution at solution_tour
int solveTSP(const Graph& graph, const vector<Arc>& all_arcs, vector<int>& solution_tour);

void constructiveHeuristic(const Graph& graph, vector<Arc>& all_arcs, vector<int>& solution_tour, PerturbationType pert_type,  double pert_parameter);


