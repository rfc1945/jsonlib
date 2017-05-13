#ifndef ERR_H_
#define ERR_H_

#ifdef _WIN32
 #define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdint.h>

#define EPRINTF(...) (eprintf(__func__,__VA_ARGS__))
#define EMALLOC(n) (emalloc(__func__,(n)))
#define EREALLOC(p,n) (safe_erealloc(__func__,(void **)(p),(n)))

extern void  eprintf(const char *func,const char *fmt,...);
extern void *emalloc(const char *func,size_t sz);
extern int safe_erealloc(const char* func,void **p,size_t sz);

#endif // ERR_H_
