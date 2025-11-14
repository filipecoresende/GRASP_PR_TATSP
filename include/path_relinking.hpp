#pragma once

#include "datatypes.hpp"

#include <random> 

void applyPathRelinkingMove(Tour& initial_solution, Tour& guiding_solution, Graph& graph, vector<Arc>& all_arcs);

Tour mixedPathRelinking(Tour solution1, Tour solution2, Graph& graph, vector<Arc>& all_arcs);

int calculateHammingDistance(Tour& solution1, Tour& solution2);

int selectGuidingSolution(Tour& initial_solution, vector<Tour>& pool, mt19937& gen_pr);

void updatePool(Tour& current_solution, vector<Tour>& pool);

int selectMostSimilarTour(Tour& new_solution, vector<Tour>& pool);

int selectSimilarWorseTour(Tour& new_solution, vector<Tour>& pool);