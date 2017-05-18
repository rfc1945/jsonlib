#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{int type};
static const char *keyword="null";
static const struct json JSON_NULL_P={JSON_NULL};

extern T
new_null(void){return &JSON_NULL_P;}

extern T
fscan_null(FILE *from)
{
    char tmp[5];
    if(fscanf(from,"%4s",tmp)!=1 ||
       strcmp("null",tmp)
    ){
        EPRINTF("expected 'null'");
        return NULL;
    }
    return &JSON_NULL_P;
}

extern T
sscan_null(const char **str)
{
    char tmp[5];
    int read=0;
    if(sscanf(*str,"%4s%n",tmp,&read)!=1 ||
       strcmp(keyword,tmp))
    {
        EPRINTF("expected 'null'");
        return NULL;
    }
    (*str)+=read;
    return &JSON_NULL_P;
}

extern int
fprint_null(FILE *to,T this)
{
    assert(to);
    return fputs("null",to);
}
