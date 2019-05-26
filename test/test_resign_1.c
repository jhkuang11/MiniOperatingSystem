
#include <kernel.h>
#include <test.h>


void test_resign_1a_process_a(PROCESS self, PARAM param)
{
    kprintf("Process: %s\n\n", self->name);
    kprintf("Param: %d\n", param);
    print_all_processes(kernel_window);
    return_to_boot();
}

void test_resign_1b_process_a(PROCESS self, PARAM param)
{
    kprintf("Process: %s\n\n", self->name);
    kprintf("Param: %d\n\n", param);

    print_all_processes(kernel_window);

    check_process(boot_name, STATE_READY, TRUE);
    check_process("Test process A", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(2);
 
    if (test_result != 0) 
       test_failed(test_result);

    if (param != 42)
       test_failed(20);

    check_sum += 1;
    return_to_boot();
}

/*
 * This test combines the old test_dispatcher_1 and _2 together
 * old test_dispatcher_1 tests:
 * 
 * Create a new process and at it to the ready queue. There should
 * be two ready processes, but since we don't do a resign(), no
 * context switch happens.
 * 
 * 
 * old test_dispatcher_2 tests:
 * 
 * Create a new process with a higher priority. When doing a resign()
 * this new process should get scheduled so that execution continues
 * in test_process_a()
 */
void test_resign_1()
{	
	/* Old test_dispatcher_1 code    */
	test_reset();
    create_process(test_resign_1a_process_a, 5, 42, "Test process A");
    print_all_processes(kernel_window);

    check_process(boot_name, STATE_READY, TRUE);
    check_process("Test process A", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(2);
    if (test_result != 0)
       test_failed (test_result);
	

	/* Old test_dispatcher_2 code    */
	
	/* This block of code does the old test_create_process_1 tests   
     * These 4 lines do the test_reset() without clearing the screen */
    test_result = 0;
    init_process();
    init_dispatcher();
    init_ipc();  
    kprintf("\n\n");  
	
    create_process(test_resign_1b_process_a, 5, 42, "Test process A");

    check_process(boot_name, STATE_READY, TRUE);
    check_process("Test process A", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(2);
    if (test_result != 0) 
       test_failed(test_result);

    check_sum = 0; 
    resign();
    if (check_sum == 0)
       test_failed(21);
}

