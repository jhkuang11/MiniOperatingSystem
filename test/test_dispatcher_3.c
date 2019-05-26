
#include <kernel.h>
#include <test.h>

PROCESS test_dispatcher_3p1, test_dispatcher_3p2;
PROCESS test_dispatcher_3pA, test_dispatcher_3pB, test_dispatcher_3pC;

void test_dispatcher_3_process(PROCESS self, PARAM param)
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
void test_dispatcher_3()
{
	/*
	 * Boot process exists. Create process A with priority 3 and 
	 * process B with priority 5
	 * Check that dispatcher() returns this process B
	 *  
	 */
	test_reset();
    kprintf("test_dispatcher_3\n");
    create_process(test_dispatcher_3_process, 3, 42, "Test process A");
    create_process(test_dispatcher_3_process, 5, 42, "Test process B");
    check_process(boot_name, STATE_READY, TRUE);
    check_process("Test process A", STATE_READY, TRUE);
    check_process("Test process B", STATE_READY, TRUE);
    check_num_proc_on_ready_queue(3);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
    test_dispatcher_3pA = find_process_by_name("Test process A");
    test_dispatcher_3pB = find_process_by_name("Test process B");
	if ((test_dispatcher_3pA == NULL) || (test_dispatcher_3pB == NULL))
    {
		test_result = 10;
		test_failed(test_result);
	}
    
    test_dispatcher_3p1 = dispatcher();

	if ((test_dispatcher_3p1 == NULL) || (test_dispatcher_3p1 != test_dispatcher_3pB))
    {
		test_failed(22); 
	}
	kprintf("Passed test 1 - returns Test process B\n");
    
    
    /*
	 * process B with priority 5. Remove process B from ready queue.
	 * Then Process A with priority 3 is highest
	 * Check that dispatcher() returns this process A
	 *  
	 */    
    remove_ready_queue(test_dispatcher_3pB);
    check_num_proc_on_ready_queue(2);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
    test_dispatcher_3p1 = dispatcher();

	if ((test_dispatcher_3p1 == NULL) || (test_dispatcher_3p1 != test_dispatcher_3pA))
    {
		test_failed(26); 
	}
	kprintf("Passed test 2 - returns Test process A\n");
	
	
	/*
	 * Create Process C with priority 5. Add Process B with priority 5
	 * back to ready queue
	 * Check that dispatcher() returns this process C. It should be first
	 * in line
	 */   
	create_process(test_dispatcher_3_process, 5, 42, "Test process C");
	test_dispatcher_3pC = find_process_by_name("Test process C");
	if (test_dispatcher_3pC == NULL)
    {
		test_result = 10;
		test_failed(test_result);
	}
	add_ready_queue(test_dispatcher_3pB);
	check_num_proc_on_ready_queue(4);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
	test_dispatcher_3p1 = dispatcher();

	if ((test_dispatcher_3p1 == NULL) || (test_dispatcher_3p1 != test_dispatcher_3pC))
    {
		test_failed(23); 
	}
	kprintf("Passed test 3 - returns Test process C\n");
	
	
	/*
	 * Remove both processes B & C from ready queue. Then add process B
	 * back to ready queue first. Then, add process C
	 * Check that dispatcher() returns this process B. It should be first
	 * in line
	 */  
	remove_ready_queue(test_dispatcher_3pC);
	remove_ready_queue(test_dispatcher_3pB);
	check_num_proc_on_ready_queue(2);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
	add_ready_queue(test_dispatcher_3pB);
	add_ready_queue(test_dispatcher_3pC);
    check_num_proc_on_ready_queue(4);
    if (test_result != 0) 
    {
		test_failed(test_result);
	}
	test_dispatcher_3p1 = dispatcher();

	if ((test_dispatcher_3p1 == NULL) || (test_dispatcher_3p1 != test_dispatcher_3pB))
    {
		test_failed(23); 
	}
	kprintf("Passed test 4 - returns Test process B\n\n");
    	
	print_all_processes(kernel_window);
	kprintf("\nAll pass!\n");

}

