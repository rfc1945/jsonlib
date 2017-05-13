#include "err.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

void eprintf(const char *fun,const char *fmt,...){
	va_list args;
	fflush(stdout);

	fprintf(stderr,"Error: %s: ",fun);
	va_start(args,fmt);
	vfprintf(stderr,fmt,args);
	va_end(args);

	if(fmt[0]!='\0' && fmt[strlen(fmt)-1]==':'){
		fprintf(stderr," %s",strerror(errno));
	}
	fputc('\n',stderr);
}

void *emalloc(const char *func,size_t n){
	void *p=malloc(n);
	if(p==NULL)
		eprintf(func,"malloc of %zu bytes failed:",n);
	return p;
}

int safe_erealloc(const char *func,void **p,size_t sz)
{
    void *tmp=realloc(*p,sz);
    if(!tmp){
        eprintf(func,"realloc of %zu bytes failed:",sz);
        return 1;
    }
    *p=tmp;
    return 0;
}
