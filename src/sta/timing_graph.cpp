#include "timing_graph.h"
#include <iostream>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>

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

TimingNode* TimingGraph::get_node(const std::string& name) {
    auto it = nodes_.find(name);
    if(it != nodes_.end()) {
        return &it->second;
    }
    return nullptr;
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

std::vector<TimingNode*> TimingGraph::topo_order() {
    std::unordered_map<std::string, int> in_degree;
    std::vector<TimingNode*> result;
    std::queue<TimingNode*> queue;
    for (auto& [name, node] : nodes_) {
        in_degree[name] = node.in_edges.size();
        if(node.in_edges.size() == 0) {
            queue.push(&node);
        }
    }

    while (!queue.empty()) {
        TimingNode* current = queue.front();
        queue.pop();
        result.push_back(current);

        for (const auto* edge : current->out_edges) {
            in_degree[edge->dst->name]--;
            if (in_degree[edge->dst->name] == 0) {
                queue.push(edge->dst);
            }
        }
    }

    if(result.size() != nodes_.size()) {
        throw std::runtime_error("Graph has at least one cycle, topological sort not possible.");
    }
    topo_order_cache_ = result;
    return result;
}

double TimingGraph::propagate_arrival_times() {
    if(topo_order_cache_.empty()) {
        topo_order();
    }
    double max_arrival_time = 0.0;
    for(auto* node : topo_order_cache_) {
        if(node->type==TimingNodeType::FF_LAUNCH) {
            node->arrival_time = node->clk_to_q;
            continue;
        }
        for(auto* edge : node->in_edges) {
            double arrival_time = edge->src->arrival_time + edge->delay;
            if(arrival_time > node->arrival_time) {
                node->worst_parent = edge->src;
                node->arrival_time = arrival_time;
            }
        }
        if(node->arrival_time > max_arrival_time) {
            max_arrival_time = node->arrival_time;
        }
    }
    return max_arrival_time;
}

double TimingGraph::propagate_required_times() {
    double max_time = clock_period_;
    if(topo_order_cache_.empty()) {
        topo_order();
    }
    for(auto* node : topo_order_cache_) {
        node->required_time = max_time;
    }
    for(auto it = topo_order_cache_.rbegin(); it != topo_order_cache_.rend(); ++it) {
        TimingNode* node = *it;
        for(auto* edge : node->out_edges) {
            if(node->type==TimingNodeType::FF_CAPTURE) {
                node->required_time = clock_period_ = node->setup_time;
                continue;
            }
            double required_time = edge->dst->required_time - edge->delay;
            if(required_time < node->required_time) {
                node->required_time = required_time;
            }
        }
    }
    return 0.0; // Placeholder, can be modified to return meaningful data if needed
}

void TimingGraph::compute_slack() {
    double worst_slack = 999.0;
    TimingNode* worst_node = nullptr;
    for(auto* node : topo_order_cache_) {
        double slack = node->required_time - node->arrival_time;
        std::cout << "Node " << node->name << " Slack: " << slack << "ns\n";
        if(slack < worst_slack) {
            worst_slack = slack;
            worst_node = node;
        }
    }
    std::cout << "Worst slack: " << worst_slack << "ns, Worst node: " << worst_node->name << "\n";
    TimingNode* cursor = worst_node;
    std::cout << "Critical path: ";
    while(cursor) {
        std::cout << cursor->name << " <--";
        cursor = cursor->worst_parent;
    }
}     

