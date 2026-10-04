#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""第二轮汉化：补充遗漏 UI 字符串 + rkrMIDI.C 控制名表 + process.C 效果名数组。"""
import re, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
from zh_dict import DICT  # noqa

EXTRA = {
    "Channel": "通道", "Color": "颜色", "Decay": "衰减", "Disabled": "禁用",
    "End": "结束", "File": "文件", "Feedback 1": "反馈 1", "Feedback 2": "反馈 2",
    "Invert": "反相", "Hard Compress": "硬压缩", "Hard Pan": "硬声像",
    "Soft Pan": "柔和声像", "Hall": "大厅", "Room": "房间", "Cathedral": "大教堂",
    "Basic": "基本", "Classic": "经典", "Dissonance": "不和谐", "Aux": "辅助",
    "Mid": "中频", "Mismatch": "失配", "PingPong": "乒乓", "Record": "录音",
    "Remover": "移除", "Resample": "重采样", "Saturation": "饱和",
    "Schema": "方案", "Sequence 11": "序列 11", "Start": "起始",
    "SwingPong": "摇摆乒乓", "Track": "轨道", "Very Long 1": "超长 1",
    "Very Long 2": "超长 2", "Jack": "JACK", "In": "入", "Out": "出",
    "Shift": "移位", "label": "标签", "Preset Gain": "预置增益",
}

MIDI_PREFIX = [
    ("Alienwah", "外星哇音"), ("Analog Phaser", "模拟移相"), ("Arpie", "琶音"),
    ("Balance", "平衡"), ("Cabinet", "箱体"), ("Chorus", "合唱"),
    ("CoilCrafter", "线圈塑形"), ("CompBand", "分频压缩"), ("Compressor", "压缩器"),
    ("Convolotron", "卷积混响"), ("Derelict", "Derelict"), ("DistBand", "分频失真"),
    ("Distortion", "失真"), ("Dual Flange", "双镶边"), ("EQ", "EQ"),
    ("Echoverse", "回声宇宙"), ("Echotron", "回声矩阵"), ("Echo", "回声"),
    ("Exciter", "激励器"), ("Expander", "扩展器"), ("Flanger", "镶边"),
    ("Harmonizer", "和声器"), ("Infinity", "无限循环"), ("Looper", "循环录音"),
    ("Master", "主控"), ("Metronome", "节拍器"), ("MusicalDelay", "音乐延迟"),
    ("NoiseGate", "噪声门"), ("Opticaltrem", "光学颤音"), ("Overdrive", "过载"),
    ("Parametric EQ", "参数均衡"), ("Parametric", "参数均衡"), ("Phaser", "移相"),
    ("Recognize", "识别"), ("Reverbtron", "混响矩阵"), ("Reverb", "混响"),
    ("ShelfBoost", "搁架提升"), ("Shifter", "移调器"), ("Shuffle", "洗牌均衡"),
    ("StereoHarm", "立体声和声"), ("StompBox", "踏板箱"), ("Sustainer", "延音器"),
    ("Synthfilter", "合成滤波"), ("Valve", "电子管"), ("VaryBand", "变频段"),
    ("Vibe", "颤音"), ("Vocoder", "声码器"), ("WahWah", "哇音"),
    ("Pan", "声像"), ("Ring", "环形"), ("Sequence", "序列"), ("Multi", "多重"),
    ("Tap", "敲击"), ("Musical Delay", "音乐延迟"), ("MuTroMojo", "MuTroMojo"),
    ("ResSolution", "谐振器"), ("AlienWah", "外星哇音"), ("Coil Crafter", "线圈塑形"),
]

MIDI_PARAM = [
    ("Dry/Wet", "干湿"), ("LFO Type", "LFO波形"), ("L/R Cross", "左右交叉"),
    ("L/R Delay", "左右延迟"), ("Stereo Df.", "立体声深度"), ("Stereo Df", "立体声深度"),
    ("Arpe's", "琶音模式"), ("P. Depth", "相位深度"), ("A.Time", "起音时间"),
    ("R.Time", "释放时间"), ("Sub Octave", "低八度"), ("Type High", "高频类型"),
    ("Type Low", "低频类型"), ("Type Mid", "中频类型"), ("Type", "类型"),
    ("Tempo", "速度"), ("Random", "随机"), ("Phase", "相位"), ("Depth", "深度"),
    ("Delay", "延迟"), ("Feedback", "反馈"), ("Pan", "声像"), ("Damp", "阻尼"),
    ("Gain", "增益"), ("Freq 1", "频率1"), ("Freq 2", "频率2"), ("Q 1", "Q1"),
    ("Q 2", "Q2"), ("Tone", "音色"), ("Cross 1", "交叉1"), ("Cross 2", "交叉2"),
    ("Cross 3", "交叉3"), ("H Ratio", "高比率"), ("H Thres", "高阈值"),
    ("L Ratio", "低比率"), ("L Thres", "低阈值"), ("MH Ratio", "中高比率"),
    ("MH Thres", "中高阈值"), ("ML Ratio", "中低比率"), ("ML Thres", "中低阈值"),
    ("H. Gain", "高频增益"), ("L. Gain", "低频增益"), ("M. Gain", "中频增益"),
    ("Knee", "拐点"), ("Output", "输出"), ("Ratio", "比率"), ("Threshold", "阈值"),
    ("Length", "长度"), ("Level", "电平"), ("Color", "颜色"), ("Drive", "驱动"),
    ("HPF", "HPF"), ("LPF", "LPF"), ("Offset", "偏移"), ("Width", "宽度"),
    ("Mismatch", "失配"), ("Distortion", "失真"), ("Volume", "音量"),
    ("Dist", "失真"), ("Input", "输入"), ("Output Level", "输出电平"),
    ("Note", "音符"), ("Velocity", "力度"), ("Speed", "速度"), ("Sweep", "扫频"),
    ("Range", "范围"), ("Stages", "级数"), ("Resonance", "谐振"), ("Res", "谐振"),
    ("Filters", "滤波器"), ("Filter", "滤波"), ("Bandpass Level", "带通电平"),
    ("Highpass Level", "高通电平"), ("Lowpass Level", "低通电平"),
    ("Wet", "湿声"), ("Dry", "干声"), ("Sense", "灵敏度"), ("Attack", "起音"),
    ("Release", "释放"), ("Decay", "衰减"), ("Hold", "保持"), ("Gate", "门限"),
    ("Boost", "增强"), ("Octave", "八度"), ("Harmonic", "谐波"), ("Left", "左"),
    ("Right", "右"), ("Interval", "音程"), ("Chroma", "半音"), ("Run", "运行"),
    ("Stop", "停止"), ("Play", "播放"), ("FX%", "FX%"), ("Gain/Master", "增益/主音量"),
    ("Grid Frequency", "电网频率"), ("Oscillator", "振荡器"), ("Mode", "模式"),
    ("On/Off", "开关"), ("Reverse", "反向"), ("#", "#"), ("Angle", "角度"),
    ("Ex Stereo", "额外立体声"), ("Har", "谐波"), ("Out Gain", "输出增益"),
    ("Shape", "形状"), ("Chord", "和弦"), ("Filter Gain", "滤波增益"),
    ("Filter Q", "滤波Q"), ("Freq", "频率"), ("SELECT", "选择"),
    ("AutoPan", "自动声像"), ("Start", "起始"), ("End", "结束"),
    ("Filter Band", "滤波频段"), ("Subdiv", "细分"), ("Auto Play", "自动播放"),
    ("Clear", "清除"), ("Pause", "暂停"), ("Record", "录音"), ("Track", "轨道"),
    ("BP", "BP"), ("HP", "HP"), ("LP", "LP"), ("E. Sens", "包络灵敏度"),
    ("Smooth", "平滑"), ("St. Freq", "起始频率"), ("Wah", "哇音"),
    ("FB", "反馈"), ("Time", "时间"), ("Initial Delay", "初始延迟"),
    ("Room Size", "房间大小"), ("Damping", "阻尼"), ("Del. E/R", "Del. E/R"),
    ("Diffusion", "扩散"), ("Fade", "淡出"), ("Stretch", "拉伸"),
    ("Sawtooth", "锯齿波"), ("Sine", "正弦"), ("Square", "方波"),
    ("Triangle", "三角波"), ("Presence", "存在感"), ("Whammy", "Whammy"),
    ("High Freq", "高频率"), ("High Gain", "高增益"), ("High Q", "高Q"),
    ("Low Freq", "低频率"), ("Low Gain", "低增益"), ("Low Q", "低Q"),
    ("Mid Freq", "中频率"), ("Mid Gain", "中增益"), ("Mid Q", "中Q"),
    ("M.H. Freq", "中高频率"), ("M.H. Gain", "中高增益"),
    ("M.L. Freq", "中低频率"), ("M.L. Gain", "中低增益"), ("Q", "Q"),
    ("Bias", "偏置"), ("Mid", "中频"), ("Sustain", "延音"), ("Distort", "失真度"),
    ("E.Sens", "包络灵敏度"), ("LFO 1 Type", "LFO1波形"), ("LFO 2 Type", "LFO2波形"),
    ("St.df", "立体声深度"), ("Ring", "环形"), ("Smear", "涂抹"),
    ("Amp S.", "振幅灵敏度"), ("Amp S.I.", "振幅灵敏度反"),
    ("Chrm L", "左半音"), ("Chrm R", "右半音"), ("Gain L", "左增益"),
    ("Gain R", "右增益"), ("Int L", "左音程"), ("Int R", "右音程"),
    ("Input Volume", "输入音量"), ("Output Volume", "输出音量"),
    ("Program Change Table", "程序切换表"),
]

# 顶层独立名称（无前缀空格分隔的）
STANDALONE = {
    "Input Volume": "输入音量",
    "Output Volume": "输出音量",
    "Program Change Table": "程序切换表",
    "Multi On/Off": "多重 开关",
}



def midi_translate(name):
    if name in STANDALONE:
        return STANDALONE[name]
    for p, pz in MIDI_PREFIX:
        if name == p:
            return pz
        if name.startswith(p + " "):
            rest = name[len(p) + 1:]
            for s, sz in MIDI_PARAM:
                if rest == s:
                    return pz + " " + sz
            # 允许 "Param 1"/"Param 2" 等数字后缀
            m = re.match(r"^(.*?)\s+(\d+)$", rest)
            if m:
                base, num = m.group(1), m.group(2)
                for s, sz in MIDI_PARAM:
                    if base == s:
                        return pz + " " + sz + num
            return None
    return None


def main():
    # ---- 1) 补充 UI 字符串（复用 apply_zh 的模式，但仅处理 EXTRA）----
    global DICT
    extra_only = dict(EXTRA)
    DICT = extra_only  # noqa
    import apply_zh
    apply_zh.DICT = extra_only
    counter = [0]
    import glob
    targets = []
    for pat in ["UI/*.cxx", "UI/*.h", "UI/*.fl", "UI/*.cpp", "*.C", "*.h"]:
        targets += glob.glob(os.path.join(ROOT, "src", pat))
    targets = [f for f in targets if not os.path.basename(f).startswith("PluginTemplate")]
    nfiles = 0
    for f in sorted(targets):
        base = os.path.basename(f)
        try:
            text = open(f, encoding="utf-8").read()
        except UnicodeDecodeError:
            continue
        orig = text
        if base.endswith(".fl"):
            text = apply_zh.translate_fl_text(text, counter)
        else:
            text = apply_zh.translate_cxx_text(text, counter)
        if text != orig:
            open(f, "w", encoding="utf-8", newline="").write(text)
            nfiles += 1
    print(f"[pass2 UI] {nfiles} files, {counter[0]} replacements")

    # ---- 2) rkrMIDI.C 控制名表 ----
    f = os.path.join(ROOT, "src", "rkrMIDI.C")
    text = open(f, encoding="utf-8").read()
    unmapped = []

    def _r(m):
        name = m.group(2)
        zh = midi_translate(name)
        if zh is None:
            unmapped.append(name)
            return m.group(0)
        if len(zh.encode("utf-8")) > 31:
            unmapped.append(name + " [TOO LONG]")
            return m.group(0)
        return m.group(1) + zh + m.group(3)

    text2 = re.sub(r'(?m)^(\s*")([^"]+)(",\s*strdup)', _r, text)
    open(f, "w", encoding="utf-8", newline="").write(text2)
    print(f"[pass2 MIDI] translated, unmapped: {len(unmapped)}")
    for u in sorted(set(unmapped)):
        print("  未映射:", u)

    # ---- 3) process.C los_names ----
    f = os.path.join(ROOT, "src", "process.C")
    text = open(f, encoding="utf-8").read()
    unmapped = []

    def _r2(m):
        name = m.group(2)
        zh = midi_translate(name)
        if zh is None:
            unmapped.append(name)
            return m.group(0)
        if len(zh.encode("utf-8")) > 23:
            unmapped.append(name + " [TOO LONG]")
            return m.group(0)
        return m.group(1) + zh + m.group(3)

    text2 = re.sub(r'(?m)^(\s*")([^"]+)(",\s*strdup)', _r2, text)
    open(f, "w", encoding="utf-8", newline="").write(text2)
    print(f"[pass2 process.C] unmapped: {len(unmapped)}")
    for u in sorted(set(unmapped)):
        print("  未映射:", u)


if __name__ == "__main__":
    main()
