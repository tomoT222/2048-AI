#include <bits/stdc++.h>
#include <sstream>
#include <utility>
#include <assert.h>
#include <math.h>
#include <chrono>
#include <algorithm>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#define rep(i, x) for (int i = 0; i < x; i++)
#define rep2(i, x) for (int i = 1; i <= x; i++)
using namespace std;

using Action = int;
using Actions = vector<int>;
using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;
// 時間を管理するクラス
class TimeKeeper
{
private:
    chrono::high_resolution_clock::time_point start_time_;
    int64_t time_threshold_;

public:
    // 時間制限をミリ秒単位で指定してインスタンスをつくる。
    TimeKeeper(const int64_t &time_threshold)
        : start_time_(chrono::high_resolution_clock::now()),
          time_threshold_(time_threshold)
    {
    }

    // インスタンス生成した時から指定した時間制限を超過したか判定する。
    bool isTimeOver() const
    {
        auto diff = chrono::high_resolution_clock::now() - this->start_time_;
        return chrono::duration_cast<chrono::milliseconds>(diff).count() >= time_threshold_;
    }
};

template <typename T>
bool chmax(T &a, const T &b)
{
    if (a < b)
    {
        a = b;
        return true;
    }
    return false;
}
template <typename T>
bool chmin(T &a, const T &b)
{
    if (a > b)
    {
        a = b;
        return true;
    }
    return false;
}

uint64_t xorshift64()
{
    static uint64_t Seed = 123456789;
    Seed ^= Seed << 13;
    Seed ^= Seed >> 17;
    Seed ^= Seed << 7;
    return Seed;
}
double randouble()
{
    return double(xorshift64()) / double(uint64_t(-1));
}

//////////////////////////////
////
////     pre-processing
////

constexpr int MAX = 16;     // Maximum number is 2^16.
constexpr int MAX4 = 65536; // 16^4.

// hexadecimal[a][b][c][d] = Hexadecimal of a,b,c,d.
int hexadecimal[MAX][MAX][MAX][MAX];
void make_hexadecimal()
{
    for (int a = 0; a < MAX; a++)
    {
        for (int b = 0; b < MAX; b++)
        {
            for (int c = 0; c < MAX; c++)
            {
                for (int d = 0; d < MAX; d++)
                {
                    hexadecimal[a][b][c][d] = d + MAX * (c + MAX * (b + MAX * a));
                }
            }
        }
    }
}

// Power16[a] = 16^a, Power16[0] = 1
int Power16[7];
void make_Power16()
{
    int num = 1;
    for (int i = 0; i <= 6; i++)
    {
        Power16[i] = num;
        num *= 16;
    }
}

// Power2[a] = 2^a, Power2[0] = 0
int Power2[MAX];
void make_Power2()
{
    int two = 2;
    for (int i = 1; i < MAX; i++)
    {
        Power2[i] = two;
        two *= 2;
    }
}

// Log2[A] = log_2_A, Log2[0] = 0
int Log2[1 << MAX];
void make_Log2()
{
    int two = 2;
    for (int i = 1; i < MAX; i++)
    {
        Log2[two] = i;
        two *= 2;
    }
}

// new_line[ hexadecimal[a][b][c][d] ] = the column after moving (a, b, c, d) to the left
int new_line[MAX4];
// change_line[ hexadecimal[a][b][c][d] ] = Do the column(a, b, c, d) change before and after moving it to the left?
bool change_line[MAX4];
// addition_score[ hexadecimal[a][b][c][d] ] = How many points can I get by moving the column(a, b, c, d) to the left?
double addition_score[MAX4];
void make_new_line()
{
    for (int a = 0; a < MAX; a++)
    {
        for (int b = 0; b < MAX; b++)
        {
            for (int c = 0; c < MAX; c++)
            {
                for (int d = 0; d < MAX; d++)
                {
                    int hex = hexadecimal[a][b][c][d];
                    int grid[4];
                    grid[0] = Power2[a];
                    grid[1] = Power2[b];
                    grid[2] = Power2[c];
                    grid[3] = Power2[d];
                    vector<int> v;
                    bool last = false; // 最後が*2されたか
                    for (int i = 0; i < 4; i++)
                    {
                        if (grid[i] != 0)
                        {
                            if (v.size() != 0 && v.back() == grid[i] && !last)
                            {
                                v[v.size() - 1] *= 2;
                                addition_score[hex] += grid[i] * 2;
                                last = true;
                            }
                            else
                            {
                                v.push_back(grid[i]);
                                last = false;
                            }
                        }
                    }
                    while (v.size() != 4)
                    {
                        v.push_back(0);
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        v[i] = Log2[v[i]];
                    }
                    new_line[hex] = hexadecimal[v[0]][v[1]][v[2]][v[3]];
                    change_line[hex] = ((v[0] != a) | (v[1] != b) | (v[2] != c) | (v[3] != d));
                }
            }
        }
    }
}

vector<vector<int>> zeros(4, vector<int>(4, 0));
class Class2048
{
public:
    long long seed;
    vector<vector<int>> grid;
    int score;

    Class2048(int _seed = 0, vector<vector<int>> _grid = zeros)
    {
        seed = 13339144;
        grid = zeros;
        grid[1][3] = 1;
        grid[3][1] = 2;
        score = 0;
    }

    // 全体において
    //  0:終わってない、!0:終わり
    int Done() const
    {
        for (int i = 0; i < 4; i++)
        {
            if (change_line[hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]])
            {
                return 0;
            }
            if (change_line[hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]])
            {
                return 0;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            if (change_line[hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]])
            {
                return 0;
            }
            if (change_line[hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]])
            {
                return 0;
            }
        }
        return score;
    }

    // 現在のプレイヤーが可能な行動を全て取得する
    Actions legalActions() const
    {
        Actions actions;
        for (int j = 0; j < 4; j++)
        {
            if (change_line[hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]])
            {
                actions.push_back(0);
                break;
            }
        }
        for (int i = 0; i < 4; i++)
        {
            if (change_line[hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]])
            {
                actions.push_back(1);
                break;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            if (change_line[hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]])
            {
                actions.push_back(2);
                break;
            }
        }
        for (int i = 0; i < 4; i++)
        {
            if (change_line[hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]])
            {
                actions.push_back(3);
                break;
            }
        }
        return actions;
    }

    // 指定したactionでゲームを1ターン進める
    void advance(const Action action)
    {
        ///////////////////////
        //
        //  Move
        //
        if (action == 0)
        { // U
            for (int j = 0; j < 4; j++)
            {
                int hex = hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]];

                score += addition_score[hex];

                int nl = new_line[hex];
                // for (int i = 0; i < 4; i++)
                // {
                //     grid[i][j] = (nl & (Power16[4 - i] - Power16[3 - i])) / Power16[3 - i];
                //     assert(grid[i][j] == ((nl & (0xF000 >> (4 * i))) >> (12 - 4 * i)));
                // }
                for (int i = 0; i < 4; i++)
                {
                    grid[i][j] = ((nl & (0xF000 >> (4 * i))) >> (12 - 4 * i));
                }
            }
        }
        if (action == 1)
        { // L
            for (int i = 0; i < 4; i++)
            {
                int hex = hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]];

                score += addition_score[hex];

                int nl = new_line[hex];
                for (int j = 0; j < 4; j++)
                {
                    grid[i][j] = ((nl & (0xF000 >> (4 * j))) >> (12 - 4 * j));
                }
            }
        }
        if (action == 2)
        { // D
            for (int j = 0; j < 4; j++)
            {
                int hex = hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]];

                score += addition_score[hex];

                int nl = new_line[hex];
                for (int i = 0; i < 4; i++)
                {
                    grid[i][j] = ((nl & (0x000F << (4 * i))) >> (4 * i));
                }
            }
        }
        if (action == 3)
        { // R
            for (int i = 0; i < 4; i++)
            {
                int hex = hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]];

                score += addition_score[hex];

                int nl = new_line[hex];
                for (int j = 0; j < 4; j++)
                {
                    grid[i][j] = ((nl & (0x000F << (4 * j))) >> (4 * j));
                }
            }
        }

        vector<int> freeCells;
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++)
            {
                if (grid[i][j] == 0)
                {
                    freeCells.push_back(i + j * 4);
                }
            }
        }
        int spawnIndex = freeCells[(int)seed % freeCells.size()];
        int value = ((seed & 0x10) == 0 ? 2 : 4);
        grid[spawnIndex % 4][spawnIndex / 4] = Log2[value];

        seed = seed * seed % 50515093L;
    }

    void Out()
    {
        rep(i, 4)
        {
            rep(j, 4)
            {
                if (j != 0)
                {
                    cout << ' ';
                }
                cout << Power2[grid[i][j]];
            }
            cout << endl;
        }
    }
};

using State = Class2048;

// ランダムに行動を決定する
Action randomAction(const State &state)
{
    auto legal_actions = state.legalActions();
    return legal_actions[xorshift64() % (legal_actions.size())];
}

int MOVE_CNT;

namespace montecarlo
{
    // 配列の最大値のインデックスを返す
    /*int argMax(const vector<double>& x) {
        return distance(x.begin(), max_element(x.begin(), x.end()));
    }*/
    // ランダムプレイアウトをして勝敗スコアを計算する
    double playout(State *state)
    { // const&にすると再帰中にディープコピーが必要になるため、高速化のためポインタにする。(constでない参照でも可)
        // int st = clock();
        int done = state->Done();
        if (done != 0)
            return done;
        // cerr << ' ' << clock() - st << endl;
        // st = clock();
        state->advance(randomAction(*state));
        return playout(state);
        // cerr << ' ' << clock() - st << endl;
    }

    constexpr const double C = 1.;            // UCB1の計算に使う定数
    constexpr const int EXPAND_THRESHOLD = 5; // ノードを展開する閾値

    // MCTSの計算に使うノード
    class Node
    {
    private:
        State state_;

    public:
        vector<Node> child_nodes;
        double w_;
        double n_;

        // ノードの評価を行う
        double evaluate()
        {
            int done = state_.Done();
            if (done != 0)
            {
                double value = done;
                this->w_ += (value - w_) / (double)(n_ + 1);
                ++this->n_;
                return value;
            }
            if (this->child_nodes.empty())
            {

                State state_copy = this->state_;

                double st = clock();
                double value = playout(&state_copy);
                // cerr << "playout " <<  (clock() - st) / CLOCKS_PER_SEC << endl;

                this->w_ += (value - w_) / (double)(n_ + 1);
                ++this->n_;

                if (this->n_ == EXPAND_THRESHOLD)
                    this->expand();

                return value;
            }
            else
            {
                double value = this->nextChiledNode().evaluate();
                this->w_ += (value - w_) / (double)(n_ + 1);
                ++this->n_;
                return value;
            }
        }

        // ノードを展開する
        void expand()
        {
            auto legal_actions = this->state_.legalActions();
            this->child_nodes.clear();
            for (const auto action : legal_actions)
            {
                this->child_nodes.emplace_back(this->state_);
                this->child_nodes.back().state_.advance(action);
            }
        }

        // どのノードを評価するか選択する
        Node &nextChiledNode()
        {
            for (auto &child_node : this->child_nodes)
            {
                if (child_node.n_ == 0)
                    return child_node;
            }
            double t = 0;
            for (const auto &child_node : this->child_nodes)
            {
                t += child_node.n_;
            }
            double best_value = -INF;
            int best_i = -1;
            for (int i = 0; i < this->child_nodes.size(); i++)
            {
                const auto &child_node = this->child_nodes[i];
                double wr = child_node.w_ / child_node.n_;
                double bias = sqrt(2. * log(t) / child_node.n_);

                double ucb1_value = wr + (double)C * bias;
                if (ucb1_value > best_value)
                {
                    best_i = i;
                    best_value = ucb1_value;
                }
            }
            return this->child_nodes[best_i];
        }

        Node(const State &state) : state_(state), w_(0), n_(0) {}
    };

    // プレイアウト数を指定してMCTSで行動を決定する
    Action mctsAction(const State &state, const int playout_number)
    {
        Node root_node = Node(state);
        root_node.expand();
        for (int i = 0; i < playout_number; i++)
        {
            root_node.evaluate();
        }
        auto legal_actions = state.legalActions();

        double best_w = -1;
        int best_i = -1;
        assert(legal_actions.size() == root_node.child_nodes.size());
        for (int i = 0; i < legal_actions.size(); i++)
        {
            int w = root_node.child_nodes[i].w_;
            if (w > best_w)
            {
                best_i = i;
                best_w = w;
            }
        }
        return legal_actions[best_i];
    }

    // 制限時間(ms)を指定してMCTSで行動を決定する
    Actions mctsActionWithTimeThreshold(State &state, const int64_t time_threshold)
    {
        Node root_node = Node(state);
        root_node.expand();
        auto time_keeper = TimeKeeper(time_threshold);
        int cnt = 0;
        // int st = clock();
        for (;; cnt++)
        {
            if (time_keeper.isTimeOver())
            {
                break;
            }
            root_node.evaluate();
            // cerr << ' ' << clock() - st << endl;
        }
        // cerr << "play out cnt " << cnt << endl;

        Actions ret;
        rep(i, MOVE_CNT)
        {
            auto legal_actions = state.legalActions();

            double best_w = -1;
            int best_i = -1;
            if (root_node.child_nodes.size() == 0)
            {
                break;
            }
            assert(legal_actions.size() == root_node.child_nodes.size());
            for (int i = 0; i < legal_actions.size(); i++)
            {
                int n = root_node.child_nodes[i].n_;
                int w = root_node.child_nodes[i].w_;
                // cerr << "legal " << legal_actions[i] << ' ' << w << ' ' << n << endl;
                if (w > best_w)
                {
                    best_i = i;
                    best_w = w;
                }
            }
            Node _node = root_node.child_nodes[best_i];
            root_node = _node;
            state.advance(legal_actions[best_i]);
            ret.push_back(legal_actions[best_i]);
        }
        return ret;
    }
}
using montecarlo::mctsAction;
using montecarlo::mctsActionWithTimeThreshold;

char Answer[4] = {'U', 'L', 'D', 'R'};

int main()
{
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);


    make_hexadecimal();
    make_Power16();
    make_Power2();
    make_Log2();
    make_new_line();

    State state;
    bool first = true;
    int TL = 1490;
    MOVE_CNT = 100;
    string ans;
    int cnt = 0;
    bool end = false;

    // game loop
    while (1)
    {
        double T = clock();

        // int seed;
        // cin >> seed;

        // int Score;
        // cin >> Score;
        // cerr << Score << endl;

        // state.seed = seed;
        // state.score = Score;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                // int g;
                // cin >> g;
                // state.grid[i][j] = Log2[g];
            }
        }
        if (state.Done() != 0)
        {
            break;
        }

        Actions action = mctsActionWithTimeThreshold(state, TL);
        rep(i, action.size())
        {
            // if (state.Done() != 0)
            // {
            //     end = true;
            //     break;
            // }
            ans.push_back(Answer[action[i]]);
            // state.advance(action[i]);
            // cout << Answer[action[i]];
        }
        if (cnt % 100 == 0)
        {
            cout << cnt << ' ' << action.size() << endl;
            state.Out();
        }
        if (end)
        {
            break;
        }

        // cerr << (clock() - T) / CLOCKS_PER_SEC << endl;

        // cout << endl;
        first = false;
        TL = 5000;
        MOVE_CNT = 4;
        cnt++;
        // state.Out();
    }
    cout << cnt << endl;
    state.Out();
    cout << ans << endl;
}
/*
score
Class2048 - Done()
montecarlo - evaluate
*/
