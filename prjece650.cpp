#include <iostream>
#include <sstream>
#include <vector>
#include "parser.hpp"
#include "constants.hpp"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>

void *input(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (!std::cin.eof()) {
        pthread_mutex_lock(&handler.input_mutex);
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
                    pthread_mutex_unlock(&handler.input_mutex);
                    handler.is_cnf_produced = false;
                    handler.is_cnf_3_produced = false;
                    handler.is_approx_1_produced = false;
                    handler.is_approx_2_produced = false;
                    
                    pthread_cond_broadcast(&handler.cnf_cond);
                    pthread_cond_broadcast(&handler.cnf_3_cond);
                    pthread_cond_broadcast(&handler.approx_1_cond);
                    pthread_cond_broadcast(&handler.approx_2_cond);
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

    handler.is_input_finished = true;
    pthread_cond_broadcast(&handler.cnf_cond);
    pthread_cond_broadcast(&handler.cnf_3_cond);
    pthread_cond_broadcast(&handler.approx_1_cond);
    pthread_cond_broadcast(&handler.approx_2_cond);
    pthread_cond_broadcast(&handler.refined_1_cond);
    pthread_cond_broadcast(&handler.refined_2_cond);
    return NULL;
};

void *cnf_sat(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.cnf_mutex);
        while(handler.is_cnf_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.cnf_cond, &handler.cnf_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_cnf_sat();
        handler.is_cnf_produced = true;
        pthread_cond_signal(&handler.cnf_cond);
        pthread_mutex_unlock(&handler.cnf_mutex);
    }
    return NULL;
};

void *cnf_3_sat(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.cnf_3_mutex);
        while(handler.is_cnf_3_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.cnf_3_cond, &handler.cnf_3_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_cnf_3_sat();
        handler.is_cnf_3_produced = true;
        pthread_cond_signal(&handler.cnf_3_cond);
        pthread_mutex_unlock(&handler.cnf_3_mutex);
    }
    return NULL;
}

void *approx_1(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.approx_1_mutex);
        while(handler.is_approx_1_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.approx_1_cond, &handler.approx_1_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_approx_1();
        
        handler.is_approx_1_produced = true;
        //broadcast to refined_approx_1 to feed it with the data
        std::cout << "I'm finish VC_1. Now i'm feeding refined_vc_1" << std::endl;
        
        pthread_cond_signal(&handler.approx_1_cond);
        pthread_mutex_unlock(&handler.approx_1_mutex);
        handler.is_refined_1_produced = false;
        pthread_cond_broadcast(&handler.refined_1_cond);
    }
    return NULL;
}
void *approx_2(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.approx_2_mutex);
        while(handler.is_approx_2_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.approx_2_cond, &handler.approx_2_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_approx_2();
        handler.is_approx_2_produced = true;
        //broadcast to refined_approx_2 to feed it with the data
        std::cout << "I'm finish VC_2. Now i'm feeding refined_vc_2" << std::endl;
        
        pthread_cond_signal(&handler.approx_2_cond);
        pthread_mutex_unlock(&handler.approx_2_mutex);
        handler.is_refined_2_produced = false;
        pthread_cond_broadcast(&handler.refined_2_cond);
    }
    return NULL;
}

void *refined_approx_1(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.refined_1_mutex);
        while(handler.is_refined_1_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.refined_1_cond, &handler.refined_1_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_refined_approx_1();
        handler.is_refined_1_produced = true;
        pthread_cond_signal(&handler.refined_1_cond);
        pthread_mutex_unlock(&handler.refined_1_mutex);
    }
    return NULL;
}

void *refined_approx_2(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    while (true) {
        pthread_mutex_lock(&handler.refined_2_mutex);
        while(handler.is_refined_2_produced && !handler.is_input_finished) {
            pthread_cond_wait(&handler.refined_2_cond, &handler.refined_2_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        handler.print_refined_approx_2();
        handler.is_refined_2_produced = true;
        pthread_cond_signal(&handler.refined_2_cond);
        pthread_mutex_unlock(&handler.refined_2_mutex);
    }
    return NULL;
}

int main(int argc, char **argv) {
    CommandHandler handler;
    pthread_t thread_input;
    pthread_t thread_cnf_sat;
    pthread_t thread_cnf_3_sat;
    pthread_t thread_approx_1;
    pthread_t thread_approx_2;
    pthread_t thread_refined_approx_1;
    pthread_t thread_refined_approx_2;

    pthread_create(&thread_input, NULL,input,(void *)&handler);
    pthread_create(&thread_cnf_sat, NULL,cnf_sat,(void *)&handler);
    pthread_create(&thread_cnf_3_sat, NULL,cnf_3_sat,(void *)&handler);
    pthread_create(&thread_approx_1, NULL,approx_1,(void *)&handler);
    pthread_create(&thread_approx_2, NULL,approx_2,(void *)&handler);
    pthread_create(&thread_refined_approx_1, NULL,refined_approx_1,(void *)&handler);
    pthread_create(&thread_refined_approx_2, NULL,refined_approx_2,(void *)&handler);


    pthread_join(thread_input, NULL);
    pthread_join(thread_cnf_sat, NULL);
    pthread_join(thread_cnf_3_sat, NULL);
    pthread_join(thread_approx_1, NULL);
    pthread_join(thread_approx_2, NULL);
    pthread_join(thread_refined_approx_1, NULL);
    pthread_join(thread_refined_approx_2, NULL);

    pthread_cond_destroy(&handler.cnf_cond);
    pthread_cond_destroy(&handler.cnf_3_cond);
    pthread_cond_destroy(&handler.approx_1_cond);
    pthread_cond_destroy(&handler.approx_2_cond);
    pthread_cond_destroy(&handler.refined_1_cond);
    pthread_cond_destroy(&handler.refined_2_cond);

    return 0;
}
