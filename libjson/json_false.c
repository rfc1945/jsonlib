#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>

#define T json_T
static const char *keyword="false";
struct json{
    int type;
    json_C class;
};

/* Class Methods */
static T new_(void);
static T fscan_(FILE *from);
static int fprint_(FILE *to,T this);
/*---------------*/

struct json_C
JSON_FALSE_T=
{
    new_,fscan_,fprint_,NULL,NULL,NULL
};

static struct json
JSON_FALSE_PROTO={JSON_FALSE,&JSON_FALSE_T};

static T
new_(void){return &JSON_FALSE_PROTO;}

static T
fscan_(FILE *from)
{
    char tmp[6];
    if(fscanf(from,"%5s",tmp)!=1 ||
       strcmp(keyword,tmp)
    ){
        EPRINTF("expected '%s'",keyword);
        return NULL;
    }
    return &JSON_FALSE_PROTO;
}

static int
fprint_(FILE *to,T this){assert(to && this);return fputs(keyword,to);}
