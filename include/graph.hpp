#pragma once
#include <vector>
#include <queue>
#include <string>
#include "linkedlist.hpp"
#include "constants.hpp"
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

class Graph {
    protected: 
        int size;
        std::vector<int> internal_edges;
        std::vector<LinkedList *> graph;
        std::vector<int> shortest_uncycle_path(int,int, std::string &);
        std::vector<int> shortest_cycle_path(int, std::string &);
        bool is_vertex_cover(int, std::vector<int> &);
        std::vector<int> cnf_sat_vc();
        std::vector<int> cnf_3_sat_vc();
        std::vector<int> approx_vc_1();
        std::vector<int> approx_vc_2();
        std::vector<int> refined_approx_vc_1();
        std::vector<int> refined_approx_vc_2();
    public:
        Graph();
        void graph_initalize(int, std::vector<int>);
        bool add_edge(int, int, std::string &);
        void reset_graph();
        int get_size();
        void set_size(int);
        std::string print_graph();
        bool is_graph_initialized = false;
        std::vector<LinkedList *> get_graph();
        std::string print_shortest_path(int,int);
        
        std::string print_vertex_cover();
};