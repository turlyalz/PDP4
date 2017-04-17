#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>

typedef unsigned int uint;

struct Solution
{
    uint a;
    uint price;
    uint* nodes;

    Solution(int _a)
    {
        a = _a;
        nodes = new uint[a];
    }

    ~Solution()
    {
        if (nodes)
        {
            delete nodes;
        }
    }
};

#endif // SOLUTION_H

