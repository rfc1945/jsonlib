#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <err/err.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{int type;};

static const new_ NEW_TAB[]=
{
      [JSON_NUM]= new_num, [JSON_STR]= new_str, [JSON_ARR]= new_arr,
      [JSON_OBJ]= new_obj,[JSON_NULL]=new_null,[JSON_TRUE]=new_true,
    [JSON_FALSE]=new_false
};

static const sscan_ SSCAN_TAB[]=
{
      [JSON_NUM]= sscan_num, [JSON_STR]= sscan_str, [JSON_ARR]= sscan_arr,
      [JSON_OBJ]= sscan_obj,[JSON_NULL]=sscan_null,[JSON_TRUE]=NULL,
    [JSON_FALSE]=NULL
};

static const fscan_ FSCAN_TAB[]=
{
      [JSON_NUM]= fscan_num, [JSON_STR]= fscan_str, [JSON_ARR]= fscan_arr,
      [JSON_OBJ]= fscan_obj,[JSON_NULL]=fscan_null,[JSON_TRUE]=fscan_true,
    [JSON_FALSE]=fscan_false
};

static const fprint_ FPRINT_TAB[]=
{
      [JSON_NUM]= fprint_num, [JSON_STR]= fprint_str, [JSON_ARR]= fprint_arr,
      [JSON_OBJ]= fprint_obj,[JSON_NULL]=fprint_null,[JSON_TRUE]=fprint_true,
    [JSON_FALSE]=fprint_false
};

static const get_ GET_TAB[]=
{
      [JSON_NUM]= get_num, [JSON_STR]= get_str, [JSON_ARR]= get_arr,
      [JSON_OBJ]= get_obj,[JSON_NULL]=    NULL,[JSON_TRUE]=    NULL,
    [JSON_FALSE]= NULL
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
    T json=FSCAN_TAB[type](from);
    if(!json)
        EPRINTF("Error parsing JSON file...\n");
    return json;
}

extern T
json_sscan(const char **str)
{
    assert(str && *str);
    while(isspace(**str))(*str)++;
    if(!(*str)[1]) return NULL;
    int type=GUESS_TAB[(unsigned char)(**str)];
    if(type==JSON_NO_TYPE){
        EPRINTF("No JSON type found that matches char '%c'",
                (*str)[1]);
        return NULL;
    }
    T json=SSCAN_TAB[type](str);
    if(!json)
        EPRINTF("Error parsing JSON string");
    return json;
}

extern T
json_new(int type)
{
    assert("Unknown JSON type" && type>0 && type<JSON_NTYPES);
    return NEW_TAB[type]();
}

extern int json_fprint(FILE *to,T json){
    assert(json && to);
    return FPRINT_TAB[json->type](to,json);
}


extern int json_get(T json,...)
{
    assert(json);
    va_list args;
    int got=0;
    va_start(args,json);
        got=GET_TAB[json->type](json,&args);
	va_end(args);
    return got;
}

/* Not implemented */
//extern T json_set(T json,...);

extern void json_free(T json){
//    assert(json);
//    if(json->class->free)
//        (json->class->free)(json);
//    free(json);
}


#undef T
