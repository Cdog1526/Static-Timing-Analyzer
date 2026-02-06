#include <iostream>
#include <thread>
#include <vector>
#include "sta/timing_graph.h"
#include "verilog/parser.h"
#include "sta/netlist_builder.cpp"

int main() {
    VerilogParser parser = VerilogParser("../verilog/test1.v");
    AST ast = parser.parse();
    ast.print();

    NetListBuilder builder(ast, 1.0, 10.0);

    TimingGraph graph = builder.build();

    // graph.add_edge("A", "B", 1.2);
    // graph.add_edge("B", "C", 0.8);
    // graph.add_edge("A", "C", 2.5);

    graph.dump();
    double max_arrival_time = graph.propagate_arrival_times();
    graph.propagate_required_times();
    graph.compute_slack();

    std::vector<std::thread> workers;
    for (int i = 0; i < 4; i++) {
        workers.emplace_back([i]() {
            std::cout << "Worker " << i << " running\n";
        });
    }

    for (auto &t : workers) {
        t.join();
    }

    std::cout << "All workers finished. STA ready.\n";
    return 0;
}