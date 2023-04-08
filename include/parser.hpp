#include <string>
#include <vector>
#include "constants.hpp"
#include "graph.hpp"
#include <mutex>
#include <pthread.h>

enum VC_Algorithm_Type {
    case_cnf = 0,
    case_cnf_3 = 1,
    case_approx_1 = 2,
    case_approx_2 = 3,
    case_refined_approx_1 = 4,
    case_refined_approx_2 = 5
};

class CommandHandler {
    protected:
        Graph *g;
        bool is_entered_V_valid;
        std::vector<std::vector<double>> matrix; //0: CNF-SAT-VC, 1:CNF-3-SAT-VC, 2:APPROX-VC-1, 3:APPROX-VC-2, 4:REFINED-APPROX-VC-1, 5: REFINED-APPROX-VC-2
        std::vector<double> columns;

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
        std::string print_cnf_sat();
        std::string print_cnf_3_sat(); 
        void print_approx_1(); 
        void print_approx_2(); 
        void print_refined_approx_1(); 
        void print_refined_approx_2(); 
        void columns_set_value(double, VC_Algorithm_Type);
        void matrix_add_value();
        void save_data();
        bool is_computing_finished();
};

