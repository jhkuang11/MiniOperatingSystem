
#include <kernel.h>
#include <test.h>

PROCESS test_dispatcher_2p1, test_dispatcher_2p2;
PROCESS test_dispatcher_2pA, test_dispatcher_2pB;

void test_dispatcher_2_process(PROCESS self, PARAM param)
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
void test_dispatcher_2() 
{
	/*
	 * Boot process exists. Create process A with priority 3
	 * Check that dispatcher() returns this process.
	 *  
	 */
	test_reset();
    kprintf("test_dispatcher_2\n");
    create_process(test_dispatcher_2_process, 3, 42, "Test process A");	
    check_process(boot_name, STATE_READY, TRUE);
    check_process("Test process A", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(2);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
    
    /* This is the process that should be returned */
	test_dispatcher_2pA = find_process_by_name("Test process A");
	if (test_dispatcher_2pA == NULL)
    {
		test_result = 10;
		test_failed(test_result);
	}

	test_dispatcher_2p1 = dispatcher();

	if ((test_dispatcher_2p1 == NULL) || (test_dispatcher_2p1 != test_dispatcher_2pA))
    {
		test_failed(22);  
	}
	kprintf("Passed test 1 - returns Test process A\n");
	
	
	/*
	 * Boot process and process A (priority) exist
	 * Create process B with priority 3. It should be on the back of the queue
	 * Process A (priority 3) should be returned
	 * Check that dispatcher() returns this process.
	 *  
	 */		
	create_process(test_dispatcher_2_process, 3, 42, "Test process B");
	check_process("Test process B", STATE_READY, TRUE);
	test_dispatcher_2pB = find_process_by_name("Test process B");
	if (test_dispatcher_2pB == NULL)
    {
		test_result = 10;
		test_failed(test_result);
	}
	    
    check_num_proc_on_ready_queue(3);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
	
	test_dispatcher_2p1 = dispatcher();

	if ((test_dispatcher_2p1 == NULL) || (test_dispatcher_2p1 != test_dispatcher_2pA))
    {
		test_failed(23); 
	}
	kprintf("Passed test 2 - returns Test process A\n");
	
	
	/*
	 * Boot process, processes A, B (priority = 3) exist
	 * Assign active_proc to Process A. Then dispatcher() should 
	 * return Process B
	 */	
	test_dispatcher_2p1 = active_proc;
	active_proc = test_dispatcher_2pA;
	test_dispatcher_2p2 = dispatcher();
	
	active_proc = test_dispatcher_2p1;
	
	if ((test_dispatcher_2p2 == NULL) || (test_dispatcher_2p2 != test_dispatcher_2pB))
    {
		test_failed(24); 
	}
	kprintf("Passed test 3 - returns Test process B\n");
	
	
	/*
	 * Boot process, processes A, B (priority = 3) exist
	 * Assign active_proc to Process B. Then dispatcher() should 
	 * return Process A
	 */	
	test_dispatcher_2p1 = active_proc;
	active_proc = test_dispatcher_2pB;
	test_dispatcher_2p2 = dispatcher();
	
	active_proc = test_dispatcher_2p1;
	
	if ((test_dispatcher_2p2 == NULL) || (test_dispatcher_2p2 != test_dispatcher_2pA))
    {
		test_failed(24); 
	}
	kprintf("Passed test 4 - returns Test process A\n\n");
	
	print_all_processes(kernel_window);
	kprintf("\nAll pass!\n");
}

