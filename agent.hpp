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
