/**
 * main.c - 高校图书馆智能借阅管理系统 入口
 *
 * 定义全局数据实例，调用 menu_cli 菜单主循环。
 *
 * 作者: [团队名称]
 * 日期: 2026-06-06
 */

#include "common.h"

/* ================================================================
 * 全局数据实例定义
 * ================================================================ */

/* 图书动态数组 */
book_vec_t g_books;

/* 读者动态数组 */
reader_vec_t g_readers;

/* 借阅记录链表头 */
RecordNode *g_records_head = NULL;

/* 日志缓冲区 */
LogEntry  *g_log_buffer = NULL;
int        g_log_count  = 0;

/* 系统配置 */
SystemConfig g_config;

/* 系统信息 */
SystemInfo g_sys_info;

/* 当前登录用户 */
int g_current_user_id = 0;
int g_is_admin        = 0;

/* ================================================================
 * 菜单主循环 (定义在 menu_cli.c)
 * ================================================================ */
extern int run_menu_loop(void);

/* ================================================================
 * 程序入口
 * ================================================================ */
int main(void) {
    /* 初始化 klb 动态数组 */
    kv_init(g_books);
    kv_init(g_readers);

    /* 初始化默认配置 */
    reset_config_to_default();

    /* 运行菜单主循环 */
    return run_menu_loop();
}
