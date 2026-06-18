/**
 * data_persist.h - 模块6：数据持久化与系统管理
 *
 * 功能: 序列化/反序列化、备份/恢复、系统配置、日志管理
 */

#ifndef DATA_PERSIST_H
#define DATA_PERSIST_H

#include "common.h"

/* ---------- 数据序列化 ---------- */
int  save_all_data(const char *dir_path);
int  load_all_data(const char *dir_path);
int  verify_data_integrity(int *error_count);

/* ---------- 备份管理 ---------- */
int  create_backup(const char *type, char *filename, int size);
int  list_backups(BackupInfo *list, int max_count, int *count);
int  restore_backup(const char *filename);
int  delete_backup(const char *filename);

/* ---------- 配置管理 ---------- */
int  load_config(const char *config_path);
int  save_config(const char *config_path);
int  reset_config_to_default(void);

/* ---------- 日志 ---------- */
int  log_write(int level, const char *module, const char *action,
               int user_id, const char *detail);
int  log_query(int level, const char *module, time_t start, time_t end,
               LogEntry *results, int max_count, int *count);

/* ---------- 系统信息 ---------- */
int  get_system_info(SystemInfo *info);
void init_system(void);

/* ---------- 演示数据 ---------- */
int  load_demo_data(void);

#endif /* DATA_PERSIST_H */
