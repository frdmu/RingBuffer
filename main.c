#include "buf.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h> 

int g_file_size = 0;
char test_filename[30] = {0};
int test_filename_index = 0;
char test_filename_suffix[10] = {'.', 'j', 'p', 'g'};

void *writer(void *args)
{

    FILE *fp = fopen("test.jpg", "r");
    struct stat statbuf;
    stat("test.jpg", &statbuf);
    int file_size = statbuf.st_size;
    int read_cnt = 0;
    g_file_size = file_size; 
    
    while(1)
    {
        while (read_cnt != file_size)
        {
            int rand_num = rand() % 6000;
            int frame_len = rand_num == 0 ? 3345: rand_num;
            if (frame_len + read_cnt > file_size)
            {
                frame_len = file_size - read_cnt;
            }

            while (buf_is_full(frame_len))
            {
                //printf("[%s] wait  >>>>>>>>>>>>>>buf\n", __func__);
            }
            
            /* 写长度和数据 */
            buf_push_back(fp, frame_len);
            printf("[%s] frame_len: %d\n", __func__, frame_len);
            read_cnt += frame_len;
        }
        
        //printf("[%s] read file once over>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", __func__);
        read_cnt = 0;
        fseek(fp, 0, SEEK_SET);
    }

    fclose(fp);
    return NULL;
}


void int_to_string(int num, char* str)
{
 
     int i = 0,j = 0;
     char temp[10];
 
     while(num)
     {
         temp[i++] = num % 10 + '0';  //将数字加字符0就变成相应字符
         num /= 10;                   //此时的字符串为逆序
     }
     temp[i] = '\0';
     i = i - 1;
     while ( i >= 0)
        str[j++]=temp[i--];          //将逆序的字符串转为正序
     str[j]='\0';                    //字符串结束标志
}

void *reader(void *args)
{
    test_filename_index = rand();
    int_to_string(test_filename_index, test_filename);
    strcat(test_filename, test_filename_suffix);
    FILE *fp = fopen(test_filename, "wb");
    int write_cnt = 0;
    
    while (1)
    {
        if (g_file_size == 0)
        {
            continue;
        }
        while (write_cnt != g_file_size)
        {
            while (buf_is_empty())
            {
                //printf("[%s] wait buf++++++++++++++++\n", __func__);
            }
            
            /* 读数据 */
            int frame_len = buf_pop_front_frame_len();
            printf("[%s] frame_len: %d\n", __func__, frame_len);
            char *tmp_frame_value_buf = (char *)malloc(frame_len);
            buf_pop_front_frame_value(frame_len, tmp_frame_value_buf);

            /* 将tmp_frame_value_buf 写入文件*/
            fwrite(tmp_frame_value_buf, 1, frame_len, fp);
            
            write_cnt += frame_len;
            free(tmp_frame_value_buf);
            usleep(60 * 1000);
            printf("[%s] write_cnt: %d frame_len %d\n", __func__, write_cnt, frame_len);
        }
            
        fclose(fp);
        printf("[%s] write to network over+++++++++++++++++++++++++++++++++++++++++++++++\n", __func__);
        write_cnt = 0;
        test_filename_index = rand();
        int_to_string(test_filename_index, test_filename);
        strcat(test_filename, test_filename_suffix);
        fp = fopen(test_filename, "wb");
    }
    return NULL;
}

int main()
{
    buf_init();
    
    pthread_t th_writer;
    pthread_t th_reader;
    
    pthread_create(&th_writer, NULL, writer, NULL);
    pthread_create(&th_reader, NULL, reader, NULL);
   
    pthread_join(th_writer, NULL);
    pthread_join(th_reader, NULL);
    
    return 0;
}
