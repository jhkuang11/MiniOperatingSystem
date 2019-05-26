
/*
Name: Jianhong Kuang
ID: 916435720
*/

#include <kernel.h>

#define BUF_LEN 50

// a buffer to hold what the user types. Length is 20.
typedef struct _input_buffer
{
	int length;
	char buffer[BUF_LEN];
} input_buffer;

/*
Read whatever the user types and saves to the buffer
*/
void read_input(int window_id, input_buffer* input)
{
	char ch;
	input->length = 0;
	do
	{
		ch = keyb_get_keystroke(window_id, TRUE);
		switch (ch)
		{
			// clear character
			case '\b':
				if (input->length == 0) continue;
				input->length--;
				wm_print(window_id, "%c", ch);
				break;
			// press enter
			case 13:
				wm_print(window_id, "%c", '\n');
				break;
			// save those typed characters
			default:
				input->buffer[input->length++] = ch;
				wm_print(window_id, "%c", ch);
				break;
		}
	} while (ch != '\n' && ch != '\r');
	input->buffer[input->length] = '\0';
}

/*
Compare whether two strings are the same
*/
int compare_cmd(char* cmd1, char* cmd2)
{
	while(*cmd1 != '\0' && *cmd2 != '\0')
	{
		if (*cmd1 != *cmd2)
			return 0;
		cmd1++;
		cmd2++;
	}
	if (*cmd1 == '\0' && *cmd2 == '\0')
		return 1;
	else
		return 0;
}

/*
help command to print all available commands
*/
void help(int window_id)
{
	wm_print(window_id, "***************************************************\n");
	wm_print(window_id, "List of commands:\n\n");
	wm_print(window_id, "help       print the list of commands\n");
	wm_print(window_id, "cls        clear the screen\n");
	wm_print(window_id, "shell      open another shell\n");
	wm_print(window_id, "pong       launch the pong game\n");
	wm_print(window_id, "echo <MSG> print string after echo command\n");
	wm_print(window_id, "ps         print the list of processes\n");
	wm_print(window_id, "conv <num> convert a number to binary & hexadecimal\n");
	wm_print(window_id, "about      print developer's name & message\n");
	wm_print(window_id, "train      launch train\n");
	wm_print(window_id, "process_a  self-define process testing\n");
	wm_print(window_id, "***************************************************\n");
}

/*
Prints out info about a process
*/
void list_process_info(int window_id, PCB* p)
{
	static const char* state[] = 
	{
		"READY          ",
		"ZOMBIE         ",
		"SEND_BLOCKED   ",
		"REPLY_BLOCKED  ",
		"RECEIVE_BLOCKED",
		"MESSAGE_BLOCKED",
		"INTR_BLOCKED   ",
	};
	// if the PCB slot is not used
	if (!p->used)
	{
		wm_print(window_id, "PCB slot unused!\n");
		return;
	}
	// print out relevant information
	wm_print(window_id, state[p->state]);
	if (p == active_proc)
		wm_print(window_id, " *    ");
	else
		wm_print(window_id, "      ");
	wm_print(window_id, "  %2d", p->priority);
	wm_print(window_id, " %s\n", p->name);

}

/*
ps command to list the process table
*/
void ps(int window_id)
{
	int i;
	PCB* p = pcb;
	wm_print(window_id,"State           Active Prio Name\n");
	wm_print(window_id,"------------------------------------------------\n");

	// check all slots in the pcb array
	for(i = 0; i < MAX_PROCS; i++, p++)
	{
		if (!p->used) continue;
		list_process_info(window_id, p);
	}
}

/*
echo command to print out strings following the echo.
echo <MSG> (anything after <MSG> is not valid)
*/
void echo(int window_id, char* cmd)
{
	wm_print(window_id, cmd);
	wm_print(window_id, "\n");
}

/*
Convert a number into its binary form
*/
char* deci_to_binary(int n)
{
	int c, d, count;
	char* p;

	count = 0;
	p = (char*)malloc(32+1);

	// bitwise operation to store each bit in 32-bit char array
	for(c = 31; c >= 0; c--)
	{
		d = n >> c;
		if (d & 1)
			*(p + count) = 1 + '0';
		else
			*(p + count) = 0 + '0';
		count++;
	}
	*(p + count) = '\0';
	return p;
}

/*
Convert a string number into integer form
*/
int Atoi(char* str)
{
	if (*str == '\0')
		return 0;

	int res = 0;
	int sign = 1;
	int i = 0;

	// sign of the number
	if (str[0] == '-')
	{
		sign = -1;
		i++;
	}

	for(; str[i] != '\0'; ++i)
	{
		if (str[i] < '0' || str[i] > '9')
			return 0;
		// turn char into int and combine the numbers
		res = res*10 + (str[i] - '0');
	}
	return sign*res;
}

/*
Calculate the length of a string
*/
int string_length(char* str)
{
	int len = 0;
	while (*(str + len) != '\0')
		len++;
	return len;
}

/*
Reverse a string
*/
void reverse_str(char* str)
{
	int length;
	char* start, *end, temp;
	length = string_length(str);
	start = end = str;
	// find out the end of the string
	for (int i = 0; i < length - 1; i++)
		end++;
	for (int i = 0; i < length/2; i++)
	{
		//swap two numbers from two sides until convergence
		temp = *end;
		*end = *start;
		*start = temp;
		start++;
		end--;
	}
}

/*
Convert a number into its hexadecimal form
*/
char* deci_to_hexadecimal(int n)
{
	char* hexaDeci;
	hexaDeci = (char*)malloc(100);
	int i = 0;

	//store each hexa number in reverse order
	while (n != 0)
	{
		int temp = 0;
		temp = n % 16;

		if (temp < 10)
		{
			hexaDeci[i] = temp + 48;
			i++;
		}
		else
		{
			hexaDeci[i] = temp + 55;
			i++;
		}
		n = n / 16;
	}
	hexaDeci[i] = '\0';
	// reverse the string to get the correct form
	reverse_str(hexaDeci);
	return hexaDeci;
}

/*
conv command to print the number in binary and hexadecimal form
*/
void conv(int window_id, char* cmd)
{
	int num = Atoi(cmd);
	char* binaryNum = deci_to_binary(num);
	char* hexaNum = deci_to_hexadecimal(num);
	wm_print(window_id, "Binary: %s\n", binaryNum);
	wm_print(window_id, "Hexadecimal: %s\n", hexaNum);
}

/*
about command to print out some texts
*/
void about(int window_id)
{
	wm_print(window_id, "This is Jianhong Kuang\n");
	wm_print(window_id, "TOS is cool!\n");
}

/*
command struct to save the first argument (command)
and the second argument (parameter)
*/
typedef struct _command
{
	char* com;
	char* para;
} command;

/*
Process what the user has typed in the buffer and
split those strings into two arguments: command
and parameter
*/
command process_cmd(char* str)
{
    char* p = (char*)malloc(20);
    char* comm = p;
    
    // skip leading spaces
    while (*str == ' ')
    	str++;
    
    // process the command or the first argument
    while (*str != ' ' && *str != '\0')
    {
        *p++ = *str++; 
    }
    *p = '\0';
    
    // if there is no second argument
    if (*str == '\0')
    {
        command COM = {comm, "\0"};
        return COM;
    }
  
    // process the parameter or the second argument
    char* parameter = (char*)malloc(20);
    char* para = parameter;
    while (*str != '\0')
    {
        while (*str == ' ') 
        	str++;

        *parameter++ = *str++;

        // any extra arguments are invalid
        if (*str == ' ') 
        	break;
    }
    *parameter = '\0';
    command COM = {comm, para};

    return COM;
}

/*
Execute the shell according to the command typed by user.
Only echo and conv are allowed to have parameter commands.
*/
void execute_cmd(int window_id, command* cmd)
{
	if (compare_cmd(cmd->com, "help")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			help(window_id);
	} else if (compare_cmd(cmd->com, "cls")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			wm_clear(window_id);
	} else if (compare_cmd(cmd->com, "ps")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			ps(window_id);
	} else if (compare_cmd(cmd->com, "shell")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			start_shell();
	} else if (compare_cmd(cmd->com, "pong")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			start_pong();
	} else if (compare_cmd(cmd->com, "echo")){
		echo(window_id, cmd->para);
	} else if (compare_cmd(cmd->com, "conv")){
		conv(window_id, cmd->para);
	} else if (compare_cmd(cmd->com, "about")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			about(window_id);
	} else if (compare_cmd(cmd->com, "train")){
		if (string_length(cmd->para) > 0)
			wm_print(window_id, "command not valid. Type help\n");
		else
			init_train();
	} else {
		wm_print(window_id, "command not found. Type help\n");
	}
}

// The shell process
void shell_process(PROCESS self, PARAM param)
{
	// create a window
	int window_id = wm_create(1,1,52,17);
    wm_print(window_id, "Welcome to TOS shell by Jianhong!\n");
    wm_print(window_id, "Input help for list of commands\n");

    // create a buffer
    input_buffer input;
    input_buffer* input_p = &input;

    while(1)
    {
    	wm_print(window_id, ">");
    	// read what the user types
    	read_input(window_id, input_p);
    	if(input_p->length > BUF_LEN){
    		wm_print(window_id, "Command is too long. Max length is %d!\n", BUF_LEN);
    		continue;
    	}
    	// process all white spaces and split the string into command and parameter
    	command cmd = process_cmd(input_p->buffer);
    	execute_cmd(window_id, &cmd);
    };
}


void start_shell()
{
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}