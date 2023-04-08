#include <iostream>
#include <sstream>
#include <vector>
#include "parser.hpp"
#include "constants.hpp"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <time.h>
#include <functional>

//return in micro seconds
double get_running_time(std::function<void(int &)> func) {
    clockid_t clockid;
    pthread_getcpuclockid(pthread_self(), &clockid);
    struct timespec start, end;
    clock_gettime(clockid, &start);
    int i = 0;
    func(i);
    clock_gettime(clockid, &end);
    return (double) (end.tv_sec - start.tv_sec) * 1e6 + (double)(end.tv_nsec - start.tv_nsec) / 1e3;

}

//return in micro seconds
double get_running_time_for_cnf(std::function<std::string(int &)> func) {
    clockid_t clockid;
    pthread_getcpuclockid(pthread_self(), &clockid);
    struct timespec start, end;
    clock_gettime(clockid, &start);
    int i = 0;
    func(i);
    clock_gettime(clockid, &end);
    return  (double) (end.tv_sec - start.tv_sec) *1e6 + (double)(end.tv_nsec - start.tv_nsec) / 1e3;
}

pthread_mutex_t input_mutex;
pthread_mutex_t cnf_mutex;
pthread_mutex_t cnf_3_mutex;
pthread_mutex_t approx_1_mutex;
pthread_mutex_t approx_2_mutex;
pthread_mutex_t refined_1_mutex;
pthread_mutex_t refined_2_mutex;
pthread_cond_t cnf_cond;
pthread_cond_t cnf_3_cond;
pthread_cond_t approx_1_cond;
pthread_cond_t approx_2_cond;
pthread_cond_t refined_1_cond;
pthread_cond_t refined_2_cond;

void *input(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    pthread_mutex_lock(&input_mutex);
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
            if (handler.parse_line(line, cmd, arg, error_msg)) {
                bool isValid = handler.process_command(cmd,arg,error_msg);
                if (handler.is_graph_initialized) {
                    handler.matrix_add_value();
                    pthread_mutex_unlock(&input_mutex);
                    handler.is_cnf_produced = false;
                    handler.is_cnf_3_produced = false;
                    handler.is_approx_1_produced = false;
                    handler.is_approx_2_produced = false;
                    pthread_cond_broadcast(&cnf_cond);
                    pthread_cond_broadcast(&cnf_3_cond);
                    pthread_cond_broadcast(&approx_1_cond);
                    pthread_cond_broadcast(&approx_2_cond);

                    while (!handler.is_computing_finished()) //slow down input thread a little bit to wait for 6 threads finishing their jobs before proceeding to the next input 
                    {
                        //do nothing
                    }  
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
    //write to file
    handler.matrix_add_value();
    handler.save_data();

    pthread_mutex_unlock(&input_mutex);
    pthread_cond_broadcast(&cnf_cond);
    pthread_cond_broadcast(&cnf_3_cond);
    pthread_cond_broadcast(&approx_1_cond);
    pthread_cond_broadcast(&approx_2_cond);
    pthread_cond_broadcast(&refined_1_cond);
    pthread_cond_broadcast(&refined_2_cond);
    return NULL;
};

void *cnf_sat(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&cnf_mutex);
        while(handler.is_cnf_produced && !handler.is_input_finished) {
            
            pthread_cond_wait(&cnf_cond, &cnf_mutex);
            
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        std::string result;
        int number_of_vc = 0;
        double running_time = get_running_time_for_cnf([&handler, &result, &number_of_vc](int &) {
            result = handler.print_cnf_sat(number_of_vc);
            return result;
        });
        std::cout << result;
        if (result.find(timeout) != std::string::npos) {  //timeout happens
            handler.columns_set_value(timeout_duration * 1e6, -1, case_cnf);
        } else {
            handler.columns_set_value(running_time, number_of_vc, case_cnf);
        }
        
        handler.is_cnf_produced = true; 
        pthread_cond_signal(&cnf_cond);
        pthread_mutex_unlock(&cnf_mutex);
    }
    
    return NULL;
};

void *cnf_3_sat(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&cnf_3_mutex);
        while(handler.is_cnf_3_produced && !handler.is_input_finished) {
            pthread_cond_wait(&cnf_3_cond, &cnf_3_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        std::string result;
        int number_of_vc = 0;
        double running_time = get_running_time_for_cnf([&handler, &result, &number_of_vc](int &) {
            result = handler.print_cnf_3_sat(number_of_vc);
            return result;
        });
        std::cout << result;
        if (result.find(timeout) != std::string::npos) { //timeout happens, then log timeout duration (in microsecond)
            handler.columns_set_value(timeout_duration * 1e6, -1, case_cnf_3);
        } else {
            handler.columns_set_value(running_time,number_of_vc , case_cnf_3);
        }
        // handler.print_cnf_3_sat();
        handler.is_cnf_3_produced = true;
        pthread_cond_signal(&cnf_3_cond);
        pthread_mutex_unlock(&cnf_3_mutex);
    }
    
    return NULL;
}

void *approx_1(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&approx_1_mutex);
        while(handler.is_approx_1_produced && !handler.is_input_finished) {
            pthread_cond_wait(&approx_1_cond, &approx_1_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        int number_of_vc = 0;
        double running_time = get_running_time([&handler, &number_of_vc](int &) { handler.print_approx_1(number_of_vc); });
        handler.columns_set_value(running_time, number_of_vc, case_approx_1);
        // handler.print_approx_1();
        
        handler.is_approx_1_produced = true;
        //broadcast to refined_approx_1 to feed it with the data
        pthread_cond_signal(&approx_1_cond);
        
        handler.is_refined_1_produced = false;
        pthread_cond_broadcast(&refined_1_cond);
        pthread_mutex_unlock(&approx_1_mutex);
    }
    
    return NULL;
}

void *approx_2(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&approx_2_mutex);
        while(handler.is_approx_2_produced && !handler.is_input_finished) {
            pthread_cond_wait(&approx_2_cond, &approx_2_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        int number_of_vc = 0;
        double running_time = get_running_time([&handler, &number_of_vc](int &) { handler.print_approx_2(number_of_vc); });
        handler.columns_set_value(running_time, number_of_vc, case_approx_2);
        // handler.print_approx_2();
        handler.is_approx_2_produced = true;
        //broadcast to refined_approx_2 to feed it with the data
        pthread_cond_signal(&approx_2_cond);
        
        handler.is_refined_2_produced = false;
        pthread_cond_broadcast(&refined_2_cond);
        pthread_mutex_unlock(&approx_2_mutex);
    }
    
    return NULL;
}

void *refined_approx_1(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&refined_1_mutex);
        while(handler.is_refined_1_produced && !handler.is_input_finished) {
            pthread_cond_wait(&refined_1_cond, &refined_1_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        int number_of_vc = 0;
        double running_time = get_running_time([&handler, &number_of_vc](int &) { handler.print_refined_approx_1(number_of_vc); });
        handler.columns_set_value(running_time, number_of_vc, case_refined_approx_1);
        // handler.print_refined_approx_1();
        handler.is_refined_1_produced = true;
        pthread_cond_signal(&refined_1_cond);
        pthread_mutex_unlock(&refined_1_mutex);
        
    }
    
    return NULL;
}

void *refined_approx_2(void *arg) {
    CommandHandler &handler = *(CommandHandler *)arg;
    
    while (true) {
        pthread_mutex_lock(&refined_2_mutex);
        while(handler.is_refined_2_produced && !handler.is_input_finished) {
            pthread_cond_wait(&refined_2_cond, &refined_2_mutex);
        }
        if (handler.is_input_finished) {
            return NULL;
        }
        int number_of_vc = 0;
        double running_time = get_running_time([&handler, &number_of_vc](int &) { handler.print_refined_approx_2(number_of_vc); });
        handler.columns_set_value(running_time,number_of_vc, case_refined_approx_2);
        // handler.print_refined_approx_2();
        handler.is_refined_2_produced = true;
        pthread_cond_signal(&refined_2_cond);
        pthread_mutex_unlock(&refined_2_mutex);
        
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

    pthread_cond_destroy(&cnf_cond);
    pthread_cond_destroy(&cnf_3_cond);
    pthread_cond_destroy(&approx_1_cond);
    pthread_cond_destroy(&approx_2_cond);
    pthread_cond_destroy(&refined_1_cond);
    pthread_cond_destroy(&refined_2_cond);

    return 0;
}
