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
    double val;
};

/* Class Methods */
static T new_num(void);
static T fscan_num(FILE *from);
static int fprint_num(FILE *to,T this);
static int get_num(T this,va_list *args);
/*---------------*/

struct json_C
JSON_NUM_T=
{
    new_num,
    fscan_num,
    fprint_num,
    NULL,
    get_num
};

static T
new_num(void)
{
    T res=EMALLOC(sizeof(*res));
    if(!res) return NULL;
    *res=(struct json){JSON_NUM,&JSON_NUM_T,0.};
    return res;
}

static T
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

static int
fprint_num(FILE *to,T this)
{
    assert(to && this);
    return fprintf(to,"%g",this->val);
}

static int get_num(T this,va_list *args){
    assert(this);
    double *px=va_arg(*args,double *);
    *px=this->val;
    return 0;
}
