#include "buf.h"
#include <string.h>
#include <stdio.h>

char g_buf[BUF_MAX_LEN] = {0};

G_BUF_INDEX g_buf_index = {0, 0};

void buf_init()
{
    g_buf_index.front = 0;
    g_buf_index.rear = 0;
}

int buf_used_len()
{
    return (g_buf_index.rear - g_buf_index.front + BUF_MAX_LEN) % BUF_MAX_LEN;
}

int buf_unused_len()
{
    return BUF_MAX_LEN - 1 - buf_used_len();
}

int buf_is_empty()
{
    return g_buf_index.front == g_buf_index.rear ? 1 : 0;
}

int buf_is_full(int frame_len)
{
    return frame_len + 4 > buf_unused_len() ? 1 : 0;
}

void buf_push_back(FILE *fp, int frame_len)
{
    if (buf_is_full(frame_len))
    {
        return;
    }

    char frame_len_buf[4] = {0};
    int i = 0;
    int fir_len = 0;
    int sec_len = 0;

    /* 放入frame_len 以及 fp读出的frame_len长度的数据 */
    for (i = 0; i < 4; i++)
    {
        frame_len_buf[i] = (frame_len >> (i * 8)) & 0xff;
    }
    if (g_buf_index.rear + 4 > BUF_MAX_LEN)
    {
        fir_len = BUF_MAX_LEN - g_buf_index.rear;
        sec_len = 4 - fir_len;
        memcpy(g_buf + g_buf_index.rear, frame_len_buf, fir_len);
        memcpy(g_buf, frame_len_buf+fir_len, sec_len);
    }
    else
    {
        memcpy(g_buf+g_buf_index.rear, frame_len_buf, 4);
    }
    g_buf_index.rear = (g_buf_index.rear + 4) % BUF_MAX_LEN;
   
    /* 判断 */
    if (g_buf_index.rear + frame_len > BUF_MAX_LEN)
    {
        fir_len = BUF_MAX_LEN - g_buf_index.rear;
        sec_len = frame_len - fir_len;

        fread(g_buf + g_buf_index.rear, 1, fir_len, fp);
        fread(g_buf, 1, sec_len, fp);
    }
    else
    {
        fread(g_buf + g_buf_index.rear, 1, frame_len, fp);
    }
    g_buf_index.rear = (g_buf_index.rear + frame_len) % BUF_MAX_LEN;
}

int buf_pop_front_frame_len()
{
    if (buf_is_empty())
    {
        return 0;
    }

    int frame_len = 0;
    char frame_len_buf[4] = {0};
    int i = 0; 
    int fir_len = 0;
    int sec_len = 0;
    
    /* 解析长度*/
    /* 判断 */
    if (g_buf_index.front + 4 > BUF_MAX_LEN)
    {
        fir_len = BUF_MAX_LEN - g_buf_index.front;
        sec_len = 4 - fir_len;

        memcpy(frame_len_buf, g_buf + g_buf_index.front, fir_len);
        memcpy(frame_len_buf+fir_len, g_buf, sec_len);
    }
    else
    {

        memcpy(frame_len_buf, g_buf + g_buf_index.front, 4);
    }
    g_buf_index.front = (g_buf_index.front + 4) % BUF_MAX_LEN;
    
    for (i = 0; i < 4; i++)
    {
       frame_len |=  ((frame_len_buf[i] << (i * 8)) & (0xff << (i * 8)));
    }

    return frame_len;
}

void buf_pop_front_frame_value(int frame_len, char *tmp_frame_value_buf)
{
    int fir_len = 0;
    int sec_len = 0;

    /* 判断 */
    /* 将帧数据读入tmp_frame_value_buf */
    if (g_buf_index.front + frame_len > BUF_MAX_LEN)
    {
        fir_len = BUF_MAX_LEN - g_buf_index.front;
        sec_len = frame_len - fir_len;

        memcpy(tmp_frame_value_buf, g_buf + g_buf_index.front, fir_len);
        memcpy(tmp_frame_value_buf + fir_len, g_buf, sec_len);
    }
    else
    {
        memcpy(tmp_frame_value_buf, g_buf + g_buf_index.front, frame_len);
    }

    g_buf_index.front = (g_buf_index.front + frame_len) % BUF_MAX_LEN;
}
