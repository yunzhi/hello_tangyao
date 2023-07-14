/*
 * Copyright (c) 2020, CMIOT
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include <os_task.h>
#include <os_errno.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "http_oneos_wrapper.h"
#include <http_application_api.h>

#ifdef OS_USING_VFS
#include <vfs_posix.h>

/**
 * send GET request and store response data into the file.
 *
 * @param URI input server address
 * @param filename store response data to filename
 *
 * @return <0: GET request failed
 *         =0: success
 */
 
/* @brief http request buffer */
#define BUF_SIZE (HTTP_REQUEST_BLOCK_SIZE + 64)
/* @save the last download file name */
char * last_file_name = NULL;
/* @used to judge if the file is downloaded */
bool download_finish_flag = 0;
/* @block transmission */
int block_number = 0;
int left_length = 0;
int count_len = 0;

/* @the last data smaller than block size*/
int last_data = 0;

static int http_get_file(const char* URI, const char* filename)
{
    int fd = -1;
    int ret = OS_EOK;
    char *buf = NULL;
    size_t total_length = 0;
    //size_t left_length = 0;
    size_t response_length = 0;
    //size_t count_len = 0;
    last_data = 0;
    http_client_data_t client_data = {0};
    http_client_t client = {0};    
    
    fd = open(filename, O_RDWR | O_CREAT | O_APPEND);
    if (fd < 0)
    {
        printf("Error %d: Failed to open file(%s) error.\r\n", errno, filename);
        
        ret = OS_ERROR;
        goto __exit;
    }
    //the first time download new file
    if(NULL == last_file_name || (0 != strcmp(last_file_name, filename)))
    {
        last_file_name =  (char*)filename;
        //printf("prepare to download new file:%s\n", filename);
        printf("prepare to download new file:%s\r\n", filename);
    }
    else if(0 == strcmp(last_file_name, filename) && !download_finish_flag)
    {
        //printf("resume from break point!\n");
        printf("resume from break point!\r\n");
    }
    else
    {
        block_number = 0;
        download_finish_flag = 0;
        count_len = 0;
        //printf("prepare to download the same file:%s again!\n", filename);
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC);
        if (fd < 0)
        {
            printf("Error %d: Failed to open file(%s) error.\r\n", errno, filename);
            
            ret = OS_ERROR;
            goto __exit;
        }
        printf("prepare to download the same file:%s again!\r\n", filename);
    }
    buf = malloc(BUF_SIZE);
    if (buf == NULL) 
    {
        printf("Malloc failed.\r\n");
        ret = OS_ERROR;
        goto __exit;
    }
    memset(buf, 0, BUF_SIZE);

    /* Sets a buffer to store the result */
    client_data.response_buf = buf;
    /* Sets the buffer size */
    client_data.response_buf_len = BUF_SIZE;

    
    ret = http_client_conn(&client, URI);

    if(!ret)
    {
        do
        {
            ret = http_client_send(&client, URI, HTTP_GET, &client_data);
            if(!ret)
            {
                ret = (HTTP_RESULT_CODE)http_client_recv(&client, &client_data);
            }
            
            response_length = client_data.response_content_len;
            total_length = client_data.content_range_len;
            if(!left_length)
            {
                left_length = total_length;
            }
            printf("the file total size is:%d", total_length);
            //printf("the file total size is:%d\r\n", total_length);
            if(ret == 0 && (response_length > 0) &&(count_len < total_length))
            {  
                write(fd, buf, response_length);
                block_number++;
                count_len += response_length;
                left_length -= response_length;
                printf("the file remaining size is:%d\r\n", left_length);
            }
            
            if(((total_length - count_len) < HTTP_REQUEST_BLOCK_SIZE) 
                && ((total_length - count_len) != 0))
            {
                last_data = total_length - count_len;
                printf("the last data size is:%d", last_data);                
                //printf("the last data size is:%d \r\n", last_data);
            }
                
            //nginx server Load Balance limited the request in 100 times;
            if(block_number%100 == 0 && ret == 0)
            {
                ret = http_client_conn(&client, URI);
            }
        }while(ret == 0 && (total_length > count_len));
    }
    
    if(total_length == count_len && 0 == ret)
    {
        download_finish_flag = 1;
        left_length = 0;
        block_number = 0;
        count_len = 0;
        //printf("the file:%s is downloaded!\n", filename);
        printf("the file:%s is downloaded!\r\n", filename);
    }
    else
    {
        
        //printf("the file:%s is not downloaded!\n", filename);
        download_finish_flag = 0;
        printf("the file:%s is not downloaded!\n", filename);
    }
    
__exit:
    if (fd >= 0)
    {
       close(fd);
    }
    
    if(buf != NULL)
    {
        free(buf);
        client_data.response_buf = NULL;
    }
    
    ret = http_client_close(&client);

    return ret;
}

int httpclient_get_file(int argc, char** argv)
{
    if (argc != 3)
    {
       printf("Please using: http_client_get_file <URI> <filename>");
       return -1;
    }

    http_get_file(argv[1], argv[2]);
    return 0;
}

#ifdef OS_USING_SHELL
#include <shell.h>
SH_CMD_EXPORT(httpclient_get_file, httpclient_get_file, "Get file by URI: httpget <URI> <filename>.");
#endif /* OS_USING_SHELL */

#endif /* OS_USING_VFS */





