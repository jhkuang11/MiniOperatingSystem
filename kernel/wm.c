
#include <kernel.h>

#define WM_TYPE_CREATE 0
#define WM_TYPE_CONTROL 1
#define WM_TYPE_PRINT 2

typedef struct {
    // Input
    int             x,
                    y;
    int             width,
                    height;
    // Output
    int             window_id;
} WM_MSG_CREATE;

#define WM_ACTION_CHANGE_FOCUS 0
#define WM_ACTION_CURRENT_FOCUS 1
#define WM_ACTION_GET_BUFFER 2
#define WM_ACTION_REDRAW 3
#define WM_ACTION_CLEAR 4
#define WM_ACTION_SET_CURSOR 5
#define WM_ACTION_MOVE_LEFT 6
#define WM_ACTION_MOVE_RIGHT 7
#define WM_ACTION_MOVE_UP 8
#define WM_ACTION_MOVE_DOWN 9

typedef struct {
    // Input
    int             action;
    int             cursor_x,
                    cursor_y;
    int             cursor_char;
    // Inout/Output
    int             window_id;
    // Output
    char           *buffer;
} WM_MSG_CONTROL;

typedef struct {
    // Input
    int             window_id;
    char           *str;
    // Output
} WM_MSG_PRINT;

typedef struct {
    int             type;
    union {
        WM_MSG_CREATE   create;
        WM_MSG_CONTROL  control;
        WM_MSG_PRINT    print;
    } u;
} MSG_WM;

typedef struct __WM {
    int             window_id;
    int             x,
                    y;
    int             width,
                    height;
    int             cursor_x,
                    cursor_y;
    char            cursor_char;
    char           *buffer;
    struct __WM    *next;
} WM;

int             next_window_id = 0;

WM             *window_tail = NULL;

PORT            wm_port;

#define FRAME_FOCUS_TOP_LEFT 0xC9
#define FRAME_FOCUS_TOP_RIGHT 0xBB
#define FRAME_FOCUS_BOTTOM_LEFT 0xC8
#define FRAME_FOCUS_BOTTOM_RIGHT 0xBC
#define FRAME_FOCUS_HORIZONTAL 0xCD
#define FRAME_FOCUS_VERTICAL 0xBA

#define FRAME_NO_FOCUS_TOP_LEFT 0xDA
#define FRAME_NO_FOCUS_TOP_RIGHT 0xBF
#define FRAME_NO_FOCUS_BOTTOM_LEFT 0xC0
#define FRAME_NO_FOCUS_BOTTOM_RIGHT 0xD9
#define FRAME_NO_FOCUS_HORIZONTAL 0xC4
#define FRAME_NO_FOCUS_VERTICAL 0xB3

#define DEFAULT_CURSOR_CHAR 0xDC

char            screen_buffer[80 * 25];

void clear_screen_buffer()
{
    for (int i = 0; i < sizeof(screen_buffer); i++) {
        screen_buffer[i] = 0;
    }
}

void copy_screen_buffer()
{
    char           *p = (char *) 0xb8000;
    for (int i = 0; i < sizeof(screen_buffer); i++) {
        *p++ = screen_buffer[i];
        *p++ = 0x0f;
    }
}

void poke_screen_buffer(int x, int y, char ch)
{
    if (x < 0 || y < 0)
        return;
    if (x >= 80 || y >= 25)
        return;
    screen_buffer[y * 80 + x] = ch;
}

void draw_window_frame(WM * window, BOOL is_top)
{
    int             frame_x = window->x - 1;
    int             frame_y = window->y - 1;
    int             frame_width = window->width + 1;
    int             frame_height = window->height + 1;
    poke_screen_buffer(frame_x, frame_y,
                       is_top ? FRAME_FOCUS_TOP_LEFT :
                       FRAME_NO_FOCUS_TOP_LEFT);
    poke_screen_buffer(frame_x + frame_width, frame_y,
                       is_top ? FRAME_FOCUS_TOP_RIGHT :
                       FRAME_NO_FOCUS_TOP_RIGHT);
    poke_screen_buffer(frame_x, frame_y + frame_height,
                       is_top ? FRAME_FOCUS_BOTTOM_LEFT :
                       FRAME_NO_FOCUS_BOTTOM_LEFT);
    poke_screen_buffer(frame_x + frame_width, frame_y + frame_height,
                       is_top ? FRAME_FOCUS_BOTTOM_RIGHT :
                       FRAME_NO_FOCUS_BOTTOM_RIGHT);
    char            ch =
        is_top ? FRAME_FOCUS_HORIZONTAL : FRAME_NO_FOCUS_HORIZONTAL;
    for (int x = 0; x < window->width; x++) {
        poke_screen_buffer(window->x + x, frame_y, ch);
        poke_screen_buffer(window->x + x, frame_y + frame_height, ch);
    }
    ch = is_top ? FRAME_FOCUS_VERTICAL : FRAME_NO_FOCUS_VERTICAL;
    for (int y = 0; y < window->height; y++) {
        poke_screen_buffer(frame_x, window->y + y, ch);
        poke_screen_buffer(frame_x + frame_width, window->y + y, ch);
    }
}

void draw_window_content(WM * window)
{
    int             i = 0;
    for (int y = 0; y < window->height; y++) {
        for (int x = 0; x < window->width; x++) {
            poke_screen_buffer(window->x + x, window->y + y,
                               window->buffer[i++]);
        }
    }
    if (window->cursor_char != 0) {
        int             pos =
            window->cursor_x + window->cursor_y * window->width;
        window->buffer[pos] = 0;
        poke_screen_buffer(window->x + window->cursor_x,
                           window->y + window->cursor_y,
                           window->cursor_char);
    }
}

void draw_window(WM * window)
{
    BOOL            is_top = window == window_tail;
    draw_window_frame(window, is_top);
    draw_window_content(window);
}

void redraw_screen()
{
    WM             *window = window_tail;
    clear_screen_buffer();
    if (window != NULL) {
        do {
            window = window->next;
            draw_window(window);
        } while (window != window_tail);
    }
    copy_screen_buffer();
}

void wm_create_impl(WM_MSG_CREATE * msg)
{
    WM             *window = (WM *) malloc(sizeof(WM));
    msg->window_id = next_window_id++;
    window->window_id = msg->window_id;
    window->x = msg->x;
    window->y = msg->y;
    window->width = msg->width;
    window->height = msg->height;
    window->cursor_x = 0;
    window->cursor_y = 0;
    window->cursor_char = DEFAULT_CURSOR_CHAR;
    int             size = msg->width * msg->height;
    window->buffer = (char *) malloc(size);
    k_memset(window->buffer, 0, size);
    if (window_tail == NULL) {
        window->next = window;
    } else {
        window->next = window_tail->next;
        window_tail->next = window;
    }
    window_tail = window;
    redraw_screen();
}

WM             *get_window_from_id(int id)
{
    WM             *window = window_tail;
    assert(window != NULL);
    do {
        if (window->window_id == id) {
            return window;
        }
        window = window->next;
    } while (window != window_tail);
    assert(0);
    return NULL;
}

void scroll_wm(WM * window)
{
    int             to = 0;
    int             from = window->width;
    int             size = window->width * (window->height - 1);
    for (int i = 0; i < size; i++) {
        window->buffer[to++] = window->buffer[from++];
    }
    for (int i = 0; i < window->width; i++) {
        window->buffer[to++] = 0;
    }
}

void wm_print_char(WM * window, char ch)
{
    if (ch == '\n') {
        window->cursor_x = 0;
        window->cursor_y++;
    } else if (ch == '\b') {
        int             pos =
            window->cursor_x + window->cursor_y * window->width;
        window->buffer[pos] = 0;
        window->cursor_x--;
        if (window->cursor_x == -1) {
            window->cursor_y--;
            if (window->cursor_y == -1) {
                window->cursor_x = 0;
                window->cursor_y = 0;
            } else {
                window->cursor_x = window->width - 1;
            }
        }
    } else {
        int             pos =
            window->cursor_x + window->cursor_y * window->width;
        window->buffer[pos] = ch;
        window->cursor_x++;
        if (window->cursor_x == window->width) {
            window->cursor_x = 0;
            window->cursor_y++;
        }
    }
    if (window->cursor_y == window->height) {
        window->cursor_y--;
        scroll_wm(window);
    }
}

void wm_print_impl(WM_MSG_PRINT * msg)
{
    WM             *window = get_window_from_id(msg->window_id);
    char           *str = msg->str;
    while (*str != '\0') {
        wm_print_char(window, *str);
        str++;
    }
    redraw_screen();
}

void wm_control_impl(WM_MSG_CONTROL * msg)
{
    if (msg->action == WM_ACTION_CURRENT_FOCUS) {
        msg->window_id = window_tail->window_id;
        return;
    }
    if (msg->action == WM_ACTION_CHANGE_FOCUS) {
        if (window_tail == NULL) {
            msg->window_id = -1;
            return;
        }
        window_tail = window_tail->next;
        msg->window_id = window_tail->window_id;
        redraw_screen();
        return;
    }
    int             id = msg->window_id;
    if (id == -1) {
        return;
    }
    WM             *window = get_window_from_id(id);
    BOOL            redraw = TRUE;
    switch (msg->action) {
    case WM_ACTION_GET_BUFFER:
        msg->buffer = window->buffer;
        redraw = FALSE;
        break;
    case WM_ACTION_REDRAW:
        // Do nothing here
        break;
    case WM_ACTION_CLEAR:
        window->cursor_x = 0;
        window->cursor_y = 0;
        int             size = window->width * window->height;
        k_memset(window->buffer, 0, size);
        break;
    case WM_ACTION_SET_CURSOR:
        window->cursor_x = msg->cursor_x;
        window->cursor_y = msg->cursor_y;
        window->cursor_char = msg->cursor_char;
        break;
    case WM_ACTION_MOVE_LEFT:
        window->x--;
        break;
    case WM_ACTION_MOVE_RIGHT:
        window->x++;
        break;
    case WM_ACTION_MOVE_UP:
        window->y--;
        break;
    case WM_ACTION_MOVE_DOWN:
        window->y++;
        break;
    default:
        assert(0);
    }
    if (redraw) {
        redraw_screen();
    }
}

void process_window_manager(PROCESS self, PARAM data)
{
    MSG_WM         *msg;
    PROCESS         sender;

    clear_screen_buffer();
    copy_screen_buffer();

    while (1) {
        msg = receive(&sender);
        switch (msg->type) {
        case WM_TYPE_CREATE:
            wm_create_impl((WM_MSG_CREATE *) & msg->u);
            break;
        case WM_TYPE_CONTROL:
            wm_control_impl((WM_MSG_CONTROL *) & msg->u);
            break;
        case WM_TYPE_PRINT:
            wm_print_impl((WM_MSG_PRINT *) & msg->u);
            break;
        default:
            assert(0);
        }
        reply(sender);
    }
    become_zombie();
}


int wm_create(int x, int y, int width, int height)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CREATE;
    msg.u.create.x = x;
    msg.u.create.y = y;
    msg.u.create.width = width;
    msg.u.create.height = height;
    send(wm_port, &msg);
    return msg.u.create.window_id;
}

char           *wm_get_buffer(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_GET_BUFFER;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
    return msg.u.control.buffer;
}

void wm_redraw_window(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_REDRAW;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void wm_clear(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_CLEAR;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void wm_set_cursor(int window_id, int x, int y, char ch)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_SET_CURSOR;
    msg.u.control.window_id = window_id;
    msg.u.control.cursor_x = x;
    msg.u.control.cursor_y = y;
    msg.u.control.cursor_char = ch;
    send(wm_port, &msg);
}

void wm_print(int window_id, const char *fmt, ...)
{
    MSG_WM          msg;
    va_list         argp;
    char            buf[160];

    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    msg.type = WM_TYPE_PRINT;
    msg.u.print.window_id = window_id;
    msg.u.print.str = buf;
    send(wm_port, &msg);
    va_end(argp);
}

int wm_change_focus()
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_CHANGE_FOCUS;
    send(wm_port, &msg);
    return msg.u.control.window_id;
}

int wm_current_focus()
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_CURRENT_FOCUS;
    send(wm_port, &msg);
    return msg.u.control.window_id;
}

void wm_move_left(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_MOVE_LEFT;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void wm_move_right(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_MOVE_RIGHT;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void wm_move_up(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_MOVE_UP;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void wm_move_down(int window_id)
{
    MSG_WM          msg;

    msg.type = WM_TYPE_CONTROL;
    msg.u.control.action = WM_ACTION_MOVE_DOWN;
    msg.u.control.window_id = window_id;
    send(wm_port, &msg);
}

void init_wm()
{
    wm_port =
        create_process(process_window_manager, 6, 0, "Window Manager");
}
