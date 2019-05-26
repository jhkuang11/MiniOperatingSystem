
#include <kernel.h>
#include <test.h>


/* 
 * Series of tests of the window.c methods
 * to enhance existing tests and check more 
 * capabilities
 * dh Mar. 2017
 * 
 */

void test_window_7()
{
    test_reset();
    output_string(kernel_window, "test_window_7\n");

    char* expected_output[] = {
	"test_window_7",
	NULL
    };
    check_screen_output(expected_output);
    if (test_result != 0 )
        test_failed(test_result);
    
    /*
	 * A simple line of text is output. Then backspace is done to see if the character is 
	 * replaced with a space 
	 * 
	 */
    
    WINDOW test_window_7a = {8, 6, 40, 6, 2, 2, '!'};
    output_string(&test_window_7a, "test message 7a");
    
    char* expected_output1[] = {
		"test_window_7",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"          test message 7a!",
		NULL
    };
    check_screen_output(expected_output1);
    
    if (test_result != 0 )
        test_failed(test_result);
    
    output_char(&test_window_7a, '\b');
    
    char* expected_output2[] = {
		"test_window_7",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output2);
    
    if (test_result != 0 )
        test_failed(8);   // '\b' error
    
    
    /*
	 * Create a window. Write a message and test multiple backspaces 
	 * 
	 */
    
    WINDOW test_window_7b = {3, 3, 8, 8, 0, 1, '?'};
    output_string(&test_window_7b, "hello");
    
    char* expected_output3[] = {
		"test_window_7",
		"",
		"",
		"",
		"   hello?",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output3);
    
    if (test_result != 0 )
        test_failed(test_result);
    
    output_string(&test_window_7b, "\b\bp");
    
    char* expected_output4[] = {
		"test_window_7",
		"",
		"",
		"",
		"   help?",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output4);
    
    if (test_result != 0 )
        test_failed(8);   // '\b' error
    
    
    /*
	 * Do enough '\b' characters so that it must wrap around to the 
	 * previous line. Check location of cursor 
	 * 
	 */
        
    output_string(&test_window_7b, "\b\b\b\b\b");
    
    char* expected_output5[] = {
		"test_window_7",
		"",
		"",
		"          ?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output5);
    
    if (test_result != 0 )
       test_failed(8);   // '\b' wrap around error 
    
    
    /*
	 * Wrap to the beginning of the line. Verify cursor location is correct.
	 * output a few characters. Make sure it is correct
	 * 
	 */
    
    output_string(&test_window_7b, "\b\b\b\b\b\b\b");
    
    char* expected_output6[] = {
		"test_window_7",
		"",
		"",
		"   ?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output6);
    
    if (test_result != 0 )
       test_failed(8);   // '\b' wrap error
    
    output_string(&test_window_7b, "works");
    
    char* expected_output7[] = {
		"test_window_7",
		"",
		"",
		"   works?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output7);
    if (test_result != 0 )
       test_failed(test_result);  

    
    /*
	 * Wrap again to the beginning of the line. Make sure cursor is correct.
	 * Then back up more. Make sure cursor does not move. It's at the beginning
	 * of the window. Then print a few characters and verify 
	 * 
	 */
    
    output_string(&test_window_7b, "\b\b\b\b\b");
    
    char* expected_output8[] = {
		"test_window_7",
		"",
		"",
		"   ?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output8);
    if (test_result != 0 )
       test_failed(8);  // '\b' wrap error
    
    output_string(&test_window_7b, "\b\b\b\b\b");
    
    char* expected_output9[] = {
		"test_window_7",
		"",
		"",
		"   ?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output9);
    
    if (test_result != 0 )
       test_failed(8);   // '\b' wrap around error 
    
    output_string(&test_window_7b, "done!");
    
    char* expected_output10[] = {
		"test_window_7",
		"",
		"",
		"   done!?",
		"",
		"",
		"",
		"",
		"          test message 7!",
		NULL
    };
    check_screen_output(expected_output10);
    if (test_result != 0 )
        test_failed(test_result); 
    
	output_string(kernel_window, "\nAll pass!\n");   
}

