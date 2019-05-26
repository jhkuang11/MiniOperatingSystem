/*
Name: Jianhong Kuang
ID: 916435720
*/

#include <kernel.h>


// **************************
// run the train application
// **************************

#define SLEEP_TICKS 30

// Use this parameter to fine-tune train-wagon connection
#define FINE_TUNE 30

// Use this loop to detect whether zamboni is present 
#define ZAMBONI_LOOP 15


/*
command to change train speed
*/
void change_train_speed(int window_id, char* speed)
{
	// added this due to lab experiment. Good lesson
	// pause between commands
	// Lab uses 5, but 1 is good here
	sleep(1);
	COM_Message speed_msg;

	speed_msg.output_buffer = speed;
	speed_msg.input_buffer = NULL;
	speed_msg.len_input_buffer = 0;

	send(com_port, &speed_msg);

	wm_print(window_id, "Changing train's speed to: %c\n", speed[4]);
}


/*
command to change switch at contact ID
*/
void set_switch(int window_id, char* switch_option)
{
	sleep(1);
	COM_Message switch_msg;

	switch_msg.output_buffer = switch_option;
	switch_msg.input_buffer= NULL;
	switch_msg.len_input_buffer = 0;

	send(com_port, &switch_msg);

	wm_print(window_id, "Setting switch %c ", switch_option[1]);
	wm_print(window_id, "to %c\n", switch_option[2]);

}


/*
command to reverse train direction
*/
void change_train_direction(int window_id, char* direction)
{
	sleep(1);
	COM_Message direction_msg;

	direction_msg.output_buffer = direction;
	direction_msg.input_buffer = NULL;
	direction_msg.len_input_buffer = 0;

	send(com_port, &direction_msg);

	wm_print(window_id, "Reversing train's direction\n");
}


/*
command to clear memory buffer
*/
void clear_memory_buffer(int window_id)
{
	sleep(1);
	COM_Message clear_buffer;

	clear_buffer.output_buffer = "R\015";
	clear_buffer.input_buffer = NULL;
	clear_buffer.len_input_buffer = 0;

	send(com_port, &clear_buffer);

	wm_print(window_id, "Clearing memory buffer\n");
}


/*
This initialization allows Zamboni to go on outer loop without
going to dead end
*/
void init_train_track(int window_id)
{
	wm_print(window_id, "Initialzing track switches\n");
	set_switch(window_id, "M5G\015");
	set_switch(window_id, "M8G\015");
	set_switch(window_id, "M1G\015");
	set_switch(window_id, "M4G\015");
	set_switch(window_id, "M9R\015");
}


/*
This method checks a contact point for the presence of
train/wagon/zamboni
*/
int probe_contact(int window_id, char* contact_id)
{
	// Since I use "C[Number]\015" as my command
	// the printing will have an unknown symbol at
	// the end, didn't really want to do extra
	// work to remove that.
	wm_print(window_id, "Probing contact: %s\n", contact_id);

	int probe_status;

	clear_memory_buffer(window_id);

	sleep(1);

	COM_Message probe_msg;
	char buffer[3];

	probe_msg.output_buffer = contact_id;
	probe_msg.input_buffer = buffer;
	probe_msg.len_input_buffer = 3;

	send(com_port, &probe_msg);

	probe_status = buffer[1] - '0';

	return probe_status;

}


/*
This method keeps probing a contact point until
train/wagon/zamboni is there
*/
void keep_probing(int window_id, char* contact_id)
{
	while(1)
	{
		if (probe_contact(window_id, contact_id) == 1)
			return;
	}
	
}


/*
Zamboni always goes on outer loop so it will always go back
to contact 4 where it begins, so keep probing contact 4 until
Zamboni comes back. ZAMBONI_LOOP is the length of time by which
Zamboni makes a round trip. Can be tuned to smaller number.
*/
int find_zamboni(int window_id)
{
	int found = 0;
	for (int i = 0; i < ZAMBONI_LOOP; i++)
	{
		if (probe_contact(window_id, "C4\015") == 1)
		{
			found = 1;
			wm_print(window_id, "Zamboni is detected!!!\n");
			break;
		}
		sleep(SLEEP_TICKS);
	}
	if (found)
		return 1;
	else
	{
		wm_print(window_id, "Zamboni is not detected!!!\n");
		return 0;
	}
}


void run_config_1(int window_id)
{
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6R\015");
	change_train_speed(window_id, "L20S5\015");

	// This part determines how the train
	// connects with wagon.
	keep_probing(window_id, "C6\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	change_train_speed(window_id, "L20S0\015");


	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C7\015");
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S3\015");
	keep_probing(window_id, "C8\015");
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_1_zamboni(int window_id)
{
	keep_probing(window_id, "C4\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");

	// This part determines how the train
	// connects with wagon. 
	keep_probing(window_id, "C6\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	set_switch(window_id, "M4G\015");
	change_train_speed(window_id, "L20S0\015");


	change_train_direction(window_id, "L20D\015");
	keep_probing(window_id, "C4\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6R\015");
	keep_probing(window_id, "C7\015");
	sleep(SLEEP_TICKS*2);
	change_train_speed(window_id, "L20S3\015");
	keep_probing(window_id, "C8\015");
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_2(int window_id)
{
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");
	change_train_speed(window_id, "L20S5\015");

	// This part determines how the train
	// connects with wagon.
	keep_probing(window_id, "C6\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	change_train_speed(window_id, "L20S0\015");


	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M9G\015");
	keep_probing(window_id, "C14\015");
	change_train_speed(window_id, "L20S3\015");
	keep_probing(window_id, "C16\015");
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_2_zamboni(int window_id)
{
	keep_probing(window_id, "C4\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C14\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");
	keep_probing(window_id, "C7\015");

	// This part determines how the train
	// connects with wagon.
	keep_probing(window_id, "C6\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	change_train_speed(window_id, "L20S3\015");
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S0\015");


	change_train_direction(window_id, "L20D\015");
	keep_probing(window_id, "C10\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C3\015");
	set_switch(window_id, "M9G\015");
	keep_probing(window_id, "C14\015");
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S2\015");
	keep_probing(window_id, "C16\015");
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S0\015");
	set_switch(window_id, "M9R\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_3(int window_id)
{
	// complete event-driven. Always works
	// No need to use sleep()
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6G\015");
	keep_probing(window_id, "C7\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M9G\015");
	keep_probing(window_id, "C14\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S3\015");
	keep_probing(window_id, "C16\015");

	// sleep() is used to make the train
	// go further
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_3_zamboni(int window_id)
{
	// complete event-driven. Always works
	// No need to use sleep()
	keep_probing(window_id, "C10\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C6\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6G\015");
	keep_probing(window_id, "C7\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C12\015");
	change_train_speed(window_id, "L20S0\015");
	keep_probing(window_id, "C13\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M9G\015");
	keep_probing(window_id, "C14\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S3\015");
	keep_probing(window_id, "C16\015");

	// sleep() is used to make the train
	// go further
	sleep(SLEEP_TICKS);
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_4(int window_id)
{
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6G\015");
	keep_probing(window_id, "C12\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	set_switch(window_id, "M7R\015");
	change_train_speed(window_id, "L20S5\015");
	keep_probing(window_id, "C13\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");

	// This part determines how the train
	// stops at its initial place
	keep_probing(window_id, "C6\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}


void run_config_4_zamboni(int window_id)
{
	keep_probing(window_id, "C10\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M5R\015");
	set_switch(window_id, "M6G\015");
	keep_probing(window_id, "C12\015");
	set_switch(window_id, "M5G\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	set_switch(window_id, "M7R\015");
	keep_probing(window_id, "C14\015");
	change_train_speed(window_id, "L20S5\015");
	set_switch(window_id, "M4R\015");
	set_switch(window_id, "M3R\015");
	keep_probing(window_id, "C7\015");

	// This part determines how the train
	// stops at its initial place
	keep_probing(window_id, "C6\015");
	change_train_speed(window_id, "L20S0\015");
	change_train_direction(window_id, "L20D\015");
	change_train_speed(window_id, "L20S5\015");
	sleep(SLEEP_TICKS + FINE_TUNE);
	change_train_speed(window_id, "L20S0\015");
	wm_print(window_id, "Finished!!!");
}

/*
This method determines which configuration it is
and whether the Zamboni is present. Gives an
execution number
*/
int check_configuration(int window_id)
{
	wm_print(window_id, "Checking the configuration number\n");
	if (probe_contact(window_id, "C8\015") == 1 && probe_contact(window_id, "C5\015") == 1)
	{
		if (find_zamboni(window_id))
			return 11;
		return 10;
	} 
	else if (probe_contact(window_id, "C16\015") == 1 && probe_contact(window_id, "C5\015") == 1)
	{
		if (find_zamboni(window_id))
			return 21;
		return 20;
	}
	else if (probe_contact(window_id, "C16\015") == 1 && probe_contact(window_id, "C9\015") == 1)
	{
		if (find_zamboni(window_id))
			return 31;
		return 30;
	}
	else if (probe_contact(window_id, "C5\015") == 1 && probe_contact(window_id, "C11\015") == 1)
	{
		if (find_zamboni(window_id))
			return 41;
		return 40;
	}
	else
	{
		wm_print(window_id, "Unclear configuration\n");
		return -1;
	}
}


/*
This method executes the wagon-extraction route
according to whatever configuration is detected
*/
void run_train(int window_id)
{
	int config_no = check_configuration(window_id);
	switch (config_no)
	{
		case 10:
			wm_print(window_id, "Configuration 1 without Zamboni\n");
			run_config_1(window_id);
			break;
		case 11:
			wm_print(window_id, "Configuration 1 with Zamboni\n");
			run_config_1_zamboni(window_id);
			break;
		case 20:
			wm_print(window_id, "Configuration 2 without Zamboni\n");
			run_config_2(window_id);
			break;
		case 21:
			wm_print(window_id, "Configuration 2 with Zamboni\n");
			run_config_2_zamboni(window_id);
			break;
		case 30:
			wm_print(window_id, "Configuration 3 without Zamboni\n");
			run_config_3(window_id);
			break;
		case 31:
			wm_print(window_id, "Configuration 3 with Zamboni\n");
			run_config_3_zamboni(window_id);
			break;
		case 40:
			wm_print(window_id, "Configuration 4 without Zamboni\n");
			run_config_4(window_id);
			break;
		case 41:
			wm_print(window_id, "Configuration 4 with Zamboni\n");
			run_config_4_zamboni(window_id);
			break;
	}
}


void train_process(PROCESS self, PARAM param)
{
	int window_id = wm_create(1,1,78,8);
	wm_print(window_id, "Train simulator logger\n");
	wm_print(window_id, "--------------------------------\n");

	init_train_track(window_id);
	run_train(window_id);
	while(1);
}


void init_train()
{
	create_process(train_process, 5, 0, "Train process");
}
