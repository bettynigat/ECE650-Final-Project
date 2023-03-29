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
    else if (c == 's') {
        int num;
        stream >> num;
        if (stream.fail()) {
            error = bad_input;
            return false;
        }
        arg.push_back(num);
        stream >> num;
        if (stream.fail()) {
            error = bad_input;
            return false;
        }
        arg.push_back(num);
        cmd = 's';

        return true;
    } else {
        error = unknown_command;
        return false;
    }
}

CommandHandler::CommandHandler() {
    g = new Graph();
}

bool CommandHandler::process_command(char c, std::vector<int> args, std::string &result) {
    std::string temp; 
    switch (c)
    {
    case 'V':
        g->set_size(args[0]);
        is_entered_V_valid = true;
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
        result = g->print_vertex_cover();
        return true;
        break;
    case 's':
         //validate vertices first before adding edges 
         //reset V if E is not input or invalid E 
         is_entered_V_valid = false;
         g->set_size(g->get_graph().size());
         if (args.size() != 2) {
            result = invalid_input;
            return false;
         }
         for (int i = 0; i<args.size(); i++) {
            if (args[i] >= g->get_size() || args[i] < 0) { // vertex input >= number of V or negative number
                result = out_of_range;
                return false;
            }
        }
        result = g->print_shortest_path(args[0],args[1]);
        if (result == vertices_not_reach) {
            return false;
        }
        return true;
    default:
        result = unknown_command;
        return false;
    }
    return true;
}