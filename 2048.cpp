#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#define rep(i, x) for (int i = 0; i < x; i++)
#define rep2(i, x) for (int i = 1; i <= x; i++)
using namespace std;

using ll = long long;

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

constexpr int MAX2 = 16;
// BaseTwelve[a][b][c][d][e][f] = base twelve number of a,b,c,d,e,f.
int BaseTwelve[MAX2][MAX2][MAX2][MAX2][MAX2][MAX2];
void make_BaseTwelve()
{
    for (int a = 0; a < MAX2; a++)
    {
        for (int b = 0; b < MAX2; b++)
        {
            for (int c = 0; c < MAX2; c++)
            {
                for (int d = 0; d < MAX2; d++)
                {
                    for (int e = 0; e < MAX2; e++)
                    {
                        for (int f = 0; f < MAX2; f++)
                        {
                            BaseTwelve[a][b][c][d][e][f] = f + MAX2 * (e + MAX2 * (d + MAX2 * (c + MAX2 * (b + MAX2 * a))));
                        }
                    }
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

//////////////////////////////
////
////      struct Agent
////
vector<vector<int>> zeros(4, vector<int>(4, 0));
struct Agent
{
    long long seed;
    vector<vector<int>> grid;

    Agent(vector<vector<int>> _grid = zeros)
    {
        grid = _grid;
        seed = 13339144;
        grid[1][3] = 1;
        grid[3][1] = 2;
    }

    ////////////////////////////////////////
    //
    //   Can I move the current state?
    //
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

    ////////////////////////////////////////
    //
    //   Move the state to the direction d. Return the reward at that time.
    //
    double move_and_reward(int d)
    {
        bool flag = false;
        double score = 0;

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

        /*if (!flag)
        {
            // If the board doesn't change
            //
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
        }*/

        /////////////////////////////////
        //
        //    add new tile.
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
            //
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

//////////////////////////////
////
////      select action
////
int select_action(vector<double> Q, double eps)
{
    double ma = *max_element(Q.begin(), Q.end());
    vector<int> _greedy;
    for (int i = 0; i < (int)Q.size(); i++)
    {
        if (Q[i] == ma)
        {
            _greedy.push_back(i);
        }
    }

    int action = _greedy[xorshift64() % (int)_greedy.size()];
    double Rand = randouble();
    if (Rand < eps)
    {
        action = xorshift64() % 4;
    }
    return action;
}

//////////////////////////////
////
////      list of n-tuple
////
constexpr int n_tuple_cnt = 4;
// vector<vector<int>> x = {{0, 0, 0, 0, 1, 1}, {0, 1, 1, 1, 2, 2}, {1, 1, 2, 3, 3, 3}, {1, 2, 3, 3, 3, 3}, {0, 0, 0, 1, 1, 1}, {0, 1, 1, 1, 2, 2}, {1, 2, 2, 2, 3, 3}, {0, 1, 1, 1, 1, 2}, {1, 2, 2, 2, 3, 3}, {1, 1, 2, 2, 3, 3}};
// vector<vector<int>> y = {{0, 1, 2, 3, 0, 1}, {3, 1, 2, 3, 1, 2}, {0, 1, 0, 0, 1, 2}, {3, 3, 0, 1, 2, 3}, {0, 1, 3, 1, 2, 3}, {1, 0, 1, 2, 1, 2}, {2, 1, 2, 3, 1, 2}, {0, 0, 1, 2, 3, 1}, {1, 0, 1, 2, 0, 1}, {2, 3, 2, 3, 2, 3}};
vector<vector<int>> x = {{0, 0, 0, 0, 1, 1}, {1, 1, 1, 1, 2, 2}, {1, 1, 1, 2, 2, 2}, {2, 2, 2, 3, 3, 3}};
vector<vector<int>> y = {{0, 1, 2, 3, 0, 1}, {0, 1, 2, 3, 0, 1}, {1, 2, 3, 1, 2, 3}, {1, 2, 3, 1, 2, 3}};
// vector<vector<int>> x = {{0, 0, 0, 0, 1, 1}, {0, 0, 0, 0, 1, 2}, {0, 0, 0, 1, 1, 1}, {0, 0, 1, 1, 1, 2}};
// vector<vector<int>> y = {{0, 1, 2, 3, 0, 1}, {0, 1, 2, 3, 0, 0}, {0, 1, 2, 0, 1, 2}, {0, 1, 0, 1, 2, 1}};
vector<vector<int>> Rotate = {{12, 8, 4, 0}, {13, 9, 5, 1}, {14, 10, 6, 2}, {15, 11, 7, 3}};

//////////////////////////////
////
////      list
////
char Action[4] = {'U', 'L', 'D', 'R'};

// state value function
vector<vector<double>> V;

//////////////////////////////
////
////      calculating_W
////
double calculating_W(vector<vector<int>> grid)
{
    double W = 0;
    rep2(times, 8)
    {
        rep(ind, n_tuple_cnt)
        {
            vector<int> BT_ind(6);
            rep(i, 6)
            {
                BT_ind[i] = min(MAX2 - 1, grid[x[ind][i]][y[ind][i]]);
            }
            int s_v = BaseTwelve[BT_ind[0]][BT_ind[1]][BT_ind[2]][BT_ind[3]][BT_ind[4]][BT_ind[5]];
            W += V[ind][s_v];
        }
        if (times % 4 == 0)
        {
            // Invert Grid
            rep(i, 4)
            {
                rep(j, 2)
                {
                    swap(grid[i][j], grid[i][3 - j]);
                }
            }
        }
        else
        {
            // Rotate Grid
            vector<vector<int>> pre_grid = grid;
            rep(i, 4)
            {
                rep(j, 4)
                {
                    grid[i][j] = pre_grid[Rotate[i][j] / 4][Rotate[i][j] % 4];
                }
            }
        }
    }
    return W;
}

int main()
{
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    double T = clock();

    // ofstream mid_results("midresults.txt");

    make_hexadecimal();
    make_BaseTwelve();
    make_Power16();
    make_Power2();
    make_Log2();
    make_new_line();

    double ALPHA = 0.1;
    double GAMMA = 0.9;
    int ADD_score = 10000;
    int num_iteration = 1000000; // Number of iterations until end of episode.
    int num_episode = 500000;    // Number of samplings.
    double ave_score = 0;
    double before_ave = 0;
    int max_score = 0;
    string max_episode;

    // state value function
    V.resize(n_tuple_cnt, vector<double>(pow(MAX2, 6)));

    cout << "start episode" << endl;
    cout << "ADD_score = " << ADD_score << endl;
    cout << "num_episode = " << num_episode << endl;

    rep2(z, num_episode)
    {
        string mid_result;
        ADD_score = max_score;

        double _eps = 0.00;
        if (z % (num_episode / 50) == 0)
        {
            cout << z << endl;
            _eps = 0;
        }

        Agent agent;
        int real_score = 0;

        // make initial state
        // rep(k, 2)
        // {
        //     int i = xorshift64() % 4, j = xorshift64() % 4, x = xorshift64() % 2 + 1;
        //     while (agent.grid[i][j] != 0)
        //     {
        //         i = xorshift64() % 4;
        //         j = xorshift64() % 4;
        //     }
        //     agent.grid[i][j] = x;
        // }

        // iteration
        rep(k, num_iteration)
        {
            if (!agent.can_move_all())
            {
                break;
            }

            //////////////////////
            //
            //   calculate before_W
            //

            double before_W = calculating_W(agent.grid);
            vector<vector<int>> before_grid;
            before_grid = agent.grid;

            //////////////////////
            //
            //   select action
            //

            vector<double> Score(4);
            vector<double> v_after_W(4);
            rep(d, 4)
            {
                Agent agent2 = agent;
                if (!agent2.can_move(d))
                {
                    Score[d] = 0 - 100000000;
                    continue;
                }
                double score = agent2.move_and_reward(d);
                double add = calculating_W(agent2.grid);
                score += add;
                v_after_W[d] = add;
                Score[d] = score;
            }

            //////////////////////
            //
            //   calculate after_W
            //

            int action = select_action(Score, _eps);
            double reward = agent.move_and_reward(action);
            // double after_W = calculating_W(agent.grid);
            double after_W = v_after_W[action];

            real_score += reward;
            mid_result.push_back(Action[action]);

            //////////////////////
            //
            //   Update V
            //

            double d = reward + after_W - before_W;
            if (!agent.can_move_all())
            {
                /*if (real_score > max_score)
                {
                    d += ADD_score;
                }
                else
                {
                    d -= max_score - real_score;
                }*/
                d += real_score - (max_score * 9 / 10);
            }
            rep2(times, 8)
            {
                rep(ind, n_tuple_cnt)
                {
                    vector<int> BT_ind(6);
                    rep(i, 6)
                    {
                        BT_ind[i] = min(MAX2 - 1, before_grid[x[ind][i]][y[ind][i]]);
                    }
                    int s_v = BaseTwelve[BT_ind[0]][BT_ind[1]][BT_ind[2]][BT_ind[3]][BT_ind[4]][BT_ind[5]];
                    V[ind][s_v] += ALPHA * (d / (double)(8 * n_tuple_cnt));
                }
                if (times % 4 == 0)
                {
                    rep(i, 4)
                    {
                        rep(j, 2)
                        {
                            swap(before_grid[i][j], before_grid[i][3 - j]);
                        }
                    }
                }
                else
                {
                    vector<vector<int>> pre_grid = before_grid;
                    rep(i, 4)
                    {
                        rep(j, 4)
                        {
                            before_grid[i][j] = pre_grid[Rotate[i][j] / 4][Rotate[i][j] % 4];
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

        // if ((z <= 10 || z >= 200000) && (real_score <= 2000 || real_score >= 100000))
        // {
        //     mid_results << z << endl;
        //     mid_results << mid_result << endl;
        // }

        if (chmax(max_score, real_score))
        {
            max_episode = mid_result;
        }

        ave_score += real_score;
        if (z % (num_episode / 50) == 0)
        {
            ave_score /= (num_episode / 50);
            cout << "  " << real_score << "  ";
            cout << fixed << setprecision(3) << ave_score << "  " << max_score << endl;
            before_ave = ave_score;
            ave_score = 0;
        }
    }

    /*ofstream ofsv("2048-TD-V.txt");
    ofsv << '{';
    rep(i, n_tuple_cnt)
    {
        ofsv << '{';
        rep(j, pow(MAX2, 6))
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

    /*ofstream ofs("2048.txt");

    Agent agent;
    int real_score = 0;
    string last_episode;
    rep(z, num_iteration)
    {
        if (!agent.can_move_all())
        {
            break;
        }
        ofs << z << endl;

        vector<double> Score(4);
        rep(d, 4)
        {
            Agent agent2 = agent;
            if (!agent2.can_move(d))
            {
                Score[d] = 0 - 1000000000;
                continue;
            }
            double score = agent2.move_and_reward(d);
            score += calculating_W(agent2.grid);
            Score[d] = score;
        }

        rep(i, 4)
        {
            ofs << fixed << setprecision(15) << "Score_" << i << ": " << Score[i] << endl;
        }

        int ans = max_element(Score.begin(), Score.end()) - Score.begin();
        ofs << Action[ans] << endl;
        last_episode.push_back(Action[ans]);
        real_score += agent.move_and_reward(ans);
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
    }
    ofs << real_score << endl;
    ofs << last_episode << endl;*/

    cout << "max : " << endl;
    cout << max_score << endl;
    cout << max_episode << endl;

    printf("Execution Time: %.4lf sec\n", 1.0 * (clock() - T) / CLOCKS_PER_SEC);
    return 0;
}
