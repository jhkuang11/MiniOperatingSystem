
#include <kernel.h>


void null_process(PROCESS self, PARAM param)
{
    while (42);
    become_zombie();
}


void init_null_process()
{
    create_process(null_process, 0, 0, "Null process");
}
