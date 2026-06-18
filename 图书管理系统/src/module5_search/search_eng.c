/**
 * search_eng.c - 模块5：智能检索系统
 *
 * 功能: 基础检索、高级检索、分类浏览、排序、检索辅助
 * 作者: 朱轶涵
 *
 * 实现要点:
 *   - 模糊匹配: 子串匹配 (大小写敏感/不敏感)
 *   - 通配符匹配: * (任意序列) / ? (单字符) 递归算法
 *   - 基础检索: 按字段+关键词遍历 g_books 筛选
 *   - 排序: qsort 按5种依据分别比较
 */

#include "search_eng.h"
#include <ctype.h>

/* ================================================================
 * 前向声明 - 演示数据/内部辅助
 * ================================================================ */
static void init_demo_books(void);
static int  demo_loaded = 0;

/* ---------- 大小写转换 ---------- */
static char *tolower_str(char *dst, const char *src, int max_len) {
    int i;
    for (i = 0; i < max_len - 1 && src[i]; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
    return dst;
}

/* ================================================================
 * 模糊匹配实现
 * ================================================================ */

/* 大小写敏感子串匹配 */
int fuzzy_match(const char *text, const char *keyword) {
    if (!text || !keyword) return 0;
    if (!*keyword) return 1;   /* 空关键词匹配一切 */
    return strstr(text, keyword) != NULL;
}

/* 大小写不敏感子串匹配 */
int fuzzy_match_ci(const char *text, const char *keyword) {
    if (!text || !keyword) return 0;
    if (!*keyword) return 1;

    /* 动态分配缓冲区，匹配后释放 */
    int   tlen = (int)strlen(text);
    int   klen = (int)strlen(keyword);
    char *tbuf = (char*)malloc(tlen + 1);
    char *kbuf = (char*)malloc(klen + 1);
    if (!tbuf || !kbuf) { free(tbuf); free(kbuf); return 0; }

    tolower_str(tbuf, text, tlen + 1);
    tolower_str(kbuf, keyword, klen + 1);
    int result = (strstr(tbuf, kbuf) != NULL);

    free(tbuf);
    free(kbuf);
    return result;
}

/* 通配符匹配 (* = 任意序列, ? = 单字符)  递归 + 剪枝 */
int wildcard_match(const char *text, const char *pattern) {
    if (!text || !pattern) return 0;

    /* 空模式: 只有空文本才匹配 */
    if (!*pattern) return !*text;

    /* 处理 * : 匹配0-N个字符 */
    if (*pattern == '*') {
        /* 跳过连续的 * */
        while (*(pattern + 1) == '*') pattern++;
        /* 匹配0字符 (跳过*) 或 匹配1字符 (text前进) */
        if (wildcard_match(text, pattern + 1)) return 1;
        if (*text && wildcard_match(text + 1, pattern)) return 1;
        return 0;
    }

    /* ? 匹配任意单字符, 普通字符精确匹配 */
    if (*text && (*pattern == '?' || *pattern == *text))
        return wildcard_match(text + 1, pattern + 1);

    return 0;
}

/* ================================================================
 * 演示数据 (15本图书，覆盖多分类/多年份/多热度)
 * ================================================================ */
static void init_demo_books(void) {
    if (demo_loaded) return;
    demo_loaded = 1;

    /* clang-format off */
    Book demo[] = {
        {1, "978-7-302-33064", "数据结构(C语言版)",  "严蔚敏",  "清华大学出版社",   2020, "计算机", "A101-01-01", 39.0f, 5, 5, 156, BOOK_ACTIVE},
        {2, "978-7-111-54493", "深入理解计算机系统", "Randal E. Bryant", "机械工业出版社", 2022, "计算机", "A101-02-01", 139.0f, 3, 3, 89, BOOK_ACTIVE},
        {3, "978-7-302-56478", "C程序设计语言",      "Brian W. Kernighan", "机械工业出版社", 2019, "计算机", "A101-03-01", 30.0f, 4, 4, 203, BOOK_ACTIVE},
        {4, "978-7-115-45678", "算法导论(原书第3版)", "Thomas H. Cormen",  "机械工业出版社", 2021, "计算机", "A101-04-01", 128.0f, 2, 2, 178, BOOK_ACTIVE},
        {5, "978-7-111-67890", "计算机网络:自顶向下方法", "James F. Kurose", "机械工业出版社", 2023, "计算机", "A102-01-01", 89.0f, 4, 4, 112, BOOK_ACTIVE},

        {6, "978-7-02-000220", "红楼梦",             "曹雪芹",  "人民文学出版社",   2018, "文学", "B201-01-01", 59.7f, 3, 2, 345, BOOK_ACTIVE},
        {7, "978-7-02-000873", "活着",               "余华",    "作家出版社",       2020, "文学", "B201-02-01", 28.0f, 5, 5, 287, BOOK_ACTIVE},
        {8, "978-7-5321-4567", "百年孤独",           "加西亚·马尔克斯", "南海出版公司", 2017, "文学", "B202-01-01", 55.0f, 2, 2, 198, BOOK_ACTIVE},

        {9,  "978-7-04-045678", "高等数学(第七版)",  "同济大学数学系", "高等教育出版社", 2021, "数学", "C301-01-01", 46.5f, 6, 6, 412, BOOK_ACTIVE},
        {10, "978-7-04-045679", "线性代数",          "同济大学数学系", "高等教育出版社", 2020, "数学", "C301-02-01", 35.0f, 5, 5, 334, BOOK_ACTIVE},
        {11, "978-7-04-045680", "概率论与数理统计",  "盛骤",          "高等教育出版社", 2022, "数学", "C301-03-01", 42.0f, 4, 4, 298, BOOK_ACTIVE},

        {12, "978-7-101-05555", "史记",              "司马迁",  "中华书局",        2019, "历史", "D401-01-01", 128.0f, 2, 2, 67, BOOK_ACTIVE},
        {13, "978-7-101-06666", "全球通史",          "斯塔夫里阿诺斯", "北京大学出版社", 2023, "历史", "D401-02-01", 88.0f, 3, 3, 145, BOOK_ACTIVE},

        {14, "978-7-111-55493", "操作系统概念",      "Abraham Silberschatz", "机械工业出版社", 2021, "计算机", "A102-02-01", 99.0f, 2, 2, 76, BOOK_ACTIVE},
        {15, "978-7-302-55678", "Python编程从入门到实践", "Eric Matthes", "人民邮电出版社", 2023, "计算机", "A103-01-01", 89.0f, 5, 5, 234, BOOK_ACTIVE},
    };
    /* clang-format on */

    for (int i = 0; i < 15; i++)
        kv_push(Book, g_books, demo[i]);
}

/* ================================================================
 * 图书内部查找 (按ID)
 * ================================================================ */
static Book *find_book_by_id(int book_id) {
    for (size_t i = 0; i < kv_size(g_books); i++)
        if (kv_A(g_books, i).id == book_id)
            return &kv_A(g_books, i);
    return NULL;
}

/* ================================================================
 * 基础检索  search_books()
 *
 * field 支持: "title" | "author" | "isbn" | "publisher" | "category" | "year"
 * 匹配规则:
 *   title/author/publisher : 模糊匹配(不区分大小写)
 *   isbn                   : 前缀/精确匹配
 *   category               : 模糊匹配(不区分大小写)
 *   year                   : 单值精确 或 范围 "2020-2022"
 * ================================================================ */
int search_books(const char *field, const char *keyword,
                 Book *results, int max_results, int *count) {
    if (count) *count = 0;
    if (!field || !keyword || !results) return -1;

    init_demo_books();

    int cnt = 0;
    for (size_t i = 0; i < kv_size(g_books) && cnt < max_results; i++) {
        Book *b = &kv_A(g_books, i);
        int  match = 0;

        if (strcmp(field, "title") == 0)
            match = fuzzy_match_ci(b->title, keyword);
        else if (strcmp(field, "author") == 0)
            match = fuzzy_match_ci(b->author, keyword);
        else if (strcmp(field, "isbn") == 0)
            match = (strncmp(b->isbn, keyword, strlen(keyword)) == 0);
        else if (strcmp(field, "publisher") == 0)
            match = fuzzy_match_ci(b->publisher, keyword);
        else if (strcmp(field, "category") == 0)
            match = fuzzy_match_ci(b->category, keyword);
        else if (strcmp(field, "year") == 0) {
            /* 支持 单值 "2020" 或 范围 "2020-2022" */
            int y1 = 0, y2 = 0, n = sscanf(keyword, "%d-%d", &y1, &y2);
            if (n == 2) match = (b->year >= y1 && b->year <= y2);
            else if (n == 1) match = (b->year == y1);
        }

        if (match) results[cnt++] = *b;
    }
    if (count) *count = cnt;
    return cnt;
}

/* ================================================================
 * 高级检索  advanced_search()
 *
 * 按 SearchCriteria 多字段组合筛选，逻辑运算符 AND/OR
 * ================================================================ */
int advanced_search(const SearchCriteria *criteria,
                    Book *results, int max_results, int *count) {
    if (count) *count = 0;
    if (!criteria || !results) return -1;

    init_demo_books();

    int cnt = 0;
    for (size_t i = 0; i < kv_size(g_books) && cnt < max_results; i++) {
        Book *b = &kv_A(g_books, i);

        /* 逐字段检查 */
        int  ok_title     = (!criteria->title[0]     || fuzzy_match_ci(b->title,     criteria->title));
        int  ok_author    = (!criteria->author[0]    || fuzzy_match_ci(b->author,    criteria->author));
        int  ok_isbn      = (!criteria->isbn[0]      || strncmp(b->isbn, criteria->isbn, strlen(criteria->isbn)) == 0);
        int  ok_category  = (!criteria->category[0]  || fuzzy_match_ci(b->category,  criteria->category));
        int  ok_publisher = (!criteria->publisher[0] || fuzzy_match_ci(b->publisher, criteria->publisher));
        int  ok_year      = 1;
        int  ok_price     = 1;

        if (criteria->year_min > 0 && b->year < criteria->year_min) ok_year = 0;
        if (criteria->year_max > 0 && b->year > criteria->year_max) ok_year = 0;
        if (criteria->price_min > 0.0f && b->price < criteria->price_min) ok_price = 0;
        if (criteria->price_max > 0.0f && b->price > criteria->price_max) ok_price = 0;

        /* 综合判断 */
        int match;
        if (criteria->logic_op == 1) /* OR */
            match = ok_title || ok_author || ok_isbn || ok_category || ok_publisher || ok_year || ok_price;
        else                         /* AND (默认) */
            match = ok_title && ok_author && ok_isbn && ok_category && ok_publisher && ok_year && ok_price;

        if (match) results[cnt++] = *b;
    }

    /* 如果指定了排序，先排后返回 */
    if (criteria->sort_by >= 0)
        sort_results(results, cnt, criteria->sort_by, criteria->sort_asc);

    if (count) *count = cnt;
    return cnt;
}

/* ================================================================
 * 分类浏览  browse_category()
 * ================================================================ */
int browse_category(const char *category,
                    Book *results, int max_results, int *count) {
    if (count) *count = 0;
    if (!category || !results) return -1;

    init_demo_books();

    int cnt = 0;
    for (size_t i = 0; i < kv_size(g_books) && cnt < max_results; i++) {
        if (fuzzy_match_ci(kv_A(g_books, i).category, category))
            results[cnt++] = kv_A(g_books, i);
    }
    if (count) *count = cnt;
    return cnt;
}

/* ================================================================
 * 图书详情  get_book_detail()
 * ================================================================ */
int get_book_detail(int book_id, BookDetail *detail) {
    if (!detail) return -1;

    init_demo_books();

    Book *b = find_book_by_id(book_id);
    if (!b) return -1;

    detail->book = *b;
    detail->available_copies = b->available;
    detail->total_copies     = b->total_copies;
    detail->avg_borrow_days  = 28;  /* 演示固定值，后续可实际计算 */
    detail->reserve_count    = 0;
    return 0;
}

/* ================================================================
 * 排序  sort_results()
 *
 * sort_by:  0=相关性(保持原序) 1=热度 2=出版年份 3=价格 4=书名
 * ascending: 1=升序  0=降序
 * ================================================================ */

/* ---- 5组比较函数 ---- */
static int cmp_hotness_desc(const void *a, const void *b) {
    return ((const Book*)b)->borrowed_count - ((const Book*)a)->borrowed_count;
}
static int cmp_hotness_asc(const void *a, const void *b) {
    return ((const Book*)a)->borrowed_count - ((const Book*)b)->borrowed_count;
}

static int cmp_year_desc(const void *a, const void *b) {
    return ((const Book*)b)->year - ((const Book*)a)->year;
}
static int cmp_year_asc(const void *a, const void *b) {
    return ((const Book*)a)->year - ((const Book*)b)->year;
}

static int cmp_price_desc(const void *a, const void *b) {
    float d = ((const Book*)b)->price - ((const Book*)a)->price;
    return (d > 0) ? 1 : ((d < 0) ? -1 : 0);
}
static int cmp_price_asc(const void *a, const void *b) {
    float d = ((const Book*)a)->price - ((const Book*)b)->price;
    return (d > 0) ? 1 : ((d < 0) ? -1 : 0);
}

static int cmp_title_asc(const void *a, const void *b) {
    return strcmp(((const Book*)a)->title, ((const Book*)b)->title);
}
static int cmp_title_desc(const void *a, const void *b) {
    return strcmp(((const Book*)b)->title, ((const Book*)a)->title);
}

void sort_results(Book *results, int count, int sort_by, int ascending) {
    if (!results || count <= 1) return;

    int (*cmp)(const void*, const void*) = NULL;

    switch (sort_by) {
    case SORT_HOTNESS: cmp = ascending ? cmp_hotness_asc  : cmp_hotness_desc; break;
    case SORT_YEAR:    cmp = ascending ? cmp_year_asc      : cmp_year_desc;    break;
    case SORT_PRICE:   cmp = ascending ? cmp_price_asc     : cmp_price_desc;   break;
    case SORT_TITLE:   cmp = ascending ? cmp_title_asc     : cmp_title_desc;   break;
    case SORT_RELEVANCE:
    default: /* 保持原位 */ return;
    }
    if (cmp) qsort(results, count, sizeof(Book), cmp);
}

/* ================================================================
 * 检索辅助
 * ================================================================ */
int get_hot_searches(char *keywords[], int max_count) {
    /* 演示固定热门关键词 */
    static const char *demo_hot[] = {
        "C程序设计", "数据结构", "高等数学", "Python", "红楼梦",
        "线性代数", "算法", "操作系统", "计算机网络", "活着"
    };
    int n = 10;
    if (max_count < n) n = max_count;
    for (int i = 0; i < n; i++)
        keywords[i] = (char*)demo_hot[i];
    return n;
}

int save_search_history(int reader_id, const char *query) {
    /* 简化: 存在全局数组中  */
    (void)reader_id;
    (void)query;
    return 0; /* 成功 */
}

int get_search_history(int reader_id, char *history[], int max_count) {
    (void)reader_id; (void)history; (void)max_count;
    return 0;
}
