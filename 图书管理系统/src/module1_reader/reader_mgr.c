/**
 * reader_mgr.c - 模块1：读者分类管理 (骨架)
 *
 * TODO: 组员实现读者注册、权限管理、状态变更等
 */
#include "reader_mgr.h"

void get_type_limits(ReaderType type, int *max_books, int *max_days, int *max_renew) {
    /* TODO: 根据读者类型返回权限配置 */
    (void)type;
    if (max_books) *max_books = 0;
    if (max_days)  *max_days  = 0;
    if (max_renew) *max_renew = 0;
}

int reader_register(Reader *r, char *err_msg) {
    /* TODO: 实现读者注册逻辑 */
    (void)r; (void)err_msg;
    return -1;
}

int reader_batch_import(const char *csv_data, int *success_count, char *report) {
    /* TODO: 实现批量导入 */
    (void)csv_data; (void)success_count; (void)report;
    return -1;
}

const Reader* get_reader_info(int reader_id) {
    /* TODO: 查询读者信息 */
    (void)reader_id;
    return NULL;
}

int reader_search(const char *field, const char *keyword,
                  int *result_ids, int max_count, int *count) {
    /* TODO: 读者多条件检索 */
    (void)field; (void)keyword; (void)result_ids; (void)max_count; (void)count;
    return -1;
}

int update_reader_status(int reader_id, int new_status, const char *reason) {
    /* TODO: 更新读者账户状态 */
    (void)reader_id; (void)new_status; (void)reason;
    return -1;
}

int check_borrow_eligibility(int reader_id, int *err_code, char *err_msg) {
    /* TODO: 校验读者借阅资格 */
    (void)reader_id; (void)err_code; (void)err_msg;
    return -1;
}

int change_reader_type(int reader_id, ReaderType new_type, const char *cert_no) {
    /* TODO: 读者类型变更 */
    (void)reader_id; (void)new_type; (void)cert_no;
    return -1;
}
