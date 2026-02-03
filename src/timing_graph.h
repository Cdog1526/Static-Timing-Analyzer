#pragma once

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

struct TimingEdge;

struct TimingNode {
    std::string name;

    double arrival_time = 0.0;
    double required_time = 0.0;

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

private:
    std::unordered_map<std::string, TimingNode> nodes_;
    std::list<TimingEdge> edges_;
};