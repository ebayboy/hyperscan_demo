#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hs.h>
#include <time.h>
#include <sys/time.h>

static int l = 0;

char * malloc_rands(size_t slen)
{
    char *s = malloc(slen + 1);
    int i;

    //可见字符范围: 32 - 128
    int a = 32;
    int b = 128;
    struct timeval tv;
    gettimeofday(&tv, NULL);

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
    for ( i = 0; i < size; i++)
    {
        data[i] = malloc_rands(base);
        //printf("+++ i[%d] base:[%d] len[%d] data:[%s]\n\n", i, base, strlen(data[i]), data[i]);
        base *= 2;
    }
}

void free_datas(char *data[], size_t size)
{
    int i;
    for(i  =0; i < size; i++)
    {
        if (data[i])
        {
            free(data[i]);
            data[i] = NULL;
        }
    }
}

/* API document:
 * http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH
 **/

#define RULES_HS_FLAGS   (HS_FLAG_CASELESS    | \
                HS_FLAG_SINGLEMATCH | \
                HS_FLAG_DOTALL)


#define RULES_HS_FLAGS_LEFTMOST        (HS_FLAG_CASELESS    | \
                HS_FLAG_DOTALL      | \
                HS_FLAG_SOM_LEFTMOST)

#define GETTIMEUSEC(x)	\
	do {	\
		struct timeval tv;	\
		gettimeofday(&tv, NULL);	\
		*x = tv.tv_usec + tv.tv_sec * 1000 * 1000;	\
	} while(0)

static int on_match(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *ctx)
{
    //printf("Match for pattern \"%s\" at from %llu  to %llu  flags:%u\n", (char *)ctx, from, to, flags);
    return 0;
}

int match_datas_vector(
		hs_database_t *db, hs_scratch_t *scratch, 
		hs_database_t *db_vec, hs_scratch_t *scratch_vec, 
		char *data[])
{
    long tm_start = 0, tm_end = 0;

    //TODO: 为什么第一次时间长？
    unsigned int len[] =
    {
        strlen(data[0]),
        strlen(data[1]),
        strlen(data[2]),
        strlen(data[3]),
        strlen(data[4]),
        strlen(data[5]),
        strlen(data[6]),
        strlen(data[7]),
        strlen(data[8]),
        strlen(data[9])
    };

	//预热匹配： 否则第一次匹配耗时高， 无法正常评估VECTOR和BLOCK模式哪个性能好
    for (int i = 0 ; i < 10; i++)
    {
        if (hs_scan(db, data[i], strlen(data[i]), 0, scratch, on_match, NULL) != HS_SUCCESS)
        {
			printf("Error: hs_scan!\n");
            return -1;
        }
    }
    printf("pre match done!\n");

	//结论： 无论vector还是block那个先匹配， 都是block性能好于vector模式
	
	//1. VECTOR Match
    GETTIMEUSEC(&tm_start);
	if (hs_scan_vector(db_vec, (const char * const*)data, len, 10, 0, scratch_vec, on_match, NULL) != HS_SUCCESS) 
	{
		printf("Error: hs_scan_vector!\n");
		return -1;
	}
	GETTIMEUSEC(&tm_end);
    printf("vector cost:%li\n", tm_end - tm_start);

	//2. BLOCK Match
    GETTIMEUSEC(&tm_start);
    for (int i = 0 ; i < 10; i++)
    {
        if (hs_scan(db, data[i], strlen(data[i]), 0, scratch, on_match, NULL) != HS_SUCCESS)
        {
			printf("Error: hs_scan!\n");
            return -1;
        }
    }
    GETTIMEUSEC(&tm_end);
    printf("block cost:%li\n", tm_end - tm_start);

    return 0;
}


int match_datas(hs_database_t *db, hs_scratch_t *scratch)
{
    long tm_start = 0, tm_end = 0;
    //TODO: 为什么第一次时间长？
    char *data[10] = {0};
    malloc_datas(data, 10);

    GETTIMEUSEC(&tm_start);
    for (int i = 0 ; i < 10; i++)
    {
        //GETTIMEUSEC(&tm_start);
        if (hs_scan(db, data[i], strlen(data[i]), 0, scratch, on_match, NULL) != HS_SUCCESS)
        {
            return -1;
        }
        //GETTIMEUSEC(&tm_end);
        //printf("cost:%li len:%d\n", tm_end - tm_start, strlen(data[i]));
    }
    GETTIMEUSEC(&tm_end);
    printf("cost:%li\n", tm_end - tm_start);

    free_datas(data, 10);

    return 0;
}

static int hs_build(hs_database_t **db, hs_scratch_t **scratch, int mode)
{
    hs_compile_error_t *compile_err = NULL;
    const char *expr[] =
    {
        "teakettle{4,10}",  /* 104 */
        "(;|\\(|\\)|,|\\*/|\\s)\\s*select(\\s+|\\()(distinct\\s+)?(\\(*\\x60?[a-zA-Z0-9\\-\\_\\.]{1,30}\\x60?\\)*|\\(*\\bELT\\([\\d=]+,[^\\r\\n]{0,60}(\\d|'|\")\\s*\\)+|([\\x20-\\x80\\s]{0,20})?\\bdecode\\s*\\([\\x20-\\x80]{2,30},\\s*['\"`][^'\"`]{1,20}['\"`]\\s*,[\\x20-\\x80\\s]{2,50}\\)(\\s+[\\x20-\\x80\\s]{0,20})?|\\(*CHA?R\\(\\d{2,3}\\)((\\|{2}|\\+)CHA?R\\(\\d{2,3}\\))+\\)*|(cast\\()?(\\s*ceiling\\()?(\\s*floor\\()?\\s*rand\\(\\d*\\)[\\s\\*a-z\\d\\)]{2,16}|count\\((\\*|[a-zA-Z0-9\\-\\_\\.\\x60]{1,30})\\)|((group_concat\\(+)?concat(_ws)?\\(+|group_concat\\(|ROUND\\()+(['\"][\\x20-\\x80\\s]{0,100}['\"]\\s*,)?\\s*[a-zA-Z0-9\\-\\_\\.\\(\\)]{1,30},?(,\\s*[a-zA-Z0-9\\-\\_\\.\\(\\)]{1,30})*\\)+)(\\s+as\\s+[a-zA-Z0-9\\-\\_\\.\\x60]{1,30})?(\\s*,\\s*(\\(*\\x60?[a-zA-Z0-9\\-\\_\\.]{1,30}\\x60?\\)*|\\(*\\bELT\\([\\d=]+,[^\\r\\n]{0,60}(\\d|'|\")\\s*\\)+|\\(*CHA?R\\(\\d{2,3}\\)((\\|{2}|\\+)CHA?R\\(\\d{2,3}\\))+\\)*|(cast\\()?(\\s*ceiling\\()?(\\s*floor\\()?\\s*rand\\(\\d*\\)[\\s\\*a-z\\d\\)]{2,16}|count\\((\\*|[a-zA-Z0-9\\-\\_\\.\\x60]{1,30})\\)|((group_concat\\(+)?concat(_ws)?\\(+|group_concat\\(|ROUND\\()+(['\"][\\x20-\\x80\\s]{0,100}['\"]\\s*,)?\\s*[a-zA-Z0-9\\-\\_\\.\\(\\)]{1,30},?(,\\s*[a-zA-Z0-9\\-\\_\\.\\(\\)]{1,30})*\\)+)(\\s+as\\s+[a-zA-Z0-9\\-\\_\\.\\x60]{1,30})?)*\\)*(\\s+|\\)*)from([\\s\\(]([\\x20-\\x80\\s]{10}|[\\x20-\\x80\\s]{0,9}$)|/\\*)",              /* 101 */
        "(java\\.lang\\.ProcessBuilder\\b|java\\.lang\\.Runtime\\b|java\\.lang\\.System\\b|class\\.classLoader\\b|class\\['classLoader'\\]|java\\.lang\\.Shutdown\\b|javax\\.script\\.ScriptEngineManager\\b|ognl\\.OgnlContext\\b|ognl\\.MemberAccess\\b|ognl\\.ClassResolver\\b|ognl\\.TypeConverter\\b|com\\.opensymphony\\.xwork2\\.ActionContext\\b|_memberAccess\\b|java\\.io\\.File\\b)",              /* 102 */
        "\\b2\\.?2\\.?2\\.?5\\.?0\\.?7\\.?3\\.?8\\.?5\\.?8\\.?5\\.?0\\.?7\\.?2\\.?0\\.?1\\.?(1|2)\\b",       /* 103 */
        "(\\b|22|27)com\\.opensymphony\\.xwork2\\.dispatcher\\.HttpServletRe(?:quest|sponse)|\\bxwork\\.MethodAccessor\\.denyMethodExecution",  /* 104 */
        "(\\s|;|\\(|\\*/)exec(ute)?[\\s\\r\\n]{1,10}(/\\*.{0,30}\\*/)?[\\s\\r\\n]{0,10}(master(\\.\\.|\\.sys\\.|\\.db(a|o)?\\.))?((xp[_\\[](cmdshell|dirtree|enumgroups|getnetname|fixeddrives|availablemedia|getfiledetails|servicecontrol|fileexist|subdirs|loginconfig|regaddmultistring|regdeletekey|regdeletevalue|regenumkeys|regenumvalues|regread|regremovemultistring|regwrite))|(sp_(msgetversion|MSforeachtable|spaceused|MSforeachtable|makewebtask|add_job|addtask|configure)))",        /* 105 */
        "^(.{0,100}[\\x20-\\x80\\s]{10}|[\\x20-\\x80\\s]{0,10})\\.(asp|php|jsp|py|cer|ascx|asa|jspx|jsp|htacces|txt|jpg|jpeg|png|gif|svg|img)\\d*\\x00\\.(php|rb)$",
        "(<\\?(php)?|<script(\\s|\\+)*language=\\\"php\\\">)\\s*[\\x20-\\x80\\s\\r\\n]{0,150}@?(?:eval\\.?|eval_r|assert|exec|system|create_function|preg_replace|\\$_(GET|POST|COOKIE|REQUEST|GLOBAL)\\[[\\x20-\\x80\\s\\r\\n]{0,150}\\]|ReflectionFunction)\\([^;]{0,150}\\$_(GET|POST|COOKIE|REQUEST|GLOBAL|FILES)\\[",
        "(<\\?(php)?|<script(\\s|\\+)*language=\\\"php\\\">)[\\x20-\\x80\\s\\r\\n]{0,150}[\\s;]\\$(\\{?[\\w_]{0,150}\\}?\\([^;]{0,150}\\$_(GET|POST|COOKIE|REQUEST|GLOBAL|FILES)\\[|\\w{1,20}\\s*=\\s*\\w{0,20}\\(\\$_(GET|POST|COOKIE|REQUEST|GLOBAL|FILES)\\[)",
        "^[\\x20-\\x80\\s]{0,15}([\\s'\"\\)]|/\\*.{0,20}\\*/\\s*)(\\b(and|or(\\s+not)?)\\b|(&&|\\|{2}))(\\s+\\(?|\\(|\\s*/\\*.{0,20}\\*/\\s*|(!|\\~){1,80})([\\x20-\\x80\\s!\\x00\\x0a\\x0d]{0,30}\\b(left|right|substring|substr|mid)\\([\\x20-\\x80\\s]{3,150},\\s*\\d+\\s*\\)+\\s*[=<>][\\x20-\\x80\\s]{2,30}|[\\x20-\\x80\\s]{2,30}[=<>][\\x20-\\x80\\s]{0,10}(left|right|substring|substr|mid)\\([\\x20-\\x80\\s]{3,15},\\s*\\d+\\s*\\))(in|\\s|\\-\\-|\\x23|/\\*.{0,20}\\*/){0,10}\\s*[\\w\\+]{0,150}$",
        "(java\\.lang\\.ProcessBuilder\\b|java\\.lang\\.Runtime\\b|java\\.lang\\.System\\b|class\\.classLoader\\b|class\\['classLoader'\\]|java\\.lang\\.Shutdown\\b|javax\\.script\\.ScriptEngineManager\\b|ognl\\.OgnlContext\\b|ognl\\.MemberAccess\\b|ognl\\.ClassResolver\\b|ognl\\.TypeConverter\\b|com\\.opensymphony\\.xwork2\\.ActionContext\\b|_memberAccess\\b|java\\.io\\.File\\b)"
    };

    /* hit 101 & 104 & 105 */
    unsigned flags[] =
    {
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS,
        RULES_HS_FLAGS
    };

    unsigned ids[] = {101, 102, 103, 104, 105, 106,107,107,109,110};

    hs_error_t err = hs_compile_multi(expr, flags, ids, sizeof(ids)/sizeof(unsigned), mode, NULL, db, &compile_err);
    if (err != HS_SUCCESS)
    {
        hs_free_compile_error(compile_err);
        printf("Error: hs_compile_multi!");
        return -1;
    }

    /* scratch */
    if (hs_alloc_scratch(*db, scratch) != HS_SUCCESS)
    {
        printf("Error: hs_alloc_scratch!!");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    hs_database_t *db = NULL, *db_vec = NULL;
    hs_scratch_t *scratch = NULL, *scratch_vec = NULL;

    hs_build(&db, &scratch, HS_MODE_BLOCK);

    hs_build(&db_vec, &scratch_vec, HS_MODE_VECTORED);

	if (db == NULL || scratch == NULL || db_vec == NULL || scratch_vec == NULL) {
		printf("Error: db:%p scratch:%p db_vec:%p scratch_vec:%p\n");
		return -1;
	}

	//构建数据
	char *data[10] = {0};
    malloc_datas(data, 10);

	printf("=====match block\n");
	match_datas_vector(db, scratch, db_vec, scratch_vec, data);

    free_datas(data, 10);


    /* free scratch & db */
    hs_free_scratch(scratch);
    hs_free_database(db);

    return 0;
}


