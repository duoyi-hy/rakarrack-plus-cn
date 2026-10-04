#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Apply Chinese translations to rakarrack-plus sources (.fl / .cxx / .C / .h / .cpp)."""
import re, os, sys, glob

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
from zh_dict import DICT  # noqa

# ---- varios.C 专用：按源代码中实际出现的字符串片段（含 \n 转义、分段拼接）----
VARIOS_RAW = {
    "Can not load this Bank file because it is from an old rakarrack version,":
        "无法载入此音色库文件，它来自旧版 rakarrack，",
    "\\n please use 'Convert Old Bank' menu entry in the Bank window.":
        "\\n 请使用音色库窗口中的“转换旧版音色库”菜单项。",
    "Can not load this Bank file because it is from an old rakarrack git version,":
        "无法载入此音色库文件，它来自旧版 rakarrack git 版本，",
    "\\n please use rakgit2new utility to convert.":
        "\\n 请使用 rakgit2new 工具转换。",
    "!! Rakarrack-plus CPU Usage Warning !!\\n":
        "！！Rakarrack-plus CPU 占用警告！！\\n",
    "It appears your CPU will not easily handle convolution with the current settings.\\n":
        "当前设置下你的 CPU 似乎难以轻松处理卷积运算。\\n",
    "Be careful with the Convolotron effect settings.\\n":
        "请谨慎调整卷积混响效果的参数。\\n",
    "Please read Help (F1) for more information.":
        "更多信息请阅读帮助 (F1)。",
    "Bank file cannot be found in user directory %s\\n\\n":
        "在用户目录中找不到音色库文件 %s\\n\\n",
    "All user banks must be put in the user directory set in:\\n":
        "所有用户音色库必须放在以下位置设置的用户目录中：\\n",
    "Settings/Preferences/User - User Directory":
        "设置/首选项/用户 - 用户目录",
    "MIDI program file cannot be found in user directory:\\n%s\\n\\n":
        "在用户目录中找不到 MIDI 程序文件：\\n%s\\n\\n",
    "All MIDI program files should be put in the user directory set in:\\n":
        "所有 MIDI 程序文件应放在以下位置设置的用户目录中：\\n",
    "Convolotron user file cannot be found in user directory:\\n%s\\n\\n":
        "在用户目录中找不到卷积混响用户文件：\\n%s\\n\\n",
    "Echotron user file cannot be found in user directory:\\n%s\\n\\n":
        "在用户目录中找不到回声矩阵用户文件：\\n%s\\n\\n",
    "Reverbtron user file cannot be found in user directory:\\n%s\\n\\n":
        "在用户目录中找不到混响矩阵用户文件：\\n%s\\n\\n",
    "All user files must be put in the user directory set in:\\n":
        "所有用户文件必须放在以下位置设置的用户目录中：\\n",
    "Cannot access User Directory at:\\n%s\\n":
        "无法访问用户目录：\\n%s\\n",
    "Do you have permission?\\n":
        "你是否有相应权限？\\n",
    "Is the User Directory a valid read/write folder?":
        "用户目录是否为可读写的有效文件夹？",
    "Cannot access DATA Directory at:\\n%s\\n":
        "无法访问数据目录：\\n%s\\n",
    "You must install Rakarrack-plus to access default preset banks.\\n":
        "必须安装 Rakarrack-plus 才能访问默认预置音色库。\\n",
    "Export of %s plugin is not supported.\\n":
        "不支持导出 %s 插件。\\n",
    "It will be ignored on export...\\n":
        "导出时将被忽略...\\n",
    "Invalid number of excluded effects = %s\\n":
        "排除效果器数量无效 = %s\\n",
    "You cannot have more than %d excluded effects\\n":
        "生成随机预置时最多只能排除 %d 个效果器\\n",
    "to generate the random preset\\n":
        "\\n",
    "Duplicate preset label: %s\\n":
        "预置名称重复：%s\\n",
    "Please try another name for your preset.\\n":
        "请为预置换一个名称。\\n",
    "You cannot use commas in the preset name: %s\\n":
        "预置名称中不能使用逗号：%s\\n",
    "An error occurred in thread: %s\\n":
        "线程中发生错误：%s\\n",
    "This setting will be changed the next time you run rakarrack-plus":
        "此设置将在下次运行 rakarrack-plus 时生效",
    "User Directory is not set!\\n\\nYou must set a User Directory in :\\nSettings/Preferences/User - User Directory.":
        "未设置用户目录！\\n\\n你必须在以下位置设置用户目录：\\n设置/首选项/用户 - 用户目录。",
    "Jack Shut Down, try to save your work":
        "JACK 已关闭，请尝试保存你的工作",
    "Cannot make a jack client, is jackd running?":
        "无法创建 JACK 客户端，jackd 是否正在运行？",
    "Please, now try to load the new files":
        "请现在尝试载入新文件",
    "This file already has the new format":
        "此文件已是新格式",
    "Please, now use Reverbtron to load the new '.rvb' file":
        "请现在用混响矩阵载入新的 '.rvb' 文件",
    "Internal Presets can not be deleted ":
        "内部预置无法被删除 ",
    "Convolotron is unable to open the audio .wav file.":
        "卷积混响无法打开音频 .wav 文件。",
    "Reverbtron is unable to open the IR .rvb file":
        "混响矩阵无法打开 IR .rvb 文件",
    "Error writing the file. Do you have permission to write to this directory?":
        "写入文件出错。你是否有此目录的写入权限？",
    "Echotron is unable to open the .dly file":
        "回声矩阵无法打开 .dly 文件",
    "Some Pan parameter is out of range in the .dly file":
        ".dly 文件中某个声像参数超出范围",
    "Some Time parameter is out of range in the .dly file":
        ".dly 文件中某个时间参数超出范围",
    "Some Level parameter is out of range in the .dly file":
        ".dly 文件中某个电平参数超出范围",
    "Some LP parameter is out of range in the .dly file":
        ".dly 文件中某个低通参数超出范围",
    "Some BP parameter is out of range in the .dly file":
        ".dly 文件中某个带通参数超出范围",
    "Some HP parameter is out of range in the .dly file":
        ".dly 文件中某个高通参数超出范围",
    "Some Freq parameter is out of range in the .dly file":
        ".dly 文件中某个频率参数超出范围",
    "Some Q parameter is out of range in the .dly file":
        ".dly 文件中某个 Q 参数超出范围",
    "Some Stages parameter is out of range in the .dly file":
        ".dly 文件中某个级数参数超出范围",
    "Error loading file %s": "载入文件出错 %s",
    "Error loading file Order %s": "载入文件顺序出错 %s",
    "Error loading file Version %s": "载入文件版本出错 %s",
    "Error loading file Author %s": "载入文件作者出错 %s",
    "Error loading file Preset Name %s": "载入文件预置名出错 %s",
    "Error loading file General %s": "载入文件常规项出错 %s",
    "Error loading file MIDI %s": "载入文件 MIDI 出错 %s",
    "fread error in load_names()": "load_names() 读取错误",
    "fread error in load_bank()": "load_bank() 读取错误",
    "Error reading file %s": "读取文件出错 %s",
    "Error running rakconvert!": "运行 rakconvert 出错！",
    "Error running rakverb!": "运行 rakverb 出错！",
    "Error removing internal preset!": "移除内部预置出错！",
    "Error merging internal presets!": "合并内部预置出错！",
    "fread error in add_bank_item() %s": "add_bank_item() 读取错误 %s",
    "Error running aconnect!": "运行 aconnect 出错！",
    "Looper": "循环录音",
    "Vocoder": "声码器",
}

# Light button 开关标签（strcmp 逻辑必须与标签一致）
LIGHTBTN_RAW = {"On": "开", "Off": "关", "FX On": "FX 开", "FX Off": "FX 关"}

# fl_choice 对话框按钮（仅 rkrprocess_gui.cxx）
CHOICE_BTN_RAW = {"No": "否", "Yes": "是", "Discard": "放弃", "Save": "保存"}

stat = {"files": 0, "repl": 0}
missing = {}


def esc(k):
    """clean key -> C literal inner text candidates (plain and fluid-escaped)."""
    a = k.replace("\\", "\\\\").replace('"', '\\"')
    b = a.replace("'", "\\'")
    return a, b


def count_and_sub(text, pattern, repl_func, flags=0):
    n = 0

    def _r(m):
        nonlocal n
        r = repl_func(m)
        if r is not None and r != m.group(0):
            n += 1
            return r
        return m.group(0)

    return re.sub(pattern, _r, text, flags=flags), n


def unesc(lit):
    """C literal inner text -> clean key (handles \\" \\' escapes)."""
    out = []
    i = 0
    while i < len(lit):
        c = lit[i]
        if c == "\\" and i + 1 < len(lit):
            nxt = lit[i + 1]
            if nxt in ('"', "'", "\\"):
                out.append(nxt)
                i += 2
                continue
            # keep other escapes (\n, \t) literally
            out.append(c)
            out.append(nxt)
            i += 2
            continue
        out.append(c)
        i += 1
    return "".join(out)


def esc_for(zh, orig_lit):
    """Escape zh using the same escaping style as the original literal."""
    style = "fluid" if "\\'" in orig_lit else "plain"
    z = zh.replace("\\", "\\\\").replace('"', '\\"')
    if style == "fluid":
        z = z.replace("'", "\\'")
    return z


def apply_raw_literals(text, mapping):
    n = 0
    for k in sorted(mapping, key=len, reverse=True):
        old = '"' + k + '"'
        new = '"' + mapping[k] + '"'
        if old in text:
            n += text.count(old)
            text = text.replace(old, new)
    return text, n


def translate_fl_text(text, counter):
    def repl(m):
        lead, kind, val, rest = m.group(1), m.group(2), m.group(3), m.group(4)
        braced = val.startswith("{") and val.endswith("}")
        key = val[1:-1] if braced else val
        zh = DICT.get(key)
        if zh is None:
            return m.group(0)
        counter[0] += 1
        newval = "{" + zh + "}" if braced else zh
        return lead + kind + " " + newval + rest
    # braced form first
    text = re.sub(r'(?m)^(\s*)(label|tooltip) (\{.*?\})(.*)$', repl, text)
    # bare token form
    text = re.sub(r'(?m)^(\s*)(label|tooltip) ([^\s{][^\s{}]*)(.*)$', repl, text)
    return text


def translate_cxx_text(text, counter):
    def make(pat, gpre=1, glit=2, gpost=3):
        def _r(m):
            lit = m.group(glit)
            key = unesc(lit)
            zh = DICT.get(key)
            if zh is None:
                return m.group(0)
            z = esc_for(zh, lit)
            counter[0] += 1
            return m.group(gpre) + z + m.group(gpost)
        return _r
    # P1 label/tooltip/copy_label
    text = re.sub(r'((?:->|\.)\s*(?:label|copy_label|tooltip)\(\s*")((?:[^"\\]|\\.)*)(")',
                  make(0), text)
    # P2 menu items
    text = re.sub(r'(?m)^(\s*\{")((?:[^"\\]|\\.)*)(",)', make(0), text)
    # P3 ctor last arg
    text = re.sub(r'(new\s+(?:RKR_|Fl_)[A-Za-z_0-9]+\([^;]*?,\s*")((?:[^"\\]|\\.)*)("\s*\))',
                  make(0), text)
    # P4 fl_* dialog first arg
    text = re.sub(r'(fl_(?:alert|message|choice|input|ask)\(\s*")((?:[^"\\]|\\.)*)(")',
                  make(0), text)
    return text


def main():
    counter = [0]
    targets = []
    for pat in ["UI/*.cxx", "UI/*.h", "UI/*.fl", "UI/*.cpp", "*.C", "*.h"]:
        targets += glob.glob(os.path.join(ROOT, "src", pat))
    targets = [f for f in targets if not os.path.basename(f).startswith("PluginTemplate")]

    for f in sorted(targets):
        base = os.path.basename(f)
        try:
            text = open(f, encoding="utf-8").read()
        except UnicodeDecodeError:
            print(f"[SKIP non-utf8] {base}")
            continue
        orig = text
        if base.endswith(".fl"):
            text = translate_fl_text(text, counter)
        else:
            text = translate_cxx_text(text, counter)
            if base == "RKR_Light_Button.cxx":
                text, n = apply_raw_literals(text, LIGHTBTN_RAW)
                counter[0] += n
            if base == "varios.C":
                text, n = apply_raw_literals(text, VARIOS_RAW)
                counter[0] += n
            if base == "rkrprocess_gui.cxx":
                text, n = apply_raw_literals(text, CHOICE_BTN_RAW)
                counter[0] += n
        if text != orig:
            open(f, "w", encoding="utf-8", newline="").write(text)
            stat["files"] += 1

    # desktop 文件
    dpath = os.path.join(ROOT, "data", "rakarrack-plus.desktop")
    if os.path.exists(dpath):
        t = open(dpath, encoding="utf-8").read()
        if "Comment[zh_CN]" not in t:
            t = t.replace("Comment=Guitar Effects Processor",
                          "Comment=Guitar Effects Processor\nComment[zh_CN]=吉他效果处理器")
            open(dpath, "w", encoding="utf-8", newline="").write(t)
            print("desktop: added zh_CN comment")

    print(f"Modified {stat['files']} files, {counter[0]} replacements.")


if __name__ == "__main__":
    main()
