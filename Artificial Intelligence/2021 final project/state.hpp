#ifndef STATE_HPP_
#define STATE_HPP_

#include <memory>
#include <vector>
#include <cstdlib>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>

#define BLACK 0
#define WHITE 1
#define SELF_BARRIER_COST -3
#define OPPONEN_BARRIER_COST 3


std::random_device rd;
std::mt19937 gen(rd());

class State {
    typedef std::vector<std::vector<int>> BoardStr;

    BoardStr board_;
    std::vector<int> budgets_;
    std::vector<int> num_pieces_;
    std::vector<int> num_barriers_;

public:
    // Place be like {x, y}.
    struct PlaceStr {
        int x, y;
    };
    // Action be like {begin, end} -> {begin.x, begin.y, end.x, end.y}.
    struct ActionStr {
        PlaceStr begin, end;
    };

    State() {}

    State(BoardStr& board, std::vector<int>& budgets,
        std::vector<int> num_pieces = std::vector<int>(2, 5),
        std::vector<int> num_barriers = std::vector<int>(2, 12))
        : board_(board), budgets_(budgets)
        , num_pieces_(num_pieces)
        , num_barriers_(num_barriers) { }

    BoardStr GetBoard() { return board_; }
    std::vector<int> GetBudgets() { return budgets_; }
    std::vector<int> GetNumPieces() { return num_pieces_; }
    std::vector<int> GetNumBarriers() { return num_barriers_; }
    
    // Get all available actions for given color.
    std::vector<ActionStr> GetActions(bool color, bool check) const {
        std::vector<ActionStr> actions;
        std::vector<PlaceStr> pieces = GetPieces(color);
        for (const auto& piece : pieces) {
            std::vector<PlaceStr> availables = GetAvailables(piece, check);
            if (check) {
                // the piece has 1 step move to win.
                if (availables.size() == 1 &&
                    (board_[availables[0].x][availables[0].y] == 1 || board_[availables[0].x][availables[0].y] == 2)) {
                    actions = std::vector<ActionStr>{{piece, availables[0]}};
                    return actions;
                }
            }

            for (const auto& available : availables) {
                actions.emplace_back(ActionStr{piece, available});
            }

        }

        std::shuffle(actions.begin(), actions.end(), gen);
        return actions;
    }

    // Get random action.
    ActionStr GetRandomAction(bool color) const {
        std::vector<ActionStr> actions = GetActions(color, false);
        if (!actions.empty()) {
            return actions[0];
        }
        return ActionStr{-1, -1, -1, -1};
    }

    // Apply the action on current state.
    void ApplyAction(ActionStr action) {
        PlaceStr begin = action.begin;
        PlaceStr end = action.end;

        if (begin.x < 0 || begin.y < 0 || end.x < 0 || end.y < 0) return;

        switch (board_[end.x][end.y]) {
        case 3:
        case 4:
            --num_pieces_[board_[end.x][end.y] - 3];
            break;
        case 5:
        case 6:
            --num_barriers_[board_[end.x][end.y] - 5];
            break;
        default:
            break;
        }

        // update budgets
        bool color = board_[begin.x][begin.y] == 3 ? BLACK : WHITE;
        budgets_[color] -= GetCost(begin, end);
        if (board_[end.x][end.y] == 5 || board_[end.x][end.y] == 6) {
            if (color == board_[end.x][end.y] - 5) {
                budgets_[color] -= SELF_BARRIER_COST;
            }
            else {
                budgets_[color] -= OPPONEN_BARRIER_COST;
            }
        }

        // update board
        board_[end.x][end.y] = board_[begin.x][begin.y];
        board_[begin.x][begin.y] = 0;
    }

    // Return whether is terminal state.
    bool IsTerminal() const {
        if (board_[0][4] != 2 || board_[8][4] != 1) {
            return true;
        }
        if (budgets_[BLACK] <= 1 && budgets_[WHITE] <= 1) {
            return true;
        }
        if ((budgets_[BLACK] <= 1 && num_pieces_[WHITE] == 0) || (budgets_[WHITE] <= 1 && num_pieces_[BLACK] == 0)) {
            return true;
        }

        return false;
    }

    // Evaluate the reward based on current state for given color.
    // win: 1, tie: 0.5, lose: 0.
    // 0.5 0 -0.5
    float Evaluate(bool color) const {
        const std::vector<std::vector<float>> rewards = {{1., 0.5, 0.}, {0., 0.5, 1.}};
        // 0. flag left
        if (board_[0][4] != 2) {
            return rewards[color][0];
        }
        if (board_[8][4] != 1) {
            return rewards[color][2];
        }
        // 1. pieces left
        if (num_pieces_[BLACK] > num_pieces_[WHITE]) {
            return rewards[color][0];
        }
        if (num_pieces_[BLACK] < num_pieces_[WHITE]) {
            return rewards[color][2];
        }
        // 2. barriers left
        if (num_barriers_[BLACK] > num_barriers_[WHITE]) {
            return rewards[color][0];
        }
        if (num_barriers_[BLACK] < num_barriers_[WHITE]) {
            return rewards[color][2];
        }
        // 3. tie
        return rewards[color][1];
    }

private:
    // Get places of exist pieces of given color.
    std::vector<PlaceStr> GetPieces(const bool& color) const {
        std::vector<PlaceStr> pieces;
        int tok = color == BLACK ? 3 : 4;
        for (int x = 0; x < board_.size(); ++x) {
            for (int y = 0; y < board_.size(); ++y) {
                if (board_[x][y] == tok) {
                    pieces.emplace_back(PlaceStr{x, y});
                }
            }
        }
        return pieces;
    }

    // Get available actions for the piece. //TODO CHECK FOR BARRIER COST
    std::vector<PlaceStr> GetAvailables(const PlaceStr& piece, bool check) const {
        std::vector<PlaceStr> availables;
        std::vector<PlaceStr> possibles = GetPossibles(piece);
        bool color = GetPieceColor(piece);
        for (const auto& possible : possibles) {
            if (GetCost(piece, possible) <= budgets_[color]) {
                if (check) {
                    switch (board_[possible.x][possible.y]) {
                    case 1: // flags
                    case 2:
                        availables = std::vector<PlaceStr>{possible};
                        return availables;
                    case 3: // pieces
                    case 4:
                        availables.emplace_back(possible);
                        break;
                    case 0: // empty
                    case 5: // barriers
                    case 6:
                        int cross_piece_diff = GetCrossPieceDiff(possible);
                        if (color == BLACK && cross_piece_diff > 0 || color == WHITE && cross_piece_diff < 0) {
                            availables.emplace_back(possible);
                        }
                        break;
                    }
                }
                else {
                    availables.emplace_back(possible);
                }
            }
        }
        return availables;
    }

    // Get possible actions for the piece. (Cost has not been checked.)
    // Possible place include empty, opponent's flag, opponent's piece,
    // and both sides' barriers.
    std::vector<PlaceStr> GetPossibles(const PlaceStr& piece) const {
        std::vector<PlaceStr> possibles;
        
        bool color = GetPieceColor(piece);
        int x = piece.x;
        int y = piece.y;

        // Check along +x.
        for (int i = x + 1, term = 0; i < board_.size() && !term; ++i) {
            switch (board_[i][y]) {
            case 0: // empty
                possibles.emplace_back(PlaceStr{i, y});
                break;
            case 1: // black flag
            case 2: // white flag
                if (color != board_[i][y] - 1)
                    possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            case 3: // black piece
            case 4: // white piece
                if (color != board_[i][y] - 3)
                    possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            case 5: // black barrier
            case 6: // white barrier
                possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            }
        }
        // Check along -x.
        for (int i = x - 1, term = 0; i >= 0 && !term; --i) {
            switch (board_[i][y]) {
            case 0: // empty
                possibles.emplace_back(PlaceStr{i, y});
                break;
            case 1: // black flag
            case 2: // white flag
                if (color != board_[i][y] - 1)
                    possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            case 3: // black piece
            case 4: // white piece
                if (color != board_[i][y] - 3)
                    possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            case 5: // black barrier
            case 6: // white barrier
                possibles.emplace_back(PlaceStr{i, y});
                term = 1;
                break;
            }
        }
        // Check along +y.
        for (int i = y + 1, term = 0; i < board_.size() && !term; ++i) {
            switch (board_[x][i]) {
            case 0: // empty
                possibles.emplace_back(PlaceStr{x, i});
                break;
            case 1: // black flag
            case 2: // white flag
                if (color != board_[x][i] - 1)
                    possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            case 3: // black piece
            case 4: // white piece
                if (color != board_[x][i] - 3)
                    possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            case 5: // black barrier
            case 6: // white barrier
                possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            }
        }
        // Check along -y.
        for (int i = y - 1, term = 0; i >= 0 && !term; --i) {
            switch (board_[x][i]) {
            case 0: // empty
                possibles.emplace_back(PlaceStr{x, i});
                break;
            case 1: // black flag
            case 2: // white flag
                if (color != board_[x][i] - 1)
                    possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            case 3: // black piece
            case 4: // white piece
                if (color != board_[x][i] - 3)
                    possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            case 5: // black barrier
            case 6: // white barrier
                possibles.emplace_back(PlaceStr{x, i});
                term = 1;
                break;
            }
        }

        return possibles;
    }

    int GetCrossPieceDiff(const PlaceStr& place) const {
        int x = place.x;
        int y = place.y;

        int cnt_black = 0, cnt_white = 0;

        // Check along +x.
        for (int i = x + 1, term = 0; i < board_.size() && !term; ++i) {
            switch (board_[i][y]) {
            case 0: // empty
                break;
            case 1: // black flag
            case 2: // white flag
                term = 1;
                break;
            case 3: // black piece
                ++cnt_black;
                break;
            case 4: // white piece
                ++cnt_white;
                break;
            case 5: // black barrier
            case 6: // white barrier
                term = 1;
                break;
            }
        }

        // Check along -x.
        for (int i = x - 1, term = 0; i >= 0 && !term; --i) {
            switch (board_[i][y]) {
            case 0: // empty
                break;
            case 1: // black flag
            case 2: // white flag
                term = 1;
                break;
            case 3: // black piece
                ++cnt_black;
                break;
            case 4: // white piece
                ++cnt_white;
                break;
            case 5: // black barrier
            case 6: // white barrier
                term = 1;
                break;
            }
        }

        // Check along +y.
        for (int i = y + 1, term = 0; i < board_.size() && !term; ++i) {
            switch (board_[x][i]) {
            case 0: // empty
                break;
            case 1: // black flag
            case 2: // white flag
                term = 1;
                break;
            case 3: // black piece
                ++cnt_black;
                break;
            case 4: // white piece
                ++cnt_white;
                break;
            case 5: // black barrier
            case 6: // white barrier
                term = 1;
                break;
            }
        }

        // Check along -y.
        for (int i = y - 1, term = 0; i >= 0 && !term; --i) {
            switch (board_[x][i]) {
            case 0: // empty
                break;
            case 1: // black flag
            case 2: // white flag
                term = 1;
                break;
            case 3: // black piece
                ++cnt_black;
                break;
            case 4: // white piece
                ++cnt_white;
                break;
            case 5: // black barrier
            case 6: // white barrier
                term = 1;
                break;
            }
        }

        return cnt_black - cnt_white;
    }

    bool NearPieces(const PlaceStr& place, bool color) const {
        int x = place.x;
        int y = place.y;

        // Check along +x.
        for (int i = x + 1, term = 0; i < board_.size() && !term; ++i) {
            if (board_[i][y] - 3 == color) return true;
        }

        // Check along -x.
        for (int i = x - 1, term = 0; i >= 0 && !term; --i) {
            if (board_[i][y] - 3 == color) return true;
        }

        // Check along +y.
        for (int i = y + 1, term = 0; i < board_.size() && !term; ++i) {
            if (board_[x][i] - 3 == color) return true;
        }

        // Check along -y.
        for (int i = y - 1, term = 0; i >= 0 && !term; --i) {
            if (board_[x][i] - 3 == color) return true;
        }

        return false;
    }

    // Get cost for moving from {begin} to {end}.
    inline int GetCost(const PlaceStr& begin, const PlaceStr& end) const {
        return 1 + abs(begin.x - end.x) + abs(begin.y - end.y);
    }

    // Get the piece's color.
    inline bool GetPieceColor(const PlaceStr& piece) const {
        return board_[piece.x][piece.y] == 3 ? BLACK : WHITE;
    }
};

#endif // STATE_HPP_