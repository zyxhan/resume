# -*- coding: utf-8 -*-
"""通过 Microsoft Word COM 实现 .doc <-> .docx 双向保真转换"""
import sys
import os
import win32com.client

# Word SaveAs format constants
FORMAT_DOC = 0       # .doc (Word 97-2003)
FORMAT_DOCX = 12     # .docx (Word 2007+)

def convert(input_path, output_path, target_format):
    """使用 Word COM 转换文档格式，完美保留中文和排版。"""
    abs_input = os.path.abspath(input_path)
    abs_output = os.path.abspath(output_path)

    if not os.path.exists(abs_input):
        print(f"[ERROR] Input not found: {abs_input}")
        return False

    word = None
    try:
        word = win32com.client.Dispatch("Word.Application")
        word.Visible = False
        word.DisplayAlerts = False

        print(f"  Opening: {os.path.basename(abs_input)}")
        doc = word.Documents.Open(abs_input, ReadOnly=True)

        if os.path.exists(abs_output):
            os.remove(abs_output)

        print(f"  Saving as: {os.path.basename(abs_output)}")
        doc.SaveAs(abs_output, FileFormat=target_format)
        doc.Close()
        return True

    except Exception as e:
        print(f"[ERROR] Word COM failed: {e}")
        return False

    finally:
        if word:
            try:
                word.Quit()
            except:
                pass

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python word-com-helper.py <input> <output> <format>")
        print("  format: 0=doc, 12=docx")
        print("  e.g.:   python word-com-helper.py a.doc a.docx 12")
        print("  e.g.:   python word-com-helper.py a.docx a.doc 0")
        sys.exit(1)

    fmt = int(sys.argv[3])
    ok = convert(sys.argv[1], sys.argv[2], fmt)
    sys.exit(0 if ok else 1)
