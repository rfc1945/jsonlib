#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <err/err.h>
#include <libjson/json.h>

#define T json_T

struct json{
    int type;
    json_C class;
};

/*-------------Classes--------------*/
extern struct json_C JSON_NUM_T;
extern struct json_C JSON_STR_T;
extern struct json_C JSON_ARR_T;
extern struct json_C JSON_OBJ_T;
extern struct json_C JSON_NULL_T;
extern struct json_C JSON_TRUE_T;
extern struct json_C JSON_FALSE_T;
/*----------------------------------*/

static const json_C
CLASS_TAB[JSON_NTYPES]=
/* Used to get class structure by id */
{
    [JSON_NUM]=&JSON_NUM_T,
    [JSON_STR]=&JSON_STR_T,
    [JSON_ARR]=&JSON_ARR_T,
    [JSON_OBJ]=&JSON_OBJ_T,
    [JSON_NULL]=&JSON_NULL_T,
    [JSON_TRUE]=&JSON_TRUE_T,
    [JSON_FALSE]=&JSON_FALSE_T
};

static const int
GUESS_TAB[256]=
/* Used to infer the next JSON token */
{
    ['[']=JSON_ARR,['{']=JSON_OBJ,['0']=JSON_NUM ,['1']=JSON_NUM,
    ['2']=JSON_NUM,['3']=JSON_NUM,['4']=JSON_NUM ,['5']=JSON_NUM,
    ['6']=JSON_NUM,['7']=JSON_NUM,['8']=JSON_NUM ,['9']=JSON_NUM,
    ['-']=JSON_NUM,['"']=JSON_STR,['n']=JSON_NULL,['t']=JSON_TRUE,
    ['f']=JSON_FALSE
};

extern T
json_fscan(FILE *from)
{
    assert(from);
    skip_space(from);
    int c=fpeek(from);
    if(c==EOF) return NULL;
    int type=GUESS_TAB[c];
    if(type==JSON_NO_TYPE){
        EPRINTF("No JSON type found that matches char '%c'\n",c);
        return NULL;
    }
    T json=(CLASS_TAB[type]->fscan)(from);
    if(!json)
        EPRINTF("Error parsing JSON file...\n");
    return json;
}

extern T
json_new(int type)
{
    assert("Unknown JSON type" && type>0 && type<JSON_NTYPES);
    assert(CLASS_TAB[type]);
    return CLASS_TAB[type]->new();
}

extern int json_fprint(FILE *to,T json){
    assert(json && to);
    return ((json->class)->fprint)(to,json);
}

extern int json_get(T json,...)
{
    assert(json);
    if(!json->class->get)
        return 0;
    va_list args;
    int got=0;
    va_start(args,json);
	got=(json->class->get)(json,&args);
	va_end(args);
    return got;
}

/* Not implemented */
//extern T json_set(T json,...);

extern void json_free(T json){
    assert(json);
    if(json->class->free)
        (json->class->free)(json);
    free(json);
}

#undef T
