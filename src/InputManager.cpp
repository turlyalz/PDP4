#include "InputManager.h"
#include "Problem.h"

#include "base.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cstring>

using namespace std;

void InputManager::printHelp() const
{
    cout << "Run program: ./bw.out -f FILE_NAME -a NATURAL_NUMBER -t NUMBER_OF_THREADS" << endl;
}

bool InputManager::parse(int argc, char* argv[])
{
	if (argc == ARGUMENT_END)
	{
        string fileName;
		if (strcmp("-f", argv[ARGUMENT_F_TAG]) == 0)
		{
			fileName = argv[ARGUMENT_FILE_NAME];
		}
		else
		{
            cerr << "Argument " << argv[ARGUMENT_F_TAG] << " unknown" << endl;
            printHelp();
			return false;
		}

        uint a;
		if (strcmp("-a", argv[ARGUMENT_A_TAG]) == 0)
		{
            a = stoi(argv[ARGUMENT_A]);
		}
		else
		{
            cerr << "Argument " << argv[ARGUMENT_A_TAG] << " unknown" << endl;
            printHelp();
			return false;
		}

        uint t;
        if (strcmp("-t", argv[ARGUMENT_T_TAG]) == 0)
        {
            t = stoi(argv[ARGUMENT_T]);
        }
        else
        {
            cerr << "Argument " << argv[ARGUMENT_T_TAG] << " unknown" << endl;
            printHelp();
            return false;
        }

        ifstream in(fileName);
		if (in.is_open())
		{
            string data;
            getline(in, data);

            istringstream iss(data);
            uint n;
            iss >> n;

           /* if (a < 5 || a > (n / 2))
			{
                cerr << "a must be in range [5, n/2]" << endl;
				return false;
            }*/
			
            m_problem = new Problem(a, n, t);

            uint i = 0;
            while (getline(in, data))
			{
                if (data.size() != m_problem->n || i >= n)
				{
                    cerr << "Incorrect file: " << fileName << endl;
					return false;
				}

                for (uint j = 0; j < m_problem->n; ++j)
                {
                    m_problem->graph[(i * n) + j] = data[j] != '0';
                }
                ++i;
			}

			in.close();
			return true;
		}
		else
		{
            cerr << "Unable to open file: " + fileName << endl;
		}
	}
	else
	{
        cerr << "Number of arguments must be " << ARGUMENT_END - 2 << endl;
        printHelp();
	}
	return false;
}
