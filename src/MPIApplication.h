#ifndef MPIAPPLICATION_H
#define MPIAPPLICATION_H

#include "Solver.h"
#include "Problem.h"

#include <memory>
#include <queue>

class MPIApplication
{

public:
    static MPIApplication& getInstance()
    {
        static MPIApplication instance;
        return instance;
    }

    MPIApplication(const MPIApplication&) = delete;
    void operator=(MPIApplication const&) = delete;

    MPIApplication() {}

    int run(int argc, char** argv);

    enum MESSAGE_TYPE
    {
        MESSAGE_TYPE_WORK_REQUEST = 1000,
        MESSAGE_TYPE_WORK_SENT    = 1001,
        MESSAGE_TYPE_NO_WORK      = 1002,
        MESSAGE_TYPE_FINISH       = 1003,
        MESSAGE_TYPE_N            = 1004,
        MESSAGE_TYPE_A            = 1005,
        MESSAGE_TYPE_T            = 1006,
        MESSAGE_TYPE_GRAPH        = 1007,
        MESSAGE_TYPE_PRICE        = 1008,
        MESSAGE_TYPE_NODES        = 1009
    };

private:
    std::unique_ptr<Problem> m_problem;
    std::unique_ptr<Solver> m_solver;
    std::unique_ptr<Solution> m_solution;

    std::queue<uint> m_workQueue;

    void readAndSendData(int argc, char** argv);
    void receiveData();
};

#endif // MPIAPPLICATION_H
