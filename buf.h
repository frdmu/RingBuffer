#ifndef __BUF_H__
#define __BUF_H__
#include <stdio.h>

#define BUF_MAX_LEN (1024 * 1024)

extern char g_buf[BUF_MAX_LEN];

typedef struct
{
    int front;
    int rear;
} G_BUF_INDEX;
extern G_BUF_INDEX g_buf_index;

void buf_init();

int buf_used_len();

int buf_unused_len();

int buf_is_empty();

int buf_is_full(int frame_len);

void buf_push_back(FILE *fp, int frame_len);

int buf_pop_front_frame_len();
void buf_pop_front_frame_value(int frame_len, char *tmp_frame_value_buf);

#endif /* __BUF_H__ */
