#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{
    int type;
    size_t len;
    json_T buf[];
};

#define INITIAL 256
#define INCR 1.5

extern T
new_arr(void){
    json_T res=EMALLOC(sizeof(*res)+sizeof(T)*INITIAL);
    if(!res) return NULL;
    res->type=JSON_ARR;
    res->len=0;
    res->buf[0]=NULL;
    return res;
}

extern T
fscan_arr(FILE *from)
{
    size_t len=0,sz=INITIAL;
    T res=new_arr();
    if(!res) return NULL;
    int c=fgetc(from);
    assert(c=='[');
    skip_space(from);
    if((c=fgetc(from))==']') goto skip_loop;
    else ungetc(c,from);
    for(;;){
        if(c==EOF){EPRINTF("expected ']' or JSON type");break;}
        if(len>=sz){
            sz*=INCR;
            if(EREALLOC(&res,sizeof(*res)+sizeof(T)*sz)) break;
        }
        if(!(res->buf[len++]=json_fscan(from)))
            {len--;break;}
        skip_space(from);
        if((c=fgetc(from))!=','){
            if(c!=']') EPRINTF("expected ',' or ']'");
            break;
        }
    }
skip_loop:
    res->len=len;
    return realloc(res,sizeof(*res)+sizeof(T)*len);
}

extern T
sscan_arr(const char **str)
/** Takes a
 *
 */
{
    size_t len=0,sz=INITIAL;
    T res=new_arr();
    if(!res) return NULL;
    assert(**str=='[');
    while(isspace(*(++*str)));
    if(**str==']') goto skip_loop;
    for(;;++*str){
        if(!**str){EPRINTF("expected ']' or JSON type");break;}
        if(len>=sz){
            if(EREALLOC(&res,sizeof(*res)+sizeof(T)*(sz*=INCR)))
                break;
        }
        if(!(res->buf[len++]=json_sscan(str)))
            {len--;break;}
        while(isspace(**str)) ++*str;
        if(**str!=','){
            if(**str!=']') EPRINTF("expected ',' or ']'");
            break;
        }
    }
skip_loop:
    ++*str;
    res->len=len;
    return realloc(res,sizeof(*res)+sizeof(T)*len);
}

extern int
fprint_arr(FILE *to,T this)
{
   putchar('[');
   size_t len=this->len;
   for(size_t i=0;i<len;i++){
    json_fprint(to,this->buf[i]);
    if( i+1<len ) putchar(',');
   }
   putchar(']');
   return 0;
}

extern void
free_arr(T this)
{
    assert(this);
    size_t n=this->len;
    for(size_t i=0; i<n ; i++)
        json_free(this->buf[i]);
}

extern int
get_arr(T this,va_list *args)
{
    assert(this);
    size_t i=va_arg(*args,size_t);
    if(i>=this->len){EPRINTF("array index out of bounds");return 0;}
    json_T *json=va_arg(*args,json_T *);
    *json=this->buf[i];
    return 1;
}
