#ifndef MCTS_HPP_
#define MCTS_HPP_

#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <cfloat>
#include <string>
#include "timer.hpp"

#define TRACE 0

template<class State, typename ActionStr>
class Node : public std::enable_shared_from_this<Node<State, ActionStr>> {
    typedef Node<State, ActionStr> NodeStr;
    typedef std::shared_ptr<NodeStr> NodePtr;

    State state_;
    ActionStr action_; // action from parent state to this state
    bool agent_id_;
    NodePtr parent_;

    int num_visits_ = 0;
    float score_sum_ = 0.;

    std::vector<NodePtr> children_;
    std::vector<ActionStr> actions_;

public:
    //ActionStr action_; // action from parent state to this state
    
    Node(State& state, ActionStr& action, bool agent_id, NodePtr parent)
        : state_(state), action_(action), agent_id_(agent_id), parent_(parent) {}

    NodePtr Expand() {
        if (IsFullyExpanded()) return nullptr;

        if (actions_.empty()) {
            actions_ = state_.GetActions(agent_id_, true);
            // if agent has no available moves, change agent
            if (actions_.empty()) {
                agent_id_ ^= 1;
                actions_ = state_.GetActions(agent_id_, true);
            }
        }

        return AddChild(actions_[children_.size()], !agent_id_);
    }

    void ApplyAction(ActionStr action) { state_.ApplyAction(action); }
    void UpdateAction(ActionStr action) { action_ = action; }
    void UpdateNumVisits(int num_visits) { num_visits_ = num_visits; }
    void UpdateScoreSum(float score_sum) { score_sum_ = score_sum; }

    State GetState() { return state_; }
    ActionStr GetAction() const { return action_; }
    NodePtr GetParent() const { return parent_; }
    int GetId() const { return agent_id_; }
    int GetNumVisits() const { return num_visits_; }
    float GetScoreSum() const { return score_sum_; }
    const NodePtr& GetChild(int i) { return children_[i]; }
    const std::vector<NodePtr>& GetChildren() const { return children_; }
    int GetChildrenSize() const { return children_.size(); }
    std::vector<ActionStr> GetActions() const { return actions_; }
    bool IsTerminal() { return state_.IsTerminal(); }
    void ShowAction() {
        std::cout << "{(" << action_.begin.x << ", " << action_.begin.y << "), ";
        std::cout << "(" << action_.end.x << ", " << action_.end.y << ")}" << std::endl;
    }

    inline bool IsFullyExpanded() const {
        return !children_.empty() && children_.size() == actions_.size();
    }

    void ShowInfo() {
        std::cout << "Node info:\n";
        std::cout << "- agent_id_:        " << agent_id_ << "\n";
        std::cout << "- num_visits_:      " << num_visits_ << "\n";
        std::cout << "- score_sum_:       " << score_sum_ << "\n";
        //std::cout << "- actions_.size():  " << actions_.size() << "\n";
        //std::cout << "- childrens_size(): " << children_.size() << "\n";
    }

private:
    NodePtr AddChild(ActionStr& new_action, bool agent_id) {
        //std::cout << "AddChild, action = {(" << new_action.begin.x << ", " << new_action.begin.y << "), ";
        //std::cout << "(" << new_action.end.x << ", " << new_action.end.y << ")}\n";
        NodePtr child_node = std::make_shared<NodeStr>(
            state_, new_action, agent_id, this->shared_from_this());
        child_node->ApplyAction(new_action);
        children_.emplace_back(child_node);
        return child_node;
    }
};

template<class State, typename ActionStr>
class UCT {
    typedef Node<State, ActionStr> NodeStr;
    typedef std::shared_ptr<NodeStr> NodePtr;
public:
    UCT(float ucb_c = 1.414f, int mode = 0) : ucb_c_(ucb_c), mode_(mode), timer_() {}

    NodePtr GetBestUCBChild(const NodePtr& node) const {
        //std::cout << "GetBestUCBChild" << std::endl;
        //std::cout << "actions_size() = " << node->GetActions().size();
        //std::cout << ", children_.size() = " << node->GetChildrenSize() << std::endl;
        NodePtr best_node = nullptr;
        float best_ucb = std::numeric_limits<float>::lowest();

        auto children = node->GetChildren();
        for (const auto& child : children) {
            float ucb;
            if (child->GetNumVisits() == 0) {
                ucb = ucb_c_ * sqrt(std::log(float(node->GetNumVisits())));
            }
            else {
                ucb = child->GetScoreSum() / child->GetNumVisits() + ucb_c_ * std::sqrt(std::log(float(node->GetNumVisits())) / child->GetNumVisits());
            }
            //float exploitation = float(child->GetScoreSum()) / (child->GetNumVisits() + FLT_EPSILON);
            //float exploration = sqrt(std::log(float(node->GetNumVisits() + 1)) / (child->GetNumVisits() + FLT_EPSILON));
            //float ucb = exploitation + ucb_c_ * exploration;

            if (ucb > best_ucb) {
                best_ucb = ucb;
                best_node = child;
            }
        }

        return best_node;
    }

    NodePtr GetMostVisitedChild(const NodePtr& node) const {
        NodePtr best_node = nullptr;
        int most_visits = -1;

        auto children = node->GetChildren();
        for (const auto& child : children) {
            if (child->GetNumVisits() > most_visits) {
                most_visits = child->GetNumVisits();
                best_node = child;
            }
        }

        return best_node;
    }

    NodePtr GetBestScoreRatioChild(const NodePtr& node) const {
        NodePtr best_node = nullptr;
        float best_ratio = std::numeric_limits<float>::lowest();

        auto children = node->GetChildren();
        for (const auto& child : children) {
            auto ratio = child->GetScoreSum() / child->GetNumVisits();
            if (ratio > best_ratio) {
                best_ratio = ratio;
                best_node = child;
            }
        }

        return best_node;
    }

    ActionStr Run(State begin_state, bool begin_id,
        const int max_iterations = 50000, const double max_milliseconds = 5000) {

        ActionStr begin_action = ActionStr({-1, -1, -1, -1});
        NodePtr root = nullptr;
        root = std::make_shared<NodeStr>(begin_state, begin_action, begin_id, nullptr);

        int iterations = 0;
        timer_.Start();
        while (true) {
            ++iterations;
            #if TRACE
            if (iterations % 2000 == 0) {
                std::cout << "Iteration #" << iterations << std::endl;
            }
            #endif
            NodePtr node = root;

            //std::cout << "1. Selection" << std::endl;
            // 1. Selection
            //node->ShowAction();
            //node->ShowInfo();
            int depth = 0;
            while (!node->IsTerminal() && node->IsFullyExpanded()) {
                node = GetBestUCBChild(node);
                if (iterations % 10000 == 0) {
                    ++depth;
                    //std::cout << "Selection to depth " << depth << std::endl;
                    //node->ShowAction();
                    //node->ShowInfo();
                }
            }

            //std::cout << "2. Expansion" << std::endl;
            // 2. Expansion
            if (!node->IsTerminal() && !node->IsFullyExpanded()) {
                node = node->Expand();
            }

            //std::cout << "3. Simulation" << std::endl;
            // 3. Simulation
            State state = node->GetState();
            if (!node->IsTerminal()) {
                //state = node->GetState();
                bool id = node->GetId();
                while (!state.IsTerminal()) {
                    ActionStr action = state.GetRandomAction(id);
                    state.ApplyAction(action);
                    //std::cout << (id == BLACK ? "BLACK" : "WHITE") << std::endl;
                    id ^= 1;
                }
            }

            //std::cout << "4. Backpropogation" << std::endl;
            // 4. Backpropogation
            std::vector<float> score = {state.Evaluate(BLACK), state.Evaluate(WHITE)};
            while (node) {
                node->UpdateNumVisits(node->GetNumVisits() + 1);
                if (!node->GetParent()) {
                    node->UpdateScoreSum(node->GetScoreSum() + score[!node->GetId()]);
                }
                else {
                    node->UpdateScoreSum(node->GetScoreSum() + score[node->GetParent()->GetId()]);
                }
                node = node->GetParent();
            }

            if (iterations % 2000 == 0) {
                #if TRACE
                std::cout << "Time elapsed: " << timer_.ElapsedMilliseconds() << " ms\n";
                std::cout << "--------------------------" << std::endl;
                #endif

                if (iterations > max_iterations || timer_.ElapsedMilliseconds() > max_milliseconds - 1000) { 
                    timer_.Stop();
                    std::cout << "Time elapsed: " << timer_.ElapsedMilliseconds() << " ms\n";
                    std::cout << "--------------------------" << std::endl;
                    break;
                }
            }
        }

        //std::cout << "RUN FINISHED" << std::endl;
        NodePtr choose;
        if (mode_ == 0) {
            choose = GetBestUCBChild(root);
        }
        else if (mode_ == 1) {
            choose = GetMostVisitedChild(root);
        }
        else if (mode_ == 2) {
            choose = GetBestScoreRatioChild(root);
        }

        if (choose) {
            #if TRACE 
            choose->ShowInfo();
            #endif
            auto res = choose->GetAction();
            root = nullptr;
            return res;
        }
        else {
            return {-1, -1, -1, -1};
        }
    }
private:
    float ucb_c_;
    int mode_;
    Timer timer_;
};

#endif // MCTS_HPP_