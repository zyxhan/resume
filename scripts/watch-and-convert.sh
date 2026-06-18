#!/bin/bash
# 监视 .md 文件变化，自动转 .docx，实现"改MD→秒见DOCX效果"
# 用法: ./scripts/watch-and-convert.sh <文件.md>

MD_FILE="$1"

if [ ! -f "$MD_FILE" ]; then
    echo "Usage: $0 <file.md>"
    exit 1
fi

MD_PATH="$(realpath "$MD_FILE")"
DIR="$(dirname "$MD_PATH")"
BASE="$(basename "$MD_PATH" .md)"
DOCX_FILE="$DIR/${BASE}_live.docx"

# 首次转换
echo "==> Watching: $MD_FILE"
echo "    Live output: $DOCX_FILE"
echo "    Open this .docx in WPS to see real-time changes: ${BASE}_live.docx"
echo "    Press Ctrl+C to stop watching"
echo ""

pandoc "$MD_PATH" -o "$DOCX_FILE" -f markdown -t docx 2>/dev/null
echo "[$(date +%H:%M:%S)] Converted (initial)"

LAST_MOD=0

while true; do
    CURRENT_MOD=$(stat -c %Y "$MD_PATH" 2>/dev/null || echo 0)
    if [ "$CURRENT_MOD" -gt "$LAST_MOD" ]; then
        sleep 0.3  # debounce
        pandoc "$MD_PATH" -o "$DOCX_FILE" -f markdown -t docx 2>/dev/null
        echo "[$(date +%H:%M:%S)] Updated: $DOCX_FILE"
        LAST_MOD="$CURRENT_MOD"
    fi
    sleep 1
done
