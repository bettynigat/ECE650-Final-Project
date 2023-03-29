#include <string>
#include <vector>
#include "constants.hpp"
#include "graph.hpp"

class CommandHandler {
    protected:
        Graph *g;
        bool is_entered_V_valid;
    public: 
        CommandHandler();
        bool process_command(char, std::vector<int>,std::string &);
        bool parse_line(const std::string &, char &, std::vector<int> &, std::string &);     
};

