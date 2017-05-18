#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{int type;};
static const char *keyword="true";
static const struct json
JSON_TRUE_P={JSON_TRUE};

extern T
new_true(void){return &JSON_TRUE_P;}

extern T
fscan_true(FILE *from)
{
    char tmp[5];
    if(fscanf(from,"%4s",tmp)!=1 ||
       strcmp(keyword,tmp)
    ){
        EPRINTF("expected '%s'",keyword);
        return NULL;
    }
    return &JSON_TRUE_P;
}

extern int
fprint_true(FILE *to,T this)
{
    assert(to && this);
    return fputs(keyword,to);
}
