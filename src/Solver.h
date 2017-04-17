#ifndef SOLVER_H
#define SOLVER_H

#include "Problem.h"
#include "Solution.h"

#include <memory>

typedef unsigned long long ull;

class Solver
{
public:
    Solver() {}

     std::shared_ptr<Solution> run(const Problem* problem, uint startNode);

private:
    ull combinations(uint n, uint k) const;
    uint calculatePrice(const std::vector<uint>& state, uint startNode) const;

    const Problem* m_problem;
    std::shared_ptr<Solution> m_solution;
};

#endif // SOLVER_H
