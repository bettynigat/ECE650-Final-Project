#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <cmath>

bool CommandHandler::parse_line(const std::string &line, char &cmd, std::vector<int> &arg, std::string &error) {
    std::istringstream stream(line);
    std::ws(stream);
    
    if (stream.eof()) {
        error = bad_input;
        return false;
    }
    char c;
    stream >> c;
    if (stream.fail()) {
        error = bad_input;
        return false;
    }
    if (c == 'V') {
        int number;
        stream >> number;
        if (stream.fail()) {
            error = bad_input;
            return false;
        }
       
        cmd = c;
        arg.push_back(number);
        return true;
    }
    else if (c == 'E') {
        cmd = c;
        char ch;
        std::string s;
        while (stream >> ch && !stream.eof()) {
            if (ch != '{' && ch != '}' && ch != '<' && ch != '>') {
                s += ch;
            }
        }
        std::istringstream number_stream (s);
        std::string number_string;
        while (std::getline(number_stream, number_string, ',')) {
            int i;
            std::istringstream (number_string) >> i;
            arg.push_back(i);
        }
        return true;
    }
     else {
        error = unknown_command;
        return false;
    }
}

CommandHandler::CommandHandler():matrix(0, std::vector<double>(6)), columns(6,-1.0) {
    g = new Graph();
    is_graph_initialized = false;
    is_cnf_produced = true;
    is_cnf_3_produced = true;
    is_approx_1_produced = true;
    is_approx_2_produced = true;
    is_refined_1_produced = true;
    is_refined_2_produced = true;
    is_input_finished = false;
}

bool CommandHandler::process_command(char c, std::vector<int> args, std::string &result) {
    std::string temp; 
    switch (c)
    {
    case 'V':
        g->set_size(args[0]);
        is_entered_V_valid = true;
        is_graph_initialized = false;
        break;
    case 'E':
        //if graph is already initialized, command E will be invalid 
        if (!is_entered_V_valid) {
            result = invalid_input;
            return false;
        }
        if (args.size() == 0) {
            //still initialize graph in case of no edge 
            g->graph_initalize(g->get_size(), args);
            return true;
        }

        for (int i = 0; i<args.size(); i++) {
            if (args[i] >= g->get_size() || args[i] < 0) {   // vertex input >= number of V or negative number
                result = out_of_range;
                is_entered_V_valid = false;
                g->set_size(g->get_graph().size());
                return false;
            }
        }

        //if V and E are all valid, reset the previous data and re-initalize the new graph
        g->graph_initalize(g->get_size(), args);
        is_entered_V_valid = false;
        is_graph_initialized = true;
        // result = g->print_vertex_cover();
        return true;
        break;

    default:
        result = unknown_command;
        return false;
    }
    return true;
}

std::string CommandHandler::print_cnf_sat() {
    return g->print_cnf_sat();
}   

std::string CommandHandler::print_cnf_3_sat() {
    return g->print_cnf_3_sat();
}

void CommandHandler::print_approx_1() {
    std::cout << g->print_approx_1();
}

void CommandHandler::print_approx_2() {
    std::cout << g->print_approx_2();
} 

void CommandHandler::print_refined_approx_1() {
    std::cout << g->print_refined_approx_1();
}

void CommandHandler::print_refined_approx_2() {
    std::cout << g->print_refined_approx_2();
}

void CommandHandler::columns_set_value(double running_time, VC_Algorithm_Type type) {
    columns[type] = running_time;
}

void CommandHandler::matrix_add_value() {
    bool is_data_valid = true;

    for (auto item: columns) {
        if (item == -1.0) {
            is_data_valid = false;
            break;
        }
    }
    if (is_data_valid) {
        matrix.push_back(columns); 
        columns.assign(columns.size(), -1.0);
    }
    
}

void CommandHandler::save_data() {
    std::ofstream myfile;
    std::string fileName = "outputV" + std::to_string(g->get_size()) + ".txt";
    myfile.open(fileName, std::ios::app);
    if (myfile.is_open()) {
        double cnf_average = 0;
        double cnf_3_average = 0;
        double approx_1_average = 0;
        double approx_2_average = 0;
        double refined_1_average = 0;
        double refined_2_average = 0;
        double cnf_variance = 0;
        double cnf_3_variance = 0;
        double approx_1_variance = 0;
        double approx_2_variance = 0;
        double refined_1_variance = 0;
        double refined_2_variance = 0;

        myfile << "CNF-SAT-VC CNF-3-SAT-VC APPROX-VC-1 APPROX-VC-2 REFINED-APPROX-VC-1 REFINED-APPROX-VC-2\n"; 
        for (int i = 0;i<matrix.size();i++) {
            std::vector<double> data = matrix[i];
            std::string time_in_string;
            for (int i=0;i<data.size();i++) {
                double time = data[i];
                if (i==0) {
                    cnf_average += time;
                } else if (i == 1) {
                    cnf_3_average += time;
                }else if (i == 2) {
                    approx_1_average += time;
                }else if (i == 3) {
                    approx_2_average += time;
                }else if (i == 4) {
                    refined_1_average += time;
                }else {
                    refined_2_average += time;
                }
            }
            
            
        }

        // myfile << std::to_string(matrix.size()) << " runs:";
        if (matrix.size() > 0) {
            cnf_average = cnf_average/matrix.size();
            cnf_3_average = cnf_3_average/matrix.size();
            approx_1_average = approx_1_average/matrix.size();
            approx_2_average = approx_2_average/matrix.size();
            refined_1_average = refined_1_average/matrix.size();
            refined_2_average = refined_2_average/matrix.size();

            myfile << std::to_string(cnf_average) << " " << std::to_string(cnf_3_average) << " " << std::to_string(approx_1_average) << " " << std::to_string(approx_2_average) << " " << std::to_string(refined_1_average) << " " << std::to_string(refined_2_average) << " " << std::endl;
            
            
            for (int i = 0;i<matrix.size();i++) {
                std::vector<double> data = matrix[i];
                std::string time_in_string;
                for (int i = 0;i<matrix.size();i++) {
                    double time = data[i];
                    if (i==0) {
                        cnf_variance += std::pow(time-cnf_average,2);
                    } else if (i == 1) {
                        cnf_3_variance += std::pow(time-cnf_3_average,2);
                    }else if (i == 2) {
                        approx_1_variance += std::pow(time-approx_1_average,2);
                    }else if (i == 3) {
                        approx_2_variance += std::pow(time-approx_2_average,2);
                    }else if (i == 4) {
                        refined_1_variance += std::pow(time-refined_1_average,2);
                    }else {
                        refined_2_variance += std::pow(time-refined_2_average,2);
                    }
                }
            }

            cnf_variance /= matrix.size();
            cnf_3_variance /= matrix.size();
            approx_1_variance /= matrix.size();
            approx_2_variance /= matrix.size();
            refined_1_variance /= matrix.size();
            refined_2_variance /= matrix.size();

            cnf_variance = std::sqrt(cnf_variance);
            cnf_3_variance = std::sqrt(cnf_3_variance);
            approx_1_variance = std::sqrt(approx_1_variance);
            approx_2_variance = std::sqrt(approx_2_variance);
            refined_1_variance = std::sqrt(refined_1_variance);
            refined_2_variance = std::sqrt(refined_2_variance);

            //calculate stadard deviation
            myfile << std::to_string(matrix.size()) << " runs:";
            myfile << std::to_string(cnf_variance) << " " << std::to_string(cnf_3_variance) << " " << std::to_string(approx_1_variance) << " " << std::to_string(approx_2_variance) << " " << std::to_string(refined_1_variance) << " " << std::to_string(refined_2_variance) << " " << std::endl;

            // myfile << "Approximation ratio after " << std::to_string(matrix.size()) << " runs:";
            //calculate approx ratio 
            

            // myfile << "Approximation ratio's standard deviation after " << std::to_string(matrix.size()) << " runs:";
            //calculate approx ratio

        }
        

        myfile.close();
    } 
}

bool CommandHandler::is_computing_finished() {
    return is_approx_1_produced && is_approx_2_produced && is_cnf_3_produced && is_cnf_produced && is_refined_1_produced && is_refined_2_produced;
}