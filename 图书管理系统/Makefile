# ================================================================
# Makefile - 高校图书馆智能借阅管理系统
#
# 编译: make
# 清理: make clean
# 运行: make run
# ================================================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g
INC     := -Iinclude -Iklib
LDFLAGS := -lm

# 源文件
SRCS := main.c \
        menu_cli.c \
        src/module1_reader/reader_mgr.c \
        src/module2_borrow/borrow_mgr.c \
        src/module3_overdue/overdue_mgr.c \
        src/module4_stats/stats_rpt.c \
        src/module5_search/search_eng.c \
        src/module6_persist/data_persist.c

OBJS := $(SRCS:.c=.o)
TARGET := libsys

# ================================================================
# 默认目标
# ================================================================
.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "  🔗 链接生成 $(TARGET)..."
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)
	@echo "  ✅ 编译成功! ./$(TARGET) 运行"

# 编译规则
%.o: %.c
	@echo "  🔨 编译 $< ..."
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# ================================================================
# 辅助目标
# ================================================================
clean:
	@echo "  🧹 清理编译产物..."
	rm -f $(OBJS) $(TARGET)
	@echo "  ✅ 清理完成"

run: all
	@echo "  🚀 启动系统...\n"
	./$(TARGET)

help:
	@echo "高校图书馆智能借阅管理系统 - 构建帮助"
	@echo "========================================"
	@echo "  make        - 编译项目"
	@echo "  make clean  - 清理编译产物"
	@echo "  make run    - 编译并运行"
	@echo "  make help   - 显示此帮助"
