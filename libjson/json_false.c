#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{int type;};
static const char *keyword="false";
static struct json JSON_FALSE_P={JSON_FALSE};

extern T
new_false(void){return &JSON_FALSE_P;}

extern T
fscan_false(FILE *from)
{
    char tmp[6];
    if(fscanf(from,"%5s",tmp)!=1 ||
       strcmp(keyword,tmp)
    ){
        EPRINTF("expected '%s'",keyword);
        return NULL;
    }
    return &JSON_FALSE_P;
}

extern int
fprint_false(FILE *to,T this){assert(to && this);return fputs(keyword,to);}
