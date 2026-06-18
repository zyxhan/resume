#!/bin/bash
# 全自动 Word <-> MD 总管
# - 启动时为所有 .md (有 .origfmt) 启动 _live.docx 实时更新
# - 持续检测新拖入的 .doc/.docx -> 自动转 .md + 启动实时预览
#
# 启动: ./scripts/auto-watch.sh

WORK_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CONVERT_SCRIPT="$WORK_DIR/scripts/docx-convert.sh"
WATCH_PID_DIR="$WORK_DIR/.qoder/watch-pids"
mkdir -p "$WATCH_PID_DIR"

echo "==============================================="
echo "  Auto-Watch: $WORK_DIR"
echo "  Drag .doc/.docx -> auto .md -> live .docx"
echo "==============================================="

# --- Startup: 为所有有 .origfmt 的 .md 启动实时监控 ---
echo "Starting live previews for existing files..."
for MD_FILE in "$WORK_DIR"/*.md; do
    [ -f "$MD_FILE" ] || continue
    BASENAME="$(basename "$MD_FILE")"
    # Skip if no tracking file
    [ -f "${MD_FILE}.origfmt" ] || continue

    DOCX_OUT="${MD_FILE%.md}_live.docx"
    echo "  -> $BASENAME -> ${DOCX_OUT##*/}"

    # Fork a sub-watcher for this file
    (
        pandoc "$MD_FILE" -o "$DOCX_OUT" -f markdown -t docx 2>/dev/null
        LAST_MOD=0
        while true; do
            CM=$(stat -c %Y "$MD_FILE" 2>/dev/null || echo 0)
            if [ "$CM" -gt "$LAST_MOD" ]; then
                sleep 0.3
                pandoc "$MD_FILE" -o "$DOCX_OUT" -f markdown -t docx 2>/dev/null
                LAST_MOD=$CM
            fi
            sleep 1
        done
    ) &
    PID=$!
    echo $PID > "$WATCH_PID_DIR/$BASENAME.pid"
done

echo ""
echo "--- All live .docx ready. Watching for new files... ---"
echo ""

# --- Monitor loop: detect new .doc/.docx ---
declare -A SEEN
for f in "$WORK_DIR"/*.doc "$WORK_DIR"/*.docx; do
    [ -f "$f" ] || continue
    [[ "$(basename "$f")" == ~\$* ]] && continue
    [[ "$(basename "$f")" == *_live.* ]] && continue
    [[ "$(basename "$f")" == *_edited.* ]] && continue
    SEEN["$(basename "$f")"]=1
done

while true; do
    for f in "$WORK_DIR"/*.doc "$WORK_DIR"/*.docx; do
        [ -f "$f" ] || continue
        BNAME="$(basename "$f")"
        [[ "$BNAME" == ~\$* ]] && continue
        [[ "$BNAME" == *_live.* ]] && continue
        [[ "$BNAME" == *_edited.* ]] && continue

        if [ -z "${SEEN[$BNAME]}" ]; then
            echo ""
            echo "========================================="
            echo "[NEW] $BNAME detected!"
            SEEN["$BNAME"]=1

            # Convert to .md
            bash "$CONVERT_SCRIPT" to-md "$f" 2>&1

            # Start live watcher for the new .md
            EXT="${BNAME##*.}"
            BASE="${BNAME%.*}"
            MD_FILE="$WORK_DIR/${BASE}.md"
            DOCX_OUT="$WORK_DIR/${BASE}_live.docx"

            if [ -f "$MD_FILE" ]; then
                (
                    pandoc "$MD_FILE" -o "$DOCX_OUT" -f markdown -t docx 2>/dev/null
                    LAST_MOD=0
                    while true; do
                        CM=$(stat -c %Y "$MD_FILE" 2>/dev/null || echo 0)
                        if [ "$CM" -gt "$LAST_MOD" ]; then
                            sleep 0.3
                            pandoc "$MD_FILE" -o "$DOCX_OUT" -f markdown -t docx 2>/dev/null
                            LAST_MOD=$CM
                        fi
                        sleep 1
                    done
                ) &
                PID=$!
                echo $PID > "$WATCH_PID_DIR/${BASE}.md.pid"
                echo "[AUTO] $BNAME -> ${BASE}.md -> live .docx"
            fi
            echo "========================================="
        fi
    done
    sleep 3
done
