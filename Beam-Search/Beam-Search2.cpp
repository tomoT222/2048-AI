#include<bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#define rep(i, x) for(int i = 0; i < x; i++)
#define rep2(i, x) for(int i = 1; i <= x; i++)
#define rep3(i, x, y) for(int i = x; i < y; i++)
#define REP(i, x) for(int i = x - 1; i >= 0; i--)
#define REP2(i, x) for(int i = x; i >= 1; i--)
#define fi first
#define se second
#define mpa make_pair
#define mpt make_tuple
#define pb push_back
#define all(a) (a).begin(),(a).end()
#define rall(a) (a).rbegin(),(a).rend()
#define puts(x) cout << (x) << "\n"
using namespace std;

typedef string::const_iterator State;
class ParseError {};

template <typename T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;  // aをbで更新
        return true;
    }
    return false;
}
template <typename T>
bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;  // aをbで更新
        return true;
    }
    return false;
}










constexpr int MAX = 17;//2^17はできない

//Power2[a] = 2^a, Power2[0] = 0
int Power2[1 << MAX];
void make_Power2() {
    int two = 2;
    rep2(i, MAX) {
        Power2[i] = two; two *= 2;
    }
}

//Log2[A] = log_2_A, Log2[0] = 0
int Log2[1 << MAX];
void make_Log2() {
    int two = 2;
    rep2(i, MAX) {
        Log2[two] = i; two *= 2;
    }
}

//new_line[a][b][c][d] = (a, b, c, d)を左に詰めた列
tuple<int, int, int, int> new_line[MAX][MAX][MAX][MAX];
//change_line[a][b][c][d] = (a, b, c, d)を左に詰めた列は前の列と変わったか
bool change_line[MAX][MAX][MAX][MAX];
void make_new_line() {
    rep(a, MAX) {
        rep(b, MAX) {
            rep(c, MAX) {
                rep(d, MAX) {
                    int grid[4];
                    grid[0] = Power2[a];
                    grid[1] = Power2[b];
                    grid[2] = Power2[c];
                    grid[3] = Power2[d];
                    vector<int> v; bool last = true;//最後が*2されたか
                    rep(i, 4) {
                        if (grid[i] != 0) {
                            if (v.size() != 0 && v.back() == grid[i] && !last) {
                                v[v.size() - 1] *= 2; last = true;
                            }
                            else { v.push_back(grid[i]); last = false; }
                        }
                    }
                    while (v.size() != 4) { v.push_back(0); }
                    rep(i, 4) {
                        v[i] = Log2[v[i]];
                    }
                    new_line[a][b][c][d] = mpt(v[0], v[1], v[2], v[3]);
                    change_line[a][b][c][d] = ((v[0] != a) | (v[1] != b) | (v[2] != c) | (v[3] != d));
                }
            }
        }
    }
}


char dl[4] = { 'U', 'L', 'D', 'R' };
struct board {
    long long seed;
    int grid[4][4];
    vector<int> moves;

    bool move_ok(int d) {
        bool flag = false;
        if (d == 0) { //U
            rep(j, 4) {
                flag |= change_line[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]];
            }
        }
        if (d == 1) { //L
            rep(i, 4) {
                flag |= change_line[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]];
            }
        }
        if (d == 2) { //D
            rep(j, 4) {
                flag |= change_line[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]];
            }
        }
        if (d == 3) { //R
            rep(i, 4) {
                flag |= change_line[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]];
            }
        }
        return flag;
    }

    board move(int d) {
        board next_b;
        next_b.moves = moves;
        next_b.moves.push_back(d);

        if (d == 0) { //U
            rep(j, 4) {
                next_b.grid[0][j] = get<0>(new_line[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]);
                next_b.grid[1][j] = get<1>(new_line[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]);
                next_b.grid[2][j] = get<2>(new_line[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]);
                next_b.grid[3][j] = get<3>(new_line[grid[0][j]][grid[1][j]][grid[2][j]][grid[3][j]]);
            }
        }
        if (d == 1) { //L
            rep(i, 4) {
                next_b.grid[i][0] = get<0>(new_line[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]);
                next_b.grid[i][1] = get<1>(new_line[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]);
                next_b.grid[i][2] = get<2>(new_line[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]);
                next_b.grid[i][3] = get<3>(new_line[grid[i][0]][grid[i][1]][grid[i][2]][grid[i][3]]);
            }
        }
        if (d == 2) { //D
            rep(j, 4) {
                next_b.grid[0][j] = get<3>(new_line[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]);
                next_b.grid[1][j] = get<2>(new_line[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]);
                next_b.grid[2][j] = get<1>(new_line[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]);
                next_b.grid[3][j] = get<0>(new_line[grid[3][j]][grid[2][j]][grid[1][j]][grid[0][j]]);
            }
        }
        if (d == 3) { //R
            rep(i, 4) {
                next_b.grid[i][0] = get<3>(new_line[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]);
                next_b.grid[i][1] = get<2>(new_line[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]);
                next_b.grid[i][2] = get<1>(new_line[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]);
                next_b.grid[i][3] = get<0>(new_line[grid[i][3]][grid[i][2]][grid[i][1]][grid[i][0]]);
            }
        }

        vector<int> freeCells;
        rep(j, 4) {
            rep(i, 4) {
                if (next_b.grid[i][j] == 0) { freeCells.push_back(i + j * 4); }
            }
        }
        int spawnIndex = freeCells[(int)seed % freeCells.size()];
        int value = ((seed & 0x10) == 0 ? 2 : 4);
        next_b.grid[spawnIndex % 4][spawnIndex / 4] = Log2[value];

        next_b.seed = seed * seed % 50515093L;

        return next_b;
    }

    void Out() {
        rep(i, 4) {
            rep(j, 4) {
                if (j != 0) { cerr << ' '; }
                cerr << Power2[grid[i][j]];
            }
            cerr << endl;
        }
    }
};

//weight[i][j] = マス(i, j)の重み
long long weight[4][4];
void make_weight() {
    rep(i, 4) {
        rep(j, 4) {
            if (i % 2 == 0) {
                weight[i][j] = pow(10, 4 - i) * (4 - j);
            }
            else {
                weight[i][j] = pow(10, 4 - i) * (j + 1);
            }
        }
    }
}

int damage = 100;
long long evaluate(board &b) {
    long long score = 0;
    rep(i, 4) {
        rep(j, 4) {
            //そのマスを評価
            score += weight[i][j] * (long long)Power2[b.grid[i][j]];

            //横
            /*if (j < 3) {
                if (i % 2 == 0 && b.cell[i][j] < b.cell[i][j + 1]) {
                    score -= pow(10, 4 - i) * (b.cell[i][j + 1] - b.cell[i][j]) * damage;
                }
                if (i % 2 == 1 && b.cell[i][j] > b.cell[i][j + 1]) {
                    score -= pow(10, 4 - i) * (b.cell[i][j] - b.cell[i][j + 1]) * damage;
                }
            }*/

            //縦
            /*if (((i % 2 == 0 && j == 3) || (i == 1 && j == 0)) && b.cell[i][j] < b.cell[i + 1][j]) {
                score -= pow(10, 4 - i) * (b.cell[i + 1][j] - b.cell[i][j]) * damage;
            }*/
        }
    }
    return score;
}

struct status {
    long long v;
    board b;
    bool operator<(const status& rhs) const { return v < rhs.v; };
    bool operator>(const status& rhs) const { return v > rhs.v; };
};

int main() {
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    double T = clock();

    make_Power2(); make_Log2(); make_new_line(); make_weight();

    cerr << (clock() - T) * 1000 / CLOCKS_PER_SEC << endl;

    int CNT = 0;
    const int BEAM_SIZE = 180;
    int MOVES = 150;

    while (1) {
        if (CNT != 0) { MOVES = 20; }

        T = clock();

        //INPUT
        int seed; // needed to predict the next spawns
        cin >> seed; cin.ignore();
        // cerr << seed << endl;

        int Score;
        cin >> Score; cin.ignore();
        //cerr << Score << endl;


        board b; b.seed = seed;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int g; cin >> g; cin.ignore();
                b.grid[i][j] = Log2[g];
            }
        }


        long long min_score = 1e9; board ans; vector<status> v(1, { 0, b });
        int actual_moves = MOVES;
        rep(i, MOVES) {
            if (CNT != 0 && (clock() - T) * 1000 / CLOCKS_PER_SEC >= 40)
            {
                actual_moves = i;
                break;
            }
            if (CNT == 0 && (i + 1) % 30 == 0)
            {
                status a = v[0];
                v.clear();
                v.push_back(a);
            }
            int SIZE = min(BEAM_SIZE, (int)v.size());
            vector<status> nv;
            rep(cnt, SIZE) {
                rep(d, 4) {
                    if (v[cnt].b.move_ok(d)) {
                        board nb = v[cnt].b.move(d);
                        nv.push_back({ 0 - evaluate(nb), nb });
                    }
                }
            }

            int SIZE2 = min(BEAM_SIZE, (int)(nv.size()));
            nth_element(nv.begin(), nv.begin() + SIZE2, nv.end());
            rep(cnt, SIZE2) {
                if (cnt < (int)v.size())v[cnt] = nv[cnt];
                else { v.push_back(nv[cnt]); }
                if (chmin(min_score, v[cnt].v)) { ans = v[cnt].b; }
            }
        }

        // ans.Out();
        // cerr << min_score << endl;
        // cerr << (clock() - T) * 1000 / CLOCKS_PER_SEC << endl;

        int ANS_SIZE = min(actual_moves - 2, (int)ans.moves.size());
        rep(i, ANS_SIZE) {
            cout << dl[ans.moves[i]];
        }
        cout << endl;

        CNT++;
    }

    return 0;
}