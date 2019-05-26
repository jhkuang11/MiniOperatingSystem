/* 
 * Internet ressources:
 * 
 * http://workforce.cup.edu/little/serial.html
 *
 * http://www.lammertbies.nl/comm/info/RS-232.html
 *
 */


#include <kernel.h>

PORT            com_port;


void init_uart()
{
    /* LineControl disabled to set baud rate */
    outportb(COM1_PORT + 3, 0x80);
    /* lower byte of baud rate */
    outportb(COM1_PORT + 0, 0x30);
    /* upper byte of baud rate */
    outportb(COM1_PORT + 1, 0x00);
    /* 8 Bits, No Parity, 2 stop bits */
    outportb(COM1_PORT + 3, 0x07);
    /* Interrupt enable */
    outportb(COM1_PORT + 1, 1);
    /* Modem control */
    outportb(COM1_PORT + 4, 0x0b);
    inportb(COM1_PORT);
}



void com_reader_process(PROCESS self, PARAM param)
{
    PORT            reply_port;
    PROCESS         sender_proc;
    COM_Message    *msg;
    int             i;

    reply_port = (PORT) param;
    while (1) {
        msg = (COM_Message *) receive(&sender_proc);
        i = 0;
        while (i != msg->len_input_buffer) {
            wait_for_interrupt(COM1_IRQ);
            msg->input_buffer[i++] = inportb(COM1_PORT);
        }
        message(reply_port, &msg);
    }
    become_zombie();
}


void send_cmd_to_com(char *cmd)
{
    while (*cmd != '\0') {
        /* 
         * Wait for the UART to accept the next byte
         */
        while (!(inportb(COM1_PORT + 5) & (1 << 5)));
        outportb(COM1_PORT, *cmd);
        cmd++;
    }
}


void com_process(PROCESS self, PARAM param)
{
    PORT            com_reader_port;
    PORT            com_writer_port;
    PROCESS         sender_proc;
    PROCESS         recv_proc;
    COM_Message    *msg;

    /* create a second port for receiving messages from COM reader process 
     */
    com_writer_port = create_new_port(self);

    /* create a port for COM reader process */
    com_reader_port = create_process(com_reader_process, 7,
                                     (PARAM) com_writer_port,
                                     "COM reader");

    while (42) {
        open_port(com_port);
        close_port(com_writer_port);
        msg = (COM_Message *) receive(&sender_proc);    // receive a
        // message from
        // user process
        message(com_reader_port, msg);
        send_cmd_to_com(msg->output_buffer);

        close_port(com_port);
        open_port(com_writer_port);
        receive(&recv_proc);    // receive a message from COM reader
        // process
        /* assert (recv_proc == com_reader_proc); */
        reply(sender_proc);
    }
    become_zombie();
}


void init_com()
{
    init_uart();
    com_port = create_process(com_process, 6, 0, "COM process");
    resign();
}
