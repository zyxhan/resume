/**
 * common.h - 高校图书馆智能借阅管理系统 共享数据结构
 *
 * 本文件定义所有模块共用的数据类型、枚举、常量。
 * 基于 klib(kvec.h/khash.h) 实现动态数组与哈希表。
 *
 * 数据规模上限:
 *   图书 ≤ 20000 种, 读者 ≤ 10000 人, 借阅记录 ≤ 50000 条
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ================================================================
 * klib 引入
 * ================================================================ */
#include "kvec.h"   /* kvec_t(T)  动态数组  */
#include "khash.h"  /* khash_t(T) 哈希表    */

/* ================================================================
 * 数据规模宏
 * ================================================================ */
#define MAX_BOOKS       20000
#define MAX_READERS     10000
#define MAX_RECORDS     50000
#define MAX_CATEGORIES  50

/* ================================================================
 * 读者类型枚举
 * ================================================================ */
typedef enum {
    READER_UNDERGRAD = 0,   /* 本科生 */
    READER_GRADUATE  = 1,   /* 研究生 */
    READER_TEACHER   = 2,   /* 教师   */
    READER_STAFF     = 3    /* 职工   */
} ReaderType;

/* 读者类型名称映射 */
static const char *READER_TYPE_NAMES[] = {
    "本科生", "研究生", "教师", "职工"
};

/* ================================================================
 * 账户状态枚举
 * ================================================================ */
typedef enum {
    ACCT_ACTIVE    = 0,  /* 正常   */
    ACCT_SUSPENDED = 1,  /* 已冻结 */
    ACCT_EXPIRED   = 2,  /* 已过期 */
    ACCT_PENDING   = 3,  /* 待审核 */
    ACCT_INACTIVE  = 4   /* 已注销 */
} AccountStatus;

/* ================================================================
 * 图书状态枚举
 * ================================================================ */
typedef enum {
    BOOK_ACTIVE     = 0,  /* 正常流通 */
    BOOK_DAMAGED    = 1,  /* 损坏     */
    BOOK_LOST       = 2,  /* 遗失     */
    BOOK_CATALOGING = 3   /* 编目中   */
} BookStatus;

/* ================================================================
 * 借阅记录状态枚举
 * ================================================================ */
typedef enum {
    REC_BORROWING = 0,  /* 借阅中 */
    REC_RETURNED  = 1,  /* 已归还 */
    REC_OVERDUE   = 2,  /* 已逾期 */
    REC_RENEWED   = 3   /* 已续借 */
} RecordStatus;

/* ================================================================
 * 日志级别枚举
 * ================================================================ */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
} LogLevel;

/* ================================================================
 * 核心数据结构
 * ================================================================ */

/* --- 图书 (模块2定义, 模块4/5/6使用) --- */
typedef struct {
    int     id;              /* 图书编号（品种级）       */
    char    isbn[20];        /* ISBN                      */
    char    title[100];      /* 书名                      */
    char    author[50];      /* 作者                      */
    char    publisher[50];   /* 出版社                    */
    int     year;            /* 出版年份                  */
    char    category[30];    /* 分类                      */
    char    location[30];    /* 馆藏位置                  */
    float   price;           /* 单价                      */
    int     total_copies;    /* 总副本数                  */
    int     available;       /* 可借副本数                */
    int     borrowed_count;  /* 累计借阅次数（热度指标） */
    BookStatus status;       /* 品种状态                  */
} Book;

/* --- 读者 (模块1定义, 模块2/3/5使用) --- */
typedef struct {
    int         id;              /* 读者编号（系统生成） */
    char        name[50];        /* 姓名                 */
    char        student_id[20];  /* 学号/工号（唯一标识）*/
    char        phone[15];       /* 手机号               */
    char        email[50];       /* 邮箱                 */
    char        password[20];    /* 密码                 */
    char        department[40];  /* 院系/部门            */
    ReaderType  type;            /* 读者类型             */
    int         max_books;       /* 最大可借数           */
    int         max_days;        /* 最大借阅天数         */
    int         borrowed;        /* 当前已借数           */
    float       fine_balance;    /* 欠费余额             */
    int         is_active;       /* 账户状态             */
    time_t      register_time;   /* 注册时间戳           */
    time_t      expire_time;     /* 借阅证到期时间       */
    int         violation_count; /* 违规次数             */
} Reader;

/* --- 借阅记录节点 (模块2定义, 模块3/4使用) --- */
typedef struct RecordNode {
    int          record_id;     /* 记录编号（唯一）     */
    int          book_id;       /* 图书编号             */
    int          reader_id;     /* 读者编号             */
    int          copy_id;       /* 副本编号             */
    time_t       borrow_time;   /* 借阅时间             */
    time_t       due_time;      /* 应还时间             */
    time_t       return_time;   /* 实际归还时间（0未还）*/
    int          renew_count;   /* 已续借次数           */
    float        fine_amount;   /* 罚金金额             */
    RecordStatus status;        /* 记录状态             */
    struct RecordNode *next;    /* 链表指针             */
} RecordNode;

/* --- 检索条件 (模块5定义) --- */
typedef struct {
    char    title[100];      /* 书名关键词        */
    char    author[50];      /* 作者关键词        */
    char    isbn[20];        /* ISBN              */
    char    category[30];    /* 分类              */
    char    publisher[50];   /* 出版社            */
    int     year_min;        /* 出版年份下限      */
    int     year_max;        /* 出版年份上限      */
    float   price_min;       /* 价格下限          */
    float   price_max;       /* 价格上限          */
    int     sort_by;         /* 排序依据(0-4)    */
    int     sort_asc;        /* 1升序 0降序      */
    int     logic_op;        /* 0=AND 1=OR       */
} SearchCriteria;

/* --- 检索历史 (模块5定义) --- */
typedef struct {
    int     id;             /* 历史编号         */
    int     reader_id;      /* 读者编号         */
    char    query[200];     /* 检索内容         */
    time_t  search_time;    /* 检索时间         */
} SearchHistory;

/* --- 图书详情 (模块5定义) --- */
typedef struct {
    Book    book;           /* 基础图书信息     */
    int     available_copies;  /* 可借副本数   */
    int     total_copies;      /* 总副本数     */
    int     avg_borrow_days;   /* 平均借阅周期 */
    int     reserve_count;     /* 预约人数     */
} BookDetail;

/* --- 系统配置 (模块6定义) --- */
typedef struct {
    int     auto_backup;        /* 自动备份开关          */
    int     backup_interval;    /* 备份间隔（秒）        */
    int     backup_keep_days;   /* 保留天数              */
    int     max_books[4];       /* 各类型最大可借数      */
    int     max_days[4];        /* 各类型借阅天数        */
    float   fine_rate;          /* 罚金费率（元/天）     */
    int     max_renew;          /* 最大续借次数          */
    int     grace_period;       /* 宽限期（天）          */
    int     session_timeout;    /* 会话超时（秒）        */
    char    admin_password[32]; /* 管理员密码            */
    char    library_name[64];   /* 图书馆名称            */
} SystemConfig;

/* --- 备份信息 (模块6定义) --- */
typedef struct {
    char    filename[64];   /* 备份文件名         */
    time_t  timestamp;      /* 备份时间戳         */
    long    size;           /* 文件大小（字节）   */
    int     is_complete;    /* 备份完整性         */
    int     type;           /* 0=自动, 1=手动    */
} BackupInfo;

/* --- 日志条目 (模块6定义) --- */
typedef struct {
    int     log_id;         /* 日志编号  */
    time_t  timestamp;      /* 时间戳    */
    int     level;          /* 日志级别  */
    char    module[20];     /* 模块名称  */
    char    action[30];     /* 操作类型  */
    int     user_id;        /* 操作用户  */
    char    detail[200];    /* 详细内容  */
    char    ip[16];         /* IP地址    */
} LogEntry;

/* --- 系统信息 (模块6定义) --- */
typedef struct {
    int     total_books;       /* 图书总数    */
    int     total_readers;     /* 读者总数    */
    int     total_records;     /* 记录总数    */
    int     active_readers;    /* 活跃读者数  */
    time_t  startup_time;      /* 启动时间    */
    time_t  last_backup;       /* 最后备份    */
    int     backup_ok;         /* 备份状态    */
    char    data_version[16];  /* 数据版本    */
} SystemInfo;

/* ================================================================
 * klib 动态数组与哈希表声明 (全局数据容器)
 * ================================================================ */

/* 图书数组 */
typedef kvec_t(Book)   book_vec_t;

/* 读者数组 */
typedef kvec_t(Reader) reader_vec_t;

/* 日志循环缓冲区 */
typedef kvec_t(LogEntry) log_vec_t;

/* 检索历史 */
typedef kvec_t(SearchHistory) hist_vec_t;

/* 备份索引 */
typedef kvec_t(BackupInfo) backup_vec_t;

/* ================================================================
 * 全局数据实例 (extern 声明)
 * ================================================================ */
extern book_vec_t   g_books;        /* 图书数据   */
extern reader_vec_t g_readers;      /* 读者数据   */
extern RecordNode  *g_records_head; /* 借阅记录链表头 */
extern LogEntry    *g_log_buffer;   /* 日志循环缓冲区 */
extern int          g_log_count;    /* 日志计数   */
extern SystemConfig g_config;       /* 系统配置   */
extern SystemInfo   g_sys_info;     /* 系统信息   */

/* 当前登录用户 */
extern int          g_current_user_id;
extern int          g_is_admin;     /* 1=管理员 0=读者 */

/* ================================================================
 * 工具宏
 * ================================================================ */
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define SAFE_FREE(p)    do { free(p); (p) = NULL; } while(0)
#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#define MIN(a,b)        ((a) < (b) ? (a) : (b))
#define CLAMP(x,lo,hi)  MIN(MAX((x), (lo)), (hi))

/* 文件魔数 */
#define MAGIC_NUMBER    0x42494C00  /* "LIB\0" */
#define BACKUP_MAGIC    0x42434B00  /* "BCK\0" */

#endif /* COMMON_H */
