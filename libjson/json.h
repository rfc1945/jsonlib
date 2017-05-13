#ifndef JSON_H_
#define JSON_H_
#include <stdio.h>
#include <stdarg.h>

#define T json_T

typedef struct json *json_T;
typedef struct json_C *json_C;

enum JSON_TYPE{
    JSON_NO_TYPE,
    JSON_ARR,
    JSON_OBJ,
    JSON_NUM,
    JSON_STR,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
};
#define JSON_NTYPES 8
struct json_C{
    T (*new)(void);
    T (*fscan)(FILE *from);
    int (*fprint)(FILE *to,T this);
    void (*free)(T this);
    int (*get)(T this,va_list *args);
    int (*set)(T this,va_list *args);
};

extern T   json_fscan(FILE *from);
extern int json_fprint(FILE *to,T json);
extern T json_new(int type);
extern int json_get(T json,...);
extern T json_set(T json,...);
extern void json_free(T json);

static inline int
fpeek(FILE *f){int c=fgetc(f);ungetc(c,f);return c;}
static inline void
skip_space(FILE *f){fscanf(f,"%*[\r\t\v\n ]");}

#undef T
#endif // JSON_H_
