/**
 * search_eng.h - 模块5：智能检索系统
 *
 * 功能: 基础检索、高级检索、分类浏览、排序筛选、检索辅助、结果展示
 * 数据规模: ≤ 20000 种图书
 *
 * 作者: [待填写]
 */

#ifndef SEARCH_ENG_H
#define SEARCH_ENG_H

#include "common.h"

/* ---------- 排序依据枚举 ---------- */
enum SortBy {
    SORT_RELEVANCE = 0,  /* 相关性 */
    SORT_HOTNESS   = 1,  /* 借阅热度 */
    SORT_YEAR      = 2,  /* 出版年份 */
    SORT_PRICE     = 3,  /* 价格 */
    SORT_TITLE     = 4   /* 书名字典序 */
};

/* ---------- 基础检索 ---------- */
int  search_books(const char *field, const char *keyword,
                  Book *results, int max_results, int *count);

/* ---------- 高级检索 ---------- */
int  advanced_search(const SearchCriteria *criteria,
                     Book *results, int max_results, int *count);

/* ---------- 分类浏览 ---------- */
int  browse_category(const char *category,
                     Book *results, int max_results, int *count);

/* ---------- 图书详情 ---------- */
int  get_book_detail(int book_id, BookDetail *detail);

/* ---------- 排序 ---------- */
void sort_results(Book *results, int count, int sort_by, int ascending);

/* ---------- 检索辅助 ---------- */
int  get_hot_searches(char *keywords[], int max_count);
int  save_search_history(int reader_id, const char *query);
int  get_search_history(int reader_id, char *history[], int max_count);

/* ---------- 模糊匹配 ---------- */
int  fuzzy_match(const char *text, const char *keyword);
int  fuzzy_match_ci(const char *text, const char *keyword);
int  wildcard_match(const char *text, const char *pattern);

#endif /* SEARCH_ENG_H */
