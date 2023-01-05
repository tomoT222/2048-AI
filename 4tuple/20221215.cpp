#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#define rep(i, x) for (int i = 0; i < x; i++)
#define rep2(i, x) for (int i = 1; i <= x; i++)
using namespace std;

using ll = long long;
using ld = long double;

template <typename T>
bool chmax(T &a, const T &b)
{
    if (a < b)
    {
        a = b; // Update a with b.
        return true;
    }
    return false;
}
template <typename T>
bool chmin(T &a, const T &b)
{
    if (a > b)
    {
        a = b; // Update a with b.
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

constexpr int MAX = 16;     // Maximum number is 2^16.
constexpr int MAX4 = 65536; // 16^4.
constexpr int MAX6 = 16777216; // 16^4.

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

// hexadecimal6[a][b][c][d] = Hexadecimal of a,b,c,d.
int hexadecimal6[MAX][MAX][MAX][MAX];
void make_hexadecimal6()
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
ld addition_score[MAX4];
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
                    bool last = true; //最後が*2されたか
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
struct Agent
{
    long long seed = 2;
    vector<vector<int>> grid;

    Agent(vector<vector<int>> _grid = zeros)
    {
        grid = _grid;
    }

    // Can I move the current state?
    bool can_move(int d)
    {
        for (int i = 0; i < 4; i++)
        {
            if (d == 1)
            {
                if (change_line[hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]])
                {
                    return true;
                }
            }
            if (d == 3)
            {
                if (change_line[hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]])
                {
                    return true;
                }
            }
        }
        for (int j = 0; j < 4; j++)
        {
            if (d == 0)
            {

                if (change_line[hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]])
                {
                    return true;
                }
            }
            if (d == 2)
            {
                if (change_line[hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]])
                {
                    return true;
                }
            }
        }
        return false;
    }
    bool can_move_all()
    {
        for (int i = 0; i < 4; i++)
        {
            if (change_line[hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]])
            {
                return true;
            }
            if (change_line[hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]])
            {
                return true;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            if (change_line[hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]])
            {
                return true;
            }
            if (change_line[hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]])
            {
                return true;
            }
        }
        return false;
    }

    // Move the state to the direction d. Return the reward at that time.
    ld move_and_reward(int d)
    {
        bool flag = false;
        ld score = 0;

        ///////////////////////
        //
        //  Move
        //
        if (d == 0)
        { // U
            for (int j = 0; j < 4; j++)
            {
                int hex = hexadecimal[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]];

                flag |= change_line[hex];
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
        if (d == 1)
        { // L
            for (int i = 0; i < 4; i++)
            {
                int hex = hexadecimal[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]];

                flag |= change_line[hex];
                score += addition_score[hex];

                int nl = new_line[hex];
                for (int j = 0; j < 4; j++)
                {
                    grid[i][j] = ((nl & (0xF000 >> (4 * j))) >> (12 - 4 * j));
                }
            }
        }
        if (d == 2)
        { // D
            for (int j = 0; j < 4; j++)
            {
                int hex = hexadecimal[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]];

                flag |= change_line[hex];
                score += addition_score[hex];

                int nl = new_line[hex];
                for (int i = 0; i < 4; i++)
                {
                    grid[i][j] = ((nl & (0x000F << (4 * i))) >> (4 * i));
                }
            }
        }
        if (d == 3)
        { // R
            for (int i = 0; i < 4; i++)
            {
                int hex = hexadecimal[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]];

                flag |= change_line[hex];
                score += addition_score[hex];

                int nl = new_line[hex];
                for (int j = 0; j < 4; j++)
                {
                    grid[i][j] = ((nl & (0x000F << (4 * j))) >> (4 * j));
                }
            }
        }

        if (!flag)
        {
            // If the board doesn't change
            // cout << "the board doesn't change " << d << endl;
            // cout << can_move(d) << endl;
            // rep(i, 4)
            // {
            //     rep(j, 4)
            //     {
            //         cout << grid[i][j] << ' ';
            //     }
            //     cout << endl;
            // }
            score -= 0;
        }

        ///////////////////////
        //
        //  add new tile.
        //
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
        if (freeCells.size() == 0)
        {
            // If there were no free cells
            // cout << "there were no free cells " << d << endl;
            // rep(i, 4)
            // {
            //     rep(j,4){
            //         cout << grid[i][j] << ' ';
            //     }
            //     cout << endl;
            // }
            score -= 0;
            return score;
        }
        int spawnIndex = freeCells[(int)seed % freeCells.size()];
        int value = ((seed & 0x10) == 0 ? 2 : 4);
        grid[spawnIndex % 4][spawnIndex / 4] = Log2[value];

        seed = seed * seed % 50515093L;

        return score;
    }

    void Out()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (j != 0)
                {
                    cerr << ' ';
                }
                cerr << Power2[grid[i][j]];
            }
            cerr << endl;
        }
    }
};

int select_action(vector<int> Q, ld eps)
{

    int ma = *max_element(Q.begin(), Q.end());
    vector<int> _greedy;
    for (int i = 0; i < (int)Q.size(); i++)
    {
        if (Q[i] == ma)
        {
            _greedy.push_back(i);
        }
    }

    int action = _greedy[xorshift64() % (int)_greedy.size()];
    ld Rand = randouble();
    if (Rand < eps)
    {
        action = xorshift64() % 4;
    }
    return action;
}

char Answer[4] = {'U', 'L', 'D', 'R'};
constexpr int n_tuple_cnt = 6;
vector<vector<int>> x = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}};
vector<vector<int>> y = {{0, 1, 2, 3}, {0, 1, 2, 3}, {0, 1, 0, 1}, {1, 2, 1, 2}, {0, 1, 2, 0}, {0, 1, 2, 1}};
vector<vector<int>> Rotate = {{12, 8, 4, 0}, {13, 9, 5, 1}, {14, 10, 6, 2}, {15, 11, 7, 3}};

int main()
{
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    double T = clock();

    make_hexadecimal();
    make_Power16();
    make_Power2();
    make_Log2();
    make_new_line();

    ld ALPHA = 0.1;
    ld GAMMA = 0.9;
    int num_iteration = 10000; // Number of iterations until end of episode.
    int num_episode = 500000;  // Number of samplings.
    ld ave_score = 0;

    // state value function
    vector<vector<ld>> V(n_tuple_cnt, vector<ld>(MAX4));

    rep2(z, num_episode)
    {
        ld _eps = 0.0;
        if (z % (num_episode / 50) == 0)
        {
            cout << z << endl;
            _eps = 0;
        }

        Agent agent;
        int real_score = 0;

        // make initial state
        rep(k, 2)
        {
            int i = xorshift64() % 4, j = xorshift64() % 4, x = xorshift64() % 2 + 1;
            while (agent.grid[i][j] != 0)
            {
                i = xorshift64() % 4;
                j = xorshift64() % 4;
            }
            agent.grid[i][j] = x;
        }

        // iteration
        rep(k, num_iteration)
        {
            if (!agent.can_move_all())
            {
                break;
            }

            // calculate before_W
            int before_W = 0;
            vector<vector<int>> s;
            rep2(times, 8)
            {
                rep(ind, n_tuple_cnt)
                {
                    int s_v = hexadecimal[agent.grid[x[ind][0]][y[ind][0]]][agent.grid[x[ind][1]][y[ind][1]]][agent.grid[x[ind][2]][y[ind][2]]][agent.grid[x[ind][3]][y[ind][3]]];
                    before_W += V[ind][s_v];
                }
                if (times % 4 == 0)
                {
                    rep(i, 4)
                    {
                        rep(j, 2)
                        {
                            swap(agent.grid[i][j], agent.grid[i][3 - j]);
                        }
                    }
                }
                else
                {
                    s = agent.grid;
                    rep(i, 4)
                    {
                        rep(j, 4)
                        {
                            agent.grid[i][j] = s[Rotate[i][j] / 4][Rotate[i][j] % 4];
                        }
                    }
                }
            }
            s = agent.grid;

            // select action

            vector<int> Score(4);
            rep(d, 4)
            {
                Agent agent2 = agent;
                if (!agent2.can_move(d))
                {
                    Score[d] = 0 - 100000000;
                    continue;
                }
                int score = agent2.move_and_reward(d);
                rep2(times, 8)
                {
                    rep(ind, n_tuple_cnt)
                    {
                        int s_v = hexadecimal[agent2.grid[x[ind][0]][y[ind][0]]][agent2.grid[x[ind][1]][y[ind][1]]][agent2.grid[x[ind][2]][y[ind][2]]][agent2.grid[x[ind][3]][y[ind][3]]];
                        score += V[ind][s_v];
                    }
                    if (times % 4 == 0)
                    {
                        rep(i, 4)
                        {
                            rep(j, 2)
                            {
                                swap(agent2.grid[i][j], agent2.grid[i][3 - j]);
                            }
                        }
                    }
                    else
                    {
                        vector<vector<int>> s_ = agent2.grid;
                        rep(i, 4)
                        {
                            rep(j, 4)
                            {
                                agent2.grid[i][j] = s_[Rotate[i][j] / 4][Rotate[i][j] % 4];
                            }
                        }
                    }
                }
                Score[d] = score;
            }

            // calculate after_W
            int action = select_action(Score, _eps);
            ld reward = agent.move_and_reward(action);
            real_score += reward;
            int after_W = 0;
            rep2(times, 8)
            {
                rep(ind, n_tuple_cnt)
                {
                    // cout << agent.grid[x[ind][0]][y[ind][0]] << ' ' << agent.grid[x[ind][1]][y[ind][1]] << ' ' << agent.grid[x[ind][2]][y[ind][2]] << ' ' << agent.grid[x[ind][3]][y[ind][3]] << endl;
                    int s_v = hexadecimal[agent.grid[x[ind][0]][y[ind][0]]][agent.grid[x[ind][1]][y[ind][1]]][agent.grid[x[ind][2]][y[ind][2]]][agent.grid[x[ind][3]][y[ind][3]]];
                    after_W += V[ind][s_v];
                }
                if (times % 4 == 0)
                {
                    rep(i, 4)
                    {
                        rep(j, 2)
                        {
                            swap(agent.grid[i][j], agent.grid[i][3 - j]);
                        }
                    }
                }
                else
                {
                    vector<vector<int>> s = agent.grid;
                    rep(i, 4)
                    {
                        rep(j, 4)
                        {
                            agent.grid[i][j] = s[Rotate[i][j] / 4][Rotate[i][j] % 4];
                        }
                    }
                }
            }

            // Update V
            int d = reward + after_W - before_W;
            rep2(times, 8)
            {
                rep(ind, n_tuple_cnt)
                {
                    int s_v = hexadecimal[s[x[ind][0]][y[ind][0]]][s[x[ind][1]][y[ind][1]]][s[x[ind][2]][y[ind][2]]][s[x[ind][3]][y[ind][3]]];
                    V[ind][s_v] += ALPHA * (d / (ld)(8 * n_tuple_cnt));
                }
                if (times % 4 == 0)
                {
                    rep(i, 4)
                    {
                        rep(j, 2)
                        {
                            swap(s[i][j], s[i][3 - j]);
                        }
                    }
                }
                else
                {
                    vector<vector<int>> s_ = s;
                    rep(i, 4)
                    {
                        rep(j, 4)
                        {
                            s[i][j] = s_[Rotate[i][j] / 4][Rotate[i][j] % 4];
                        }
                    }
                }
            }

            // if (z % (num_episode / 50) == 0)
            // {
            //     ofs << Answer[action] << endl;
            //     rep(i, 4)
            //     {
            //         rep(j, 4)
            //         {
            //             if (j != 0)
            //             {
            //                 ofs << ' ';
            //             }
            //             ofs << Power2[agent.grid[i][j]];
            //         }
            //         ofs << endl;
            //     }
            // }
        }

        ave_score += real_score;
        if (z % (num_episode / 50) == 0)
        {
            ave_score /= (num_episode / 50);
            cout << "  " << real_score << "  ";
            cout << fixed << setprecision(3) << ave_score << endl;
            ave_score = 0;
        }
    }

    /*ofstream ofsv("2048-TD-V.txt");
    ofsv << '{';
    rep(i, 4)
    {
        ofsv << '{';
        rep(j, MAX4)
        {
            if (j != 0)
            {
                ofsv << ", ";
            }
            ofsv << V[i][j];
        }
        ofsv << '}' << endl;
    }
    ofsv << '}' << endl;*/

    /*Agent agent;
    rep(z, 2)
    {
        int i = xorshift64() % 4, j = xorshift64() % 4, x = xorshift64() % 2 + 1;
        agent.grid[i][j] = x;
    }
    rep(z, 10000)
    {
        if (!agent.can_move_all())
        {
            break;
        }
        ofs << z << endl;

        vector<ld> Score(4);
        rep(d, 4)
        {
            Agent agent2 = agent;
            if (!agent2.can_move(d))
            {
                Score[d] = 0 - 1000000000;
                continue;
            }
            ld score = agent2.move_and_reward(d);
            vector<vector<int>> s;
            rep2(times, 8)
            {
                rep(ind, n_tuple_cnt)
                {
                    int s_v = hexadecimal[agent2.grid[x[ind][0]][y[ind][0]]][agent2.grid[x[ind][1]][y[ind][1]]][agent2.grid[x[ind][2]][y[ind][2]]][agent2.grid[x[ind][3]][y[ind][3]]];
                    score += V[ind][s_v];
                }
                if (times % 4 == 0)
                {
                    rep(i, 4)
                    {
                        rep(j, 2)
                        {
                            swap(agent2.grid[i][j], agent2.grid[i][3 - j]);
                        }
                    }
                }
                else
                {
                    s = agent2.grid;
                    rep(i, 4)
                    {
                        rep(j, 4)
                        {
                            agent2.grid[i][j] = s[Rotate[i][j] / 4][Rotate[i][j] % 4];
                        }
                    }
                }
            }
            Score[d] = score;
        }

        // rep(i, 4)
        // {
        //     if (z % 10 == 0) cout << fixed << setprecision(15) << "Score_" << i << ": " << Score[i] << endl;
        // }

        int ans = max_element(Score.begin(), Score.end()) - Score.begin();
        ofs << Answer[ans] << endl;
        agent.move_and_reward(ans);
        rep(i, 4)
        {
            rep(j, 4)
            {
                if (j != 0)
                {
                    ofs << ' ';
                }
                ofs << Power2[agent.grid[i][j]];
            }
            ofs << endl;
        }
    }*/
    return 0;
}
