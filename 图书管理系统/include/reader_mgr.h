/**
 * reader_mgr.h - 模块1：读者分类管理
 *
 * 功能: 读者注册、权限配置、账户状态管理、类型变更、查询统计
 * 数据规模: ≤ 10000 名读者
 */

#ifndef READER_MGR_H
#define READER_MGR_H

#include "common.h"

/* ---------- 权限查询 ---------- */
void get_type_limits(ReaderType type, int *max_books, int *max_days, int *max_renew);

/* ---------- 读者注册 ---------- */
int  reader_register(Reader *r, char *err_msg);
int  reader_batch_import(const char *csv_data, int *success_count, char *report);

/* ---------- 读者查询 ---------- */
const Reader* get_reader_info(int reader_id);
int  reader_search(const char *field, const char *keyword,
                   int *result_ids, int max_count, int *count);

/* ---------- 账户状态 ---------- */
int  update_reader_status(int reader_id, int new_status, const char *reason);
int  check_borrow_eligibility(int reader_id, int *err_code, char *err_msg);

/* ---------- 类型变更 ---------- */
int  change_reader_type(int reader_id, ReaderType new_type, const char *cert_no);

#endif /* READER_MGR_H */
