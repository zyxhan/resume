/**
 * stats_rpt.h - 模块4：统计报表展示
 *
 * 功能: 借阅排行榜、月度/年度报表、读者画像、专项分析
 */

#ifndef STATS_RPT_H
#define STATS_RPT_H

#include "common.h"

/* ---------- 排行榜 ---------- */
int  get_top_books(int top_n, Book *results, int *count);
int  get_top_readers(int top_n, int *reader_ids, int *borrow_counts);

/* ---------- 报表 ---------- */
int  generate_monthly_report(int year, int month, char *report, int max_len);
int  generate_annual_report(int year, char *report, int max_len);

/* ---------- 专项分析 ---------- */
int  analyze_reader_type(ReaderType type, char *report, int max_len);
int  analyze_category(const char *cat, char *report, int max_len);

#endif /* STATS_RPT_H */
