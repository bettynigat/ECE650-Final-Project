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
        int length;
        LinkedList();
        void add_node(int);
        bool delete_node(int);
        void set_empty();
        std::string print_list();
        LinkedList * copy();
};