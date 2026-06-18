/**
 * overdue_mgr.h - 模块3：逾期罚金管理
 *
 * 功能: 逾期自动检测、罚金计算、催还通知、罚金缴纳、豁免
 */

#ifndef OVERDUE_MGR_H
#define OVERDUE_MGR_H

#include "common.h"

/* ---------- 逾期检测 ---------- */
int  detect_overdue(int reader_id, RecordNode **overdue_list, int *count);
int  detect_all_overdue(RecordNode **overdue_list, int *count);

/* ---------- 罚金 ---------- */
float calc_overdue_fine(time_t due_time, time_t return_time);
int   pay_fine(int reader_id, float amount, float *remaining);

/* ---------- 催还 ---------- */
int  send_reminder(int reader_id, char *messages[], int max_count);

/* ---------- 豁免 ---------- */
int  waive_fine(int reader_id, float amount, const char *reason);

#endif /* OVERDUE_MGR_H */
