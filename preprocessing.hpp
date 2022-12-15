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
                    bool last = true; // Whether the last was doubled or not.
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
