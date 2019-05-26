
#include <kernel.h>
#include <test.h>


/* 
 * Series of tests of the window.c methods
 * to enhance existing tests and check more 
 * capabilities
 * dh Mar. 2017
 * 
 */

void test_window_6()
{	
    test_reset();
    output_string(kernel_window, "test_window_6\n");

    char* expected_output1[] = {
		"test_window_6",
		NULL
    };
    check_screen_output(expected_output1);
    if (test_result != 0 )
        test_failed(test_result);
    
    WINDOW test_window_6a = {20, 10, 20, 6, 0, 0, '%'};
    output_string(&test_window_6a, "test message 6a");
    
    char* expected_output2[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"                    test message 6a%",
		NULL
    };
    check_screen_output(expected_output2);
    
    if (test_result != 0 )
        test_failed(test_result);  
       

    /*
	 * A window is created with a special cursor character and text is written into it. The
	 * cursor is moved onto of the text and the updated display with the cursor in the middle 
	 * of the text is verified. Then the cursor is moved. It is verified that there is no 
	 * visible change in the cursor display on the screen.
	 * 
	 */
    
    WINDOW test_window_6b = {0, 0, 40, 7, 10, 6, '='};
    output_string(&test_window_6b, "test message 6b!@#$%");
    
    char* expected_output3[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"          test message 6b!@#$%=",
		"",
		"",
		"",
		"                    test message 6a%",
		NULL
    };
    check_screen_output(expected_output3);
    if (test_result != 0 )
       test_failed(test_result); 
    
    move_cursor(&test_window_6b, 9, 5);
    char* expected_output4[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"          test message 6b!@#$%=",
		"",
		"",
		"",
		"                    test message 6a%",
		NULL
    };
    check_screen_output(expected_output4);
    
    if (test_result != 0 )
       test_failed(7);  // bad move_cursor() error code
    
    
    /*
	 * Then, show_cursor() is executed and it is verified that the special cursor appears
	 * 
	 */
    
    show_cursor(&test_window_6b);
    char* expected_output5[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"         =",
		"          test message 6b!@#$%=",
		"",
		"",
		"",
		"                    test message 6a%",
		NULL
    };
    check_screen_output(expected_output5);
    if (test_result != 0 )
       test_failed(5);  // problem with show_cursor()
    
    
    /*
	 * Then, remove_cursor() is executed and it is verified that the special cursor has 
	 * been replaced by a space.
	 * 
	 */    
    
    remove_cursor(&test_window_6b);
    char* expected_output6[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"          test message 6b!@#$%=",
		"",
		"",
		"",
		"                    test message 6a%",
		NULL
    };
    check_screen_output(expected_output6);
    
    if (test_result != 0 )
       test_failed(6);  // problem with remove_cursor()

    
    /*
	 * A window is created with a special cursor character and text is written into it. The 
	 * text and cursor character are verified. Then the cursor is moved to a new location and 
	 * the clearing of the old cursor and appearing in the new location is verified. The cursor
	 * is moved by going through the 3 step process: remove_cursor(), move_cursor() and
	 * show_cursor()
	 * 
	 */    
    
    remove_cursor(&test_window_6a);
    char* expected_output7[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"          test message 6b!@#$%=",
		"",
		"",
		"",
		"                    test message 6a ",
		NULL
    };
    check_screen_output(expected_output7);
    
    if (test_result != 0 )
        test_failed(6);   // replace with remove_cursor() error

    move_cursor(&test_window_6a, 5, 0);
    show_cursor(&test_window_6a);
    char* expected_output8[] = {
		"test_window_6",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"                    test %essage 6a ",
		NULL
    };
    check_screen_output(expected_output8);
    
    if (test_result != 0 )
        test_failed(5);   // replace with show_cursor() error
    
	output_string(kernel_window, "\nAll pass!\n");
}

