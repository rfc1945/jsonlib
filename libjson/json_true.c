#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>

#define T json_T
static const char *keyword="true";
struct json{
    int type;
    json_C class;
};

/* Class Methods */
static T new_true(void);
static T fscan_true(FILE *from);
static int fprint_true(FILE *to,T this);
/*---------------*/

struct json_C
JSON_TRUE_T=
{
    new_true,
    fscan_true,
    fprint_true,
    NULL,
    NULL,
    NULL
};

static struct json
JSON_TRUE_PROTO={JSON_TRUE,&JSON_TRUE_T};

static T
new_true(void){return &JSON_TRUE_PROTO;}

static T
fscan_true(FILE *from)
{
    char tmp[5];
    if(fscanf(from,"%4s",tmp)!=1 ||
       strcmp(keyword,tmp)
    ){
        EPRINTF("expected '%s'",keyword);
        return NULL;
    }
    return &JSON_TRUE_PROTO;
}

static int
fprint_true(FILE *to,T this)
{
    assert(to && this);
    return fputs(keyword,to);
}
