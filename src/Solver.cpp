#include "Solver.h"

#include "base.h"

#include <algorithm>
#include <limits>
#include <iostream>

#include <omp.h>

using namespace std;

ull Solver::combinations(uint n, uint k) const
{
    if (k > n)
    {
        return 0;
    }
    ull r = 1;

    for (ull d = 1; d <= k; ++d)
    {
        r *= n--;
        r /= d;
    }
    return r;
}

uint Solver::calculatePrice(const vector<uint>& state, uint startNode) const
{
    uint price = 0;

    for (uint row = 0; row < m_problem->n; ++row)
    {
        for (uint col = row + 1; col < m_problem->n; ++col)
        {
            if (m_problem->graph[(row * m_problem->n) + col] &&
                (row == startNode || col == startNode) &&
                (binary_search(state.cbegin(), state.cend(), row) != binary_search(state.cbegin(), state.cend(), col)))
            {
                price++;
            }
        }
    }
    return price;
}

shared_ptr<Solution> Solver::run(const Problem* problem, uint startNode)
{
    m_problem = problem;
    if (!m_problem)
    {
        return nullptr;
    }

    vector<uint> state(m_problem->a - 1);

    // Start state: (startNode, startNode + 1, ...)
    for (uint i = 0; i < m_problem->a - 1; ++i)
    {
        state[i] = startNode + i + 1;
    }

    m_solution.reset(new Solution(m_problem->a));
    m_solution->price = calculatePrice(state, startNode);
    ull comb = combinations(m_problem->n, m_problem->a - 1);

    cout << "Start price: " << m_solution->price << endl;
    cout << "Number of combinations: " << comb << endl;

    uint prevIter = 0;
    ull c = 1;

    #pragma omp parallel for shared(comb) firstprivate(state, prevIter) private(c) num_threads(m_problem->t)
    for (c = 1; c < comb; ++c)
    {
        if (state[0] >= m_problem->n - m_problem->a - 1)
        {
            continue;
        }
        
        uint i = m_problem->a - 1;
        uint lastNode = m_problem->n;

        while (state[--i] == --lastNode);
        ++state[i];

        if (i != 0 && i != prevIter)
        {
            prevIter = i;

            // Calculate predicate price
            vector<uint> predState(state.begin(), state.begin() + i);
            uint price = calculatePrice(predState, startNode);

            // If the price is bigger than the min price then ignore branch
            if (price > m_solution->price)
            {
                state[i] = m_problem->n - m_problem->a - 1 + i;
                continue;
            }
        }

        for (uint j = i + 1; j < m_problem->a - 1; ++j)
        {
            state[j] = state[j - 1] + 1;
        }

        uint price = calculatePrice(state, startNode);
        if (price < m_solution->price)
        {
            #pragma omp critical
            {
                m_solution->price = price;
                m_solution->nodes[0] = startNode;
                for (uint i = 0; i < m_problem->a - 1; ++i)
                {
                    m_solution->nodes[i + 1] = state[i];
                }
            }
        }
    }

    return m_solution;
}
