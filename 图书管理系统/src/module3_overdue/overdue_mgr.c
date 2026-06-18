/**
 * overdue_mgr.c - 模块3：逾期罚金管理 (骨架)
 *
 * TODO: 组员实现逾期检测、罚金计算、催还、缴纳、豁免
 */
#include "overdue_mgr.h"

int detect_overdue(int reader_id, RecordNode **overdue_list, int *count) {
    /* TODO: 检测指定读者逾期记录 */
    (void)reader_id; (void)overdue_list; (void)count;
    return -1;
}

int detect_all_overdue(RecordNode **overdue_list, int *count) {
    /* TODO: 检测全部逾期记录 */
    (void)overdue_list; (void)count;
    return -1;
}

float calc_overdue_fine(time_t due_time, time_t return_time) {
    /* TODO: 计算逾期罚金 */
    (void)due_time; (void)return_time;
    return 0.0f;
}

int pay_fine(int reader_id, float amount, float *remaining) {
    /* TODO: 缴纳罚金 */
    (void)reader_id; (void)amount; (void)remaining;
    return -1;
}

int send_reminder(int reader_id, char *messages[], int max_count) {
    /* TODO: 发送催还通知 */
    (void)reader_id; (void)messages; (void)max_count;
    return -1;
}

int waive_fine(int reader_id, float amount, const char *reason) {
    /* TODO: 豁免罚金 */
    (void)reader_id; (void)amount; (void)reason;
    return -1;
}
