#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

enum class PortDirection {
    INPUT,
    OUTPUT
};

struct Port {
    std::string name;
    PortDirection direction;
};


struct AssignStmt {
    std::string lhs;              
    std::vector<std::string> rhs; 
};

struct FlipFlop {
    std::string clock; 
    std::string d;    
    std::string q; 
};

/*
 *Verilog module
 */
struct Module {
    std::string name;

    std::vector<Port> ports;
    std::unordered_set<std::string> wires;

    std::vector<AssignStmt> assigns;
    std::vector<FlipFlop> flipflops;
};

/*
 * Root AST object
 */
struct AST {
    Module top;
    void print(){
        std::cout << "Module: " << top.name << "\n";
        std::cout << "Ports:\n";
        for(const auto& port : top.ports) {
            std::cout << "  " << port.name << " (" 
                      << (port.direction == PortDirection::INPUT ? "INPUT" : "OUTPUT") 
                      << ")\n";
        }
        std::cout << "Wires:\n";
        for(const auto& wire : top.wires) {
            std::cout << "  " << wire << "\n";
        }
        std::cout << "Assign Statements:\n";
        for(const auto& assign : top.assigns) {
            std::cout << "  " << assign.lhs << " = ";
            for(size_t i = 0; i < assign.rhs.size(); ++i) {
                std::cout << assign.rhs[i];
                if(i != assign.rhs.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "Flip-Flops:\n";
        for(const auto& ff : top.flipflops) {
            std::cout << "  D: " << ff.d << ", Q: " << ff.q 
                      << ", Clock: " << ff.clock << "\n";
        }
    }
};