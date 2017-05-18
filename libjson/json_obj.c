#include <err/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <libjson/json.h>
#include <libjson/json_common.h>

#define T json_T
typedef struct llst *llst_T;
struct json{
    int type;
    llst_T  inorder;
    llst_T *inorder_tail;
    size_t have, sz;
    llst_T hashtab[];
};


#define LOAD_FACTOR (2./3)
#define INITIAL 256
#define INCR (2)

/* Local functions */
static llst_T llst_new(const char *key,T value,llst_T tail);
static int hash_insert(json_T *json,const char *key,json_T value);
static size_t hash_func(const char *str);
/*-----------------*/

struct llst{
    const char *key;
    json_T value;
    llst_T next;
    llst_T next_inorder;
};

/* Methods */

extern T new_obj(void){
    json_T res=EMALLOC(sizeof(*res)+sizeof(llst_T)*INITIAL);
    if(!res) return NULL;
    *res=(struct json){JSON_OBJ,NULL,&res->inorder,0,INITIAL};
    for(size_t i=0; i<INITIAL ; i++)
        res->hashtab[i]=NULL;
    return res;
}

extern T fscan_obj(FILE *from){
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
        if(hash_insert(&res,key,value)){
            EPRINTF("failed to insert key/value in JSON object...");
            break;
        }

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

extern T sscan_obj(const char **str)
{
    T res=new_obj();
    if(!res) return NULL;
    assert(**str=='{');
    while(isspace(*++*str));
    if(**str=='}') goto skip_loop;
    for(;;++*str){
        if(**str==EOF){
            EPRINTF("no closing '}' found");
            break;
        }
        const char *key;
        T tmp=json_sscan(str);
        if(!tmp || tmp->type!=JSON_STR){
            EPRINTF("expected JSON string as key in Object");
            break;
        }
        json_get(tmp,&key);

        while(isspace(**str)) ++*str;
        if(*((*str)++)!=':'){
            EPRINTF("expected ':' separator\n");
            json_free(tmp);
            break;
        }

        T value=json_sscan(str);
        if(!value){json_free(tmp);break;}
        if(hash_insert(&res,key,value)){
            EPRINTF("failed to insert key/value in JSON object...");
            break;
        }

        while(isspace(**str)) ++*str;
        if(**str!=','){
            if(**str!='}')
                EPRINTF("expected ',' or '}' got '%c'\n",**str);
            break;
        }
    }
skip_loop:
    ++*str;
    return res;
}

extern int fprint_obj(FILE *to,T this){
   fputc('{',to);
   for(llst_T i=this->inorder; i ; i=i->next_inorder){
       fprintf(to,"\"%s\":",i->key);
       json_fprint(to,i->value);
       if(i->next_inorder) fputc(',',to);
   }
   fputc('}',to);
   return 0;
}

extern void free_obj(T this){/* NOP */}

extern int
hash_resize(T *pobj,size_t newsz)
{
    if(EREALLOC(pobj,sizeof(**pobj)+sizeof(llst_T)*newsz))
        return 1;
    T obj=*pobj;
    for(size_t i=0; i<newsz ;i++)
        obj->hashtab[i]=NULL;
    for(llst_T i=obj->inorder; i ;i=i->next_inorder){
        size_t idx=hash_func(i->key)%newsz;
        i->next=obj->hashtab[idx];
        obj->hashtab[idx]=i;
    }
    obj->sz=newsz;
    return 0;
}

extern int
hash_insert(T *json,const char *key,T value)
{
    assert(*json && key);
    size_t sz=(*json)->sz;
    if((float)(*json)->have/sz >= LOAD_FACTOR)
        if(hash_resize(json, sz*=INCR )) return 1;
    size_t pos=hash_func(key)%sz;
    llst_T head=llst_new(key,value,(*json)->hashtab[pos]);
    if(!head) return 1;
    *((*json)->inorder_tail)=head;
    (*json)->inorder_tail=&head->next_inorder;
    (*json)->hashtab[pos]=head;
    (*json)->have++;
    return 0;
}

static llst_T
llst_new(const char *key ,T value,llst_T tail)
{
    assert(key);
    llst_T lst=EMALLOC(sizeof(*lst));
    if(!lst) return NULL;
    if(!(key=strdup(key))){
        free(lst);
        return NULL;
    }
    *lst=(struct llst){key,value,.next=tail,.next_inorder=NULL};
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

extern int
get_obj(json_T this,va_list *args)
{
    const char *str=va_arg(*args,const char *);
    json_T *dest=va_arg(*args,json_T *);
    size_t i=hash_func(str)%(this->sz);
    for(llst_T j=this->hashtab[i]; j ; j=j->next){
        if(!strcmp(j->key,str))
            return *dest=j->value, 1;
    }
    *dest=NULL;
    return 0;
}
