#pragma once 
#include <string>

static const std::string unknown_command = "Error: Unknown command.";
static const std::string out_of_range = "Error: Out of index.";
static const std::string vertices_not_reach = "Error: two verticles are unreachable.";
static const std::string bad_input = "Error: Format of the input is incorrect.";
static const std::string invalid_input = "Error: Invalid command.";

static const std::string cnf_prefix = "CNF-SAT-VC: ";
static const std::string cnf_3_prefix = "CNF-3-SAT-VC: ";
static const std::string approx_1_prefix = "APPROX-VC-1: ";
static const std::string approx_2_prefix = "APPROX-VC-2: ";
static const std::string refined_1_prefix = "REFINED-APPROX-VC-1: ";
static const std::string refined_2_prefix = "REFINED-APPROX-VC-2: ";
static const std::string timeout = "timeout\n";
static const double timeout_duration = 10; //in second
