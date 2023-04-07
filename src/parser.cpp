#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

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

void CommandHandler::print_cnf_sat() {
    std::string result = g->print_cnf_sat();
    std::cout << result;
    
}   

void CommandHandler::print_cnf_3_sat() {
    std::cout << g->print_cnf_3_sat();
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
        myfile << "CNF-SAT-VC CNF-3-SAT-VC APPROX-VC-1 APPROX-VC-2 REFINED-APPROX-VC-1 REFINED-APPROX-VC-2\n"; 
        for (int i = 0;i<matrix.size();i++) {
            std::vector<double> data = matrix[i];
            std::string time_in_string;
            for (auto time: data) {
                 time_in_string += std::to_string(time) + " ";
            }
            time_in_string += "\n";
            myfile << time_in_string;
        }
        myfile.close();
    } 
}

bool CommandHandler::is_computing_finished() {
    return is_approx_1_produced && is_approx_2_produced && is_cnf_3_produced && is_cnf_produced && is_refined_1_produced && is_refined_2_produced;
}