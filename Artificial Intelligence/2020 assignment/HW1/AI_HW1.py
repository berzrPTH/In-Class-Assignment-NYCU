import queue
from copy import deepcopy

class TicTacToe:
    def __init__(self, board, mode):
        self.board = board
        self.p1 = 1
        self.p2 = -1
        self.hist = [[] for i in range(5)]
        # mode = {0: normal, 1: with alpha-beta pruning}
        self.mode = mode

    def minimax(self):
        return self.max_value(deepcopy(self.board), depth=0, a=float('-inf'), b=float('inf'))

    def max_value(self, state, depth, a, b):
        if self.terminal_test(state, self.p2):
            value = self.win(state, self.p2)
            self.hist[depth].append((deepcopy(state), value))
            return value

        value = float('-inf')
        for i, action in enumerate(self.actions(state, self.p1)):
            new_state = self.result(state, action, self.p1)
            value = max(value, self.min_value(new_state, depth+1))
            if mode:
                a = max(a, value)
                if b <= a:
                    break


        self.hist[depth].append((deepcopy(state), value))
        return value
    
    def min_value(self, state, depth, a, b):
        if self.terminal_test(state, self.p1):
            value = self.win(state, self.p1)
            self.hist[depth].append((deepcopy(state), value))
            return self.win(state, self.p1)

        value = float('inf')
        for i, action in enumerate(self.actions(state, self.p2)):
            new_state = self.result(state, action, self.p2)
            value = min(value, self.max_value(new_state, depth+1))
            if mode:
                b = min(b, value)
                if b <= a:
                    break
        self.hist[depth].append((deepcopy(state), value))
        return value

    def terminal_test(self, state, player):
        return len(self.actions(state, player)) == 0 or self.win(state, player) != 0
    
    def win(self, state, player):
        for i in range(3):
            if state[i][0] == state[i][1] == state[i][2] == player:
                return player
            if state[0][i] == state[1][i] == state[2][i] == player:
                return player

        if state[0][0] == state[1][1] == state[2][2] == player:
            return player
        if state[0][2] == state[1][1] == state[2][0] == player:
            return player

        return 0
    
    def actions(self, state, player):
        cells = []
        for i, row in enumerate(state):
            for j, cell in enumerate(row):
                if cell == 0:
                    cells.append((i, j))
        
        return cells

    def result(self, state, action, player):
        new_state = deepcopy(state)
        i, j = action
        new_state[i][j] = player
        return new_state

board = [
    [-1,  1, -1],
    [ 1,  0,  0],
    [ 0, -1,  0]
]
t = TicTacToe(board)
result = t.minimax()

"""
for depth, row in enumerate(t.hist):
    print('depth =', depth)
    for node in row:
        state, value= node
        print(state)
        print(value)
"""

print(result)
