#include <iostream>
#include <sstream>
#include <vector>
#include "parser.hpp"
#include "constants.hpp"
#include <memory>


int main(int argc, char **argv) {
 CommandHandler *handler = new CommandHandler();
    while (!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);
        if (line.size() == 0) {
            continue;
        }

        char cmd;
        std::vector<int> arg;
        std::string error_msg;
        try {
            if (handler->parse_line(line, cmd, arg, error_msg)) {
                bool isValid = handler->process_command(cmd,arg,error_msg);
                if (cmd == 'E' && isValid ) {
                    std::cout << error_msg << std::endl;
                } else if (!isValid) {
                    std::cerr << error_msg << std::endl;
                }
            }
            else {
                std::cerr << error_msg << std::endl;
            }
        } 
        catch (...) {
            std::cerr << bad_input << std::endl;
        }
    }
    return 0;
}
