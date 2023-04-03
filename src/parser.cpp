#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <vector>

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

CommandHandler::CommandHandler() {
    g = new Graph();
    is_graph_initialized = false;
    is_cnf_produced = true;
    is_cnf_3_produced = true;
    is_approx_1_produced = true;
    is_approx_2_produced = true;
    is_refined_1_produced = true;
    is_refined_2_produced = true;
    // is_cnf_produced = false;
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
    std::cout << g->print_cnf_sat();
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