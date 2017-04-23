#include "MPIApplication.h"
#include "InputManager.h"
#include "base.h"

#include <iostream>
#include <chrono>

#include <mpi.h>

int MPIApplication::run(int argc, char **argv)
{
    int rank;

    m_solution = nullptr;

    // Start up MPI
    MPI_Init(&argc, &argv);

    // Find out process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    bool error = false;
    // Master
    if (rank == 0)
    {
        error = !readAndSendData(argc, argv);
    }
    else
    {
        receiveData();
    }

    // Initialize Solver
    m_solver.reset(new Solver);

    std::chrono::time_point<std::chrono::system_clock> start;
    if (rank == 0)
    {
        start = std::chrono::high_resolution_clock::now();

        if (!error)
        {
            for (uint i = 0; i < m_problem->n - m_problem->a + 1; ++i)
            {
                m_workQueue.push(i);
            }
        }
    }
    else
    {
        uint i = 0;
        MPI_Send(&i, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_WORK_REQUEST, MPI_COMM_WORLD);
        std::cout << "SEND work request from " << rank << " process" << std::endl;
    }

    MPI_Status status, recv_status;
    int flag = 0;
    int finishedProcess = 1;
    bool running = true;

    while (running)
    {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag)
        {
            switch (status.MPI_TAG)
            {
                case MESSAGE_TYPE_WORK_REQUEST:
                {
                    uint recv;
                    MPI_Recv(&recv, 1, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_WORK_REQUEST, MPI_COMM_WORLD, &recv_status);
                    std::cout << "RECEIVED work request from " << status.MPI_SOURCE << " process" << std::endl;

                    if (!m_workQueue.empty())
                    {
                        // Send work as an answer to the request
                        uint startNode = m_workQueue.front();
                        m_workQueue.pop();
                        MPI_Send(&startNode, 1, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_WORK_SENT, MPI_COMM_WORLD);
                    }
                    else
                    {
                        uint i = 0;
                        MPI_Send(&i, 1, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_NO_WORK, MPI_COMM_WORLD);
                        ++finishedProcess;

                        if (finishedProcess == m_processes)
                        {
                            running = false;
                        }
                    }
                    break;
                }

                case MESSAGE_TYPE_WORK_SENT:
                {
                    uint startNode;
                    MPI_Recv(&startNode, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_WORK_SENT, MPI_COMM_WORLD, &recv_status);
                    std::cout << "Process #" << rank << " RECEIVED work " << std::endl;

                    // Solve with multi-thread algoritm
                    const auto solution = m_solver->run(m_problem.get(), startNode);
                    if (!solution)
                    {
                        running = false;
                        break;
                    }

                    // Send results to Master
                    uint i = 0;
                    MPI_Send(&i, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_FINISH, MPI_COMM_WORLD);
                    MPI_Send(&solution->price, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_PRICE, MPI_COMM_WORLD);
                    MPI_Send(solution->nodes, m_problem->a, MPI_UNSIGNED, 0, MESSAGE_TYPE_NODES, MPI_COMM_WORLD);
                    std::cout << "SEND finish from " << rank << " process" << std::endl;

                    // Request new work
                    MPI_Send(&i, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_WORK_REQUEST, MPI_COMM_WORLD);
                    std::cout << "SEND work request from " << rank << " process" << std::endl;
                    break;
                }

                case MESSAGE_TYPE_NO_WORK:
                {
                    uint recv;
                    MPI_Recv(&recv, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_NO_WORK, MPI_COMM_WORLD, &recv_status);

                    std::cout << "No work received, ending process #" << rank << std::endl;
                    running = false;
                    break;
                }

                case MESSAGE_TYPE_FINISH:
                {
                    uint recv;
                    MPI_Recv(&recv, 1, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_FINISH, MPI_COMM_WORLD, &recv_status);
                    std::cout << "RECEIVED finish from " << status.MPI_SOURCE << " process" << std::endl;

                    uint price;
                    uint* nodes = new uint[m_problem->a];
                    MPI_Recv(&price, 1, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_PRICE, MPI_COMM_WORLD, &recv_status);
                    MPI_Recv(nodes, m_problem->a, MPI_UNSIGNED, status.MPI_SOURCE, MESSAGE_TYPE_NODES, MPI_COMM_WORLD, &recv_status);

                    if (!m_solution)
                    {
                        m_solution.reset(new Solution(m_problem->a));
                    }

                    if (m_solution->price > price)
                    {
                        m_solution->price = price;
                        delete[] m_solution->nodes;
                        m_solution->nodes = nodes;
                    }
                    else
                    {
                        delete[] nodes;
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << m_solution.get();

        auto stop = std::chrono::high_resolution_clock::now();
        using fpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
        auto elapsedTime = fpSeconds(stop - start).count();
        std::cout << "Elapsed time: " << elapsedTime << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}

bool MPIApplication::readAndSendData(int argc, char** argv)
{
    // Find out number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &m_processes);

    std::unique_ptr<InputManager> input(new InputManager);
    std::cout << "Processes: " << m_processes << std::endl;

    std::cout << "-------------------- Bisection width of graph --------------------" << std::endl;

    bool result = input->parse(argc, argv);
    if (!result)
    {
        m_problem.reset(new Problem(0, 0, 0));
    }
    else
    {
        m_problem.reset(input->getProblem());
    }
    // Sent data to all processes
    for (int i = 1; i < m_processes; ++i)
    {
        MPI_Send(&m_problem->n, 1, MPI_UNSIGNED, i, MESSAGE_TYPE_N, MPI_COMM_WORLD);
        MPI_Send(&m_problem->a, 1, MPI_UNSIGNED, i, MESSAGE_TYPE_A, MPI_COMM_WORLD);
        MPI_Send(&m_problem->t, 1, MPI_UNSIGNED, i, MESSAGE_TYPE_T, MPI_COMM_WORLD);
        MPI_Send(m_problem->graph, m_problem->n * m_problem->n, MPI_UNSIGNED_SHORT, i, MESSAGE_TYPE_GRAPH, MPI_COMM_WORLD);
    }
    std::cout << m_problem.get();
    std::cout << "------------------------------------------------------------------" << std::endl;

    return result;
}

void MPIApplication::receiveData()
{
    MPI_Status status;
    uint n, a, t;

    MPI_Recv(&n, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_N, MPI_COMM_WORLD, &status);
    MPI_Recv(&a, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_A, MPI_COMM_WORLD, &status);
    MPI_Recv(&t, 1, MPI_UNSIGNED, 0, MESSAGE_TYPE_T, MPI_COMM_WORLD, &status);

    m_problem.reset(new Problem(a, n, t));
    MPI_Recv(m_problem->graph, m_problem->n * m_problem->n, MPI_UNSIGNED_SHORT, 0, MESSAGE_TYPE_GRAPH, MPI_COMM_WORLD, &status);
}
