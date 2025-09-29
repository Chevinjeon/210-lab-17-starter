// COMSC-210 | Lab 17 | Modular Linked List
// Author: Chevin Jeon
// Date: 2025-09-29
//
// Goal:
//  - Refactor the provided non-modular linked list into a modular version.
//  - All common operations are moved into functions with prototypes.
//  - main() demonstrates usage.
//
// Build (example):
//   g++ -std=c++17 -Wall -Wextra -pedantic -O2 main.cpp -o linkedlist
//
// ---------------------------------------------------------------------

#include <iostream>   // std::cout, std::cin, std::endl
#include <cstdlib>    // std::rand, std::srand
#include <ctime>      // std::time for seeding rand
using namespace std;

// -------------------- Data model --------------------
struct Node {
    float value;   // payload
    Node* next;    // pointer to next node
};

// -------------------- Function Prototypes (required) --------------------
void   output(const Node* head);                         // print list
int    size(const Node* head);                           // count nodes

Node*  push_front(Node* head, float value);              // add at head
Node*  push_back(Node* head, float value);               // add at tail

Node*  insert_after(Node* head, int pos1based, float v); // insert after position
Node*  delete_at(Node* head, int pos1based);             // delete at position

void   clear_list(Node*& head);                          // delete entire list (sets head=nullptr)

// (private helper) get pointer to node at 1-based position; nullptr if out of range
Node*  get_node_at(Node* head, int pos1based);

// -------------------- main() --------------------
int main() {
    // Seed PRNG once so the list contents differ per run.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int SIZE = 7;  // initial list size to build
    Node* head = nullptr;

    // Build initial list by pushing to the head, matching the original behavior.
    for (int i = 0; i < SIZE; ++i) {
        int tmp_val = std::rand() % 100;     // random value in [0, 99]
        head = push_front(head, static_cast<float>(tmp_val)); // add at head
    }

    cout << "Initial list:\n";
    output(head);

    // ----- Delete a node by 1-based position -----
    cout << "Which node to delete? (1-" << size(head) << "): ";
    int delPos = 0;
    cin >> delPos;
    head = delete_at(head, delPos);
    cout << "\nList after deletion:\n";
    output(head);

    // ----- Insert a node after a given position -----
    cout << "After which node to insert value 10000? (0 inserts at head, 1-"
         << size(head) << " inserts after that node): ";
    int insAfter = 0;
    cin >> insAfter;

    if (insAfter == 0) {
        // insert at the very front if user enters 0
        head = push_front(head, 10000.0f);
    } else {
        head = insert_after(head, insAfter, 10000.0f);
    }

    cout << "\nList after insertion:\n";
    output(head);

    // ----- Demonstrate adding to tail (reusable function) -----
    cout << "Appending 777 to the tail.\n";
    head = push_back(head, 777.0f);
    output(head);

    // ----- Delete entire list -----
    cout << "Clearing entire list...\n";
    clear_list(head);
    output(head);

    return 0;
}

// -------------------- Implementations --------------------

// Print all nodes (or "Empty list.")
void output(const Node* head) {
    if (!head) {
        cout << "Empty list.\n\n";
        return;
    }
    int idx = 1;                        // 1-based index for display
    const Node* cur = head;             // traversal pointer
    while (cur) {                       // walk until nullptr
        cout << "[" << idx++ << "] " << cur->value << '\n';
        cur = cur->next;
    }
    cout << '\n';                       // extra line for readability
}

// Count how many nodes are in the list.
int size(const Node* head) {
    int n = 0;                          // counter
    const Node* cur = head;             // traversal pointer
    while (cur) {                       // accumulate count
        ++n;
        cur = cur->next;
    }
    return n;                           // return total
}

// Add a new node at the head and return the new head.
Node* push_front(Node* head, float value) {
    Node* node = new Node{value, nullptr}; // allocate + initialize new node
    node->next = head;                     // new node points to old head
    return node;                           // new node becomes the new head
}

// Add a new node at the tail and return (possibly updated) head.
Node* push_back(Node* head, float value) {
    Node* node = new Node{value, nullptr}; // new tail with next=nullptr
    if (!head) {                           // empty list case
        return node;                       // node is both head and tail
    }
    Node* cur = head;                      // otherwise find the tail
    while (cur->next) {                    // advance until last node
        cur = cur->next;
    }
    cur->next = node;                      // link last node to new node
    return head;                           // head does not change
}

// Insert a node with value v *after* the 1-based position pos1based.
// If pos1based is out of range, no insertion occurs (prints a message).
// Returns (possibly updated) head.
Node* insert_after(Node* head, int pos1based, float v) {
    if (pos1based < 1) {
        cout << "insert_after: position must be >= 1 (got " << pos1based << "). No insertion.\n";
        return head;
    }
    Node* at = get_node_at(head, pos1based); // get target node
    if (!at) {
        cout << "insert_after: position " << pos1based << " is out of range. No insertion.\n";
        return head;
    }
    Node* node = new Node{v, at->next};     // new node points where 'at' was pointing
    at->next = node;                         // link 'at' to the new node
    return head;                             // head unchanged
}

// Delete the node at 1-based position pos1based.
// Returns the new head (which changes if we delete position 1).
Node* delete_at(Node* head, int pos1based) {
    if (!head) {                             // empty list guard
        cout << "delete_at: list is empty. Nothing to delete.\n";
        return head;
    }
    if (pos1based < 1) {                     // invalid index
        cout << "delete_at: position must be >= 1 (got " << pos1based << "). No deletion.\n";
        return head;
    }
    if (pos1based == 1) {                    // special case: delete head
        Node* toDel = head;                  // remember old head
        head = head->next;                   // advance head
        delete toDel;                        // free node
        return head;                         // updated head
    }

    // For >1, we need the previous node to re-route pointers.
    Node* prev = get_node_at(head, pos1based - 1); // node before the one to delete
    if (!prev || !prev->next) {               // out of range (no node to delete)
        cout << "delete_at: position " << pos1based << " is out of range. No deletion.\n";
        return head;
    }

    Node* toDel = prev->next;                 // node to delete
    prev->next = toDel->next;                 // bypass toDel
    delete toDel;                             // free memory
    return head;                              // head unchanged
}

// Delete the entire list and set head to nullptr.
void clear_list(Node*& head) {
    Node* cur = head;                         // traversal pointer
    while (cur) {                             // walk and free nodes
        Node* nxt = cur->next;                // remember next
        delete cur;                           // free current
        cur = nxt;                            // advance
    }
    head = nullptr;                           // caller sees pointer cleared
}

// Return pointer to node at 1-based position pos1based; nullptr if out of range.
Node* get_node_at(Node* head, int pos1based) {
    if (pos1based < 1) return nullptr;        // invalid index
    Node* cur = head;                         // start at head
    int idx = 1;                              // 1-based counter
    while (cur && idx < pos1based) {          // advance until we reach target
        cur = cur->next;
        ++idx;
    }
    // cur is either the node at pos1based or nullptr
    return cur;
}
