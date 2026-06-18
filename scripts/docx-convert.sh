#!/bin/bash
# Word 文档格式转换脚本 -- 全链路保真，完美支持中文
#
# 拖入时:
#   .doc  -> [Word COM] -> .docx (保真) -> [Pandoc] -> .md (VSCode可编辑)
#   .docx -> [Pandoc] -> .md (VSCode可编辑)
#
# 完成时:
#   .md -> [Pandoc] -> .docx -> [Word COM] -> 原格式 (.doc或.docx)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PY_HELPER="$SCRIPT_DIR/word-com-helper.py"

usage() {
    echo "Usage:"
    echo "  $0 to-md    <file.doc|.docx>  -> Convert to .md for VSCode editing"
    echo "  $0 back     <file.md>         -> Convert back to original format"
    exit 1
}

[ $# -lt 2 ] && usage

ACTION="$1"
INPUT="$2"

case "$ACTION" in
    to-md)
        # Skip WPS temp files
        if [[ "$(basename "$INPUT")" == ~\$* ]]; then
            echo "[SKIP] Temp file: $INPUT"
            exit 0
        fi

        if [[ ! "$INPUT" =~ \.(docx|doc)$ ]]; then
            echo "[ERROR] Input must be .docx or .doc"
            exit 1
        fi
        if [ ! -f "$INPUT" ]; then
            echo "[ERROR] File not found: $INPUT"
            exit 1
        fi

        EXT="${INPUT##*.}"
        DIR="$(dirname "$INPUT")"
        BASE="$(basename "$INPUT" ".$EXT")"
        MD_FILE="$DIR/${BASE}.md"
        FMT_FILE="${MD_FILE}.origfmt"

        # Step 1: .doc -> .docx via Word COM
        if [ "$EXT" = "doc" ]; then
            DOCX_FILE="$DIR/${BASE}.docx"
            echo "==> Step 1/2: .doc -> .docx (Word COM)"
            python3 "$PY_HELPER" "$INPUT" "$DOCX_FILE" 12
            echo "doc" > "$FMT_FILE"
        else
            DOCX_FILE="$INPUT"
            echo "docx" > "$FMT_FILE"
        fi

        # Step 2: .docx -> .md via Pandoc
        echo "==> Step 2/2: .docx -> .md (Pandoc)"
        pandoc "$DOCX_FILE" -o "$MD_FILE" -f docx -t markdown

        # If original was .doc, remove intermediate .docx
        if [ "$EXT" = "doc" ]; then
            rm -f "$DOCX_FILE"
        fi

        echo "[OK] Ready to edit: $MD_FILE"
        echo "    Open it in VSCode and we can edit together. Say 'done' when finished."
        ;;

    back)
        if [[ ! "$INPUT" =~ \.md$ ]]; then
            echo "[ERROR] Input must be .md"
            exit 1
        fi
        if [ ! -f "$INPUT" ]; then
            echo "[ERROR] File not found: $INPUT"
            exit 1
        fi

        FMT_FILE="${INPUT}.origfmt"
        DIR="$(dirname "$INPUT")"
        BASE="$(basename "$INPUT" .md)"

        if [ -f "$FMT_FILE" ]; then
            ORIG_FMT="$(cat "$FMT_FILE")"
        else
            echo "[WARN] No original format record, defaulting to .docx"
            ORIG_FMT="docx"
        fi

        case "$ORIG_FMT" in
            doc)
                # .md -> .docx -> .doc
                TMP_DOCX="$DIR/${BASE}.docx"
                OUTPUT_FILE="$DIR/${BASE}_edited.doc"

                echo "==> Step 1/2: .md -> .docx (Pandoc)"
                pandoc "$INPUT" -o "$TMP_DOCX" -f markdown -t docx

                echo "==> Step 2/2: .docx -> .doc (Word COM)"
                python3 "$PY_HELPER" "$TMP_DOCX" "$OUTPUT_FILE" 0

                rm -f "$TMP_DOCX"
                echo "[OK] Converted back: $OUTPUT_FILE"
                ;;

            docx)
                # .md -> .docx
                OUTPUT_FILE="$DIR/${BASE}_edited.docx"
                echo "==> .md -> .docx (Pandoc)"
                pandoc "$INPUT" -o "$OUTPUT_FILE" -f markdown -t docx
                echo "[OK] Converted back: $OUTPUT_FILE"
                ;;

            *)
                echo "[ERROR] Unknown original format: $ORIG_FMT"
                exit 1
                ;;
        esac

        rm -f "$FMT_FILE"
        echo "    Done!"
        ;;

    *)
        usage
        ;;
esac
