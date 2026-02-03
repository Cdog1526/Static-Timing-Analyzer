#include <iostream>
#include <thread>
#include <vector>
#include "timing_graph.h"

int main() {
    TimingGraph graph;

    graph.add_edge("A", "B", 1.2);
    graph.add_edge("B", "C", 0.8);
    graph.add_edge("A", "C", 2.5);

    graph.dump();

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