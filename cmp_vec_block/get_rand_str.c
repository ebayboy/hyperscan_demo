

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include <string.h>

char * malloc_rands(size_t slen)
{
    char *s = malloc(slen + 1);
    int i;

	//可见字符范围: 32 - 128
	int a = 32;
	int b = 128;
	struct timeval tv;
	gettimeofday(&tv);

	long usec = tv.tv_sec * 1000000 + tv.tv_usec;
	usec += slen;

    srand(usec);
    for (i=0; i < slen; i++)
    {
        *(s + i) = (rand() % (b-a + 1)) + a ;
    }
	*(s + slen) = 0;

    return s;
}

int malloc_datas(char *data[], size_t size)
{
    int base = 4;
	int i;
    for ( i = 0; i < size; i++) {
        data[i] = malloc_rands(base);
        printf("+++ i[%d] base:[%d] len[%d] data:[%s]\n\n", i, base, strlen(data[i]), data[i]);
        base *= 2;
    }
}

int main()
{
	char *ss[10] = {0};

	malloc_datas(ss, 10);

	return 0;
}
