#pragma once
#include <iostream>
#include <string>



class Node {
    public:
        int val;
        Node *next;
        Node();
        Node(int);
};

class LinkedList {   
    public:
        Node *head;
        LinkedList();
        void add_node(int);
        bool delete_node(int);
        std::string print_list();
};