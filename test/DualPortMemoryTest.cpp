#include "SimpleMemory/DualPortMemory.h"

int sc_main(int argc, char *argv[])
{
    DualPortMemory *dualPortMemory = new DualPortMemory("dualPortMemory");
    delete dualPortMemory;
    return 0;
}
