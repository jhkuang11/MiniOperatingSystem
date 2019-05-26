
#include <kernel.h>

PORT            keyb_port;

#define KEYBD           0x60
#define PORT_B          0x61
#define KBIT            0x80
#define MAXSIZE         1024
#define UP_ARROW	1
#define LEFT_ARROW	2
#define RIGHT_ARROW	3
#define DOWN_ARROW	4

/* Variables indicating scancodes */
static unsigned char brk = 0;
static unsigned ignore = 0;

/* Variables indicating keys */
static unsigned char control = 0;
static unsigned char shift = 0;
static unsigned char new_char;
static unsigned new_key;
static char     value;

static char     done;
static unsigned char special = 0;
static unsigned char capslock = 0;
static unsigned char numlock = 0;
static unsigned char scrlock = 0;
static unsigned char alt = 0;


#define MULT            0x100   /* 1 for testing the computed codes of the 
                                 * cursor and function-keys else 0x100 */

/*******************************************
  START SCAN CODE SECTION.
********************************************/

static unsigned char small[] =
    { 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e',
    'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 13, 0, 'a', 's', 'd', 'f',
    'g', 'h', 'j', 'k',
    'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4',
    '5', '6', '+', '1', '2', '3',
    '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char capped[] =
    { 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'Q', 'W', 'E',
    'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 13, 0, 'A', 'S', 'D', 'F',
    'G', 'H', 'J', 'K',
    'L', ';', 0, '`', '\'', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',',
    '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4',
    '5', '6', '+', '1', '2', '3',
    '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char shfted[] =
    { 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E',
    'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 13, 0, 'A', 'S', 'D', 'F',
    'G', 'H', 'J', 'K',
    'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?', 0, 0, 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*******************************************
  END SCAN CODE SECTION.
********************************************/

void out_data(unsigned ch)
{                               /* send data to the keyboard */
#ifdef KEYBOARD_PS2
    while ((inportb(0x64) & 1) > 0);    /* for some reason, this won't
                                         * work */
#else
    while ((inportb(0x64) & 2) > 0);
#endif
    outportb(0x60, ch);
}

void set_led()
{                               /* set led-status an keyboard */
    out_data(0xed);
    out_data(scrlock + 2 * numlock + 4 * capslock);
}


unsigned get_keycode(unsigned char ch)
{
    /* If it is a printable character, return the ascii value. If it is a
     * non-printable character, return the scan code * MULT. */

    if (!brk) {

        /* F1 to F12 */
        if (!special && !brk && !alt) {
            if ((ch > 0x3A) && (ch < 59))
                return ch * MULT;
        }


        /* Keypad keys */

        /* UP, DN, LF, RT */
        /* If numlock off || arrow keys */
        // if ((ch==0x48) || (ch==0x4B) || (ch==0x50) || (ch==0x4D)) 
        // return ch*MULT;
        if (ch == 0x48)
            return UP_ARROW;
        else if (ch == 0x4B)
            return LEFT_ARROW;
        else if (ch == 0x4D)
            return RIGHT_ARROW;
        else if (ch == 0x50)
            return DOWN_ARROW;

        /* INS, DEL, HM, END, PG-UP, PD-DN */
        if (special) {
            if ((ch == 0x52) || (ch == 0x47) || (ch == 0x49)
                || (ch == 0x53) || (ch == 0x4F) || (ch == 0x51))
                return ch * MULT;
        }
    }

    if (!special && !brk && !alt) {
        if (!shift && !capslock)
            return small[ch - 1];
        if (shift && !capslock)
            return shfted[ch - 1];
        if (capslock)
            return capped[ch - 1];
        if (!shift && capslock)
            return capped[ch - 1];
    }

    /* specials */
    if (special && !brk) {

        if (ch == 0x1C)         /* Keypad enter */
            return 13;
        if (ch == 0x35)         /* Keypad / */
            return '/';

    }
#if 0
    if (control && alt) {
        return;
    }

    if (alt)
        return;
#endif

    return 0;
}


void keyb_notifier(PROCESS self, PARAM param)
{
    Keyb_Message    msg;

    while (1) {
        wait_for_interrupt(KEYB_IRQ);

        new_char = inportb(KEYBD);
#if 0
        kprintf("%02X ", new_char);
#endif
        value = inportb(PORT_B);
        value |= KBIT;
        outportb(PORT_B, value);
        value &= 0x7f;
        outportb(PORT_B, value);

        outportb(PORT_B, value | KBIT);
        outportb(PORT_B, value);
        done = FALSE;

        if ((new_char == 0xE1) && !ignore) {    /* For weird scancodes */
            ignore = 5;
            done = TRUE;
        }

        if (!done && ignore) {  /* Ignore the remaining codes */
            ignore--;           /* of the key with odd scancodes and the
                                 * next */
            done = TRUE;        /* pressed key will clear ignore */
        }

        if (!done && (new_char == 0xE0)) {      /* Flag for codes which
                                                 * are */
            special = 2;        /* used by two keys */
            done = TRUE;
        }

        if (!done && (new_char & 128) == 128) { /* Flag for codes which
                                                 * are break codes */
            brk = 1;
            new_char = new_char & 127;  /* Reset MSB */
        }

        if (!done && !brk && (new_char == 0x2A)) {
            if (special == 0)
                shift = 1;      /* For Left Shift key */
            else
                ignore = 3;     /* For Print screen */
            done = TRUE;
        }

        if (!done) {            /* For those that req key press */
            if (new_char == 0x3A) {     /* go into effect only when key */
                capslock = 1 - capslock;        /* is pressed while it is
                                                 * pressed.  */
                done = TRUE;
                set_led();
            }
            if (new_char == 0x45) {     /* go into effect only when key */
                numlock = 1 - numlock;  /* is pressed. */
                done = TRUE;
                set_led();
            }
            if (new_char == 0x46) {
                scrlock = 1 - scrlock;
                done = TRUE;
                set_led();
            }
        }

        if (!done) {
            if (brk) {          /* For those that req key release */
                if (new_char == 0x1D) {
                    control = 0;        /* Ctrl, alt, shift lose their
                                         * effect */
                    done = TRUE;
                }
                if (new_char == 0x28) { /* once key is lifted. */
                    alt = 0;
                    done = TRUE;
                }
                if (new_char == 0x36) {
                    shift = 0;
                    done = TRUE;
                }
                if (new_char == 0x2A) {
                    shift = 0;
                    done = TRUE;
                }
            } else {            /* For those that req key press */
                if (new_char == 0x1D) { /* Left + Right Ctrl */
                    control = 1;
                    done = TRUE;
                }
                if (new_char == 0x38) { /* Left + Right Alt */
                    alt = 1;
                    done = TRUE;
                }
                if (new_char == 0x36) { /* Right Shift Key */
                    shift = 1;
                    done = TRUE;
                }
            }
        }

        if (!done && ((new_key = get_keycode(new_char)) != 0)) {
            /* we actually have a new keystroke. Send it to the keyboard
             * process */
            msg.key_buffer = (char *) &new_key;
            message(keyb_port, &msg);
        }

        if (special)
            special--;          /* these decrements will allow the */
        if (brk)
            brk--;              /* vars to become false (0) */
        /* after due number of scan codes */

    }
    become_zombie();
}


#define MAX_KEYB_BUFFER 10

typedef struct __KEYB_CLIENT {
    int             window_id;
    PROCESS         client;
    Keyb_Message   *msg;
    BOOL            is_waiting;
    char            buffer[MAX_KEYB_BUFFER];
    int             head;
    int             tail;
    struct __KEYB_CLIENT *next;
} KEYB_CLIENT;

KEYB_CLIENT    *keyb_first_client = NULL;

int             current_window = -1;

KEYB_CLIENT    *get_client_record(int window_id)
{
    assert(window_id != -1);
    KEYB_CLIENT    *record = keyb_first_client;
    while (record != NULL) {
        if (record->window_id == window_id) {
            return record;
        }
        record = record->next;
    }
    // Haven't seen this window_id. Ask WM for the current window
    current_window = wm_current_focus();
    record = malloc(sizeof(KEYB_CLIENT));
    record->window_id = window_id;
    record->client = NULL;
    record->is_waiting = FALSE;
    record->head = 0;
    record->tail = 0;
    record->next = keyb_first_client;
    keyb_first_client = record;
    return record;
}

void enqueue_key(KEYB_CLIENT * client, char key)
{
    client->buffer[client->head] = key;
    client->head = (client->head + 1) % MAX_KEYB_BUFFER;
    if (client->head == client->tail) {
        client->tail = (client->tail + 1) % MAX_KEYB_BUFFER;
    }
}

BOOL has_key_enqueued(KEYB_CLIENT * client)
{
    return client->head != client->tail;
}

char dequeue_key(KEYB_CLIENT * client)
{
    assert(has_key_enqueued(client));
    char            key = client->buffer[client->tail];
    client->tail = (client->tail + 1) % MAX_KEYB_BUFFER;
    return key;
}

#define KEYB_CONTROL_CHANGE_FOCUS 9
#define KEYB_CONTROL_MOVE_LEFT    LEFT_ARROW
#define KEYB_CONTROL_MOVE_RIGHT   RIGHT_ARROW
#define KEYB_CONTROL_MOVE_UP      UP_ARROW
#define KEYB_CONTROL_MOVE_DOWN    DOWN_ARROW

BOOL keyb_handle_control(char key)
{
    switch (key) {
    case KEYB_CONTROL_CHANGE_FOCUS:
        current_window = wm_change_focus();
        return TRUE;
    case KEYB_CONTROL_MOVE_LEFT:
        current_window = wm_current_focus();
        wm_move_left(current_window);
        return TRUE;
    case KEYB_CONTROL_MOVE_RIGHT:
        current_window = wm_current_focus();
        wm_move_right(current_window);
        return TRUE;
    case KEYB_CONTROL_MOVE_UP:
        current_window = wm_current_focus();
        wm_move_up(current_window);
        return TRUE;
    case KEYB_CONTROL_MOVE_DOWN:
        current_window = wm_current_focus();
        wm_move_down(current_window);
        return TRUE;
    }
    return FALSE;
}

void keyb_process(PROCESS self, PARAM param)
{
    Keyb_Message   *msg;
    PROCESS         sender_proc;
    PORT            keyb_notifier_port;
    PROCESS         keyb_notifier_proc;

    keyb_notifier_port =
        create_process(keyb_notifier, 7, 0, "Keyboard Notifier");
    keyb_notifier_proc = keyb_notifier_port->owner;

    while (1) {
        msg = (Keyb_Message *) receive(&sender_proc);
        if (sender_proc == keyb_notifier_proc) {
            /* the notifier has sent us a new keystroke */
            char            key = *msg->key_buffer;
            if (keyb_handle_control(key)) {
                continue;
            }
            if (current_window == -1) {
                current_window = wm_current_focus();
                if (current_window == -1) {
                    // No window exists. Just discard key
                    continue;
                }
            }
            KEYB_CLIENT    *record = get_client_record(current_window);
            if (record->is_waiting) {
                *record->msg->key_buffer = key;
                reply(record->client);
                record->is_waiting = FALSE;
            } else {
                enqueue_key(record, key);
            }
        } else {
            // Message is from a client
            KEYB_CLIENT    *record = get_client_record(msg->window_id);
            if (has_key_enqueued(record)) {
                *msg->key_buffer = dequeue_key(record);
                reply(sender_proc);
            } else {
                if (msg->block) {
                    record->client = sender_proc;
                    record->msg = msg;
                    record->is_waiting = TRUE;
                } else {
                    // No key and also don't block. Reply immediately
                    *msg->key_buffer = 0;
                    reply(sender_proc);
                }
            }
        }
    }
    become_zombie();
}


char keyb_get_keystroke(int window_id, BOOL block)
{
    Keyb_Message    msg;
    char            ch;

    msg.window_id = window_id;
    msg.block = block;
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    return ch;
}

/*-------------------------------------------------------------------*\
  init_keyb() - creates the keyb_process
\*-------------------------------------------------------------------*/

void init_keyb()
{
    keyb_port = create_process(keyb_process, 6, 0, "Keyboard Process");
    resign();
}
