#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/heuristic.h"

#include "downward/utils/not_implemented.h"

#include "downward/plugins/plugin.h"

using namespace std;

namespace blind_search_heuristic {

class BlindSearchHeuristic : public Heuristic {
public:
    explicit BlindSearchHeuristic(
        const std::shared_ptr<ClassicalPlanningTask>& task);

    int compute_heuristic(const State& state) override;
};

BlindSearchHeuristic::BlindSearchHeuristic(
    const shared_ptr<ClassicalPlanningTask>& task)
    : Heuristic(task)
{
}

int BlindSearchHeuristic::compute_heuristic(const State& state)
{
// Check if this is a goal state
    int num_goals = task->get_num_goals();
    auto state_facts = task->get_state_facts(state);
    
    bool is_goal = true;
    for (int i = 0; i < num_goals; ++i) {
        FactPair goal_fact = task->get_goal_fact(i);
        if (goal_fact.value != state_facts[goal_fact.var].get_value()) {
            is_goal = false;
            break;
        }
    }
    
    if (is_goal) {
        return 0;
    }
    
    // Otherwise, return the minimal operator cost
    int num_operators = task->get_num_operators();
    
    // If no operators exist, return infinity (DEAD_END)
    if (num_operators == 0) {
        return INFINITY;  
    }
    
    // Find minimal operator cost
    int min_cost = numeric_limits<int>::max();
    for (int op = 0; op < num_operators; ++op) {
        int cost = task->get_operator_cost(op);
        if (cost < min_cost) {
            min_cost = cost;
        }
    }
    
    return min_cost; // All goal facts satisfied, this is a goal state
}

std::unique_ptr<Heuristic>
create_blind_heuristic(std::shared_ptr<ClassicalPlanningTask> task)
{
    return std::make_unique<blind_search_heuristic::BlindSearchHeuristic>(
        std::move(task));
}

class BlindSearchHeuristicFeature
    : public plugins::TypedFeature<Evaluator, Heuristic> {
public:
    BlindSearchHeuristicFeature()
        : TypedFeature("blind")
    {
        document_title("Blind heuristic");
        document_synopsis(
            "Returns cost of cheapest action for non-goal states, "
            "0 for goal states");

        add_heuristic_options_to_feature(*this, "blind");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support("axioms", "supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<Heuristic>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return create_blind_heuristic(
            std::get<0>(get_heuristic_arguments_from_options(opts)));
    }
};

static plugins::FeaturePlugin<BlindSearchHeuristicFeature> _plugin;
} // namespace blind_search_heuristic
