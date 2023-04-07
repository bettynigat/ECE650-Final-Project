#include "graph.hpp"
#include "linkedlist.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <time.h>
#include <future>
#include <chrono>

Graph::Graph() {
    size = -1;
    is_graph_initialized = false;
}

void Graph::set_size(int size_g) {
    this->size = size_g;
}
void Graph::graph_initalize(int size, std::vector<int> args) {
    if (size == -1) {
        return;
    }
    //since this is valid pair of V,E, all data of old graph will be clear and re-initialized
    this->reset_graph();
    this->size = size;
    this->internal_edges = args;
    std::string error;
    graph = std::vector<LinkedList *>(size);
    //init array of linked list 
    for (int i=0; i<size; i++) {
        LinkedList *list = new LinkedList();
        graph[i] = list;
    }

    //if input no edge, still initialize array of linked list
    if (args.size() == 0) {
        is_graph_initialized = true;
        return;
    } 
    
    //add nodes to array 
    for (int i=0;i<=args.size()-2;i=i+2) {
        if (args[i] < size && args[i+1] < size) {
            add_edge(args[i],args[i+1], error);
        }
    }
    is_graph_initialized = true;

}

bool Graph::add_edge(int v1, int v2, std::string &error) {
    if (v1 >= size || v2 >= size) {
        error = out_of_range;
        return false;
    }
    
    graph[v1]->add_node(v2);
    graph[v2]->add_node(v1);
    error = "";
    return true;
}

void Graph::reset_graph() {
    graph.clear();
    internal_edges.clear();
    size = -1;
    is_graph_initialized = false;
}

std::string Graph::print_graph() {
    std::string s;
    for (int i =0;i<graph.size(); i++) {
        LinkedList *l = graph[i];
        if (l->head != NULL) {
            s += l->print_list();
            s += '\n'; 
        }
    }
    return s;
}

int Graph::get_size() {
    return size;
}

std::vector<LinkedList *>  Graph::get_graph() {
    return graph;
}

std::vector<int> Graph::shortest_uncycle_path(int src,int des, std::string &error) {
    if (src >= graph.size() || des >= graph.size()) {
        error = out_of_range;
        return std::vector<int>();
    }
    
    std::vector<int> path;
    std::queue<std::vector<int>> queue;
    std::vector<bool> visited_paths(graph.size(), false);
    
    path.push_back(src);
    queue.push(path);
    visited_paths[src] = true;
    
    while (!queue.empty()) {
        path = queue.front();
        queue.pop();
        
        int last_node = path.back();
        
        if (last_node == des) {
            return path;
        }
        
        LinkedList *l = graph[last_node];
        Node *n = l->head;
        while (n != NULL) {
            if (!visited_paths[n->val]) {
                std::vector<int> new_path = path;
                new_path.push_back(n->val);
                queue.push(new_path);
                visited_paths[n->val] = true;
            }
            n = n->next;
        }
    }
    
    error = vertices_not_reach;
    return std::vector<int>();

}

std::vector<int> Graph::shortest_cycle_path(int src, std::string &error) {
    if (src >= graph.size()) {
        error = out_of_range;
        return std::vector<int>();
    }
    
    std::vector<int> shortest;
    std::queue<std::vector<int>> queue;
    std::vector<std::vector<int>> visited_paths(graph.size(), std::vector<int>(graph.size(), 0));
    
    std::vector<int> path;
    path.push_back(src);
    queue.push(path);
    
    while (!queue.empty()) {
        std::vector<int> curr_path = queue.front();
        queue.pop();
        
        
        int last_node = curr_path.back();
        
        LinkedList *l = graph[last_node];
        Node *n = l->head;
        while (n != NULL) {
            if (std::find(curr_path.begin(), curr_path.end(), n->val) == curr_path.end() && visited_paths[last_node][n->val] < 2) {
                std::vector<int> new_path = curr_path;
                new_path.push_back(n->val);
                
                queue.push(new_path);
                visited_paths[last_node][n->val]++;
                visited_paths[n->val][last_node]++;
            } 
            else if (n->val == src && curr_path.size() > 2 && visited_paths[last_node][n->val] == 1){
                std::vector<int> new_path = curr_path;
                new_path.push_back(n->val);
                
                if (shortest.size() == 0 || new_path.size() < shortest.size()) {
                    shortest = new_path;
                }
                
                // reset the visited edges
                visited_paths.clear();
                visited_paths.resize(graph.size(), std::vector<int>(graph.size(), 0));
            }
            n = n->next;
        }
    }
    
    if (shortest.size() == 0) {
        error = vertices_not_reach;
    }
    
    return shortest;
}

std::string Graph::print_shortest_path(int src, int des) {
    std::string s;
    std::string shortest_string;
    std::vector<int> path;
    std::vector<int> shortest;
    if (src != des) {
        shortest = shortest_uncycle_path(src,des, s);
    } else {
        shortest = shortest_cycle_path(src, s);
    }
    
    if (shortest.size() > 0) {
        for (int i =0;i<shortest.size();i++) {
            shortest_string += std::to_string(shortest[i]);
            if (i<shortest.size()-1) {
                shortest_string += '-';
            }
        }
        return shortest_string;
    }
    else if (s != out_of_range) {
        return vertices_not_reach;
    }
    return s; 
    
}
/*Reference here: https://git.uwaterloo.ca/ece650-1231/minisat-demo/-/blob/master/ece650-minisat.cpp */
bool Graph::is_vertex_cover(int k, std::vector<int> &verticles, CNF_type type, std::string& message) {
    //n × k atomic propositions
    Minisat::Var atomic_pro[size][k];
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    

    //initialize vars 
    for (int i =0;i<size;i++) {
        for (int j=0;j<k;j++) {
            atomic_pro[i][j] = solver->newVar();
        }
    }

    /*clause 1:
    ∀i∈[0,k), ∀j∈[0,n) a clause(xj,i)
    */

    Minisat::vec<Minisat::Lit> cl;
    for (int i =0;i<k;i++) {
        cl.clear();
        for (int j = 0;j<size;j++) {
            //(xj,i)
            Minisat::Lit l = Minisat::mkLit(atomic_pro[j][i]);
            cl.push(l);
        }

        
        solver->addClause(cl);
    }

     /*clause 2:
     ∀m ∈ [0, n), ∀p, q ∈ [0, k) with p < q (aka p = q + 1, p∈[0,k-1),q ∈[p+1,k)), a clause (¬xm,p ∨ ¬xm,q)
     */
     for (int m = 0;m<size;m++) {
        for (int p=0;p<k-1;p++) {
            for (int q =p+1;q<k;q++) {
                //xm,p
                Minisat::Lit l1 = Minisat::mkLit(atomic_pro[m][p]);
                //xm,q
                Minisat::Lit l2 = Minisat::mkLit(atomic_pro[m][q]);
                //(¬xm,p ∨ ¬xm,q)
                solver->addClause(~l1,~l2);
            }
        }
     }
    /*clause 3: 
    ∀m∈[0,k),∀p,q∈[0,n) with p<q (aka p = q+1, p∈[0,n-1),q ∈[p+1,n)), a clause(¬xp,m ∨ ¬xq,m)
    */

    for (int m=0;m<k;m++) {
        for (int p = 0;p<size-1;p++) {
            for (int q = p+1;q<size;q++) {
                //xp,m
                Minisat::Lit l1 = Minisat::mkLit(atomic_pro[p][m]);
                //xq,m
                Minisat::Lit l2 = Minisat::mkLit(atomic_pro[q][m]);
                //(¬xp,m ∨ ¬xq,m)
                solver->addClause(~l1,~l2);
            }
        }
    }
    /*clause 4: 
    ∀⟨i,j⟩∈E, i and j are adjency edges,  a clause(xi,1 ∨ xi,2 ∨···∨ xi,k ∨ xj,1 ∨ xj,2 ∨···∨ xj,k)
    */
   Minisat::vec<Minisat::Lit> clause1;
    for (int i = 0;i<internal_edges.size();i=i+2) {
        clause1.clear();
        for (int j = 0;j<k;j++) {
            //xi,k
            Minisat::Lit l1 = Minisat::mkLit(atomic_pro[internal_edges[i]][j]);
            //xj,k
            Minisat::Lit l2 = Minisat::mkLit(atomic_pro[internal_edges[i+1]][j]);
            clause1.push(l1);
            clause1.push(l2);
        }

        solver->addClause(clause1); 
    }
    std::future<bool> fut = std::async(std::launch::async, [&solver]() {
        return solver->solve();
    });
    // Wait for the result with a timeout
    std::future_status status = fut.wait_for(std::chrono::duration<double>(timeout_duration));
    
    if (status == std::future_status::timeout) {
        solver->interrupt();
        // solver.reset (new Minisat::Solver());
        message = timeout;
        return false;
    }
    bool satisfiable = fut.get();
    if (satisfiable) {
        for (int i=0;i<size;i++) {
            for (int j=0;j<k;j++) {
                //find assignment 
                if (Minisat::toInt(solver->modelValue(atomic_pro[i][j])) == 0) {
                    verticles.push_back(i);
                }
            }
        }
    }
    solver.reset(new Minisat::Solver());
    return satisfiable;
}

bool Graph::is_vertex_cover_sat_3(int k, std::vector<int> &verticles, CNF_type type, std::string& message) {
    //n × k atomic propositions
    Minisat::Var atomic_pro[size][k];
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    

    //initialize vars 
    for (int i =0;i<size;i++) {
        for (int j=0;j<k;j++) {
            atomic_pro[i][j] = solver->newVar();
        }
    }

    /*clause 1:
    ∀i∈[0,k), ∀j∈[0,n) a clause(xj,i)
    */

    Minisat::vec<Minisat::Lit> cl;
    for (int i =0;i<k;i++) {
        cl.clear();
        for (int j = 0;j<size;j++) {
            //(xj,i)
            Minisat::Lit l = Minisat::mkLit(atomic_pro[j][i]);
            cl.push(l);
        }

            if (cl.size() <=3){
                solver->addClause(cl);
            }

            else {
                Minisat::Lit B[cl.size()]; //bo, b1, .... bn
                for (int l=0; l<cl.size(); l++){ // initialize the variables
                    B[l] = Minisat::mkLit(solver->newVar()); 
                }

                for (int l=0; l<cl.size(); l++){ // initialize the variables
                    Minisat::vec<Minisat::Lit> tempClause;
                    if (l==0){
                        tempClause.push(cl[0]); 
                        tempClause.push(B[0]); 
                        //L(0) v b(0)
                        solver->addClause(tempClause);
                    }
                    else if (l==cl.size()-1){//last clause
                        tempClause.push(~B[l-1]);
                        tempClause.push(cl[l]);
                        //~b(n-1) v L(n)
                        solver->addClause(tempClause);
                    }
                    else { //in the middle 
                        tempClause.push(~B[l-1]);
                        tempClause.push(cl[l]);
                        tempClause.push(B[l]);
                        //~b(n-1) v L(n) v b[n]
                        solver->addClause(tempClause);
                    }
                }
            } 
    }

     /*clause 2:
     ∀m ∈ [0, n), ∀p, q ∈ [0, k) with p < q (aka p = q + 1, p∈[0,k-1),q ∈[p+1,k)), a clause (¬xm,p ∨ ¬xm,q)
     */
     for (int m = 0;m<size;m++) {
        for (int p=0;p<k-1;p++) {
            for (int q =p+1;q<k;q++) {
                //xm,p
                Minisat::Lit l1 = Minisat::mkLit(atomic_pro[m][p]);
                //xm,q
                Minisat::Lit l2 = Minisat::mkLit(atomic_pro[m][q]);
                //(¬xm,p ∨ ¬xm,q)
                solver->addClause(~l1,~l2);
            }
        }
     }
    /*clause 3: 
    ∀m∈[0,k),∀p,q∈[0,n) with p<q (aka p = q+1, p∈[0,n-1),q ∈[p+1,n)), a clause(¬xp,m ∨ ¬xq,m)
    */

    for (int m=0;m<k;m++) {
        for (int p = 0;p<size-1;p++) {
            for (int q = p+1;q<size;q++) {
                //xp,m
                Minisat::Lit l1 = Minisat::mkLit(atomic_pro[p][m]);
                //xq,m
                Minisat::Lit l2 = Minisat::mkLit(atomic_pro[q][m]);
                //(¬xp,m ∨ ¬xq,m)
                solver->addClause(~l1,~l2);
            }
        }
    }
    /*clause 4: 
    ∀⟨i,j⟩∈E, i and j are adjency edges,  a clause(xi,1 ∨ xi,2 ∨···∨ xi,k ∨ xj,1 ∨ xj,2 ∨···∨ xj,k)
    */
   Minisat::vec<Minisat::Lit> clause1;
    for (int i = 0;i<internal_edges.size();i=i+2) {
        clause1.clear();
        for (int j = 0;j<k;j++) {
            //xi,k
            Minisat::Lit l1 = Minisat::mkLit(atomic_pro[internal_edges[i]][j]);
            //xj,k
            Minisat::Lit l2 = Minisat::mkLit(atomic_pro[internal_edges[i+1]][j]);
            clause1.push(l1);
            clause1.push(l2);
        }

            if (clause1.size() <=3){
                solver->addClause(clause1);
            }

            else{
                Minisat::Lit B[clause1.size()]; //bo, b1, .... bn
                for (int l=0; l<clause1.size(); l++){ // initialize the variables
                    B[l] = Minisat::mkLit(solver->newVar()); 
                }

                for (int l=0; l<clause1.size(); l++){ // initialize the variables
                    Minisat::vec<Minisat::Lit> tempClause;
                    if (l==0){
                        tempClause.push(clause1[0]); 
                        tempClause.push(B[0]); 
                        //L(0) v b(0)
                        solver->addClause(tempClause);
                    }
                    else if (l==clause1.size()-1){
                        tempClause.push(~B[l-1]);
                        tempClause.push(clause1[l]);
                        //~b(n-1) v L(n)
                        solver->addClause(tempClause);
                    }
                    else { //in the middle 
                        tempClause.push(~B[l-1]);
                        tempClause.push(clause1[l]);
                        tempClause.push(B[l]);
                        //~b(n-1) v L(n) v b[n]
                        solver->addClause(tempClause);
                    }
                } 
            }
    }
    std::future<bool> fut = std::async(std::launch::async, [&solver]() {
        return solver->solve();
    });
    // Wait for the result with a timeout
    std::future_status status = fut.wait_for(std::chrono::duration<double>(timeout_duration));
    
    if (status == std::future_status::timeout) {
        solver->interrupt();
        // solver.reset (new Minisat::Solver());
        message = timeout;
        return false;
    }
    bool satisfiable = fut.get();
    if (satisfiable) {
        for (int i=0;i<size;i++) {
            for (int j=0;j<k;j++) {
                //find assignment 
                if (Minisat::toInt(solver->modelValue(atomic_pro[i][j])) == 0) {
                    verticles.push_back(i);
                }
            }
        }
    }
    solver.reset(new Minisat::Solver());
    return satisfiable;
}

std::string Graph::solve_cnf_sat(CNF_type type) {
    int min_k = 1;
    int max_k = graph.size();
    std::vector<int> verticles;
    std::vector<int> result;
    std::string message;
    //determine the minimum size by binary search
    while (min_k <= max_k) {
        int mid_k = (min_k + max_k) / 2;
        bool satisfiable = is_vertex_cover(mid_k,verticles,type, message);
        if (satisfiable) {
            max_k = mid_k - 1;
            result = verticles; //keep the latest satisfiable list
            verticles.clear();
        } else {
            if (message == timeout) {
                return timeout;
            }
            min_k = mid_k + 1;
        }
    }
    //sort vertex cover vector
    std::sort(result.begin(), result.end());
    std::string string_verticles;

    for (int i = 0;i<result.size();i++) {
        string_verticles += std::to_string(result[i]) + " "; 
    }
    string_verticles += "\n";
    return string_verticles;
}

std::string Graph::solve_cnf_sat_3(CNF_type type) {
    int min_k = 1;
    int max_k = graph.size();
    std::vector<int> verticles;
    std::vector<int> result;
    std::string message;
    //determine the minimum size by binary search
    while (min_k <= max_k) {
        int mid_k = (min_k + max_k) / 2;
        bool satisfiable = is_vertex_cover_sat_3(mid_k,verticles,type, message);
        if (satisfiable) {
            max_k = mid_k - 1;
            result = verticles; //keep the latest satisfiable list
            verticles.clear();
        } else {
            if (message == timeout) {
                return timeout;
            }
            min_k = mid_k + 1;
        }
    }
    //sort vertex cover vector
    std::sort(result.begin(), result.end());
    std::string string_verticles;

    for (int i = 0;i<result.size();i++) {
        string_verticles += std::to_string(result[i]) + " "; 
    }
    string_verticles += "\n";
    return string_verticles;
}

std::vector<LinkedList *> Graph::copy() {
    std::vector<LinkedList*> graph_copy;
    for (int i=0;i<graph.size();i++) {
        LinkedList* list_copy = graph[i]->copy();
        graph_copy.push_back(list_copy);
    }
    return graph_copy;
}

std::vector<int> Graph::approx_vc_1() {
    /*
        1.get vertex with highest degree
        2. add to vertex cover list
        3. remove vertex from the graph 
        4. repeat until no edge left (a.k.a all lengths = 0)
    */
    
    std::vector<int> vertextCover;
    //step 0: extremely important: MUST work on the copied version of graph for multi-threading purpose
    std::vector<LinkedList*> graph_copy = copy();
    
    while (!is_graph_empty(graph_copy)) {  //step 4
        //step 1.
        auto max = std::max_element(graph_copy.begin(), graph_copy.end(),
            [](const LinkedList *a, const LinkedList *b) {
                return a->length < b->length;
        });
        int max_index = std::distance(graph_copy.begin(), max);

        //step 2
        vertextCover.push_back(max_index);
        //step 3
        remove_vertex(max_index, graph_copy);
    }
    //sort the vector 
    std::sort(vertextCover.begin(), vertextCover.end());
    return vertextCover;
}

std::vector<int> Graph::approx_vc_2() {
    
    std::vector<int> vertextCover;
    std::vector<LinkedList*> graph_copy = copy();
    int i = 0;
    while (!is_graph_empty(graph_copy)) {  //step 4
        LinkedList *l = graph_copy[i];
        Node *n = l->head;
        if (l->length > 0) {
            int val = n->val;
            vertextCover.push_back(i);
            vertextCover.push_back(val);
            remove_vertex(i, graph_copy);
            remove_vertex(val, graph_copy);
        }
        i++;
    }
    /*
    int num_edges = internal_edges.size()/2; 

    //keep record of deleted edges that are incident on a v in vertext cover 
    std::vector<bool> deletedEdges(num_edges ,false); 

    bool finish; 
    for (int i = 0;i<internal_edges.size();i=i+2) {
        finish = true; 
        int u = internal_edges[i];
        int v = internal_edges[i+1]; 

        //only u and v to vertextCover only if they are not already there
        auto it_u = find(vertextCover.begin(), vertextCover.end(), u);
        if (it_u == vertextCover.end()){
            vertextCover.push_back(u);
        }

        auto it_v = find(vertextCover.begin(), vertextCover.end(), v );
        if (it_v == vertextCover.end()){//element not found means we can't remove it from vertex cover
            vertextCover.push_back(v);
        }

        //delete all edges adjecent to u or v
        for (int i = 0;i<internal_edges.size();i=i+2){
            if (deletedEdges[i/2]==false){ //edge not deleted yet
                if ((internal_edges[i]==u) || (internal_edges[i]==v) ||(internal_edges[i+1]==u) || (internal_edges[i+1]==v)){
                        deletedEdges[i/2] = true; 
                    }
            }
        }

        //break loop when all edges have been deleted 
         for (int i = 0; i<num_edges; i++){
            if(deletedEdges[i]==false){
               finish = false; 
               break;  
            }
        }
        if (finish==true){
            break; 
        }
    }
    */
    std::sort(vertextCover.begin(), vertextCover.end());
    return vertextCover;
}
        
std::vector<int> Graph::refined_approx_vc_1() {

    std::vector<int> vertextCover =  approx_vc_1();     
    return refine_vertext_cover_set(vertextCover);
}

std::vector<int> Graph::refine_vertext_cover_set(std::vector<int> vertextCover) {
    std::vector<int> removedVerticels;
    for (auto& vertex: vertextCover) { 
        bool is_vc = true;
        //loop each neighor of vertex
        LinkedList *l = graph[vertex];
        Node *n = l->head;
        while (n != NULL) {
            int val = n->val;
            if (std::find(vertextCover.begin(), vertextCover.end(), val) == vertextCover.end()) { //not exist mean we can not remove it 
                is_vc = false;
                break;
            }
            n = n->next;
        }
        if (is_vc) {
            removedVerticels.push_back(vertex);
        }
    }
    for (auto& vertex: removedVerticels) {
        auto it = std::find(vertextCover.begin(), vertextCover.end(), vertex);
        if (it != vertextCover.end()) { //element exists, then remove it from vertextCover
            vertextCover.erase(it);
        }
    }

    //sort the vector
    std::sort(vertextCover.begin(), vertextCover.end());
    return vertextCover;
}

std::vector<int> Graph::refined_approx_vc_2() { 
    std::vector<int> vertextCover = approx_vc_2();    
    return refine_vertext_cover_set(vertextCover);
}

std::string Graph::print_cnf_sat() {
    std::string result = cnf_prefix + solve_cnf_sat(cnf);
    
    return result;
}

std::string Graph::print_cnf_3_sat() {
    std::string result = cnf_3_prefix + solve_cnf_sat_3(cnf_3);
    return result;
}

std::string Graph::print_approx_1() {
    std::vector<int> approx_1_verticles = approx_vc_1();
    std::string result = approx_1_prefix;
    for (int i = 0;i<approx_1_verticles.size();i++) {
        result += std::to_string(approx_1_verticles[i]) + " "; 
    }
    result += "\n";
    return result;
}

std::string Graph::print_approx_2() {
    std::vector<int> approx_2_verticles = approx_vc_2();
    std::string result = approx_2_prefix;
    for (int i = 0;i<approx_2_verticles.size();i++) {
        result += std::to_string(approx_2_verticles[i]) + " "; 
    }
    result += "\n";
    return result;
}

std::string Graph::print_refined_approx_1() {
    std::vector<int> refined_1_verticles = refined_approx_vc_1();
    std::string result = refined_1_prefix;
    for (int i = 0;i<refined_1_verticles.size();i++) {
        result += std::to_string(refined_1_verticles[i]) + " "; 
    }
    result += "\n";
    return result;
}

std::string Graph::print_refined_approx_2() {
    std::vector<int> refined_2_verticles = refined_approx_vc_2();
    std::string result = refined_2_prefix;
    for (int i = 0;i<refined_2_verticles.size();i++) {
        result += std::to_string(refined_2_verticles[i]) + " "; 
    }
    result += "\n";
    return result;
}

void Graph::remove_vertex(int vertex, std::vector<LinkedList *> &graph_copy) {
    if (vertex >= graph_copy.size()) {
        return;
    }

    LinkedList *l = graph_copy[vertex];
    Node *n = l->head;
    //loop every neigbour of vertex
    while (n != NULL) {
        int val = n->val;
        //for each neighbor, try to delete vertex
        LinkedList *neighbor = graph_copy[val];
        neighbor->delete_node(vertex);
        n = n->next;
    }

    //finially delete the whole linkedlist at vertex index
    l->set_empty();
}

bool Graph::is_graph_empty(std::vector<LinkedList *> graph) {
    bool is_empty = true;
    for (int i =0;i<graph.size();i++) {
        if (graph[i]->head != NULL && graph[i]->length > 0) {
            is_empty = false;
        }
    }
    return is_empty;
}