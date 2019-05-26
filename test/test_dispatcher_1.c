
#include <kernel.h>
#include <test.h>

PROCESS test_dispatcher_1p1;
PROCESS test_dispatcher_1pA, test_dispatcher_1pB;

void test_dispatcher_1_process(PROCESS self, PARAM param)
{
    /*
     * Since we don't do a context switch yet, this code will actually
     * not be executed.
     */
    kprintf("Process: %s\n\n", self->name);
    return_to_boot();
}



/* 
 * Series of tests of the dispatcher() method
 * dh Feb. 2017
 * 
 */
void test_dispatcher_1() 
{
	/*
	 * Only boot process exists. Check that dispatcher() returns this process
	 * Check two times
	 * 
	 */
    test_reset();
    kprintf("test_dispatcher_1\n");
    check_process(boot_name, STATE_READY, TRUE);
    check_num_proc_on_ready_queue(1);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}       
    if (active_proc != dispatcher()) 
    {
		test_failed(27);
	}
	if (active_proc != dispatcher()) 
    {
		test_failed(27);
	}
	kprintf("Passed test 1 - returns boot process\n");
	
	
	/*
	 * Create a process with the same priority (1) as the boot process. 
	 * Dispatcher should return that process
	 * Check two times
	 */
	create_process(test_dispatcher_1_process, 1, 42, "Test process A");	
	check_process(boot_name, STATE_READY, TRUE);
	check_process("Test process A", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(2);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}   
	
	test_dispatcher_1pA = find_process_by_name("Test process A");
  
    //if the process is not in the pcb array, it is incorrect.
    if (test_dispatcher_1pA == NULL)
    {
		test_result = 10;
		test_failed(test_result);
	}
   
    if (test_dispatcher_1pA != dispatcher())
    {
		test_failed(24);
	}
	if (test_dispatcher_1pA != dispatcher())
    {
		test_failed(24);
	}
	kprintf("Passed test 2 - returns Test process A\n");


	/*
	 * There are two equal priority processes, boot process and "Test process A".
	 * active_proc is set to boot process.
	 * Temporarily set active_proc to "Test process A" (even though boot process is running).
	 * Check dispatcher(). It should return boot process
	 * 
	 */	 
	test_dispatcher_1p1 = active_proc;  /* Store boot process for later recovery */
	active_proc = test_dispatcher_1pA;  /* Set active_proc to other process */
	if (test_dispatcher_1p1 != dispatcher()) 
	{
		active_proc = test_dispatcher_1p1;   /* Restore active_proc before giving error  */
		test_failed(24); 
	}
	active_proc = test_dispatcher_1p1;   /* Restore active_proc  */
	kprintf("Passed test 3 - returns boot process\n");
	 
	 
	/*
	 * Create another priority 1 process. Add to the queue. It will be at
	 * the tail of the queue. Set active_proc to second process. 
	 * dispatcher() should return just added process
	 * 
	 */
	create_process(test_dispatcher_1_process, 1, 42, "Test process B");	
	check_process(boot_name, STATE_READY, TRUE);
	check_process("Test process A", STATE_READY, TRUE);
	check_process("Test process B", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(3);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}   
	
	test_dispatcher_1pB = find_process_by_name("Test process B");
  
    /* if the process is not in the pcb array, it is incorrect. */
    if (test_dispatcher_1pB == NULL)
    {
		test_result = 10;
		test_failed(test_result);
	}
	
	test_dispatcher_1p1 = active_proc;  /* Store boot process for later recovery */
	active_proc = test_dispatcher_1pA;  /* Set active_proc to process A */
	if (test_dispatcher_1pB != dispatcher()) 
	{
		active_proc = test_dispatcher_1p1;   /* Restore active_proc before giving error  */
		test_failed(24); 
	}
	active_proc = test_dispatcher_1p1;   /* Restore active_proc  */
	kprintf("Passed test 4 - returns Test process B\n\n");
	
	print_all_processes(kernel_window);
	kprintf("\nAll pass!\n");
}

