#include "timing_graph.h"
#include <iostream>

TimingNode* TimingGraph::add_node(const std::string& name) {
    auto [it, inserted] = nodes_.emplace(name, TimingNode{name});
    return &it->second;
}

void TimingGraph::add_edge(const std::string& src,
                           const std::string& dst,
                           double delay) {
    TimingNode* src_node = add_node(src);
    TimingNode* dst_node = add_node(dst);

    edges_.push_back({src_node, dst_node, delay});
    src_node->out_edges.push_back(&edges_.back());
    dst_node->in_edges.push_back(&edges_.back());
}

void TimingGraph::dump() const {
    for (const auto& [name, node] : nodes_) {
        std::cout << "Node " << name << " -> ";
        for (const auto* edge : node.out_edges) {
            std::cout << edge->dst->name
                      << "(" << edge->delay << "ns) ";
        }
        std::cout << "\n";
    }
}