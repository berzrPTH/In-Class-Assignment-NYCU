#include <iostream>
#include <string>
#include <vector>
#include "state.hpp"
#include "mcts.hpp"

#define BLACK 0
#define WHITE 1
#define SELF_BARRIER_COST -3
#define OPPONEN_BARRIER_COST 3
#define MAX_PRICE 140

using namespace std;
// utf-8
// const string Horizon = "\u2f00";
// const string S[7] = {"  ", "\u25b2", "\u25b3", "\u25cf", "\u25cb", "\u25a0", "\u25a1"};

// Big-5
const string Horizon = "\xA1\x58";
const string S[7] = {"\xA1\x40", "\xA1\xBB", "\xA1\xBA", "\xA1\xB4", "\xA1\xB3", "\xA1\xBD", "\xA1\xBC"};
                 // {' ', '��', '��', '��', '��', '��', '��'};
                 // 0: empty, 1: black flag, 2: white flag, 3: black pieces,
                 // 4: white pieces, 5: black barriers, 6: white barriers


class Meichu {
private:
    bool game_over = 0;
    std::vector< std::vector<int> > board;
    std::vector<int> n_pieces = {5, 5};
    std::vector<int> n_barriers = {12, 12};
    std::vector<int> budgets = {150, 150};
    bool color;
    UCT<State, State::ActionStr> uct;

public:
    Meichu() : uct(5, 0) {
        board = vector< vector<int> >(9, vector<int>(9, 0));
        
        board[0][4] = 2;
        board[8][4] = 1;
        board[0][2] = board[0][6] = board[1][3] = board[1][5] = board[2][4] = 4;
        board[8][2] = board[8][6] = board[7][3] = board[7][5] = board[6][4] = 3;
        board[0][3] = board[0][5] = board[1][1] = board[1][4] = board[1][7] = board[2][1] = board[2][7]
            = board[3][2] = board[3][3] = board[3][4] = board[3][5] = board[3][6] = 6;
        board[8][3] = board[8][5] = board[7][1] = board[7][4] = board[7][7] = board[6][1] = board[6][7]
            = board[5][2] = board[5][3] = board[5][4] = board[5][5] = board[5][6] = 5;
    }

    ~Meichu() {
        for (auto v : board) v.clear();
        board.clear();
    }

    pair<bool, int> bid() {
        /************************************/
        /* You should modify this function! */
        /************************************/
        // color -> BLACK:0  WHITE:1
        // price -> an non-negative <int> value

        bool color_choice = BLACK;
        int price = 40;
        return {color_choice, price};
    }

    vector<int> make_decision(bool who) {
        /************************************/
        /* You should modify this function! */
        /************************************/
        // return format: {begin_x, begin_y, end_x, end_y}
        vector<int> ret(4, 0);
        if (who == color) {
            cout << "My move:\n";
            // Use AI to make decision !
            // cin is only for testing !
            //for (int i = 0; i < 4; ++i)
            //    cin >> ret[i];
            State state(board, budgets, n_pieces, n_barriers);
            auto act = uct.Run(state, who, 1000000, 30000);
            ret = {act.begin.x, act.begin.y, act.end.x, act.end.y};
        }
        return ret;
    }

    void start() {
    	// bid for black or white
        // please set "bid" function by yourself
        // bid function will return two <int> values
        auto bid_res = bid();
        bool color_choice = bid_res.first;
        int price = min(bid_res.second, MAX_PRICE);
        cout << "My bid: " << color_choice << " " << price << endl;

        bool opponent_color_choice;
        int opponent_price;
        // color -> BLACK:0  WHITE:1
        cout << "Please input opponent_color_choice: "; 
        cin >> opponent_color_choice;
        cout << "Please input opponent_price: "; 
        cin >> opponent_price;
        if ((color_choice != 0 && color_choice != 1) ||
            (opponent_color_choice != 0 && opponent_color_choice != 1)) {
                cout << "Invalid color choice." << endl;
                exit(0);
            }

        if (color_choice != opponent_color_choice) {
        	cout << "Case 1 : different color \n";
            color = color_choice;
        } else if (price > opponent_price) {
        	cout << "Case 2 : same color, I win \n";
            color = color_choice;
            budgets[color] -= max(0, opponent_price)+1;
        } else if (price < opponent_price) {
        	cout << "Case 3 : same color, I lose \n";
            color = !color_choice;
            budgets[!color] -= max(0, price)+1;
        } else {
            cout << "Tie!\n" << "Set my player color manually: ";
            cin >> color;
            if(color == color_choice)
                budgets[color] -= max(0, opponent_price)+1;
            else
                budgets[!color] -= max(0, price)+1;
        }

        cout << "My color is: ";
        cout << (color == BLACK ? "BLACK" : "WHITE") << "\n";

        int step = 0;
        while (!game_over) {
            show_board();

            if (step % 2 == BLACK) { // black's turn

                if (budgets[BLACK] <= 1) {
                    cout << "No budget to move! Only to pass!\n";
                    ++step;
                    continue;
                }
                if (color==BLACK) {
                    cout << "My color is BLACK and this is my turn:\n";
                    vector<int> move = make_decision(BLACK);
                    cout << "Move (" << move[0] << ", " << move[1] << ") to ("
                        << move[2] << ", " << move[3] << ").\n";
                    make_move(BLACK, move);
                } else {
                    cout << "Now it's opponent(BLACK)'s turn:\n";
                    vector<int> move(4);
                    cout << "Enter the opponent's move: ";
                    cin >> move[0] >> move[1] >> move[2] >> move[3];
                    make_move(BLACK, move);
                }
                ++step;
                

            } else { // white's turn

                if (budgets[WHITE] <= 1) {
                    cout << "No budget to move! Only to pass!\n";
                    ++step;
                    continue;
                }
                if (color==WHITE) {
                    cout << "My color is WHITE and this is my turn:\n";
                    vector<int> move = make_decision(WHITE);
                    cout << "Move (" << move[0] << ", " << move[1] << ") to ("
                        << move[2] << ", " << move[3] << ").\n";
                    make_move(WHITE, move);
                } else {
                    cout << "Now it's oppenent(WHITE)'s turn:\n";
                    vector<int> move(4);
                    cout << "Enter the opponent's move: ";
                    cin >> move[0] >> move[1] >> move[2] >> move[3];
                    make_move(WHITE, move);
                }
                ++step;

            }

        }
        terminate();
    }

    vector< vector<bool> > get_pieces(bool color) {
        /******************************************************/
        /* You can remove this function if you don't need it. */
        /******************************************************/
        vector< vector<bool> >b(9, vector<bool>(9, 0));
        int target = color == BLACK ? 4 : 3;
        for (int i = 0; i < 9; ++i)
            for (int j = 0; j < 9; ++j)
                if (board[i][j] == target)
                    b[i][j] = 1;
        return b;
    }

    vector< vector<bool> > get_barriers(bool color) {
        /******************************************************/
        /* You can remove this function if you don't need it. */
        /******************************************************/
        vector< vector<bool> >b(9, vector<bool>(9, 0));
        int target = color == BLACK ? 6 : 5;
        for (int i = 0; i < 9; ++i)
            for (int j = 0; j < 9; ++j)
                if (board[i][j] == target)
                    b[i][j] = 1;
        return b;
    }

    void make_move(bool who, vector<int> move) {
        int piece = who == BLACK ? 3 : 4;
        int begin_x = move[0], begin_y = move[1], end_x = move[2], end_y = move[3];

        // check valid
        if ((board[begin_x][begin_y] != piece) ||
            (begin_x < 0 || begin_x > 8) ||
            (begin_y < 0 || begin_y > 8) ||
            (end_x < 0 || end_x > 8) ||
            (end_y < 0 || end_y > 8) ||
            (begin_x != end_x && begin_y != end_y) || // should move along the axis
            (begin_x == end_x && begin_y == end_y)    // no passing
            ) {
            cout << "Invalid values." << endl;
            exit(0);
        }

        // check along the path
        int sum = 0;
        if (begin_x == end_x) { // move horizontally
            int start = min(begin_y, end_y), end = max(begin_y, end_y);
            for (int i = start+1; i < end; ++i)
                sum += board[begin_x][i];
        } else { // move vertically
            int start = min(begin_x, end_x), end = max(begin_x, end_x);
            for (int i = start+1; i < end; ++i)
                sum += board[i][begin_y];
        }
        if (sum > 0) {
            cout << "Invalid path." << endl;
            exit(0);
        }

        int distance = abs(begin_x-end_x) + abs(begin_y-end_y);
        if (who == BLACK) {
            budgets[BLACK] -= (1 + distance);
            if (budgets[BLACK] < 0) {
                cout << "Budget lower than 0." << endl;
                exit(0);
            }
            switch (board[end_x][end_y]) {
                case 0:
                    break;
                case 2:
                    game_over = 1;
                    break;
                case 6:
                    cout << "Break a white barrier!\n";
                    n_barriers[WHITE]--;
                    budgets[BLACK] -= OPPONEN_BARRIER_COST;
                    break;
                case 4:
                    cout << "Take a white piece!\n";
                    n_pieces[WHITE]--;
                    if (n_pieces[WHITE] == 0) game_over = 1;
                    break;
                case 5:
                    cout << "Break a black barrier!\n";
                    n_barriers[BLACK]--;
                    budgets[BLACK] -= SELF_BARRIER_COST;
                    break;
                default:
                    cout << "Do not move your piece to an occupied place!\n";
                    exit(0);
            }
            if (budgets[BLACK] < 0) {
                cout << "Budget lower than 0." << endl;
                exit(0);
            }
        } else {
            budgets[WHITE] -= (1 + distance);
            if (budgets[WHITE] < 0) {
                cout << "Budget lower than 0." << endl;
                exit(0);
            }
            switch (board[end_x][end_y]) {
                case 0:
                    break;
                case 1:
                    game_over = 1;
                    break;
                case 5:
                    cout << "Break a black barrier!\n";
                    n_barriers[BLACK]--;
                    budgets[WHITE] -= OPPONEN_BARRIER_COST;
                    break;
                case 3:
                    cout << "Take a black piece!\n";
                    n_pieces[BLACK]--;
                    if (n_pieces[BLACK] == 0) game_over = 1;
                    break;
                case 6:
                    cout << "Break a white barrier!\n";
                    n_barriers[WHITE]--;
                    budgets[WHITE] -= SELF_BARRIER_COST;
                    break;
                default:
                    cout << "Do not move your piece to an occupied place!\n";
                    exit(0);
            }
            if (budgets[WHITE] < 0) {
                cout << "Budget lower than 0." << endl;
                exit(0);
            }
        }

        // move
        board[begin_x][begin_y] = 0;
        board[end_x][end_y] = piece;
        if (budgets[BLACK] <= 1 && budgets[WHITE] <= 1)
            game_over = 1;
    }

    void terminate() {
        if (board[0][4] == 3) cout << "BLACK wins!" << endl;
        else if (board[8][4] == 4) cout << "WHITE wins!" << endl;
        else if (n_pieces[BLACK] > n_pieces[WHITE]) cout << "BLACK wins!" << endl;
        else if (n_pieces[BLACK] < n_pieces[WHITE]) cout << "WHITE wins!" << endl;
        else if (n_barriers[BLACK] > n_barriers[WHITE]) cout << "BLACK wins!" << endl;
        else if (n_barriers[BLACK] < n_barriers[WHITE]) cout << "WHITE wins!" << endl;
        else cout << "Draw!" << endl;
    }

    void show_board() {
        cout << "--------------------------\n"
            << "Budget:\n"
            << "        Black: " << budgets[BLACK] << "\n"
            << "        White: " << budgets[WHITE] << "\n"
            << "n_pieces:\n"
            << "        Black: " << n_pieces[BLACK] << "\n"
            << "        White: " << n_pieces[WHITE] << "\n"
            << "n_barriers:\n"
            << "        Black: " << n_barriers[BLACK] << "\n"
            << "        White: " << n_barriers[WHITE] << "\n\n"
            << " y  0  1  2  3  4  5  6  7  8\n"
            <<"x  ";
        for (int i = 0; i < 9; ++i) {
        	cout << Horizon << " ";
		}
        cout << endl;    
        for (int i = 0; i < 9; ++i) {
            cout << i << " |";
            for (int j = 0; j < 9; ++j)
                cout << S[board[i][j]] << "|";
            cout << endl << "   "; 
            for (int j = 0; j < 9; ++j) {
        	    cout << Horizon << " ";
		    }
		    cout << endl; 
        }
        cout << "--------------------------\n";
    }
};

int main() {
    ios_base::sync_with_stdio(false);

    Meichu game;
    game.start();
    system("PAUSE");
    return 0;
}
