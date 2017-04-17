#include "base.h"
#include "Solution.h"
#include "Problem.h"

using namespace std;

ostream& operator<<(ostream& os, const vector<vector<bool>>& graph)
{
    os << "--------------------- Graph ---------------------" << endl;
    for (uint i = 0; i < graph.size(); ++i)
    {
        os << graph[i] << endl;
    }
    os << "-------------------------------------------------" << endl;
    return os;
}

ostream& operator<<(ostream& os, const Problem* problem)
{
    if (!problem)
    {
        return os;
    }

    os << "-------------- Problem -------------------------" << endl;
    os << "Number of nodes: " << problem->n << endl;
    os << "A: " << problem->a << endl;
    os << "Number of threads: " << problem->t << endl;

    cout << "------------- Graph -------------" << endl;
    uint i = 0;
    for (uint j = 0; j < problem->n*problem->n; ++j)
    {
        cout << problem->graph[j] << ' ';
        if (++i == problem->n)
        {
            i = 0;
            cout << endl;
        }
    }
    cout << "---------------------------------" << endl;

    os << "-------------------------------------------------" << endl;
    return os;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vec)
{
    for (uint i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << " ";
        }
    }
    return os;
}

ostream& operator<<(ostream& os, const Solution* solution)
{
    if (!solution)
    {
        return os;
    }

    os << "-------------- Solution -------------------------" << endl;
    os << "Price: " << solution->price << endl;
    os << "Nodes: " << '{';
    for (uint i = 0; i < solution->a; ++i)
    {
        os << solution->nodes[i];
        if (i != solution->a - 1)
        {
             os << " ";
        }
    }
    os << '}' << endl;
    os << "-------------------------------------------------" << endl;
    return os;
}
