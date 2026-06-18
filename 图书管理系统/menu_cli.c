/**
 * menu_cli.c - 高校图书馆智能借阅管理系统  CLI 界面
 *
 * 负责全部菜单界面的绘制与流转，采用ANSI控制码实现彩色绚丽效果。
 * 各模块子菜单调用对应模块接口（当前为骨架桩），模块实现另行填充。
 *
 * 编译: gcc -Iinclude -Iklib -o libsys menu_cli.c main.c \
 *        src/module*/**.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 模块头文件 */
#include "common.h"
#include "reader_mgr.h"
#include "borrow_mgr.h"
#include "overdue_mgr.h"
#include "stats_rpt.h"
#include "search_eng.h"
#include "data_persist.h"

/* ================================================================
 * ANSI 色彩与样式宏
 * ================================================================ */
#define CLR_RESET     "\033[0m"
#define CLR_BOLD      "\033[1m"
#define CLR_DIM       "\033[2m"
#define CLR_UNDERLINE "\033[4m"
#define CLR_BLINK     "\033[5m"

/* 前景色 */
#define CLR_BLACK   "\033[30m"
#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_MAGENTA "\033[35m"
#define CLR_CYAN    "\033[36m"
#define CLR_WHITE   "\033[37m"

/* 高亮前景色 */
#define CLR_H_BLACK   "\033[90m"
#define CLR_H_RED     "\033[91m"
#define CLR_H_GREEN   "\033[92m"
#define CLR_H_YELLOW  "\033[93m"
#define CLR_H_BLUE    "\033[94m"
#define CLR_H_MAGENTA "\033[95m"
#define CLR_H_CYAN    "\033[96m"
#define CLR_H_WHITE   "\033[97m"

/* 背景色 */
#define CLR_BG_BLUE   "\033[44m"
#define CLR_BG_CYAN   "\033[46m"
#define CLR_BG_GREEN  "\033[42m"
#define CLR_BG_RED    "\033[41m"
#define CLR_BG_YELLOW "\033[43m"

/* 组合样式 */
#define TITLE_STYLE   CLR_BOLD CLR_H_CYAN
#define HEADER_STYLE  CLR_BOLD CLR_H_WHITE CLR_BG_BLUE
#define SUCCESS_STYLE CLR_H_GREEN
#define WARN_STYLE    CLR_H_YELLOW
#define ERROR_STYLE   CLR_H_RED
#define INFO_STYLE    CLR_CYAN
#define MENU_STYLE    CLR_H_CYAN
#define HOTKEY_STYLE  CLR_BOLD CLR_H_YELLOW
#define DIM_STYLE     CLR_DIM CLR_H_BLACK

/* ================================================================
 * 界面宽度常量
 * ================================================================ */
#define BOX_WIDTH    62

/* ================================================================
 * 辅助绘图函数
 * ================================================================ */

/* 清屏 */
static void clear_screen(void) {
    printf("\033[2J\033[H");
}

/* 暂停等待回车 */
static void press_enter(void) {
    printf("\n" DIM_STYLE "  按 Enter 键继续..." CLR_RESET);
    while (getchar() != '\n');
}

/* 清除输入缓冲 */
static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* 绘制顶边 */
static void box_top(const char *color) {
    printf("%s  ╔", color);
    for (int i = 0; i < BOX_WIDTH; i++) printf("═");
    printf("╗\n" CLR_RESET);
}

/* 绘制底边 */
static void box_bottom(const char *color) {
    printf("%s  ╚", color);
    for (int i = 0; i < BOX_WIDTH; i++) printf("═");
    printf("╝\n" CLR_RESET);
}

/* 绘制分隔线 */
static void box_sep(const char *color) {
    printf("%s  ╠", color);
    for (int i = 0; i < BOX_WIDTH; i++) printf("═");
    printf("╣\n" CLR_RESET);
}

/* 居中文字行 */
static void box_center(const char *text, const char *color) {
    int len = strlen(text);
    int pad = (BOX_WIDTH - len) / 2;
    printf("%s  ║", color);
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s", text);
    for (int i = 0; i < BOX_WIDTH - pad - len; i++) printf(" ");
    printf("║\n" CLR_RESET);
}

/* 普通文字行（带颜色） */
static void box_line(const char *text, const char *color) {
    int len = strlen(text);
    int visible = 0;
    /* 粗略计算去除ANSI码后的可见长度 */
    int in_esc = 0;
    for (int i = 0; text[i]; i++) {
        if (text[i] == '\033') in_esc = 1;
        if (!in_esc) visible++;
        if (in_esc && text[i] == 'm') in_esc = 0;
    }
    printf("%s  ║  %s", color, text);
    int pad = BOX_WIDTH - 2 - visible;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i++) printf(" ");
    printf("║\n" CLR_RESET);
}

/* 空行 */
static void box_empty(void) {
    printf("  ║");
    for (int i = 0; i < BOX_WIDTH; i++) printf(" ");
    printf("║\n");
}

/* 菜单选项行 */
static void menu_option(int num, const char *desc, const char *icon) {
    printf("  ║  " HOTKEY_STYLE "[%d]" CLR_RESET " %-*s  %s%-26s" CLR_RESET "║\n",
           num, BOX_WIDTH - 10, desc,
           DIM_STYLE, icon ? icon : "");
}

/* 头部横幅 */
static void print_banner(void) {
    clear_screen();
    printf("\n");
    printf(CLR_BOLD CLR_H_CYAN);
    printf("  ╔");
    for (int i = 0; i < BOX_WIDTH; i++) printf("═");
    printf("╗\n");
    box_center("█▀▀█ █▀▀█ █▀▀█ █▀▀▄ █▀▀▄", CLR_BOLD CLR_H_CYAN);
    box_center("█▀▀▄ █▄▄▀ █▄▄▀ █▀▀▄ █▀▀▄", CLR_BOLD CLR_H_CYAN);
    box_center("▀▀▀▀ ▀─▀▀ ▀─▀▀ ▀▀▀─ ▀▀▀─", CLR_BOLD CLR_H_CYAN);
    box_empty();
    box_center("高校图书馆智能借阅管理系统 v1.0", CLR_BOLD CLR_WHITE);
    box_center("Library Intelligent Management System", CLR_DIM CLR_H_CYAN);
    printf(CLR_BOLD CLR_H_CYAN);
    printf("  ╚");
    for (int i = 0; i < BOX_WIDTH; i++) printf("═");
    printf("╝\n" CLR_RESET);
}

/* ================================================================
 * 登录界面
 * ================================================================ */
static int show_login(void) {
    int choice = 0;
    while (1) {
        clear_screen();
        print_banner();
        printf("\n");
        printf(CLR_CYAN);
        box_top(CLR_CYAN);
        box_center("🔐  系 统 登 录", CLR_BOLD CLR_H_CYAN);
        box_sep(CLR_CYAN);
        box_empty();
        menu_option(1, "管理员登录  (Administrator)", "🔧");
        box_empty();
        menu_option(2, "读者登录    (Reader)",          "👤");
        box_empty();
        box_sep(CLR_CYAN);
        menu_option(0, "退出系统    (Exit)",            "🚪");
        box_bottom(CLR_CYAN);
        printf("\n  " HOTKEY_STYLE "请选择 [0-2]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();

        if (choice == 0) return 0;  /* 退出 */
        if (choice == 1) return 1;  /* 管理员 */
        if (choice == 2) return 2;  /* 读者 */
    }
}

/* ================================================================
 * 模块子菜单 - 桩 (展示菜单结构，调用模块接口占位)
 * ================================================================ */

/* -- 模块1: 读者分类管理 -- */
static void menu_module1_reader(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_H_MAGENTA);
        box_center("👥  模块1：读者分类管理", CLR_BOLD CLR_H_MAGENTA);
        box_sep(CLR_H_MAGENTA);
        box_empty();
        menu_option(1, "注册新读者",        "📝");
        menu_option(2, "批量导入读者",      "📥");
        menu_option(3, "读者列表 / 检索",   "🔍");
        menu_option(4, "冻结 / 解冻账户",  "🔒");
        menu_option(5, "修改读者信息",      "✏️");
        menu_option(6, "类型变更",          "🔄");
        menu_option(7, "统计报表",          "📊");
        box_empty();
        box_sep(CLR_H_MAGENTA);
        printf("  ║  %-60s║\n", DIM_STYLE "  各功能待实现，当前仅展示菜单框架");
        box_sep(CLR_H_MAGENTA);
        menu_option(0, "返回上级菜单",      "↩️");
        box_bottom(CLR_H_MAGENTA);
        printf("\n  " HOTKEY_STYLE "请选择 [0-7]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        /* 调用对应模块接口（当前为桩） */
        printf("\n  " WARN_STYLE "⚠ 模块1功能尚未实现，返回菜单..." CLR_RESET);
        press_enter();
    }
}

/* -- 模块2: 图书借还管理 -- */
static void menu_module2_borrow(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_GREEN);
        box_center("📖  模块2：图书借还管理", CLR_BOLD CLR_H_GREEN);
        box_sep(CLR_GREEN);
        box_empty();
        menu_option(1, "借阅图书",      "📖");
        menu_option(2, "归还图书",      "🔄");
        menu_option(3, "续借图书",      "🔁");
        menu_option(4, "批量归还",      "📚");
        menu_option(5, "库存查询",      "📋");
        menu_option(6, "借阅历史",      "📜");
        box_empty();
        box_sep(CLR_GREEN);
        printf("  ║  %-60s║\n", DIM_STYLE "  各功能待实现，当前仅展示菜单框架");
        box_sep(CLR_GREEN);
        menu_option(0, "返回上级菜单",  "↩️");
        box_bottom(CLR_GREEN);
        printf("\n  " HOTKEY_STYLE "请选择 [0-6]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        printf("\n  " WARN_STYLE "⚠ 模块2功能尚未实现，返回菜单..." CLR_RESET);
        press_enter();
    }
}

/* -- 模块3: 逾期罚金管理 -- */
static void menu_module3_overdue(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_RED);
        box_center("💰  模块3：逾期罚金管理", CLR_BOLD CLR_H_RED);
        box_sep(CLR_RED);
        box_empty();
        menu_option(1, "逾期检测",        "🔎");
        menu_option(2, "罚金计算",        "🧮");
        menu_option(3, "缴纳罚金",        "💳");
        menu_option(4, "催还通知",        "📢");
        menu_option(5, "罚金豁免",        "🎁");
        box_empty();
        box_sep(CLR_RED);
        printf("  ║  %-60s║\n", DIM_STYLE "  各功能待实现，当前仅展示菜单框架");
        box_sep(CLR_RED);
        menu_option(0, "返回上级菜单",    "↩️");
        box_bottom(CLR_RED);
        printf("\n  " HOTKEY_STYLE "请选择 [0-5]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        printf("\n  " WARN_STYLE "⚠ 模块3功能尚未实现，返回菜单..." CLR_RESET);
        press_enter();
    }
}

/* -- 模块4: 统计报表展示 -- */
static void menu_module4_stats(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_BLUE);
        box_center("📊  模块4：统计报表展示", CLR_BOLD CLR_H_BLUE);
        box_sep(CLR_BLUE);
        box_empty();
        menu_option(1, "图书借阅排行榜",   "🏆");
        menu_option(2, "读者活跃度排行",   "🌟");
        menu_option(3, "月度报表",         "📅");
        menu_option(4, "年度报表",         "📆");
        menu_option(5, "读者类型分析",     "👥");
        menu_option(6, "分类专项分析",     "📂");
        box_empty();
        box_sep(CLR_BLUE);
        printf("  ║  %-60s║\n", DIM_STYLE "  各功能待实现，当前仅展示菜单框架");
        box_sep(CLR_BLUE);
        menu_option(0, "返回上级菜单",     "↩️");
        box_bottom(CLR_BLUE);
        printf("\n  " HOTKEY_STYLE "请选择 [0-6]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        printf("\n  " WARN_STYLE "⚠ 模块4功能尚未实现，返回菜单..." CLR_RESET);
        press_enter();
    }
}

/* ================================================================
 * 模块5辅助 - 结果展示
 * ================================================================ */

/* 展示检索结果列表 */
static void show_result_list(Book *results, int total) {
    if (total == 0) {
        box_empty();
        box_center("( 无匹配结果 )", CLR_DIM);
        return;
    }

    /* 表头 */
    char hdr[128];
    snprintf(hdr, sizeof(hdr),
             "共找到 " CLR_BOLD CLR_H_YELLOW "%d" CLR_RESET " 本图书", total);
    box_line(hdr, "");
    box_empty();

    /* 列标题 */
    printf("  ║  %-4s  %-20s  %-12s  %-6s  %-6s  ║\n",
           CLR_DIM "编号" CLR_RESET,
           CLR_DIM "书名" CLR_RESET,
           CLR_DIM "作者" CLR_RESET,
           CLR_DIM "年份" CLR_RESET,
           CLR_DIM "热度" CLR_RESET);

    for (int i = 0; i < total; i++) {
        char line[128];
        /* 截断书名到18字符 */
        char t_short[22];
        snprintf(t_short, sizeof(t_short), "%.18s%s",
                 results[i].title,
                 strlen(results[i].title) > 18 ? ".." : "");

        snprintf(line, sizeof(line),
                 "  %-4d  %-20s  %-12s  %-6d  %-6d  ",
                 results[i].id, t_short,
                 results[i].author, results[i].year,
                 results[i].borrowed_count);
        printf("  ║ %s ║\n", line);
    }
}

/* 展示单本图书详情 */
static void show_book_detail(int book_id) {
    BookDetail d;
    if (get_book_detail(book_id, &d) != 0) {
        box_line("❌ 图书不存在", CLR_H_RED);
        return;
    }
    Book *b = &d.book;

    box_sep(CLR_H_CYAN);
    char buf[128];
    snprintf(buf, sizeof(buf), "📖  《%s》", b->title);
    box_center(buf, CLR_BOLD CLR_H_YELLOW);
    box_sep(CLR_H_CYAN);

    snprintf(buf, sizeof(buf), "作者: %s", b->author);         box_line(buf, "");
    snprintf(buf, sizeof(buf), "出版社: %s", b->publisher);    box_line(buf, "");
    snprintf(buf, sizeof(buf), "出版年份: %d", b->year);       box_line(buf, "");
    snprintf(buf, sizeof(buf), "ISBN: %s", b->isbn);           box_line(buf, "");
    snprintf(buf, sizeof(buf), "分类: %s", b->category);       box_line(buf, "");
    snprintf(buf, sizeof(buf), "馆藏位置: %s", b->location);   box_line(buf, "");
    snprintf(buf, sizeof(buf), "价格: %.2f元", b->price);      box_line(buf, "");
    box_empty();
    snprintf(buf, sizeof(buf), "📚 馆藏: %s%d/%d 本可借%s  🔥 累计借阅: %d 次",
             CLR_H_GREEN, d.available_copies, d.total_copies, CLR_RESET,
             b->borrowed_count);
    box_line(buf, "");
}

/* -- 模块5: 智能检索系统 (已实现基础功能) -- */
static void menu_module5_search(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_H_CYAN);
        box_center("🔍  模块5：智能检索系统", CLR_BOLD CLR_H_CYAN);
        box_sep(CLR_H_CYAN);
        box_empty();
        menu_option(1, "书名检索",                      "📖");
        menu_option(2, "作者检索",                      "✍️");
        menu_option(3, "分类浏览",                      "🌳");
        menu_option(4, "高级检索 (多字段组合)",          "⚙️");
        menu_option(5, "热门搜索",                      "🔥");
        menu_option(6, "查看图书详情",                  "📋");
        box_empty();
        box_sep(CLR_H_CYAN);
        menu_option(0, "返回上级菜单",                  "↩️");
        box_bottom(CLR_H_CYAN);
        printf("\n  " HOTKEY_STYLE "请选择 [0-6]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();

        switch (choice) {
        case 1: case 2: {
            /* 书名/作者检索 */
            clear_screen();
            char keyword[128], field[10];
            strcpy(field, (choice == 1) ? "title" : "author");
            printf("\n");
            box_top(CLR_H_CYAN);
            box_center((choice == 1) ? "🔍  书名检索" : "🔍  作者检索", CLR_BOLD CLR_H_CYAN);
            box_sep(CLR_H_CYAN);
            box_empty();
            printf("  ║  输入关键词: ");
            flush_stdin();
            fgets(keyword, sizeof(keyword), stdin);
            keyword[strcspn(keyword, "\n")] = '\0';
            box_empty();
            box_sep(CLR_H_CYAN);

            Book results[200];
            int  cnt = 0;
            search_books(field, keyword, results, 200, &cnt);
            sort_results(results, cnt, SORT_HOTNESS, 0); /* 默认按热度降序 */
            show_result_list(results, cnt);
            box_bottom(CLR_H_CYAN);
            press_enter();
            break;
        }
        case 3: {
            /* 分类浏览 */
            clear_screen();
            char cat[50];
            printf("\n");
            box_top(CLR_H_CYAN);
            box_center("🌳  分类浏览", CLR_BOLD CLR_H_CYAN);
            box_sep(CLR_H_CYAN);
            box_line("可选分类: 计算机 | 文学 | 数学 | 历史", DIM_STYLE);
            box_empty();
            printf("  ║  输入分类: ");
            flush_stdin();
            fgets(cat, sizeof(cat), stdin);
            cat[strcspn(cat, "\n")] = '\0';
            box_empty();
            box_sep(CLR_H_CYAN);

            Book results[200]; int cnt = 0;
            browse_category(cat, results, 200, &cnt);
            sort_results(results, cnt, SORT_HOTNESS, 0);
            show_result_list(results, cnt);
            box_bottom(CLR_H_CYAN);
            press_enter();
            break;
        }
        case 4: {
            /* 高级检索 */
            clear_screen();
            printf("\n");
            box_top(CLR_H_CYAN);
            box_center("⚙️  高级检索", CLR_BOLD CLR_H_CYAN);
            box_sep(CLR_H_CYAN);

            SearchCriteria c;
            memset(&c, 0, sizeof(c));
            c.sort_by = SORT_HOTNESS;
            c.sort_asc = 0;
            c.year_min = 0; c.year_max = 0;
            c.price_min = 0; c.price_max = 0;
            c.logic_op = 0; /* AND */

            flush_stdin();
            printf("  ║  书名 (回车跳过): ");  fgets(c.title, sizeof(c.title), stdin);
            c.title[strcspn(c.title, "\n")] = '\0';
            printf("  ║  作者 (回车跳过): ");  fgets(c.author, sizeof(c.author), stdin);
            c.author[strcspn(c.author, "\n")] = '\0';
            printf("  ║  分类 (回车跳过): ");  fgets(c.category, sizeof(c.category), stdin);
            c.category[strcspn(c.category, "\n")] = '\0';
            printf("  ║  出版社 (回车跳过): "); fgets(c.publisher, sizeof(c.publisher), stdin);
            c.publisher[strcspn(c.publisher, "\n")] = '\0';
            printf("  ║  年份范围 (如 2020-2022, 回车跳过): ");
            char yb[32]; fgets(yb, sizeof(yb), stdin);
            if (strlen(yb) > 1) sscanf(yb, "%d-%d", &c.year_min, &c.year_max);
            printf("  ║  排序: [0]相关性 [1]热度 [2]年份 [3]价格 [4]书名: ");
            scanf("%d", &c.sort_by); flush_stdin();
            printf("  ║  顺序: [0]降序 [1]升序: ");
            scanf("%d", &c.sort_asc); flush_stdin();
            box_empty();
            box_sep(CLR_H_CYAN);

            Book results[200]; int cnt = 0;
            advanced_search(&c, results, 200, &cnt);
            show_result_list(results, cnt);
            box_bottom(CLR_H_CYAN);
            press_enter();
            break;
        }
        case 5: {
            /* 热门搜索 */
            clear_screen();
            printf("\n");
            box_top(CLR_H_CYAN);
            box_center("🔥  热门搜索", CLR_BOLD CLR_H_CYAN);
            box_sep(CLR_H_CYAN);
            char *kw[20];
            int n = get_hot_searches(kw, 20);
            for (int i = 0; i < n; i++) {
                char line[80];
                snprintf(line, sizeof(line), "  %2d.  %s", i + 1, kw[i]);
                box_line(line, "");
            }
            box_bottom(CLR_H_CYAN);
            press_enter();
            break;
        }
        case 6: {
            /* 查看图书详情 */
            clear_screen();
            int bid;
            printf("\n");
            box_top(CLR_H_CYAN);
            box_center("📋  查看图书详情", CLR_BOLD CLR_H_CYAN);
            box_empty();
            printf("  ║  输入图书编号: ");
            flush_stdin();
            scanf("%d", &bid); flush_stdin();
            box_empty();
            show_book_detail(bid);
            box_bottom(CLR_H_CYAN);
            press_enter();
            break;
        }
        case 0: return;
        default: break;
        }
    }
}

/* -- 模块6: 数据持久化与系统管理 -- */
static void menu_module6_persist(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_H_YELLOW);
        box_center("⚙️  模块6：数据持久化与系统管理", CLR_BOLD CLR_H_YELLOW);
        box_sep(CLR_H_YELLOW);
        box_empty();
        menu_option(1, "数据备份管理",         "💾");
        menu_option(2, "数据恢复管理",         "📥");
        menu_option(3, "系统参数配置",         "🔧");
        menu_option(4, "日志查看与管理",       "📋");
        menu_option(5, "数据完整性检查",       "✅");
        menu_option(6, "系统信息查看",         "ℹ️");
        menu_option(7, "加载演示数据",         "🎲");
        box_empty();
        box_sep(CLR_H_YELLOW);
        printf("  ║  %-60s║\n", DIM_STYLE "  各功能待实现，当前仅展示菜单框架");
        box_sep(CLR_H_YELLOW);
        menu_option(0, "返回上级菜单",         "↩️");
        box_bottom(CLR_H_YELLOW);
        printf("\n  " HOTKEY_STYLE "请选择 [0-7]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        printf("\n  " WARN_STYLE "⚠ 模块6功能尚未实现，返回菜单..." CLR_RESET);
        press_enter();
    }
}

/* ================================================================
 * 主菜单
 * ================================================================ */

/* 管理员主菜单 */
static void menu_admin(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_H_BLUE);
        box_center("🛡️  管 理 员 控 制 台", CLR_BOLD CLR_H_WHITE CLR_BG_BLUE);
        box_sep(CLR_H_BLUE);
        box_empty();
        menu_option(1, "读者分类管理  (Module 1)",   "👥 P0");
        box_empty();
        menu_option(2, "图书借还管理  (Module 2)",   "📖 P0");
        box_empty();
        menu_option(3, "逾期罚金管理  (Module 3)",   "💰 P0");
        box_empty();
        menu_option(4, "统计报表展示  (Module 4)",   "📊 P1");
        box_empty();
        menu_option(5, "智能检索系统  (Module 5)",   "🔍 P1");
        box_empty();
        menu_option(6, "数据持久化    (Module 6)",   "💾 P0");
        box_empty();
        box_sep(CLR_H_BLUE);
        menu_option(9, "查看系统信息",               "ℹ️");
        box_empty();
        menu_option(0, "注销退出",                   "🚪");
        box_bottom(CLR_H_BLUE);
        printf("\n  " HOTKEY_STYLE "请选择 [0-6 / 9]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();

        switch (choice) {
            case 1: menu_module1_reader();  break;
            case 2: menu_module2_borrow();  break;
            case 3: menu_module3_overdue(); break;
            case 4: menu_module4_stats();   break;
            case 5: menu_module5_search();  break;
            case 6: menu_module6_persist(); break;
            case 9: {
                clear_screen();
                box_top(CLR_CYAN);
                box_center("ℹ️  系统信息", CLR_BOLD CLR_CYAN);
                box_sep(CLR_CYAN);
                box_line("图书馆: XX大学图书馆  |  版本: 1.0.0", "");
                box_line("图书总量: 0     读者总量: 0     记录总量: 0", "");
                box_line("运行状态: 🟢 正常  |  数据目录: ./data/", "");
                box_sep(CLR_CYAN);
                box_line("模块1(读者) 模块2(借还) 模块3(逾期) -- 待实现", DIM_STYLE);
                box_line("模块4(统计) 模块5(检索) 模块6(持久化) -- 待实现", DIM_STYLE);
                box_bottom(CLR_CYAN);
                press_enter();
                break;
            }
            case 0: return;
            default: break;
        }
    }
}

/* 读者主菜单 */
static void menu_reader(void) {
    int choice;
    while (1) {
        clear_screen();
        printf("\n");
        box_top(CLR_H_GREEN);
        box_center("👤  读 者 服 务 中 心", CLR_BOLD CLR_H_WHITE CLR_BG_GREEN);
        box_sep(CLR_H_GREEN);
        box_empty();
        menu_option(1, "🔍  图书检索",    "搜索馆藏图书");
        menu_option(2, "📖  我的借阅",    "查看当前借阅与历史");
        menu_option(3, "👤  个人信息",    "查看与修改个人资料");
        menu_option(4, "🔥  热门推荐",    "看看大家都在读什么");
        box_empty();
        box_sep(CLR_H_GREEN);
        menu_option(0, "注销退出",        "");
        box_bottom(CLR_H_GREEN);
        printf("\n  " HOTKEY_STYLE "请选择 [0-4]: " CLR_RESET);

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();
        if (choice == 0) return;

        /* 读者端功能：跳转到对应模块子菜单 */
        switch (choice) {
            case 1: menu_module5_search(); break;  /* 检索 */
            case 2:
            case 3:
            case 4:
                printf("\n  " WARN_STYLE "⚠ 该功能尚未实现，返回菜单..." CLR_RESET);
                press_enter();
                break;
            default: break;
        }
    }
}

/* ================================================================
 * 程序主入口 (由 menu_cli.c 提供入口逻辑)
 *   实际 main() 在 main.c 中调用此函数
 * ================================================================ */
int run_menu_loop(void) {
    int role;
    init_system();

    while (1) {
        role = show_login();
        if (role == 0) {
            /* 退出系统 */
            clear_screen();
            printf("\n");
            box_top(CLR_H_GREEN);
            box_center("✅  感谢使用高校图书馆智能借阅管理系统", CLR_BOLD CLR_H_GREEN);
            box_center("See you next time!  (´-ω-`)ノ", CLR_DIM);
            box_bottom(CLR_H_GREEN);
            printf("\n");
            return 0;
        }
        if (role == 1) {
            menu_admin();
        } else {
            menu_reader();
        }
    }
}
