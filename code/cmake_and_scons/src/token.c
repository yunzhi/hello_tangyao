#include "token.h"
#include "mbedtls/base64.h"
#include "mbedtls/hmac.h"
#include "mbedtls/md5.h"
#include "mbedtls/sha.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"


typedef struct {
    char  et[32];
    char* version;
    char* method;
    char res[128];
    char sign[128];
} sign_msg;


typedef  struct {
    char* old_str;
    char* str;
}URL_PARAMETES;


static int url_encoding_for_token(sign_msg* msg,char *token,int maxlen)
{
    int i,j,k,slen;
    sign_msg* temp_msg = msg;
    URL_PARAMETES url_patametes[] = {
       {"+","%2B"},
       {" ","%20"},
       {"/","%2F"},
       {"?","%3F"},
       {"%","%25"},
       {"#","%23"},
       {"&","%26"},
       {"=","%3D"},
    };

    char temp[128]     = {0};
    /**
     * @brief res ����url����
     *
     */
    slen = strlen(temp_msg->res);
    for (i = 0,j = 0; i < slen; i++) {
        for(k = 0; k < 8; k++){
            if(temp_msg->res[i] == url_patametes[k].old_str[0]) {
                memcpy(&temp[j],url_patametes[k].str,strlen(url_patametes[k].str));
                j+=3;
                break;
            }
        }
        if (k == 8) {
            temp[j++] = temp_msg->res[i];
        }

	}
    memcpy(temp_msg->res,temp,strlen(temp));
    temp_msg->res[strlen(temp)] = 0;

    /**
     * @brief sign ����url ����
     *
     */
    memset(temp,0x00,sizeof(temp));
    slen = strlen(temp_msg->sign);
    for (i = 0,j = 0; i < slen; i++) {
        for(k = 0; k < 8; k++){
            if(temp_msg->sign[i] == url_patametes[k].old_str[0]) {
                memcpy(&temp[j],url_patametes[k].str,strlen(url_patametes[k].str));
                j+=3;
                break;
            }
        }
        if(k == 8)
        {
            temp[j++] = temp_msg->sign[i];
        }
	}
    memcpy(temp_msg->sign,temp,strlen(temp));
    temp_msg->sign[strlen(temp)] = 0;

    sprintf(token, "version=%s&res=%s&et=%s&method=%s&sign=%s",
        temp_msg->version, temp_msg->res, temp_msg->et, temp_msg->method, temp_msg->sign);
    return strlen(token);
}


int onenet_creat_token_init(onenet_msg_t* msg, long long time,method_t token_method,char *token,int maxlen)
{
    int declen = 0, enclen =  0;
    char plaintext[64]     = { 0 };
    char hmac[64]          = { 0 };
    sign_msg sign ;
    memset(&sign,0x00,sizeof(sign));
    sign.version = "2018-10-31";
    sprintf(sign.et,"%lld", time);
    sprintf(sign.res,"products/%s/devices/%s",msg->produt_id,msg->device_name);
    mbedtls_base64_decode((unsigned char*)plaintext, sizeof(plaintext), &declen, (unsigned char*)msg->key, strlen((char*)msg->key));

    char StringForSignature[256] = { 0 };

    switch (token_method)
    {
        case ONENET_METHOD_MD5:
            sign.method = "md5";
            sprintf(StringForSignature, "%s\n%s\n%s\n%s", sign.et, sign.method, sign.res, sign.version);
            esp_hmac_md5((unsigned char*)plaintext, declen, (unsigned char*)StringForSignature, strlen(StringForSignature), (unsigned char*)hmac);
            break;
        case ONENET_METHOD_SHA1:
            sign.method = "sha1";
            sprintf(StringForSignature, "%s\n%s\n%s\n%s", sign.et, sign.method, sign.res, sign.version);
            esp_hmac_sha1((unsigned char*)plaintext, declen, (unsigned char*)StringForSignature, strlen(StringForSignature), (unsigned char*)hmac);
            break;
        case ONENET_METHOD_SHA256:
            sign.method = "sha256";
            sprintf(StringForSignature, "%s\n%s\n%s\n%s", sign.et, sign.method, sign.res, sign.version);
            esp_hmac_sha256((unsigned char*)plaintext, declen, (unsigned char*)StringForSignature, strlen(StringForSignature), (unsigned char*)hmac);
            break;
    }
    mbedtls_base64_encode((unsigned char*)sign.sign, sizeof(sign.sign), &enclen, (unsigned char*)hmac, strlen(hmac));
    sprintf(sign.et,"%lld", time);
    return url_encoding_for_token(&sign,token,maxlen);
}
