
#include <kernel.h>
#include <test.h>


/* 
 * Series of tests of the window.c methods
 * to enhance existing tests and check more 
 * capabilities
 * dh Mar. 2017
 * 
 */

void test_window_5()
{
	/*
	 * A smaller window is created. Then a message is printed to the kernel window, but
	 * outside of that smaller window. A message is printed to the smaller window. Then
	 * clear_window() is executed on the smaller window. It is verified that this window 
	 * is cleared but not the message out of the kernel window. Also, the cursor location
	 * and cursor character (not a space) are verified
	 * 
	 */
	 
	unsigned video_base = 0xB8000;
	unsigned columns = 80;
	unsigned rows = 25;
	int i,j;
	unsigned base;
	char ch;

    test_reset();
    output_string(kernel_window, "test_window_5");
    
    WINDOW test_window_5a = {6, 6, 20, 6, 0, 0, '#'};
    output_string(&test_window_5a, "test window 5");

    char* expected_output1[] = {
		"test_window_5 ",
		"",
		"",
		"",
		"",
		"",
		"      test window 5#",
		NULL
    };
    check_screen_output(expected_output1);
    if (test_result != 0 )
    {
		test_failed(test_result);
	}
	
	
	/* Here, we clear test_window_5a. It should just clear the inside message
	 * The cursor (#) should be at the beginning of the window */
	
	clear_window(&test_window_5a);
		
    char* expected_output2[] = {
		"test_window_5",
		"",
		"",
		"",
		"",
		"",
		"      #                   ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		NULL
    };
	check_screen_output(expected_output2);
    if (test_result != 0 )
    {
		test_failed(4);   // bad clear_window()
	}


	/*
	 * This tests the clear_window() method. A message is sent to the kernel window
	 * The clear_window() is executed. The whole window is verified to be either 0x00
	 * or ' '. Also, the cursor must be at (0,0).
	 * 
	 */
	
	clear_window(kernel_window);
	for (j = 0; j < rows; j++) 
	{
		base = video_base + j * columns * 2;
		for (i = 0; i < columns; i++) 
		{
			ch = peek_b(base + 2 * i);
			if ((ch != 0) && (ch != ' '))
			{
				test_result = 4;   // bad clear_window() error code 
			}			 
		}
	}
	if (test_result != 0 )
    {
		test_failed(test_result);
	}
	
	
	/*
	 * This tests does a show_cursor for the kernel window and 
	 * test_window_5a and test_window_5b. Kernel window uses a ' '
	 * but the other two windows have different characters. These are 
	 * verified. A simple message is also printed on test_window_5a.
	 * 
	 */	
	
	WINDOW test_window_5b = {3, 9, 20, 6, 0, 0, '@'};
	output_string(&test_window_5a, "Hello World!");
	show_cursor(kernel_window);
	show_cursor(&test_window_5a);
	show_cursor(&test_window_5b);
	char* expected_output3[] = {
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"      Hello World!#       ",
		"                          ",
		"                          ",
		"   @                      ",
		"                          ",
		"                          ",
		NULL
    };
	check_screen_output(expected_output3);
	
    if (test_result != 0 )
    {
		test_failed(5);   // bad show_cursor() code
	}
		
	
	/*
	 * This test takes the screen of the previous test and turns off all 
	 * three cursors with remove_cursor() and sees if the result is correct
	 * 
	 */	
	
	remove_cursor(kernel_window);
	remove_cursor(&test_window_5a);
	remove_cursor(&test_window_5b);
	char* expected_output4[] = {
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"      Hello World!        ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		"                          ",
		NULL
    };
	check_screen_output(expected_output4);
	
    if (test_result != 0 )
    {
		test_failed(6);   // bad remove_cursor() code
	}

	output_string(kernel_window, "test_window_5\n");
	output_string(kernel_window, "\nAll pass!\n");
}

