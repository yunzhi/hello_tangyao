#ifndef __HTTP_ONENET_METHOD_H__
#define __HTTP_ONENET_METHOD_H__

#define BUF_SIZE 4096
#define RETRY_TIME 3

typedef struct
{ 
    int tid; 
    int file_size;
    char *md5;
} UPGRADE_RES_T;

typedef struct
{ 
    int get_result;
    char *get_version_res;
    UPGRADE_RES_T upgrade_task_res;
    int check_task_status_res;
} get_respone_data;


char *str_dup(char *s);
int http_post_url(char *ota_posturl , char *auth_token);
get_respone_data http_get_url(char *ota_geturl, char *auth_token);
int http_get_download(char *ota_geturl, char *auth_token, int file_size);
int http_post_download_firmware_status(char *ota_posturl,
    char *auth_token, int state_value);

#endif //__HTTP_ONENET_METHOD_H__