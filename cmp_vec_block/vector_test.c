
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <hs.h>

#define GETTIMEUSEC(x)	\
	do {	\
		struct timeval tv;	\
		gettimeofday(&tv, NULL);	\
		*x = tv.tv_usec + tv.tv_sec * 1000 * 1000;	\
	} while(0)


#define RULES_HS_FLAGS   (HS_FLAG_CASELESS    | \
                HS_FLAG_SINGLEMATCH | \
                HS_FLAG_DOTALL)


#define RULES_HS_FLAGS_LEFTMOST        (HS_FLAG_CASELESS    | \
                HS_FLAG_DOTALL      | \
                HS_FLAG_SOM_LEFTMOST) 


static int on_match(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *ctx)
{
//    printf("id:[%d] Match for at offset %llu flags:[%u]\n", id, to, flags);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *data[] =
    {
        "Aug 13 16:16:51 A01-R15-I35-102-6002177 alb-jxn7xrmlyq-ins-1 waf: |q|2022-08-13T16:16:51+08:00|q|waf-h8u3avvvfn|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|211.152.1.97|q|2791|q|10.6.168.8|q|80|q|-|q|10.6.169.103:6233|q|0.640|q|0.292|q|0.639|q|200|q|200|q|3399|q|1294|q|POST|q|jdthx.yaic.com.cn|q|http://jdthx.yaic.com.cn",
        "http,//admin-auction.autostreets.com/panel/25866",
        "match_info,0|0|1068|0|0,1|0|1068|0|0,2|0|0|0|0,3|0|0|0|0,4|0|0|0|0,5|0|0|0|0,6|0|0|0|0,7|0|0|0|0,8|0|0|0|0,9|0|0|0|0,10|0|0|0|0,",
        "Jul 20 15,38,01 A02-R25-I148-106-F9JFTP2 alb-4ajg6ym3mn-ins waf, |q|2022-07-20T15,38,01+08,00|q|waf-zomovs0ofm|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|171.34.210.229|q|4554|q|172.24.50.4|q|80|q|_ga=GA1.2.1619888629.1634876439; Hm_lvt_f2054e32ca07a97a92f435e02f5e73aa=1651217917,1652938032; _gid=GA1.2.809329083.1658218442; liIndex=0; _back_login_flag=1658302210153; JSESSIONID=D55349913DDB7F86DB091C5579FCC42B|q|172.24.50.51,80|q|1.992|q|0.000|q|1.992|q|200|q|200|q|654|q|1183|q|GET|q|admin-auction.autostreets.com|q|http,//admin-auction.autostreets.com/panel/sync?avId=2037890&t=1658302679081&auctionId=25866|q|HTTP/1.1|q|http,//admin-auction.autostreets.com/panel/25866|q|Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36|q|-|q|-|q|match_info,0|0|1068|0|0,1|0|1068|0|0,2|0|0|0|0,3|0|0|0|0,4|0|0|0|0,5|0|0|0|0,6|0|0|0|0,7|0|0|0|0,8|0|0|0|0,9|0|0|0|0,10|0|0|0|0,|q|line_info,-|q|waf_message,1|0|0|all_hit_rule_id,-|fingerprint,-|qps,116|waf_cpu_bypass_set,0|waf_rules_version,v3.2.0040|waf_hit_payload_after,|waf_sa_hit_payload_after,|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|171.34.210.229|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|-|q|",
        "5c3efa20388c1e50bd864c2346a20026541eddf4",
        "admin-auction.autostreets.com",
        "abc teakettleeeeeeeeijk",
        "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36",
        "http,//admin-auction.autostreets.com/panel/sync?avId=2037890&t=1658302679081&auctionId=25866",
        "_ga=GA1.2.1619888629.1634876439; Hm_lvt_f2054e32ca07a97a92f435e02f5e73aa=1651217917,1652938032; _gid=GA1.2.809329083.1658218442; liIndex=0; _back_login_flag=1658302210153; JSESSIONID=D55349913DDB7F86DB091C5579FCC42B",
        "_back_login_flag=1659578552826; JSESSIONID=C1E5BCCE88CCA6B4635BD9087355A6F5; liIndex=0"
    };

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
        strlen(data[9]),
        strlen(data[10])
    };

    hs_database_t *db = NULL;
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

    hs_error_t err = hs_compile_multi(expr, flags, ids, sizeof(ids)/sizeof(unsigned), HS_MODE_VECTORED, NULL, &db, &compile_err);

    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS)
    {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(db);
        return -1;
    }
    long sum = 0;
    int i;
	long tm_start = 0, tm_end = 0;
	int loop = 1;
	for(i = 0; i < loop; i++)
    {
        GETTIMEUSEC(&tm_start);
        err = hs_scan_vector(db, data, len, sizeof(len)/sizeof(unsigned int), 0, scratch, on_match, NULL);
        if (err != HS_SUCCESS)
        {
            fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
            hs_free_scratch(scratch);
            hs_free_database(db);
            return -1;
        }
        GETTIMEUSEC(&tm_end);
        sum += (tm_end - tm_start);
    }

    printf("hs_scan_vector i:%d cost:%li(us)\n", i, sum / i);

    /*
	hs_scan第一次匹配时间长， 后面时间短
    i:0 cost:89
    i:1 cost:5
    i:2 cost:3
    i:3 cost:3
    i:4 cost:3
    i:5 cost:3
    i:6 cost:3
    i:7 cost:2
    i:8 cost:3
    i:9 cost:3
    */

    hs_free_scratch(scratch);
    hs_free_database(db);

    return 0;
}


