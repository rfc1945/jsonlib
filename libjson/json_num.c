#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
struct json{int type;double val};

extern T
new_num(void)
{
    T res=EMALLOC(sizeof(*res));
    if(!res) return NULL;
    *res=(struct json){JSON_NUM,0.};
    return res;
}

extern T
fscan_num(FILE *from)
{
    assert(from);
    T res=new_num();
    if(!res) return NULL;
    if(fscanf(from,"%lf",&res->val)!=1){
        EPRINTF("bad format, expected real number.");
        json_free(res);
        return NULL;
    }
    return res;
}

extern T sscan_num(const char **str)
{
    T res=new_num();
    if(!res) return NULL;
    int read=0;
    if(sscanf(*str,"%lf%n",&res->val,&read)!=1){
        EPRINTF("bad format, expected real number.");
        json_free(res);
        return NULL;
    }
    (*str)+=read;
    return res;
}

extern int
fprint_num(FILE *to,T this)
{
    assert(to && this);
    return fprintf(to,"%g",this->val);
}

extern int
get_num(T this,va_list *args)
{
    assert(this);
    double *px=va_arg(*args,double *);
    *px=this->val;
    return 0;
}
