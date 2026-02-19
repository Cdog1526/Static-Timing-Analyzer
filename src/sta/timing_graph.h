#pragma once

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

enum class TimingNodeType {
    COMBINATIONAL,
    FF_LAUNCH,
    FF_CAPTURE,
    PRIMARY_INPUT,
    PRIMARY_OUTPUT
};

struct TimingEdge;

struct TimingNode {
    std::string name;

    double arrival_time = 0.0;
    double required_time = 0.0;
    double setup_time = 0.0;
    double clk_to_q = 0.0;
    TimingNode* worst_parent = nullptr;
    TimingNodeType type = TimingNodeType::COMBINATIONAL;
    std::vector<TimingEdge*> out_edges;
    std::vector<TimingEdge*> in_edges;
};

struct TimingEdge {
    TimingNode* src;
    TimingNode* dst;
    double delay;
};

class TimingGraph {
public:
    TimingNode* add_node(const std::string& name);
    void add_edge(const std::string& src,
                  const std::string& dst,
                  double delay);

    void dump() const;
    std::vector<TimingNode*> topo_order();
    double propagate_arrival_times();
    double propagate_required_times();
    void compute_slack();
    TimingNode* get_node(const std::string& name);
    std::vector<TimingNode*> inputs;
    std::vector<TimingNode*> outputs;

private:
    std::unordered_map<std::string, TimingNode> nodes_;
    std::list<TimingEdge> edges_;
    std::vector<TimingNode*> topo_order_cache_;
    double clock_period_ = 10.0; // Default clock period
};