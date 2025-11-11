#pragma once

#include "datatypes.hpp"
#include "gurobi_c++.h"
#include <iostream>
#include <map>
#include <string>
#include <random>

enum class PerturbationType {
    NONE,           
    ADDITIVE,
    MULTIPLICATIVE
};

// alpha is the perturbation parameter for additive perturbation
void additivePerturbation(vector<Arc>& all_arcs, double alpha, mt19937& gen);

// beta is the perturbation parameter for multiplicative perturbation
void multiplicativePerturbation(vector<Arc>& all_arcs, double beta, mt19937& gen);

//solves asymmetric TSP and stores the solution at solution_tour
int solveTSP(const Graph& graph, const vector<Arc>& all_arcs, Tour& solution_tour);

void constructiveHeuristic(Graph& graph, vector<Arc>& all_arcs, Tour& solution_tour, 
                           PerturbationType pert_type, double pert_parameter, mt19937& gen);

void calculateTATSPcost(Tour& solution_tour, vector<Arc>& all_arcs, Graph& graph);

// modified mod to handle negative int properly
int modified_mod(int a, int b);