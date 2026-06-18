# 项目规则

## Word 文档自动工作流

> **全局自动化已启用** — SessionStart hook 在任意目录打开 Claude 时自动启动 `auto-watch.sh $(pwd)`
> 用户只需拖入文件 → 编辑 .md → 说"完成"。其余全自动。

### 检测到新文件时
自动执行 `scripts/docx-convert.sh to-md <文件>` 转为 `.md`，并启动 `_live.docx` 实时更新。

### 用户说"完成"时
对当前打开的 `.md` 或全部 `.md` 执行 `scripts/docx-convert.sh back <.md文件>`
- 原 `.doc` → 输出 `_edited.doc`
- 原 `.docx` → 输出 `_edited.docx`

### 实时预览
- VSCode 编辑 `.md` → 1秒内自动更新 `_live.docx`
- 用户用 **WPS** 打开 `_live.docx` 即可实时看到排版效果
- 改完说"完成"，转回原格式

### 文件说明
- `_live.docx` — 实时预览用的临时文件
- `_edited.doc` / `_edited.docx` — 最终输出，不覆盖原文件
- `.md.origfmt` — 记录原格式（doc 或 docx），用于转回
