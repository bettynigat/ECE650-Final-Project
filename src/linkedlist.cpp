#include <iostream>
#include "linkedlist.hpp"

Node::Node() {
    val = 0;
    next = NULL;
}

Node::Node(int data) {
    this->val = data;
    this->next = NULL;
}

LinkedList::LinkedList() {
    head = NULL;
    length = 0;
}

void LinkedList::add_node(int val) {
    Node *n = new Node(val);
    if (head == NULL) {
        head = n;
        length++;
        return;
    }
    if (head->val == val) {
        return;
    }
    Node *temp = head;
    //move to tail node 
    while (temp->next != NULL) {
        if (temp->val == val) {  //assum that all nodes are different and duplicate values should be removed, and val is found -> do nothing
            return;
        }
        temp = temp->next;
    }
    //check tail node 
    if (temp->val == val) {
        return;
    }
    // val not found, add a node at the end
    temp->next = n;
    length++;
}

bool LinkedList::delete_node(int val) {
    if (head == NULL) {
        return false;
    } 

    if (head->val == val) {
        if (head -> next == NULL) {
            head = NULL;
        }
        else {
            head = head -> next;
        }
        return true;
    }

    Node *current = head;
    Node *prev = head;
    while (current != NULL) {
        if (current->val == val) {
            prev->next = current->next;
            current->next = NULL;
            delete current;
            return true;
        }
        prev = current;
        current = current->next;

    }
    return false;
}

std::string LinkedList::print_list() {
    Node *n = head;
    std::string s;
    if (head == NULL) {
        return "";
    }
    while (n != NULL) {
        std::string val_string = std::to_string(n->val);
        s += val_string + " ";
        n = n->next;
    }
    return s;
}