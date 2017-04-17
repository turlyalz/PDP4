#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

struct Problem;

class InputManager
{
public:
	InputManager() : m_problem(nullptr) {}

	bool parse(int argc, char* argv[]);
	inline Problem* getProblem() { return m_problem; }

private:
    enum EArguments
    {
        ARGUMENT_PROGRAM_NAME = 0,
        ARGUMENT_F_TAG = 1,
        ARGUMENT_FILE_NAME = 2,
        ARGUMENT_A_TAG = 3,
        ARGUMENT_A = 4,
        ARGUMENT_T_TAG = 5,
        ARGUMENT_T = 6,
        ARGUMENT_END = 7
    };

	Problem* m_problem;

    void printHelp() const;
};

#endif // INPUT_MANAGER_H
