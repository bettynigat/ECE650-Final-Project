#include <string>
#include <vector>
#include "constants.hpp"
#include "graph.hpp"
#include <mutex>
#include <pthread.h>

class CommandHandler {
    protected:
        Graph *g;
        bool is_entered_V_valid;
    public: 
        CommandHandler();
        bool is_graph_initialized;
        bool is_cnf_produced;
        bool is_cnf_3_produced;
        bool is_approx_1_produced;
        bool is_approx_2_produced;
        bool is_refined_1_produced;
        bool is_refined_2_produced;
        bool is_input_finished;
        bool process_command(char, std::vector<int>,std::string &);
        bool parse_line(const std::string &, char &, std::vector<int> &, std::string &);
        void print_cnf_sat();
        void print_cnf_3_sat(); 
        void print_approx_1(); 
        void print_approx_2(); 
        void print_refined_approx_1(); 
        void print_refined_approx_2();      
};

