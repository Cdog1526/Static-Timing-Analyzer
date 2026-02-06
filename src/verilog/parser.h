#pragma once
#include "ast.h"
#include <string>

class VerilogParser {
public:
    explicit VerilogParser(const std::string& filename);
    AST parse();

private:
    std::string filename;
};