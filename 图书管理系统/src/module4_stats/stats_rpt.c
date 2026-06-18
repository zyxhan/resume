/**
 * stats_rpt.c - 模块4：统计报表展示 (骨架)
 *
 * TODO: 组员实现排行榜、月度/年度报表、专项分析
 */
#include "stats_rpt.h"

int get_top_books(int top_n, Book *results, int *count) {
    /* TODO: 图书借阅排行榜 */
    (void)top_n; (void)results; (void)count;
    return -1;
}

int get_top_readers(int top_n, int *reader_ids, int *borrow_counts) {
    /* TODO: 读者活跃度排行榜 */
    (void)top_n; (void)reader_ids; (void)borrow_counts;
    return -1;
}

int generate_monthly_report(int year, int month, char *report, int max_len) {
    /* TODO: 生成月报 */
    (void)year; (void)month; (void)report; (void)max_len;
    return -1;
}

int generate_annual_report(int year, char *report, int max_len) {
    /* TODO: 生成年报 */
    (void)year; (void)report; (void)max_len;
    return -1;
}

int analyze_reader_type(ReaderType type, char *report, int max_len) {
    /* TODO: 读者类型专项分析 */
    (void)type; (void)report; (void)max_len;
    return -1;
}

int analyze_category(const char *cat, char *report, int max_len) {
    /* TODO: 分类专项分析 */
    (void)cat; (void)report; (void)max_len;
    return -1;
}
