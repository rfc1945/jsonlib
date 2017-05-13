#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>

#define T json_T

struct json{
    int type;
    json_C class;
};

/* Class Methods */
static T new_null(void);
static T fscan_null(FILE *from);
static int fprint_null(FILE *to,T this);
/*---------------*/

struct json_C
JSON_NULL_T=
{
    new_null,
    fscan_null,
    fprint_null,
    NULL,
    NULL,
    NULL
};

static struct json
JSON_NULL_PROTO={JSON_NULL,&JSON_NULL_T};

static T
new_null(void)
{
    return &JSON_NULL_PROTO;
}

static T
fscan_null(FILE *from)
{
    char tmp[5];
    if(fscanf(from,"%4s",tmp)!=1 ||
       strcmp("null",tmp)
    ){
        EPRINTF("expected 'null'");
        return NULL;
    }
    return &JSON_NULL_PROTO;
}

static int
fprint_null(FILE *to,T this)
{
    assert(to && this);
    return fputs("null",to);
}
