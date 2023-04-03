#include <string>
#include <vector>
#include "constants.hpp"
#include "graph.hpp"
#include <mutex>

class CommandHandler {
    protected:
        Graph *g;
        bool is_entered_V_valid;
    public: 
        CommandHandler();
        std::mutex input_mutex;
        std::mutex cnf_mutex;
        std::mutex cnf_3_mutex;
        std::mutex approx_1_mutex;
        std::mutex approx_2_mutex;
        std::mutex refined_1_mutex;
        std::mutex refined_2_mutex;
        std::condition_variable cnf_cond;
        std::condition_variable cnf_3_cond;
        std::condition_variable approx_1_cond;
        std::condition_variable approx_2_cond;
        std::condition_variable refined_1_cond;
        std::condition_variable refined_2_cond;
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

