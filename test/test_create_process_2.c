
#include <kernel.h>
#include <test.h>


void test_create_process_2a_process_a (PROCESS self, PARAM param)
{
    /*
     * Since we don't do a context switch yet, this code will actually
     * not be executed.
     */
    kprintf("Process: %s\n\n", self->name);
    return_to_boot();
}

void check_test_create_process_2b();

void test_create_process_2b_process_a(PROCESS self, PARAM param)
{
    kprintf("Process: %s\n\n", self->name);
    return_to_boot();
}

void test_create_process_2b_process_b(PROCESS self, PARAM param)
{
    kprintf("Process: %s\n\n", self->name);
    return_to_boot();
}

void test_create_process_2b_process_c(PROCESS self, PARAM param)
{
    kprintf("Process: %s\n\n", self->name);
    return_to_boot();
}


/*
 * Explicitly create a new process. We don't do a context switch just yet.
 * The new process should be in STATE_READY.
 * 
 * Explicitly create three new processes of the same priority. 
 * This tests add_ready_queue.
 * 
 * This is the old test_create_proces_3 and _4 combined. dh 2017-2-23
 */
void test_create_process_2() 
{
	/* This is the old test_create_process_3 code */
	test_reset();

    create_process(test_create_process_2a_process_a, 5, 42, "Test process A");
    kprintf("Created Test Process A. \n\n");
    print_all_processes(kernel_window);

    check_create_process(boot_name, 1, NULL, 0);
    if (test_result != 0)
       test_failed(test_result);

    check_create_process("Test process A", 5,
			 test_create_process_2a_process_a, 42);
    if (test_result != 0)
       test_failed(test_result);

    check_num_of_pcb_entries(2);
    if (test_result != 0)
       test_failed(test_result);

    check_process(boot_name, STATE_READY, TRUE);
    if (test_result != 0)
       test_failed(test_result);

    check_process("Test process A", STATE_READY, TRUE);
    if (test_result != 0)
       test_failed(test_result);

    check_num_proc_on_ready_queue(2);
    if (test_result != 0)
       test_failed(test_result);
	
	
	/* This is the old test_create_process_4 code     */
	
	kprintf("\n\n");
	/* This block of code does the old test_create_process_1 tests   
     * These 4 lines do the test_reset() without clearing the screen */
    test_result = 0;
    init_process();
    init_dispatcher();
    init_ipc();    

    create_process(test_create_process_2b_process_a, 1, 42, "Test process A");
    kprintf("Created Test Process A. \n");
    create_process(test_create_process_2b_process_b, 1, 42, "Test process B");
    kprintf("Created Test Process B. \n");
    create_process(test_create_process_2b_process_b, 1, 42, "Test process C");
    kprintf("Created Test Process C. \n\n");
    print_all_processes(kernel_window);

    check_test_create_process_2b();
}


void check_test_create_process_2b() {
    //check the four processes's pcb entries
    check_create_process(boot_name, 1, NULL, 0);
    check_create_process("Test process A", 1,
			 test_create_process_2b_process_a, 42);
    check_create_process("Test process B", 1,
			 test_create_process_2b_process_b, 42);
    check_create_process("Test process C", 1,
			 test_create_process_2b_process_b, 42);
    check_num_of_pcb_entries(4);
    if (test_result != 0) 
       test_failed(test_result);
   
    //check number of processes on ready queue
    check_num_proc_on_ready_queue(4);
    if (test_result != 0) 
       test_failed(test_result);
 
    //check the ready queue. make sure the processes are in the right order. 
    //(Processes created later should be after processes that are created 
    //earlier.)
    PROCESS first_process = ready_queue[1];
    if (string_compare(first_process->name, "Boot process") == 0 ||
        string_compare(first_process->next->name, "Test process A") == 0 ||
        string_compare(first_process->next->next->name, "Test process B") == 0 ||
        string_compare(first_process->next->next->next->name, "Test process C") == 0) 
       test_failed(17);
}

