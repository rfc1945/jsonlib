#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{
    int type;
    size_t len;
    char  buf[];
};

#define INITIAL 64
#define INCR 2

extern T new_str(void)
{
    T res=EMALLOC(sizeof(*res)+INITIAL);
    if(!res) return NULL;
    *res=(struct json){JSON_STR,0};
    res->buf[0]='\0';
    return res;
}

extern T fscan_str(FILE *from)
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

extern T
sscan_str(const char **str)
{
    json_T res=new_str();
    if(!res) return NULL;
    assert(*(*str)++=='\"');
    size_t len=0,sz=INITIAL;
    for(int c; **str!='\"'; ++*str){
        if(!(c=**str)){
            EPRINTF("no closing '\"' found...\n");
            break;
        }
        if(len+1 >= sz){
            sz*=INCR;
            if(EREALLOC(&res,sizeof(*res)+sz))
                break;
        }
        if(c=='\\'){
            if((*str)[1]=='"') c='"', ++*str;
            else if((*str)[1]!='\\') c='\\';
        }
        res->buf[len++]=c;
    }
    ++*str;
    res->buf[len]='\0';
    return realloc(res,sizeof(*res)+(res->len=len)+1);
}

extern int
fprint_str(FILE *to,T this)
{
    return fprintf(to,"\"%s\"",this->buf);
}

extern int
get_str(T this,va_list *args)
{
    assert(this);
    const char **str=va_arg(*args,const char **);
    *str=this->buf;
    return 0;
}
