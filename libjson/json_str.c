#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>

#define T json_T
#define INITIAL 64
#define INCR 2

struct json{
    int type;
    json_C class;
    size_t len;
    char  buf[];
};

/* Class Methods */
static T new_str(void);
static T fscan_str(FILE *from);
static int fprint_str(FILE *to,T this);
static int get_str(T this,va_list *args);
/*---------------*/

struct json_C
JSON_STR_T=
{
    new_str,
    fscan_str,
    fprint_str,
    NULL,
    get_str
};

static T new_str(void)
{
    T res=EMALLOC(sizeof(*res)+INITIAL);
    if(!res) return NULL;
    *res=(struct json){JSON_STR,&JSON_STR_T,0};
    res->buf[0]='\0';
    return res;
}

static T fscan_str(FILE *from)
{
    json_T res=new_str();
    if(!res) return NULL;
    int c=fgetc(from);
    assert(c=='\"');
    size_t len=0,sz=INITIAL;
    for(;(c=fgetc(from))!='\"';){
        if(c==EOF){
            EPRINTF("no closing '\"' found...\n");
            break;
        }
        if(len+1 >= sz){
            sz*=INCR;
            if(EREALLOC(&res,sizeof(*res)+sz))
                break;
        }
        if(c=='\\'){
            if((c=fgetc(from))=='"') c='"';
            else if(c!='\\') ungetc(c,from), c='\\';
        }
        res->buf[len++]=c;
    }
    res->buf[len]='\0';
    return realloc(res,sizeof(*res)+(res->len=len)+1);
}

static int fprint_str(FILE *to,T this){
    return fprintf(to,"\"%s\"",this->buf);
}

static int get_str(T this,va_list *args)
{
    assert(this);
    const char **str=va_arg(*args,const char **);
    *str=this->buf;
    return 0;
}
