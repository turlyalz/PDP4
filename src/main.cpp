#include "MPIApplication.h"

int main(int argc, char* argv[])
{
    return MPIApplication::getInstance().run(argc, argv);
}
