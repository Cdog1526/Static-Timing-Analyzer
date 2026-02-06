#include "timing_graph.h"
#include "../verilog/ast.h"

struct NetListBuilder {
    explicit NetListBuilder(const AST& ast, const int delay, const double clock) : ast_(ast), delay_(delay), clock_(clock) {}

    TimingGraph build() {
        TimingGraph graph;
        const Module& module = ast_.top;
        for (const auto& port : module.ports) {
            TimingNode* node = graph.add_node(port.name);
            if(port.direction == PortDirection::INPUT) {
                graph.inputs.push_back(node);
            } else {
                graph.outputs.push_back(node);
            }
        }
        for (const auto& wire : module.wires) {
            graph.add_node(wire);
        }
        for (const auto& assign : module.assigns) {
            for (const auto& signal : assign.rhs) {
                graph.add_edge(signal, assign.lhs, delay_);
            }
        }
        for (const auto& ff : module.flipflops) {
            graph.get_node(ff.q)->type = TimingNodeType::FF_CAPTURE;
            graph.get_node(ff.q)->clk_to_q = clock_;
            
            graph.get_node(ff.d)->type = TimingNodeType::FF_LAUNCH;
            graph.get_node(ff.d)->setup_time = clock_;
        }
        return graph;
    }

    private:
        const AST& ast_;
        const double delay_;
        const double clock_;
};