#include <iostream>
#include <sstream>
#include <vector>
#include "parser.hpp"
#include "constants.hpp"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>

void input(CommandHandler &handler) {
    
    while (!std::cin.eof()) {
        std::unique_lock<std::mutex> locker(handler.input_mutex);
        std::string line;
        std::getline(std::cin, line);
        if (line.size() == 0) {
            continue;
        }

        char cmd;
        std::vector<int> arg;
        std::string error_msg;
        try {
            if (handler.parse_line(line, cmd, arg, error_msg)) {
                bool isValid = handler.process_command(cmd,arg,error_msg);
                if (handler.is_graph_initialized) {
                    locker.unlock();
                    handler.is_cnf_produced = false;
                    handler.is_cnf_3_produced = false;
                    handler.is_approx_1_produced = false;
                    handler.is_approx_2_produced = false;
                    handler.is_refined_1_produced = false;
                    handler.is_refined_2_produced = false;
                    handler.cnf_cond.notify_all();
                    handler.cnf_3_cond.notify_all();
                    handler.approx_1_cond.notify_all();
                    handler.approx_2_cond.notify_all();
                    handler.refined_1_cond.notify_all();
                    handler.refined_2_cond.notify_all();
                }
                
                // std::cout << "Call here" << std::endl;
            }
            else {
                std::cerr << error_msg << std::endl;
            }
        } 
        catch (...) {
            std::cerr << bad_input << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    handler.is_input_finished = true;
    handler.cnf_cond.notify_all();
    handler.cnf_3_cond.notify_all();
    handler.approx_1_cond.notify_all();
    handler.approx_2_cond.notify_all();
    handler.refined_1_cond.notify_all();
    handler.refined_2_cond.notify_all();
};

void cnf_sat(CommandHandler &handler) {
    while (true) {
        std::unique_lock<std::mutex> locker(handler.cnf_mutex);
        handler.cnf_cond.wait(locker, [&handler]{return !handler.is_cnf_produced || handler.is_input_finished; });
        if (handler.is_input_finished) {
            return;
        }
        handler.print_cnf_sat();
        handler.is_cnf_produced = true;
        locker.unlock();
    }
};

void cnf_3_sat(CommandHandler &handler) {
    while (true) {
        std::unique_lock<std::mutex> locker(handler.cnf_3_mutex);
        handler.cnf_3_cond.wait(locker, [&handler]{return !handler.is_cnf_3_produced || handler.is_input_finished; });
        if (handler.is_input_finished) {
            return;
        }
        handler.print_cnf_3_sat();
        handler.is_cnf_3_produced = true;
        locker.unlock();
    }
}

void approx_1(CommandHandler &handler) {
    while (true) {
            std::unique_lock<std::mutex> locker(handler.approx_1_mutex);
            handler.approx_1_cond.wait(locker, [&handler]{return !handler.is_approx_1_produced || handler.is_input_finished; });
            if (handler.is_input_finished) {
                return;
            }
            handler.print_approx_1();
            handler.is_approx_1_produced = true;
            locker.unlock();
        }
}
void approx_2(CommandHandler &handler) {
    while (true) {
        std::unique_lock<std::mutex> locker(handler.approx_2_mutex);
        handler.approx_2_cond.wait(locker, [&handler]{return !handler.is_approx_2_produced || handler.is_input_finished; });
        if (handler.is_input_finished) {
            return;
        }
        handler.print_approx_2();
        handler.is_approx_2_produced = true;
        locker.unlock();
    }
}
void refined_approx_1(CommandHandler &handler) {
    while (true) {
            std::unique_lock<std::mutex> locker(handler.refined_1_mutex);
            handler.refined_1_cond.wait(locker, [&handler]{return !handler.is_refined_1_produced || handler.is_input_finished; });
            if (handler.is_input_finished) {
                return;
            }
            handler.print_refined_approx_1();
            handler.is_refined_1_produced = true;
            locker.unlock();
        }
}

void refined_approx_2(CommandHandler &handler) {
    while (true) {
            std::unique_lock<std::mutex> locker(handler.refined_2_mutex);
            handler.refined_2_cond.wait(locker, [&handler]{return !handler.is_refined_2_produced || handler.is_input_finished; });
            if (handler.is_input_finished) {
                return;
            }
            handler.print_refined_approx_2();
            handler.is_refined_2_produced = true;
            locker.unlock();
        }
}

int main(int argc, char **argv) {
    CommandHandler handler;
    std::thread thread_input(input, std::ref(handler));
    std::thread thread_cnf_sat(cnf_sat, std::ref(handler));
    std::thread thread_cnf_3_sat(cnf_3_sat, std::ref(handler));
    std::thread thread_approx_1(approx_1, std::ref(handler));
    std::thread thread_approx_2(approx_2, std::ref(handler));
    std::thread thread_refined_approx_1(refined_approx_1, std::ref(handler));
    std::thread thread_refined_approx_2(refined_approx_2, std::ref(handler));

    thread_input.join();
    thread_cnf_sat.join();
    thread_cnf_3_sat.join();
    thread_approx_1.join();
    thread_approx_2.join();
    thread_refined_approx_1.join();
    thread_refined_approx_2.join();
    return 0;
}
