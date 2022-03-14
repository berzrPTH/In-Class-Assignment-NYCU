# AI_Prog3

## Abstract

這次的作業是使用 knowledge base 來找尋 minesweeper 的解答，而 knowledge base 中的 sentences 表達方式為 propositional logic。

為了觀察 logical inference 在 propositional logic 在解 minesweeper 中的表現如何，做了以下實驗：

1. 給定不同數量的開局 safe cells，觀察正確（找出所有的地雷）率變化。
2. 給定不同數量的開局 safe cells，觀察到程序終止所需的時間變化。
3. 給定不同的地雷佔比，觀察正確率變化。
4. 給定不同的地雷佔比，觀察到程序終止所需的時間變化。

## Result

因為 Medium 與 Hard 兩者所花的時間較多，以下大部分都使用 Easy 難度以及自訂難度運行。

我的程式 Easy 的正確率為 0.96。Medium 的正確率為 0.96，在 50 個回合內。

### 給定不同數量的開局 safe cells

Iterations = 50.  
Easy (9x9 board, 10 mines)、Medium (16x16 board, 25 mines).  
Column 所代表的是在不同 ratio 的開局 safe cells 數目下的正確率（#init_safe_cells = round( sqrt( board_size ) * ratio）。

**Accuracy**

|        | 1.00 | 2.00 | 3.00 |
| ------ | ---- | ---- | ---- |
| Easy   | 0.96 | 0.92 | 0.98 |
| Medium | 0.96 | n/a  | n/a  |

從 Easy 的結果可看出基本在 1 倍時就已達到九成以上的正確率，更多的 initial safe cell 對於正確率會提高，但因為本身就很接近全對的正確率，所以提高的幅度也不大。  
之中 2 倍反而有較低的正確率我覺得是隨機產生 initial safe cells 導致的，所擁有的資訊越多理應得到越高的正確率。

因為 Medium 的 board size 較大運行較慢且增加 #init_safe_cells 帶來的變化較小，因此沒有繼續做 2 倍與 3 倍。

#### 較少的開局 safe cells

**Accuracy**

|        | 0.25 | 0.50 | 0.75 | 1.00 |
| ------ | ---- | ---- | ---- | ---- |
| Easy   | 0.52 | 0.76 | 0.80 | 0.96 |
| Medium | 0.8  | 0.94 | 0.92 | 0.96 |

可以看到隨著 #init_safe_cells 的增加，正確率也跟著增加，符合我們的推論：起始資訊越多越能得到較高的正確率。  
Medium 的正確率皆較 Easy 高推測是因為題目給定的 Medium 的地雷比例還比 Easy 的地雷比例低（Medium：Easy = 0.10：0.12）。

**Rule generated**

因為不同的實作方式都會影響到所需時間，所以這邊討論 KB 內的 rule 數目。如果有較多的 rule 被 generate，則對 KB 作相關操作如內部互相 resolution 等所需的時間也會增加。以下專注在討論在 Easy 成功時的平均 generated rule 數目。

|       | 0.25  | 0.50   | 0.75   | 1.00   | 2.00   | 3.00   |
| ----- | ----- | ------ | ------ | ------ | ------ | ------ |
| #rule | 967.0 | 1321.4 | 1551.2 | 1765.6 | 1706.3 | 1009.4 |
| time  | 0.45  | 0.67   | 0.85   | 1.01   | 0.94   | 0.66   |

可以看到在 ratio 小於 1 時隨著 #init_safe_cells 的增加，產生過的 rule 數目也隨之增加，原因應為起始在 KB0 中的 rule 數增多，使得程式在初期加入了許多用那些 safe cell 的 hint 所產生的 clause，如此一來經過 resolution 等操作也會產生較多 rule。  
在 ratio 大於 1 時隨著 #init_safe_cells 的增加，產生的 rule 數反而減少，推論應是因為已確定的位置較多，需要進行的 iteration 次數較少所導致。

Custom difficulty, 12x12 board, 18 mines (same ratio as Easy one).

|       | 0.25   | 0.50   | 0.75   | 1.00   | 2.00   | 3.00   |
| ----- | ------ | ------ | ------ | ------ | ------ | ------ |
| #rule | 2786.5 | 3987.8 | 4056.4 | 4775.3 | 4810.4 | 4662.1 |
| time  | 2.38   | 3.46   | 4.72   | 4.95   | 5.63   | 5.51   |

也可得出相同推論。

### 給定不同的地雷佔比

Iterations = 50.  
9x9 board, #init_safe_cells = round( sqrt( board_size ) ).

|         | 0.1    | 0.12   | 0.15   | 0.18   | 0.20   |
| ------- | ------ | ------ | ------ | ------ | ------ |
| accuacy | 0.96   | 0.90   | 0.88   | 0.7    | 0.52   |
| #rule   | 1445.4 | 1729.2 | 2143.2 | 3186.4 | 3059.5 |

（在題目中給定的地雷佔比， Easy 為 0.12、Medium 為 0.10、Hard 為 0.20。）

可以看到正確率都隨著地雷數增加而減少。一是因為我們只能從確定的 safe celle 的 hint 得到資訊，只要有被地雷包圍住的區塊，且內部沒有已知的 safe cells，此時除非 global constraint 剛好能用，否則程式都無法找出解，因為我們沒有辦法推出包含內部區塊的 clause；二是會有某些情況下，我們需要去猜測地雷的分布，現有的資訊無法給我們確定的答案。第二種情況較難討論。

Generated rule 數目隨著地雷數增加而增加，推測是因為在 generating clause 時，作排列組合的 n 隨之容易較大，因此才在 KB  中產生較多的 rule，

為了印證推論，又做了較大的盤面的實驗。

12x12 board.

|          | 0.1    | 0.12   | 0.15   | 0.18   | 0.20   |
| -------- | ------ | ------ | ------ | ------ | ------ |
| accuracy | 1.0    | 0.88   | 0.88   | 0.68   | 0.4    |
| #rule    | 2429.6 | 4676.5 | 5384.0 | 7168.8 | 7657.1 |

可以看到不管是較大或較小的盤面正確率都隨著地雷增加而減少，generated rule 隨之增加。

而在較大的盤面中，儘管地雷佔比相同，正確率卻是較大的盤面較小。前面推測正確率降低的原因是由於被地雷包圍的區塊導致，也可以由這個得到一點佐證，較大的盤面有較多地雷，較容易有包圍的 pattern 產生。

## Further discussion

### 如何在 stuck game 作 guessing

當一個盤面 stuck 時，通常是有一塊被地雷包覆住的區域，而區域內還有其他的 safe cells。我們的目標是希望作出較好的 guess 來讓 game 能繼續進行，這時進行猜測時選擇周圍的 unmarked cells 最少的 cell，這樣有較大機率可以利用已知資訊再繼續進行推論。  
同時如果是非包圍情況，要猜測地雷分布時，未得出的區域通常會成群出現。此時不同區域的 cell 是為 safe cell 的機率也不同，則應該先從機率高的先猜測。

## Appendix

```python
from itertools import combinations

POSITIVE = 1
NEGATIVE = -1

class CNF():
    def __init__(self, expr):
        # expr = set( tuple( coordinate, POSITIVE(mine)/NEGATIVE(safe) ) )
        self.expr = expr
        return None

    def equal(self, cnf):
        return self.expr == cnf.expr

    def stricter(self, cnf):
        return len(self.expr) < len(cnf.expr) and \
            self.expr == self.expr.intersection(cnf.expr)
    
    def to_coordinates(self):
        result = set()
        for literal in self.expr:
            coordinate, state = literal
            result.add(coordinate)
        return result
    
class KnowledgeBase():
    def __init__(self):
        self.KB = set()
        return None
    
    def tell_KB(self, cnf):
        tmp_KB = self.KB.copy()
        for rule in tmp_KB:
            if rule.equal(cnf):
                return 0
            elif rule.stricter(cnf):
                return 0
            elif cnf.stricter(rule):
                self.KB.remove(rule)

        self.KB.add(cnf)
        return 1

    def ask_KB(self):
        for rule in self.KB:
            if len(rule.expr) == 1:
                return rule
        return None

    def remove(self, rule):
        if rule in self.KB:
            self.KB.remove(rule)
            return True
        return False

class GameControl():
    def __init__(self, difficulty, length=None, width=None, num_mines=None,
                 init_factor=1):
        self.init_factor = init_factor
        difficulties = {'Easy', 'Medium', 'Hard', 'Custom'}
        assert difficulty in difficulties
        if difficulty == 'Easy':
            self.length = 9
            self.width = 9
            self.num_mines = 10
        elif difficulty == 'Medium':
            self.length = 16
            self.width = 16
            self.num_mines = 25
        elif difficulty == 'Hard':
            self.length = 30
            self.width = 16
            self.num_mines = 99
        elif difficulty == 'Custom':
            assert num_mines < length * width
            self.length = length
            self.width = width
            self.num_mines = num_mines

        self.board_size = self.length * self.width
        self.initialize()
        return None

    def initialize(self):
        self.rng = np.random.default_rng()
        self.mines, self.hints = self.generate_board()
        self.init_safe_cells = self.generate_init_safe_cells()
        return None

    def generate_board(self):
        # Place mines.
        mines_numbers = self.rng.choice(
            self.board_size, size=self.num_mines, replace=False
        )
        mines = set()
        for number in mines_numbers:
            mines.add(self.get_coordinate(number))
        
        hints = self.get_hints(mines) # {Safe cells: hints}
        return mines, hints

    # Return the coordinate of given place number on board.
    def get_coordinate(self, number):
        row = number // self.width
        column = number % self.width
        return row, column

    # Return hints of safe cells from given mines.
    def get_hints(self, mines):
        result = dict()
        for row in range(self.length):
            for column in range(self.width):
                coordinate = row, column
                # If the coordinate is a safe cell.
                if coordinate not in mines:
                    surrounds = self.get_surrounds(coordinate)
                    # Count the surrounding mines.
                    count = 0
                    for cell in surrounds:
                        if cell in mines:
                            count += 1
                    result.update({coordinate: count})
        return result

    # Return the surrounds of given coordinate.
    def get_surrounds(self, coordinate):
        offsets = [(-1, -1), (-1, 0), (-1, 1),
                   ( 0, -1),          ( 0, 1),
                   ( 1, -1), ( 1, 0), ( 1, 1)]
        surrounds = list()
        for offset in offsets:
            row, column = coordinate
            x, y = offset
            surrounds.append((row+x, column+y))
        surrounds = self.possible(surrounds)
        return surrounds
    
    def possible(self, coordinates):
        result = list()
        for coordinate in coordinates:
            row, column = coordinate
            if 0 <= row < self.length and 0 <= column < self.width:
                result.append(coordinate)
        return result

    # Return list contained random chose safe cells.
    def generate_init_safe_cells(self):
        num_init_safe_cells = min(
            int(round( np.sqrt(self.board_size) ) * self.init_factor),
            self.board_size - self.num_mines
        )
        numbers = self.rng.choice(
            len(self.hints), size=num_init_safe_cells, replace=False
        )
        safe_cells = list(self.hints)
        init_safe_cells = [safe_cells[i] for i in numbers]
        return init_safe_cells

class Player():
    def __init__(self):
        self.game = None
        self.KB0 = None
        self.KB = None
        self.safes = None
        self.mines = None
        return None

    def restart(self, difficulty, init_factor=1,
                length=None, width=None, num_mines=None):
        self.game = GameControl(difficulty, length, width, num_mines, init_factor)
        self.safes = set()
        self.mines = set()
        # Initialize KB0.
        self.KB0 = KnowledgeBase()
        for cell in self.game.init_safe_cells:
            self.KB0.tell_KB(CNF( {(cell, NEGATIVE)} ))
        # Update safes and mines.
        for cnf in self.KB0.KB:
            (atomic,) = cnf.expr
            coordinate, state = atomic
            if state == POSITIVE:
                self.mines.add(coordinate)
            elif state == NEGATIVE:
                self.safes.add(coordinate)

        # Initialize KB.
        self.KB = KnowledgeBase()

        return None

    def main(self):
        count = 0 # count node in KB
        # Global constraint.
        max_mines = len(self.game.mines)
        max_safes = len(self.game.hints)
        grids = {(i, j) for i in range(self.game.length)
                        for j in range(self.game.width)}
        
        count += self.update_KB()
        while self.KB.ask_KB() != None:
            # Ask KB.
            rule = self.KB.ask_KB()
            self.KB.remove(rule)

            # Tell KB0.
            self.KB0.tell_KB(rule)

            # Update marked set.
            (atomic,) = rule.expr
            coordinate, state = atomic
            if state == POSITIVE:
                self.mines.add(coordinate)
            elif state == NEGATIVE:
                self.safes.add(coordinate)

            # Check global constraint.
            if len(self.mines) == max_mines:
                for cell in grids:
                    if cell not in self.mines and cell not in self.safes:
                        self.safes.add(cell)
                break
            elif len(self.safes) == max_safes:
                for cell in grids:
                    if cell not in self.mines and cell not in self.safes:
                        self.mines.add(cell)
                break

            # Tell KB.
            count += self.update_KB()

        self.marked, self.unmarked = self.clf_marked(grids)
        return self.marked == grids, count

    def update_KB(self):
        count = 0
        for coordinate in self.safes:
            surrounds = self.game.get_surrounds(coordinate)
            marked, unmarked = self.clf_marked(surrounds)
            m = len(unmarked)
            marked_mines = {cell for cell in marked if cell in self.mines}
            n = self.game.hints[coordinate] - len(marked_mines)

            if m == n:
                for cell in unmarked:
                    count += self.KB.tell_KB(CNF( {(cell, POSITIVE)} ))
            elif n == 0:
                for cell in unmarked:
                    count += self.KB.tell_KB(CNF( {(cell, NEGATIVE)} ))
            else:
                sentences_positive = combinations(unmarked, m-n+1)
                sentences_negative = combinations(unmarked, n+1)
                for sentence in sentences_positive:
                    tmp_cnf = self.to_cnf(sentence, POSITIVE)
                    for rule in self.KB0.KB:
                        if self.resolution(rule, tmp_cnf) != None:
                            tmp_cnf = self.resolution(rule, tmp_cnf)
                    if tmp_cnf.expr != set():
                        count += self.KB.tell_KB(tmp_cnf)
                for sentence in sentences_negative:
                    tmp_cnf = self.to_cnf(sentence, NEGATIVE)
                    for rule in self.KB0.KB:
                        if self.resolution(rule, tmp_cnf) != None:
                            tmp_cnf = self.resolution(rule, tmp_cnf)
                    if tmp_cnf.expr != set():
                        count += self.KB.tell_KB(tmp_cnf)

        # Do resolution in KB itself.
        tmp_KB = self.KB.KB.copy()
        for cnf in tmp_KB:
            remove_set = set()
            add_set = set()
            if 0 < len(cnf.expr) <= 2:
                for rule in self.KB.KB:
                    tmp_cnf = self.resolution(rule, cnf)
                    if tmp_cnf != None:
                        remove_set.add(rule)
                        add_set.add(tmp_cnf)
            for rule in remove_set:
                self.KB.remove(rule)
            for rule in add_set:
                if rule.expr != set():
                    count += self.KB.tell_KB(rule)
        return count

    def clf_marked(self, cells):
        marked = set()
        unmarked = set()
        for cell in cells:
            if cell in self.safes or cell in self.mines:
                marked.add(cell)
            else:
                unmarked.add(cell)
        return marked, unmarked
    
    def to_cnf(self, sentence, state):
        result = set()
        for literal in sentence:
            result.add((literal, state))
        return CNF(result)

    def resolution(self, cnf1, cnf2):
        result_expr = None
        diff1 = cnf1.expr.difference(cnf2.expr)
        diff2 = cnf2.expr.difference(cnf1.expr)
        coordinate1 = {literal[0] for literal in diff1}
        coordinate2 = {literal[0] for literal in diff2}
        complementary = coordinate1.intersection(coordinate2)
        if len(complementary) == 1:
            (coordinate,) = complementary
            result_expr = cnf1.expr.union(cnf2.expr)
            result_expr.remove((coordinate, POSITIVE))
            result_expr.remove((coordinate, NEGATIVE))
            return CNF(result_expr)
        return None

"""
player = Player()
player.restart('Easy')
success = player.main()
print(success)
"""
```

