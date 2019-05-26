
#include <kernel.h>

#define PONG_WINDOW_WIDTH 30
#define PONG_WINDOW_HEIGHT 10

#define PONG_RACKET_HEIGHT 3
#define PONG_RACKET_CHAR 0xb0

#define PONG_BALL_CHAR 0x04

#define PONG_STATE_INIT 0
#define PONG_STATE_MOVE 1
#define PONG_STATE_DRAW 2
#define PONG_STATE_GAME_OVER 3


void clear_buffer(char *buffer)
{
    k_memset(buffer, 0, PONG_WINDOW_WIDTH * PONG_WINDOW_HEIGHT);
}

void fill_buffer(char *buffer)
{
    k_memset(buffer, 0xDB, PONG_WINDOW_WIDTH * PONG_WINDOW_HEIGHT);
}

void draw_racket(char *buffer, int racket_pos)
{
    for (int i = 0; i < PONG_RACKET_HEIGHT; i++) {
        buffer[(racket_pos + i) * PONG_WINDOW_WIDTH] = PONG_RACKET_CHAR;
    }
}

void show_game_over(int window_id, char *buffer)
{
    static const char *msg = "GAME OVER!";

    int             x = (PONG_WINDOW_WIDTH - k_strlen(msg)) / 2;
    int             y = PONG_WINDOW_HEIGHT / 2;
    for (int i = 0; i < 10; i++) {
        sleep(10);
        clear_buffer(buffer);
        wm_set_cursor(window_id, x, y, 0);
        wm_print(window_id, msg);
        wm_redraw_window(window_id);
        sleep(10);
        fill_buffer(buffer);
        wm_set_cursor(window_id, x, y, 0);
        wm_print(window_id, msg);
        wm_redraw_window(window_id);
    }
}

void pong_process(PROCESS self, PARAM param)
{
    int             window_id =
        wm_create(15, 10, PONG_WINDOW_WIDTH, PONG_WINDOW_HEIGHT);
    char           *buffer = wm_get_buffer(window_id);
    int             state = PONG_STATE_INIT;
    int             x,
                    y,
                    dx,
                    dy,
                    racket = 0;

    wm_print(window_id, "\n  PONG\n\n");
    wm_print(window_id, "  Keys:\n    'q' for up\n    'a' for down\n\n");
    wm_print(window_id, "  Hit any key to start... ");
    keyb_get_keystroke(window_id, TRUE);
    wm_set_cursor(window_id, 0, 0, 0);

    while (1) {
        switch (state) {
        case PONG_STATE_INIT:
            x = PONG_WINDOW_WIDTH / 2;
            y = PONG_WINDOW_HEIGHT / 2;
            dx = 1;
            dy = -1;
            state = PONG_STATE_MOVE;
            break;
        case PONG_STATE_MOVE:
            // Move ball
            if (y == 0)
                dy = 1;
            if (y == PONG_WINDOW_HEIGHT - 1)
                dy = -1;
            if (x == PONG_WINDOW_WIDTH - 1)
                dx = -1;
            if (x == 0) {
                // Check if we hit the racket
                if (y < racket || y >= racket + PONG_RACKET_HEIGHT) {
                    // Game over
                    state = PONG_STATE_GAME_OVER;
                    break;
                }
                dx = 1;
            }
            x += dx;
            y += dy;
            // Move racket
            char            key = keyb_get_keystroke(window_id, FALSE);
            switch (key) {
            case 'q':
                if (racket != 0)
                    racket--;
                break;
            case 'a':
                if (racket + PONG_RACKET_HEIGHT != PONG_WINDOW_HEIGHT)
                    racket++;
                break;
            }
            state = PONG_STATE_DRAW;
            break;
        case PONG_STATE_DRAW:
            clear_buffer(buffer);
            draw_racket(buffer, racket);
            buffer[y * PONG_WINDOW_WIDTH + x] = PONG_BALL_CHAR;
            wm_redraw_window(window_id);
            sleep(5);
            state = PONG_STATE_MOVE;
            break;
        case PONG_STATE_GAME_OVER:
            show_game_over(window_id, buffer);
            state = PONG_STATE_INIT;
            break;
        }
    }
    become_zombie();
}

void start_pong()
{
    create_process(pong_process, 5, 0, "Pong Process");
    resign();
}
