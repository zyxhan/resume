/**
 * borrow_mgr.h - 模块2：图书借还管理
 *
 * 功能: 借阅、归还、续借、多副本库存管理、历史记录
 * 数据规模: 图书≤20000, 记录≤50000
 */

#ifndef BORROW_MGR_H
#define BORROW_MGR_H

#include "common.h"

/* ---------- 借阅 ---------- */
int  borrow_book(int reader_id, int book_id, int *record_id, char *err_msg);

/* ---------- 归还 ---------- */
int  return_book(int record_id, float *fine_amount, char *err_msg);
int  batch_return(int record_ids[], int count, float *total_fine, char *report);

/* ---------- 续借 ---------- */
int  renew_book(int record_id, int *new_due_days, char *err_msg);

/* ---------- 库存 ---------- */
int  check_book_available(int book_id, int *available_count);

/* ---------- 历史 ---------- */
int  get_reader_borrowed(int reader_id, int *count, int record_ids[]);
int  get_book_history(int book_id, RecordNode *results, int max_count, int *count);

/* ---------- 罚金计算 ---------- */
float calculate_fine(time_t due_time, time_t return_time);

#endif /* BORROW_MGR_H */
