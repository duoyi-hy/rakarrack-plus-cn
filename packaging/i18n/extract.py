#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Extract translatable UI strings from rakarrack-plus sources."""
import re, json, sys, glob, os

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", ".."))
SRC = os.path.join(ROOT, "src")

files = []
for pat in ["UI/*.cxx", "UI/*.h", "UI/*.fl", "UI/*.cpp", "*.C", "*.h"]:
    files += glob.glob(os.path.join(SRC, pat))
files = [f for f in files if not os.path.basename(f).startswith("PluginTemplate")]

pat_label = re.compile(r'(?:->|\.)\s*(label|copy_label|tooltip)\(\s*"((?:[^"\\]|\\.)*)"\s*\)')
pat_menu  = re.compile(r'^\s*\{\s*"((?:[^"\\]|\\.)*)"\s*,', re.M)
pat_ctor  = re.compile(r'new\s+(?:RKR_|Fl_)[A-Za-z_0-9]+\(\s*[^()]*?\(\s*[^()]*?\)\s*[^()]*?,\s*"((?:[^"\\]|\\.)*)"\s*\)')
pat_ctor2 = re.compile(r'new\s+(?:RKR_|Fl_)[A-Za-z_0-9]+\(\s*(?:[^()"\\]|\\.)+?,\s*"((?:[^"\\]|\\.)*)"\s*\)')
pat_fl_block = re.compile(r'^\s*(label|tooltip)\s+\{(.*)\}\s*$', re.M)
pat_fluid_label = re.compile(r'^\s*label\s+\{(.+)\}\s*$', re.M)
pat_fl_label_quoted = re.compile(r'^\s*(label|tooltip)\s+"(.*)"\s*$', re.M)
pat_dialog = re.compile(r'fl_(?:alert|message|choice|input|ask)\(\s*"((?:[^"\\]|\\.)*)"')

strings = {}  # string -> {contexts:set, files:set}

def add(s, ctx, fname):
    if not s or s.strip() == "":
        return
    ent = strings.setdefault(s, {"ctx": set(), "files": set()})
    ent["ctx"].add(ctx)
    ent["files"].add(os.path.relpath(fname, ROOT))

for f in files:
    try:
        text = open(f, encoding="utf-8", errors="replace").read()
    except Exception:
        continue
    base = os.path.basename(f)
    is_fl = base.endswith(".fl")
    if is_fl:
        for m in pat_fluid_label.finditer(text):
            add(m.group(1), "fl:label", f)
        for m in pat_fl_label_quoted.finditer(text):
            add(m.group(2), "fl:"+m.group(1), f)
    else:
        for m in pat_label.finditer(text):
            add(m.group(2), "call:"+m.group(1), f)
        for m in pat_menu.finditer(text):
            add(m.group(1), "menu", f)
        for m in pat_ctor2.finditer(text):
            add(m.group(1), "ctor", f)
        for m in pat_dialog.finditer(text):
            add(m.group(1), "dialog", f)

out = {s: {"ctx": sorted(v["ctx"]), "files": sorted(v["files"])[:4]} for s, v in strings.items()}
out_path = os.path.join(os.path.dirname(__file__), "extracted.json")
with open(out_path, "w", encoding="utf-8") as fh:
    json.dump(out, fh, ensure_ascii=False, indent=1)

# Print summary grouped by length
print(f"Total unique strings: {len(out)}")
simple = sorted([s for s in out if len(s) <= 40])
print("---- short strings ----")
for s in simple:
    print(json.dumps({"s": s, "ctx": out[s]["ctx"]}, ensure_ascii=False))
