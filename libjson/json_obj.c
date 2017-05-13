#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <libjson/json.h>

typedef struct llst *llst_T;
struct json{
    int type;
    json_C class;
    size_t have, sz;
    llst_T hashtab[];
};

#define T json_T
#define LOAD_FACTOR (2./3)
#define INITIAL 256
#define INCR (2)

/*--Class Methods--*/
static T new_obj(void);
static T fscan_obj(FILE *from);
static int fprint_obj(FILE *to,T this);
static void free_obj(T this);
static int get_obj(T this,va_list *args);
/*-----------------*/
/* Local functions */
static llst_T llst_new(const char *key,T value,llst_T tail);
static int hash_insert(json_T *json,const char *key,json_T value);
static size_t hash_func(const char *str);
/*-----------------*/

struct json_C
JSON_OBJ_T=
{
    new_obj,
    fscan_obj,
    fprint_obj,
    free_obj,
    get_obj
};

struct llst{
    const char *key;
    json_T value;
    llst_T next;
};

/* Methods */

static T new_obj(void){
    json_T res=EMALLOC(sizeof(*res)+sizeof(llst_T)*INITIAL);
    if(!res) return NULL;
    *res=(struct json){JSON_OBJ,&JSON_OBJ_T,0,INITIAL};
    for(size_t i=0; i<INITIAL ; i++)
        res->hashtab[i]=NULL;
    return res;
}

static T fscan_obj(FILE *from){
    T res=new_obj();
    if(!res) return NULL;

    int c=fgetc(from);
    assert(c=='{');

    skip_space(from);
    if((c=fgetc(from))=='}') goto skip_loop;
    else ungetc(c,from);

    while(1){
        if(c==EOF){
            EPRINTF("no closing '}' found");
            break;
        }
        const char *key;
        T tmp=json_fscan(from);
        if(!tmp || tmp->type!=JSON_STR){
            EPRINTF("expected JSON string as key in Object");
            break;
        }
        json_get(tmp,&key);

        skip_space(from);
        if(fgetc(from)!=':'){
            EPRINTF("expected ':' separator\n");
            json_free(tmp);
            break;
        }

        T value=json_fscan(from);
        if(!value){json_free(tmp);break;}

        hash_insert(&res,key,value);

        skip_space(from);
        if((c=getc(from))!=','){
            if(c!='}')
                EPRINTF("expected ',' or '}' got '%c'\n",c);
            break;
        }
    }
skip_loop:
    return res;
}

static int fprint_obj(FILE *to,T this){
   putchar('{');
   size_t sz=this->sz;
   for(size_t i=0; i<sz ;i++){
        llst_T j=this->hashtab[i];
        if(!j) continue;
        for(; j ; j=j->next){
            fprintf(to,"\"%s\"",j->key);
            fputc(':',to);
            json_fprint(to,j->value);
            fputc(' ',to);
        }
   }
   putchar('}');
   return 0;
}

static void free_obj(T this){/* NOP */}

static int
hash_insert(T *json,const char *key,T value)
{
    assert(*json && key);
    size_t sz=(*json)->sz;
    if((float)(*json)->have/sz >= LOAD_FACTOR){
        sz *= INCR;
        if(!EREALLOC(json,sizeof(**json)+sizeof(llst_T)*sz))
            return 1;
    }
    size_t pos= hash_func(key)%sz;
    if(!(key=strdup(key))) return 1;
    llst_T head=llst_new(key,value,(*json)->hashtab[pos]);
    if(!head) return 1;
    (*json)->hashtab[pos]=head;
    (*json)->have++;
    (*json)->sz=sz;
    return 0;
}

static llst_T
llst_new(const char *key ,T value,llst_T tail)
{
    llst_T lst=EMALLOC(sizeof(*lst));
    if(!lst) return NULL;
    *lst=(struct llst){key,value,tail};
    return lst;
}

static size_t
hash_func(const char *str)
{
    size_t hash = 5381;
    size_t i;
    for (i = 0; str[i] ; ++i)
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}

static int
get_obj(json_T this,va_list *args)
{
    const char *str=va_arg(*args,const char *);
    json_T *dest=va_arg(*args,json_T *);
    size_t i=hash_func(str);
    for(llst_T j=this->hashtab[i]; j ; j=j->next){
        if(!strcmp(j->key,str)){
            *dest=j->value;
            return 1;
        }
    }
    return 0;
}
