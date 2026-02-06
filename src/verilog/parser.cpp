#include "ast.h"
#include "parser.h"
#include <iostream>
#include <string>
#include <cassert>
#include <regex>
#include <algorithm>
#include <sstream>
#include <fstream>

// NOTE: Parser assumes one statement per line and no line continuations

std::vector<std::string> splitSignals(const std::string& expr) {
    std::vector<std::string> result;
    std::regex re("[&|]");
    std::sregex_token_iterator it(expr.begin(), expr.end(), re, -1);
    std::sregex_token_iterator end;

    for (; it != end; ++it) {
        std::string s = it->str();
        s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
        if (!s.empty()) result.push_back(s);
    }
    return result;
}

inline std::string trim(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char x){return std::isspace(x);}), s.end());
    return s;
}

VerilogParser::VerilogParser(const std::string& filename)
    : filename(filename) {}

AST VerilogParser::parse(){
    std::cout << "Parsing Verilog file: " << this->filename << "\n";
    std::ifstream file(this->filename);
    if(!file.is_open()) {
        throw std::runtime_error("Could not open file: " + this->filename);
    }
    AST ast;
    Module& currModule = ast.top;
    for(std::string line; std::getline(file, line); ) {
        std::vector<std::string> tokens;
        std::string token;
        std::stringstream ss(line); // Create a stringstream from the input string

        // Use getline to extract tokens until the delimiter is found
        while (std::getline(ss, token, ' ')) {
            tokens.push_back(token);
        }
        
        if(tokens.size() == 0) {
            continue; // skip empty lines
        }
        if (tokens[0].rfind("//", 0) == 0) continue;
        if (tokens[0] == "module") {
            int pos = tokens[1].find('(');
            if(pos == std::string::npos) {
                tokens[1].pop_back();
                currModule.name = tokens[1];
            }
            else {
                currModule.name = tokens[1].substr(0, pos);
                tokens[2].pop_back();
                tokens[3].pop_back();
                tokens[3].pop_back();
                Port port;
                port.name = tokens[2];
                port.direction = PortDirection::INPUT;
                currModule.ports.push_back(port);
                Port port2;
                port2.name = tokens[3];
                port2.direction = PortDirection::OUTPUT;
                currModule.ports.push_back(port2);
            }
            
        }
        else if (tokens[0] == "endmodule") {
            continue;
        } else if (tokens[0] == "input") {
            Port port;
            tokens[1].pop_back();
            port.name = tokens[1];
            port.direction = PortDirection::INPUT;
            currModule.ports.push_back(port);
        } else if (tokens[0] == "output") {
            Port port;
            tokens[1].pop_back();
            port.name = tokens[1];
            port.direction = PortDirection::OUTPUT;
            currModule.ports.push_back(port);
        } else if (tokens[0] == "wire") {
            for (size_t i = 1; i < tokens.size(); ++i) {
                std::string wire_name = tokens[i];
                if (wire_name.back() == ';' || wire_name.back() == ','){
                    wire_name.pop_back();
                }
                currModule.wires.insert(wire_name);
            }
        } else if (tokens[0] == "assign") {
            std::string a;
            for (size_t i = 1; i < tokens.size(); ++i) {
                a += tokens[i];
            }
            std::string c = a.substr(0, a.find("="));
            std::string expr = a.substr(a.find("=") + 1); // remove semicolon
            expr.pop_back(); // remove semicolon
            std::vector<std::string> rhs = splitSignals(expr);
            currModule.assigns.push_back({trim(c), rhs});
        } else if (tokens[0] == "always") {
            assert(tokens[1] == "@(posedge");
            tokens[2].pop_back(); // remove closing parenthesis
            std::string clock = tokens[2];
            std::string a;
            for (size_t i = 3; i < tokens.size(); ++i) {
                a += tokens[i];
            }
            std::string d = a.substr(0, a.find("<="));
            std::string q = a.substr(a.find("<=") + 2);
            // remove semicolon
            q.pop_back();
            currModule.flipflops.push_back({clock, q, d});
        } else {
            throw std::runtime_error("Syntax error: could not handle keyword " + tokens[0]);
        }
        
    }
    file.close();
    return ast;
}