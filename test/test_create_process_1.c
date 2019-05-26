
#include <kernel.h>
#include <test.h>

/*
 * We don't explicitly create a new process, but because of init_process()
 * and init_dispatcher(), the main thread should be initialized as a process
 * and be added to the ready queue.  
 * This also test print_all_processes() 
 * 
 * Tests if init_process() cleans up the pcb array and if init_dispatcher(). 
 * cleans up the ready queue. 
 * 
 * Combination of the old test_create_process_1 and _2 code  dh 2017-2-23
 */
void test_create_process_1() 
{
	/* This is the old test_create_process_2 code  */
    test_reset();
    init_process();
    init_dispatcher();    
    kprintf("Called init_process() and init_dispatcher().\n\n");
    
    print_all_processes(kernel_window);
    
    //check if the boot process is still initialized correctly and 
    //if there is only one used pcb entry and one process on ready queue.
    check_create_process(boot_name, 1, NULL, 0);
    if (test_result != 0) 
       test_failed(test_result);

    check_num_of_pcb_entries(1);
    if (test_result != 0)
       test_failed(test_result);

    check_process(boot_name, STATE_READY, TRUE);
    if (test_result != 0)
       test_failed(test_result);

    check_num_proc_on_ready_queue(1);
    if (test_result != 0)
       test_failed(test_result);
    
    
    /* This block of code does the old test_create_process_1 tests   
     * These 4 lines do the test_reset() without clearing the screen */
    test_result = 0;
    init_process();
    init_dispatcher();
    init_ipc();    
    
    kprintf("\n\n");
    print_all_processes(kernel_window);

    //check if the boot process is initialized correctly.
    check_create_process(boot_name, 1, NULL, 0);
    if (test_result != 0) 
       test_failed(test_result);

    check_num_of_pcb_entries(1);
    if (test_result != 0)
       test_failed(test_result);

    check_process(boot_name, STATE_READY, TRUE);
    if (test_result != 0)
       test_failed(test_result);

    check_num_proc_on_ready_queue(1);
    if (test_result != 0)
       test_failed(test_result);
}

