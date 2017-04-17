#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>

typedef unsigned int uint;
typedef unsigned short int ushort;

struct Problem
{
    uint a;
    uint n;
    uint t;
    ushort* graph;

    Problem(uint _a, uint _n, uint _t)
    {
        a = _a;
        n = _n;
        t = _t;

        graph = new ushort[n * n];
    }

    ~Problem()
    {
        if (graph)
        {
            delete[] graph;
        }
    }
};

#endif // PROBLEM_H
