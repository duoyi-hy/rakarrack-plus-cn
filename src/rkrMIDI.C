/*
  rakarrack - a guitar effects software

 rkrMIDI.C  -  MIDI functions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */


#include "process.h"
#include "strlcpy.h"
#include <FL/fl_ask.H>  // for error pop up
#include <FL/Fl_Preferences.H>

#ifdef RKR_PLUS_LV2
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <unistd.h>		// usleep()
#endif

/* MIDI control defines (Max - Min) / 127 - parameter ranges  */

const float C_MC_LFO_RNGE       =  float(LFO_NUM_TYPES - 1) / 127.0;

const float C_MC_7_RANGE        = 0.05511811f;      /* 7 / 127 = 0.055118110236 */
const float C_MC_8_RANGE        = 0.06299f;         /* 8 / 127 = 0.062992125984252 */
const float C_MC_12_RANGE       = 0.094488189f;     /* 12 / 127 = 0.094488188976 */
const float C_MC_23_RANGE       = 0.18110236f;      /* 23 / 127 = 0.1811023622047 */
const float C_MC_24_RANGE       = 0.18897638f;      /* 24 / 127 = 0.188976377953  */
const float C_MC_30_RANGE       = 0.236220472441f;  /* 30 / 127 = 0.2362204724409449 */
const float C_MC_32_RANGE       = 0.25196850393701f;  /* 32 / 127 = 0.2519685039370079 */
const float C_MC_33_RANGE       = 0.25984252f;      /* 33 / 127 = 0.2598425196850 */
const float C_MC_40_RANGE       = 0.31496063f;      /* (42 - 2) / 127 = 0.314960629921 */
const float C_MC_49_RANGE       = 0.385827f;        /* (50 - 1) / 127 = 0.385826771654 */
const float C_MC_55_RANGE       = 0.43307087f;      /* (65 - 10) / 127 = 0.433070866142 */
const float C_MC_57_RANGE       = 0.448818898f;     /* (-60 - -3) / 127 = 0.448818897638 */
const float C_MC_63_RANGE       = 0.49606299f;      /* (127 - 64) / 127 = 0.4960629921259843 */
const float C_MC_80_RANGE       = 0.62992126f;      /* 80 / 127 = 0.629921259843 */
const float C_MC_90_RANGE       = 0.708661417f;     /* 90 / 127 = 0.7086614173228 */
const float C_MC_94_RANGE       = 0.74015748f;      /* (-70 + 24) / 127 = 0.740157480315 */
const float C_MC_100_RANGE      = 0.7874016f;       /* 100 / 127 = 0.787401574803 */
const float C_MC_126_RANGE      = 0.99212598f;      /* (127 - 1) / 127 = 0.992125984252 */
const float C_MC_128_RANGE      = 1.007874015748f;  /* 128 / 127 = 1.007874015748 */
const float C_MC_130_RANGE      = 1.023622f;        /* (170 - 40) / 127 = 1.0236220472441 */
const float C_MC_240_RANGE      = 1.890f;           /* (250 - 10) / 127 = 1.889763779528 */
const float C_MC_245_RANGE      = 1.9291339f;       /* (250 - 5) / 127 = 1.929133858268 */
const float C_MC_248_RANGE      = 1.9527559055f;    /* (250 - 2) / 127 = 1.9527559055118 */
const float C_MC_249_RANGE      = 1.9606299212598f; /* (250 - 1) / 127 = 1.9606299212598 */
const float C_MC_360_RANGE      = 2.83464567f;      /* (380 - 20) / 127 = 2.8346456692913 */
const float C_MC_470_RANGE      = 3.7007874f;       /* (480 - 10) / 127 = 3.7007874015748 */
const float C_MC_490_RANGE      = 3.85826772f;      /* (500 - 10) / 127 = 3.858267716535 */
const float C_MC_495_RANGE      = 3.8976378f;       /* (500 - 5) / 127 = 3.8976377952756 */
const float C_MC_498_RANGE      = 3.92125984f;      /* (500 - 2) / 127 = 3.921259842519685 */
const float C_MC_500_RANGE      = 3.9370079f;       /* 500 / 127 = 3.937007874 */
const float C_MC_600_RANGE      = 4.724f;           /* 600  / 127 = 4.724409448819 */
const float C_MC_770_RANGE      = 6.062992126f;     /* (800 - 30) / 127 = 6.062992125984 */
const float C_MC_980_RANGE      = 7.716535433f;     /* (1000 - 20) / 127 = 7.716535433071 */
const float C_MC_990_RANGE      = 7.7952756f;       /* (1000 - 10) / 127 = 7.795275590551 */
const float C_MC_995_RANGE      = 7.8346457f;       /* (1000 - 5) / 127 = 7.8346456692913 */
const float C_MC_1480_RANGE     = 11.653543f;       /* (1500 - 20) / 127 = 11.6535433071 */
const float C_MC_1900_RANGE     = 14.96063f;        /* (4500 - 2600) / 127 = 14.96062992126 */
const float C_MC_1980_RANGE     = 15.59055118f;     /* (2000 - 20) / 127 = 15.5905511811 */
const float C_MC_1999_RANGE     = 15.748031f;       /* (2000 - 1) / 127 = 15.740157480315 */
const float C_MC_2000_RANGE     = 15.7480315f;      /* 2000 / 127 = 15.748031496063 */
const float C_MC_2480_RANGE     = 19.52756f;        /* (2500 - 20) / 127 =  19.527559055118 */
const float C_MC_3600_RANGE     = 28.34645669f;     /* (4000 - 400) / 127 = 28.3464566929134 */
const float C_MC_4000_RANGE     = 31.496063f;       /* 4000 / 127 = 31.496062992126 */
const float C_MC_4380_RANGE     = 34.488189f;       /* (4400 - 20) / 127 = 34.488188976378 */
const float C_MC_4999_RANGE     = 39.362205f;       /* (5000 - 1) / 127 = 39.362204724409 */
const float C_MC_5990_RANGE     = 47.165354f;       /* (6000 - 10) / 127 = 47.1653543307087 */
const float C_MC_6000_RANGE     = 47.2441f;         /* 6000 / 127 = 47.244094488189 */
const float C_MC_6800_RANGE     = 53.54330709f;     /* (8000 - 1200) / 127 = 53.5433070866142 */
const float C_MC_7000_RANGE     = 55.11811f;        /* (8000 - 1000) / 127 = 55.11811023622 */
const float C_MC_7200_RANGE     = 56.6929134f;      /* (8000 - 800) / 127 = 56.6929133858 */
const float C_MC_11200_RANGE    = 88.18898f;        /* (12000 - 800) / 127 = 88.188976377953 */
const float C_MC_15780_RANGE    = 124.25197f;       /* (16000 - 220) / 127 = 124.251968503937 */
const float C_MC_19980_RANGE    = 157.322835f;      /* (20000 - 20) / 127 = 157.322834645669 */
const float C_MC_19999_RANGE    = 157.472441f;      /* (20000 - 1) / 127 = 157.4724409448819 */
const float C_MC_20000_RANGE    = 157.480315f;      /* (26000 - 6000) / 127 = 157.480314961 */
const float C_MC_24000_RANGE    = 188.97638f;       /* (26000 - 2000) / 127 = 188.976377952756 */
const float C_MC_25980_RANGE    = 204.566929f;      /* (26000 - 20) / 127 = 204.5669291338583 */


/**
 *  The MIDI control parameters. This includes the default MIDI control
 *  parameters (1 - 127) as well as MIDI learn.
 *
 *  The los_params[] array is the order used by the MIDI learn window.
 * 
 *  The items are copied to Effects_Params struct:
 * 
 *      char Description[32]    = MC Parameter Description - MIDI Learn Window
 *      int MC_params_index     = Parameter MIDI control number
 *      int Effect_index        = Rack Effect number
 *      int Efx_param_index     = Effect parameter number
 *      int MC_offset           = MIDI control parameter offset
 *      double MC_range         = MIDI control parameter range multiplier
 * 
 * If any additional parameters are added, then the constant
 * C_MC_PARAMETER_SIZE must be adjusted.
 */
void
RKR::MIDI_control()
{
    static const char *los_params[] =
    {
        "外星哇音 干湿",         strdup( NTS(MC_Alien_DryWet).c_str()),           strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_DryWet).c_str()),         "127",     "-1.0",
        "外星哇音 声像",             strdup( NTS(MC_Alien_Pan).c_str()),              strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Pan).c_str()),              "0",     "1.0",
        "外星哇音 速度",           strdup( NTS(MC_Alien_LFO_Tempo).c_str()),        strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "外星哇音 随机",          strdup( NTS(MC_Alien_LFO_Random).c_str()),       strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Random).c_str()),       "0",     "1.0",
        "外星哇音 LFO波形",        strdup( NTS(MC_Alien_LFO_Type).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Type).c_str()),         "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "外星哇音 相位",           strdup( NTS(MC_Alien_Phase).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Phase).c_str()),            "0",     "1.0",
        "外星哇音 立体声深度",      strdup( NTS(MC_Alien_LFO_Stereo).c_str()),       strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Stereo).c_str()),       "0",     "1.0",
        "外星哇音 深度",           strdup( NTS(MC_Alien_Depth).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Depth).c_str()),            "0",     "1.0",
        "外星哇音 延迟",           strdup( NTS(MC_Alien_Delay).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Delay).c_str()),            "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "外星哇音 反馈",        strdup( NTS(MC_Alien_Feedback).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Feedback).c_str()),         "0",     "1.0",
        "外星哇音 左右交叉",       strdup( NTS(MC_Alien_LR_Cross).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LR_Cross).c_str()),         "0",     "1.0",

        "模拟移相 干湿",    strdup( NTS(MC_APhase_DryWet).c_str()),          strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_DryWet).c_str()),        "127",     "-1.0",
        "模拟移相 LFO波形",   strdup( NTS(MC_APhase_LFO_Type).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "模拟移相 速度",      strdup( NTS(MC_APhase_LFO_Tempo).c_str()),       strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "模拟移相 相位深度",   strdup( NTS(MC_APhase_Depth).c_str()),           strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Depth).c_str()),           "0",     "1.0",
        "模拟移相 宽度",      strdup( NTS(MC_APhase_Width).c_str()),           strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Width).c_str()),           "0",     "1.0",
        "模拟移相 反馈",   strdup( NTS(MC_APhase_Feedback).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Feedback).c_str()),        "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "模拟移相 失配",   strdup( NTS(MC_APhase_Mismatch).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Mismatch).c_str()),        "0",     "1.0",
        "模拟移相 失真", strdup( NTS(MC_APhase_Distortion).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Distortion).c_str()),      "0",     "1.0",
        "模拟移相 随机",     strdup( NTS(MC_APhase_LFO_Random).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Random).c_str()),      "0",     "1.0",
        "模拟移相 立体声深度", strdup( NTS(MC_APhase_LFO_Stereo).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Stereo).c_str()),      "0",     "1.0",

        "琶音 干湿",            strdup( NTS(MC_Arpie_DryWet).c_str()),           strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_DryWet).c_str()),         "127",     "-1.0",
        "琶音 琶音模式",             strdup( NTS(MC_Arpie_ArpeWD).c_str()),           strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_ArpeWD).c_str()),           "0",     "1.0",
        "琶音 声像",                strdup( NTS(MC_Arpie_Pan).c_str()),              strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Pan).c_str()),              "0",     "1.0",
        "琶音 速度",              strdup( NTS(MC_Arpie_Tempo).c_str()),            strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Tempo).c_str()),            "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "琶音 左右延迟",          strdup( NTS(MC_Arpie_LR_Delay).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_LR_Delay).c_str()),         "0",     "1.0",
        "琶音 左右交叉",          strdup( NTS(MC_Arpie_LR_Cross).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_LR_Cross).c_str()),         "0",     "1.0",
        "琶音 反馈",           strdup( NTS(MC_Arpie_Feedback).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Feedback).c_str()),         "0",     "1.0",
        "琶音 阻尼",               strdup( NTS(MC_Arpie_Damp).c_str()),             strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Damp).c_str()),             "0",     "1.0",

        "平衡 FX%",              strdup( NTS(MC_Balance_FX).c_str()),             strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "箱体 增益",             strdup( NTS(MC_Cabinet_Gain).c_str()),           strdup( NTS(EFX_CABINET).c_str()),       strdup( NTS(Cabinet_Gain).c_str()),           "0",     "1.0",

        "合唱 干湿",           strdup( NTS(MC_Chorus_DryWet).c_str()),          strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_DryWet).c_str()),        "127",     "-1.0",
        "合唱 声像",               strdup( NTS(MC_Chorus_Pan).c_str()),             strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Pan).c_str()),             "0",     "1.0",
        "合唱 速度",             strdup( NTS(MC_Chorus_LFO_Tempo).c_str()),       strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "合唱 随机",            strdup( NTS(MC_Chorus_LFO_Random).c_str()),      strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Random).c_str()),      "0",     "1.0",
        "合唱 LFO波形",          strdup( NTS(MC_Chorus_LFO_Type).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "合唱 立体声深度",         strdup( NTS(MC_Chorus_LFO_Stereo).c_str()),      strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Stereo).c_str()),      "0",     "1.0",
        "合唱 深度",             strdup( NTS(MC_Chorus_Depth).c_str()),           strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Depth).c_str()),           "0",     "1.0",
        "合唱 延迟",             strdup( NTS(MC_Chorus_Delay).c_str()),           strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Delay).c_str()),           "0",     "1.0",
        "合唱 反馈",          strdup( NTS(MC_Chorus_Feedback).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Feedback).c_str()),        "0",     "1.0",
        "合唱 左右交叉",         strdup( NTS(MC_Chorus_LR_Cross).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LR_Cross).c_str()),        "0",     "1.0",

        "线圈塑形 增益",         strdup( NTS(MC_Coil_Gain).c_str()),              strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Gain).c_str()),              "0",     "1.0",
        "线圈塑形 音色",         strdup( NTS(MC_Coil_Tone).c_str()),              strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Tone).c_str()),             "20",     strdup( NTS(C_MC_4380_RANGE).c_str()),
        "线圈塑形 频率1",       strdup( NTS(MC_Coil_Freq_1).c_str()),            strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Freq_1).c_str()),         "2600",     strdup( NTS(C_MC_1900_RANGE).c_str()),
        "线圈塑形 Q1",          strdup( NTS(MC_Coil_Q_1).c_str()),               strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Q_1).c_str()),              "10",     strdup( NTS(C_MC_55_RANGE).c_str()),
        "线圈塑形 频率2",       strdup( NTS(MC_Coil_Freq_2).c_str()),            strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Freq_2).c_str()),         "2600",     strdup( NTS(C_MC_1900_RANGE).c_str()),
        "线圈塑形 Q2",          strdup( NTS(MC_Coil_Q_2).c_str()),               strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Q_2).c_str()),              "10",     strdup( NTS(C_MC_55_RANGE).c_str()),

        "分频压缩 干湿",         strdup( NTS(MC_CompBand_DryWet).c_str()),        strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_DryWet).c_str()),      "127",     "-1.0",
        "分频压缩 增益",            strdup( NTS(MC_CompBand_Gain).c_str()),          strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Gain).c_str()),          "0",     "1.0",
        "分频压缩 低比率",         strdup( NTS(MC_CompBand_Low_Ratio).c_str()),     strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Low_Ratio).c_str()),     "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "分频压缩 中低比率",        strdup( NTS(MC_CompBand_Mid_1_Ratio).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_1_Ratio).c_str()),   "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "分频压缩 中高比率",        strdup( NTS(MC_CompBand_Mid_2_Ratio).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_2_Ratio).c_str()),   "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "分频压缩 高比率",         strdup( NTS(MC_CompBand_High_Ratio).c_str()),    strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_High_Ratio).c_str()),    "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "分频压缩 低阈值",         strdup( NTS(MC_CompBand_Low_Thresh).c_str()),    strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Low_Thresh).c_str()),  "-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "分频压缩 中低阈值",        strdup( NTS(MC_CompBand_Mid_1_Thresh).c_str()),  strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_1_Thresh).c_str()),"-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "分频压缩 中高阈值",        strdup( NTS(MC_CompBand_Mid_2_Thresh).c_str()),  strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_2_Thresh).c_str()),"-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "分频压缩 高阈值",         strdup( NTS(MC_CompBand_High_Thresh).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_High_Thresh).c_str()), "-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "分频压缩 交叉1",         strdup( NTS(MC_CompBand_Cross_1).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "分频压缩 交叉2",         strdup( NTS(MC_CompBand_Cross_2).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_2).c_str()),    "1000",     strdup( NTS(C_MC_7000_RANGE).c_str()),
        "分频压缩 交叉3",         strdup( NTS(MC_CompBand_Cross_3).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_3).c_str()),    "2000",     strdup( NTS(C_MC_24000_RANGE).c_str()),

        "压缩器 起音时间",        strdup( NTS(MC_Compress_Attack).c_str()),        strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Attack).c_str()),       "10",     strdup( NTS(C_MC_240_RANGE).c_str()),
        "压缩器 拐点",          strdup( NTS(MC_Compress_Knee).c_str()),          strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Knee).c_str()),          "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "压缩器 输出",        strdup( NTS(MC_Compress_Output).c_str()),        strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Output).c_str()),      "-40",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "压缩器 比率",         strdup( NTS(MC_Compress_Ratio).c_str()),         strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Ratio).c_str()),         "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "压缩器 释放时间",        strdup( NTS(MC_Compress_Release).c_str()),       strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Release).c_str()),      "10",     strdup( NTS(C_MC_490_RANGE).c_str()),
        "压缩器 阈值",     strdup( NTS(MC_Compress_Threshold).c_str()),     strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Threshold).c_str()),   "-60",     strdup( NTS(C_MC_57_RANGE).c_str()),

        "卷积混响 干湿",      strdup( NTS(MC_Convo_DryWet).c_str()),           strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_DryWet).c_str()),         "127",     "-1.0",
        "卷积混响 声像",          strdup( NTS(MC_Convo_Pan).c_str()),              strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Pan).c_str()),              "0",     "1.0",
        "卷积混响 电平",        strdup( NTS(MC_Convo_Level).c_str()),            strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Level).c_str()),            "0",     "1.0",
        "卷积混响 阻尼",         strdup( NTS(MC_Convo_Damp).c_str()),             strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Damp).c_str()),             "0",     "1.0",
        "卷积混响 反馈",     strdup( NTS(MC_Convo_Feedback).c_str()),         strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Feedback).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "卷积混响 长度",       strdup( NTS(MC_Convo_Length).c_str()),           strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Length).c_str()),           "5",     strdup( NTS(C_MC_245_RANGE).c_str()),

        "Derelict 干湿",         strdup( NTS(MC_Dere_DryWet).c_str()),            strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_DryWet).c_str()),          "127",     "-1.0",
        "Derelict 左右交叉",       strdup( NTS(MC_Dere_LR_Cross).c_str()),          strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_LR_Cross).c_str()),          "0",     "1.0",
        "Derelict 驱动",           strdup( NTS(MC_Dere_Drive).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Drive).c_str()),             "0",     "1.0",
        "Derelict 电平",           strdup( NTS(MC_Dere_Level).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Level).c_str()),             "0",     "1.0",
        "Derelict 类型",            strdup( NTS(MC_Dere_Type).c_str()),              strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Type).c_str()),              "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "Derelict 颜色",           strdup( NTS(MC_Dere_Color).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Color).c_str()),             "0",     "1.0",
        "Derelict 低八度",      strdup( NTS(MC_Dere_Suboctave).c_str()),         strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Suboctave).c_str()),         "0",     "1.0",
        "Derelict 声像",             strdup( NTS(MC_Dere_Pan).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Pan).c_str()),               "0",     "1.0",
        "Derelict LPF",             strdup( NTS(MC_Dere_LPF).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Derelict HPF",             strdup( NTS(MC_Dere_HPF).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "分频失真 干湿",         strdup( NTS(MC_DistBand_DryWet).c_str()),        strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_DryWet).c_str()),      "127",     "-1.0",
        "分频失真 左右交叉",       strdup( NTS(MC_DistBand_LR_Cross).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_LR_Cross).c_str()),      "0",     "1.0",
        "分频失真 驱动",           strdup( NTS(MC_DistBand_Drive).c_str()),         strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Drive).c_str()),         "0",     "1.0",
        "分频失真 电平",           strdup( NTS(MC_DistBand_Level).c_str()),         strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Level).c_str()),         "0",     "1.0",
        "分频失真 低频增益",         strdup( NTS(MC_DistBand_Gain_Low).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Low).c_str()),      "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "分频失真 中频增益",         strdup( NTS(MC_DistBand_Gain_Mid).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Mid).c_str()),      "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "分频失真 高频增益",         strdup( NTS(MC_DistBand_Gain_Hi).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Hi).c_str()),       "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "分频失真 交叉1",         strdup( NTS(MC_DistBand_Cross_1).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "分频失真 交叉2",         strdup( NTS(MC_DistBand_Cross_2).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Cross_2).c_str()),     "800",     strdup( NTS(C_MC_11200_RANGE).c_str()),
        "分频失真 低频类型",        strdup( NTS(MC_DistBand_Type_Low).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Low).c_str()),      "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "分频失真 中频类型",        strdup( NTS(MC_DistBand_Type_Mid).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Mid).c_str()),      "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "分频失真 高频类型",       strdup( NTS(MC_DistBand_Type_Hi).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Hi).c_str()),       "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "分频失真 声像",             strdup( NTS(MC_DistBand_Pan).c_str()),           strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Pan).c_str()),           "0",     "1.0",

        "失真 干湿",       strdup( NTS(MC_Dist_DryWet).c_str()),            strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_DryWet).c_str()),          "127",     "-1.0",
        "失真 左右交叉",     strdup( NTS(MC_Dist_LR_Cross).c_str()),          strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_LR_Cross).c_str()),          "0",     "1.0",
        "失真 驱动",         strdup( NTS(MC_Dist_Drive).c_str()),             strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Drive).c_str()),             "0",     "1.0",
        "失真 电平",         strdup( NTS(MC_Dist_Level).c_str()),             strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Level).c_str()),             "0",     "1.0",
        "失真 类型",          strdup( NTS(MC_Dist_Type).c_str()),              strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Type).c_str()),              "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "失真 声像",           strdup( NTS(MC_Dist_Pan).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Pan).c_str()),               "0",     "1.0",
        "失真 低八度",    strdup( NTS(MC_Dist_Suboctave).c_str()),         strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Suboctave).c_str()),         "0",     "1.0",
        "失真 LPF",           strdup( NTS(MC_Dist_LPF).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "失真 HPF",           strdup( NTS(MC_Dist_HPF).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "双镶边 干湿",      strdup( NTS(MC_DFlange_DryWet).c_str()),         strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_DryWet).c_str()),       "127",     "-1.0",
        "双镶边 声像",          strdup( NTS(MC_DFlange_Pan).c_str()),            strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Pan).c_str()),          "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "双镶边 左右交叉",    strdup( NTS(MC_DFlange_LR_Cross).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LR_Cross).c_str()),       "0",     "1.0",
        "双镶边 深度",        strdup( NTS(MC_DFlange_Depth).c_str()),          strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Depth).c_str()),         "20",     strdup( NTS(C_MC_2480_RANGE).c_str()),
        "双镶边 宽度",        strdup( NTS(MC_DFlange_Width).c_str()),          strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Width).c_str()),          "0",     strdup( NTS(C_MC_6000_RANGE).c_str()),
        "双镶边 偏移",       strdup( NTS(MC_DFlange_Offset).c_str()),         strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Offset).c_str()),         "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "双镶边 反馈",     strdup( NTS(MC_DFlange_Feedback).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Feedback).c_str()),     "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "双镶边 LPF",          strdup( NTS(MC_DFlange_LPF).c_str()),            strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LPF).c_str()),           "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "双镶边 速度",        strdup( NTS(MC_DFlange_LFO_Tempo).c_str()),      strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "双镶边 立体声深度",    strdup( NTS(MC_DFlange_LFO_Stereo).c_str()),     strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Stereo).c_str()),     "0",     "1.0",
        "双镶边 LFO波形",     strdup( NTS(MC_DFlange_LFO_Type).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "双镶边 随机",       strdup( NTS(MC_DFlange_LFO_Random).c_str()),     strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Random).c_str()),     "0",     "1.0",

        "回声 干湿",             strdup( NTS(MC_Echo_DryWet).c_str()),            strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_DryWet).c_str()),          "127",     "-1.0",
        "回声 反向",             strdup( NTS(MC_Echo_Reverse).c_str()),           strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Reverse).c_str()),           "0",     "1.0",
        "回声 声像",                 strdup( NTS(MC_Echo_Pan).c_str()),               strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Pan).c_str()),               "0",     "1.0",
        "回声 延迟",               strdup( NTS(MC_Echo_Delay).c_str()),             strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Delay).c_str()),            "20",     strdup( NTS(C_MC_1980_RANGE).c_str()),
        "回声 左右延迟",           strdup( NTS(MC_Echo_LR_Delay).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_LR_Delay).c_str()),          "0",     "1.0",
        "回声 左右交叉",           strdup( NTS(MC_Echo_LR_Cross).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_LR_Cross).c_str()),          "0",     "1.0",
        "回声 反馈",            strdup( NTS(MC_Echo_Feedback).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Feedback).c_str()),          "0",     "1.0",
        "回声 阻尼",                strdup( NTS(MC_Echo_Damp).c_str()),              strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Damp).c_str()),              "0",     "1.0",

        "回声矩阵 干湿",         strdup( NTS(MC_Echotron_DryWet).c_str()),        strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_DryWet).c_str()),      "127",     "-1.0",
        "回声矩阵 声像",             strdup( NTS(MC_Echotron_Pan).c_str()),           strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Pan).c_str()),           "0",     "1.0",
        "回声矩阵 速度",           strdup( NTS(MC_Echotron_Tempo).c_str()),         strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "回声矩阵 阻尼",            strdup( NTS(MC_Echotron_Damp).c_str()),          strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Damp).c_str()),          "0",     "1.0",
        "回声矩阵 反馈",        strdup( NTS(MC_Echotron_Feedback).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Feedback).c_str()),    "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "回声矩阵 左右交叉",       strdup( NTS(MC_Echotron_LR_Cross).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LR_Cross).c_str()),      "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "回声矩阵 宽度",           strdup( NTS(MC_Echotron_LFO_Width).c_str()),     strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Width).c_str()),     "0",     "1.0",
        "回声矩阵 深度",           strdup( NTS(MC_Echotron_Depth).c_str()),         strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Depth).c_str()),         "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "回声矩阵 立体声深度",       strdup( NTS(MC_Echotron_LFO_Stereo).c_str()),    strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Stereo).c_str()),    "0",     "1.0",
        "回声矩阵 LFO波形",        strdup( NTS(MC_Echotron_LFO_Type).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Type).c_str()),      "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "回声矩阵 #",               strdup( NTS(MC_Echotron_Taps).c_str()),          strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Taps).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),

        "回声宇宙 干湿",        strdup( NTS(MC_Echoverse_DryWet).c_str()),       strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_DryWet).c_str()),     "127",     "-1.0",
        "回声宇宙 反向",        strdup( NTS(MC_Echoverse_Reverse).c_str()),      strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Reverse).c_str()),      "0",     "1.0",
        "回声宇宙 声像",            strdup( NTS(MC_Echoverse_Pan).c_str()),          strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Pan).c_str()),          "0",     "1.0",
        "回声宇宙 速度",          strdup( NTS(MC_Echoverse_Tempo).c_str()),        strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "回声宇宙 左右延迟",      strdup( NTS(MC_Echoverse_LR_Delay).c_str()),     strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_LR_Delay).c_str()),     "0",     "1.0",
        "回声宇宙 反馈",       strdup( NTS(MC_Echoverse_Feedback).c_str()),     strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Feedback).c_str()),     "0",     "1.0",
        "回声宇宙 阻尼",           strdup( NTS(MC_Echoverse_Damp).c_str()),         strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Damp).c_str()),         "0",     "1.0",
        "回声宇宙 额外立体声",      strdup( NTS(MC_Echoverse_Ext_Stereo).c_str()),   strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Ext_Stereo).c_str()),   "0",     "1.0",
        "回声宇宙 角度",          strdup( NTS(MC_Echoverse_Angle).c_str()),        strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Angle).c_str()),        "0",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "EQ 增益",                  strdup( NTS(MC_EQ_Gain).c_str()),                strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_Gain).c_str()),                "0",     "1.0",
        "EQ Q",                     strdup( NTS(MC_EQ_Q).c_str()),                   strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_Q).c_str()),                   "0",     "1.0",
        "EQ 31 Hz",                 strdup( NTS(MC_EQ_31_HZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_31_HZ).c_str()),               "0",     "1.0",
        "EQ 63 Hz",                 strdup( NTS(MC_EQ_63_HZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_63_HZ).c_str()),               "0",     "1.0",
        "EQ 125 Hz",                strdup( NTS(MC_EQ_125_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_125_HZ).c_str()),              "0",     "1.0",
        "EQ 250 Hz",                strdup( NTS(MC_EQ_250_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_250_HZ).c_str()),              "0",     "1.0",
        "EQ 500 Hz",                strdup( NTS(MC_EQ_500_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_500_HZ).c_str()),              "0",     "1.0",
        "EQ 1 Khz",                 strdup( NTS(MC_EQ_1_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_1_KHZ).c_str()),               "0",     "1.0",
        "EQ 2 Khz",                 strdup( NTS(MC_EQ_2_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_2_KHZ).c_str()),               "0",     "1.0",
        "EQ 4 Khz",                 strdup( NTS(MC_EQ_4_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_4_KHZ).c_str()),               "0",     "1.0",
        "EQ 8 Khz",                 strdup( NTS(MC_EQ_8_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_8_KHZ).c_str()),               "0",     "1.0",
        "EQ 16 Khz",                strdup( NTS(MC_EQ_16_KHZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_16_KHZ).c_str()),              "0",     "1.0",

        "激励器 增益",             strdup( NTS(MC_Exciter_Gain).c_str()),           strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Gain).c_str()),           "0",     "1.0",
        "激励器 LPF",              strdup( NTS(MC_Exciter_LPF).c_str()),            strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_LPF).c_str()),           "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "激励器 HPF",              strdup( NTS(MC_Exciter_HPF).c_str()),            strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_HPF).c_str()),           "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "激励器 谐波1",            strdup( NTS(MC_Exciter_Harm_1).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_1).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波2",            strdup( NTS(MC_Exciter_Harm_2).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_2).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波3",            strdup( NTS(MC_Exciter_Harm_3).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_3).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波4",            strdup( NTS(MC_Exciter_Harm_4).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_4).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波5",            strdup( NTS(MC_Exciter_Harm_5).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_5).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波6",            strdup( NTS(MC_Exciter_Harm_6).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_6).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波7",            strdup( NTS(MC_Exciter_Harm_7).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_7).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波8",            strdup( NTS(MC_Exciter_Harm_8).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_8).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波9",            strdup( NTS(MC_Exciter_Harm_9).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_9).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "激励器 谐波10",           strdup( NTS(MC_Exciter_Harm_10).c_str()),        strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_10).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "扩展器 起音时间",          strdup( NTS(MC_Expander_Attack).c_str()),        strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Attack).c_str()),        "1",     strdup( NTS(C_MC_4999_RANGE).c_str()),
        "扩展器 释放时间",          strdup( NTS(MC_Expander_Release).c_str()),       strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Release).c_str()),      "10",     strdup( NTS(C_MC_990_RANGE).c_str()),
        "扩展器 形状",           strdup( NTS(MC_Expander_Shape).c_str()),         strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Shape).c_str()),         "1",     strdup( NTS(C_MC_49_RANGE).c_str()),
        "扩展器 阈值",       strdup( NTS(MC_Expander_Threshold).c_str()),     strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Threshold).c_str()),     "0",     strdup( NTS(-C_MC_80_RANGE).c_str()),
        "扩展器 输出增益",        strdup( NTS(MC_Expander_Gain).c_str()),          strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Gain).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "扩展器 LPF",             strdup( NTS(MC_Expander_LPF).c_str()),           strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_LPF).c_str()),          "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "扩展器 HPF",             strdup( NTS(MC_Expander_HPF).c_str()),           strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_HPF).c_str()),          "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "镶边 干湿",          strdup( NTS(MC_Flanger_DryWet).c_str()),         strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_DryWet).c_str()),       "127",     "-1.0",
        "镶边 声像",              strdup( NTS(MC_Flanger_Pan).c_str()),            strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Pan).c_str()),            "0",     "1.0",
        "镶边 速度",            strdup( NTS(MC_Flanger_LFO_Tempo).c_str()),      strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "镶边 随机",           strdup( NTS(MC_Flanger_LFO_Random).c_str()),     strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Random).c_str()),     "0",     "1.0",
        "镶边 LFO波形",         strdup( NTS(MC_Flanger_LFO_Type).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "镶边 立体声深度",        strdup( NTS(MC_Flanger_LFO_Stereo).c_str()),     strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Stereo).c_str()),     "0",     "1.0",
        "镶边 深度",            strdup( NTS(MC_Flanger_Depth).c_str()),          strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Depth).c_str()),          "0",     "1.0",
        "镶边 延迟",            strdup( NTS(MC_Flanger_Delay).c_str()),          strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Delay).c_str()),          "0",     "1.0",
        "镶边 反馈",         strdup( NTS(MC_Flanger_Feedback).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Feedback).c_str()),       "0",     "1.0",
        "镶边 左右交叉",        strdup( NTS(MC_Flanger_LR_Cross).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LR_Cross).c_str()),       "0",     "1.0",

        "和声器 干湿",       strdup( NTS(MC_Harm_DryWet).c_str()),            strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_DryWet).c_str()),          "127",     "-1.0",
        "和声器 音程",      strdup( NTS(MC_Harm_Interval).c_str()),          strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Interval).c_str()),          "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "和声器 增益",          strdup( NTS(MC_Harm_Gain).c_str()),              strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Gain).c_str()),              "0",     "1.0",
        "和声器 声像",           strdup( NTS(MC_Harm_Pan).c_str()),               strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Pan).c_str()),               "0",     "1.0",
        "和声器 频率",          strdup( NTS(MC_Harm_Filter_Freq).c_str()),       strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Freq).c_str()),      "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "和声器 滤波增益",   strdup( NTS(MC_Harm_Filter_Gain).c_str()),       strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Gain).c_str()),       "0",     "1.0",
        "和声器 滤波Q",      strdup( NTS(MC_Harm_Filter_Q).c_str()),          strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Q).c_str()),          "0",     "1.0",
        "和声器 选择",        strdup( NTS(MC_Harm_Select).c_str()),            strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Select).c_str()),            "0",     "1.0",
        "和声器 音符",          strdup( NTS(MC_Harm_Note).c_str()),              strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Note).c_str()),              "0",     strdup( NTS(C_MC_23_RANGE).c_str()),
        "和声器 和弦",         strdup( NTS(MC_Harm_Chord).c_str()),             strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Chord).c_str()),             "0",     strdup( NTS(C_MC_33_RANGE).c_str()),

        "无限循环 干湿",         strdup( NTS(MC_Infinity_DryWet).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_DryWet).c_str()),      "127",     "-1.0",
        "无限循环 谐振",             strdup( NTS(MC_Infinity_Resonance).c_str()),     strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Resonance).c_str()), "-1000",     strdup( NTS(C_MC_2000_RANGE).c_str()),
        "无限循环 滤波频段1",   strdup( NTS(MC_Infinity_Band_1).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_1).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段2",   strdup( NTS(MC_Infinity_Band_2).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_2).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段3",   strdup( NTS(MC_Infinity_Band_3).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_3).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段4",   strdup( NTS(MC_Infinity_Band_4).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_4).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段5",   strdup( NTS(MC_Infinity_Band_5).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_5).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段6",   strdup( NTS(MC_Infinity_Band_6).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_6).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段7",   strdup( NTS(MC_Infinity_Band_7).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_7).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 滤波频段8",   strdup( NTS(MC_Infinity_Band_8).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_8).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 自动声像",         strdup( NTS(MC_Infinity_AutoPan).c_str()),       strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_AutoPan).c_str()),       "0",     "1.0",
        "无限循环 立体声深度",       strdup( NTS(MC_Infinity_LR_Delay).c_str()),      strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_LR_Delay).c_str()),    "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "无限循环 起始",           strdup( NTS(MC_Infinity_Start).c_str()),         strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Start).c_str()),         "0",     "1.0",
        "无限循环 结束",             strdup( NTS(MC_Infinity_End).c_str()),           strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_End).c_str()),           "0",     "1.0",
        "无限循环 速度",           strdup( NTS(MC_Infinity_Tempo).c_str()),         strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "无限循环 细分",          strdup( NTS(MC_Infinity_Subdivision).c_str()),   strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Subdivision).c_str()), "-16",     strdup( NTS(C_MC_32_RANGE).c_str()),

        "输入音量",             strdup( NTS(MC_Input_Volume).c_str()),           strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "循环录音 干湿",           strdup( NTS(MC_Looper_DryWet).c_str()),          strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_DryWet).c_str()),        "127",     "-1.0",
        "循环录音 电平1",           strdup( NTS(MC_Looper_Level_1).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Level_1).c_str()),         "0",     "1.0",
        "循环录音 电平2",           strdup( NTS(MC_Looper_Level_2).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Level_2).c_str()),         "0",     "1.0",
        "循环录音 速度",             strdup( NTS(MC_Looper_Tempo).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Tempo).c_str()),          "20",     strdup( NTS(C_MC_360_RANGE).c_str()),
        "循环录音 反向",           strdup( NTS(MC_Looper_Reverse).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Reverse).c_str()),         "0",     "1.0",
        "循环录音 自动播放",         strdup( NTS(MC_Looper_AutoPlay).c_str()),        strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_AutoPlay).c_str()),        "0",     "1.0",
        "循环录音 播放",              strdup( NTS(MC_Looper_Play).c_str()),            strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Play).c_str()),            "0",     "1.0",
        "循环录音 暂停",             strdup( NTS(MC_Looper_Stop).c_str()),            strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Stop).c_str()),            "0",     "1.0",
        "循环录音 录音",            strdup( NTS(MC_Looper_Record).c_str()),          strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Record).c_str()),          "0",     "1.0",
        "Looper R1",                strdup( NTS(MC_Looper_Rec_1).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Rec_1).c_str()),           "0",     "1.0",
        "Looper R2",                strdup( NTS(MC_Looper_Rec_2).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Rec_2).c_str()),           "0",     "1.0",
        "循环录音 轨道1",           strdup( NTS(MC_Looper_Track_1).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Track_1).c_str()),         "0",     "1.0",
        "循环录音 轨道2",           strdup( NTS(MC_Looper_Track_2).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Track_2).c_str()),         "0",     "1.0",
        "循环录音 清除",             strdup( NTS(MC_Looper_Clear).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Clear).c_str()),           "0",     "1.0",

        "多重 开关",             strdup( NTS(MC_Multi_On_Off).c_str()),           strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "音乐延迟 干湿",    strdup( NTS(MC_Music_DryWet).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_DryWet).c_str()),         "127",     "-1.0",
        "音乐延迟 左右交叉",  strdup( NTS(MC_Music_LR_Cross).c_str()),         strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_LR_Cross).c_str()),         "0",     "1.0",
        "音乐延迟 声像1",      strdup( NTS(MC_Music_Pan_1).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Pan_1).c_str()),            "0",     "1.0",
        "音乐延迟 声像2",      strdup( NTS(MC_Music_Pan_2).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Pan_2).c_str()),            "0",     "1.0",
        "音乐延迟 速度",      strdup( NTS(MC_Music_Tempo).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Tempo).c_str()),           "10",     strdup( NTS(C_MC_470_RANGE).c_str()),
        "音乐延迟 增益1",     strdup( NTS(MC_Music_Gain_1).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Gain_1).c_str()),           "0",     "1.0",
        "音乐延迟 增益2",     strdup( NTS(MC_Music_Gain_2).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Gain_2).c_str()),           "0",     "1.0",
        "音乐延迟 反馈1",       strdup( NTS(MC_Music_Feedback_1).c_str()),       strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Feedback_1).c_str()),       "0",     "1.0",
        "音乐延迟 反馈2",       strdup( NTS(MC_Music_Feedback_2).c_str()),       strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Feedback_2).c_str()),       "0",     "1.0",
        "音乐延迟 阻尼",       strdup( NTS(MC_Music_Damp).c_str()),             strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Damp).c_str()),             "0",     "1.0",

        "MuTroMojo 干湿",        strdup( NTS(MC_MuTro_DryWet).c_str()),           strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_DryWet).c_str()),         "127",     "-1.0",
        "MuTroMojo LP",             strdup( NTS(MC_MuTro_LowPass).c_str()),          strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LowPass).c_str()),        "-64",     "1.0",
        "MuTroMojo BP",             strdup( NTS(MC_MuTro_BandPass).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_BandPass).c_str()),       "-64",     "1.0",
        "MuTroMojo HP",             strdup( NTS(MC_MuTro_HighPass).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_HighPass).c_str()),       "-64",     "1.0",
        "MuTroMojo LFO波形",       strdup( NTS(MC_MuTro_LFO_Type).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Type).c_str()),         "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "MuTroMojo 深度",          strdup( NTS(MC_MuTro_Depth).c_str()),            strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Depth).c_str()),            "0",     "1.0",
        "MuTroMojo 速度",          strdup( NTS(MC_MuTro_LFO_Tempo).c_str()),        strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "MuTroMojo 谐振",            strdup( NTS(MC_MuTro_Resonance).c_str()),        strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Resonance).c_str()),        "0",     "1.0",
        "MuTroMojo 范围",          strdup( NTS(MC_MuTro_Range).c_str()),            strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Range).c_str()),           "10",     strdup( NTS(C_MC_5990_RANGE).c_str()),
        "MuTroMojo 哇音",            strdup( NTS(MC_MuTro_Wah).c_str()),              strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Wah).c_str()),              "0",     "1.0",
        "MuTroMojo 包络灵敏度",        strdup( NTS(MC_MuTro_Env_Sens).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Env_Sens).c_str()),       "-64",     "1.0",
        "MuTroMojo 平滑",         strdup( NTS(MC_MuTro_Env_Smooth).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Env_Smooth).c_str()),       "0",     "1.0",
        "MuTroMojo 随机",         strdup( NTS(MC_MuTro_LFO_Random).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Random).c_str()),       "0",     "1.0",
        "MuTroMojo 立体声深度",      strdup( NTS(MC_MuTro_LFO_Stereo).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Stereo).c_str()),       "0",     "1.0",
        "MuTroMojo 起始频率",       strdup( NTS(MC_MuTro_St_Freq).c_str()),          strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_St_Freq).c_str()),         "30",     strdup( NTS(C_MC_770_RANGE).c_str()),

        "噪声门 起音时间",        strdup( NTS(MC_Gate_Attack).c_str()),            strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Attack).c_str()),            "1",     strdup( NTS(C_MC_249_RANGE).c_str()),
        "噪声门 释放时间",        strdup( NTS(MC_Gate_Release).c_str()),           strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Release).c_str()),           "2",     strdup( NTS(C_MC_248_RANGE).c_str()),
        "噪声门 范围",          strdup( NTS(MC_Gate_Range).c_str()),             strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Range).c_str()),           "-90",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "噪声门 阈值",      strdup( NTS(MC_Gate_Threshold).c_str()),         strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Threshold).c_str()),       "-70",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "噪声门 保持",           strdup( NTS(MC_Gate_Hold).c_str()),              strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Hold).c_str()),              "2",     strdup( NTS(C_MC_498_RANGE).c_str()),
        "噪声门 LPF",            strdup( NTS(MC_Gate_LPF).c_str()),               strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "噪声门 HPF",            strdup( NTS(MC_Gate_HPF).c_str()),               strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "光学颤音 深度",        strdup( NTS(MC_Optical_Depth).c_str()),          strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_Depth).c_str()),          "0",     "1.0",
        "光学颤音 速度",        strdup( NTS(MC_Optical_LFO_Tempo).c_str()),      strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "光学颤音 随机",       strdup( NTS(MC_Optical_LFO_Random).c_str()),     strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Random).c_str()),     "0",     "1.0",
        "光学颤音 LFO波形",     strdup( NTS(MC_Optical_LFO_Type).c_str()),       strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "光学颤音 立体声深度",    strdup( NTS(MC_Optical_LFO_Stereo).c_str()),     strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Stereo).c_str()),     "0",     "1.0",
        "光学颤音 声像",          strdup( NTS(MC_Optical_Pan).c_str()),            strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_Pan).c_str()),            "0",     "1.0",

        "输出音量",            strdup( NTS(MC_Output_Volume).c_str()),          strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "过载 干湿",        strdup( NTS(MC_Overdrive_DryWet).c_str()),       strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_DryWet).c_str()),     "127",     "-1.0",
        "过载 左右交叉",      strdup( NTS(MC_Overdrive_LR_Cross).c_str()),     strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_LR_Cross).c_str()),     "0",     "1.0",
        "过载 驱动",          strdup( NTS(MC_Overdrive_Drive).c_str()),        strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Drive).c_str()),        "0",     "1.0",
        "过载 电平",          strdup( NTS(MC_Overdrive_Level).c_str()),        strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Level).c_str()),        "0",     "1.0",
        "过载 类型",           strdup( NTS(MC_Overdrive_Type).c_str()),         strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Type).c_str()),         "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "过载 声像",            strdup( NTS(MC_Overdrive_Pan).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Pan).c_str()),          "0",     "1.0",
        "过载 LPF",            strdup( NTS(MC_Overdrive_LPF).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_LPF).c_str()),         "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "过载 HPF",            strdup( NTS(MC_Overdrive_HPF).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_HPF).c_str()),         "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "声像 干湿",              strdup( NTS(MC_Pan_DryWet).c_str()),             strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_DryWet).c_str()),           "127",     "-1.0",
        "声像 声像",                  strdup( NTS(MC_Pan_Pan).c_str()),                strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_Pan).c_str()),                "0",     "1.0",
        "声像 速度",                strdup( NTS(MC_Pan_LFO_Tempo).c_str()),          strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Tempo).c_str()),          "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "声像 随机",               strdup( NTS(MC_Pan_LFO_Random).c_str()),         strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Random).c_str()),         "0",     "1.0",
        "声像 LFO波形",             strdup( NTS(MC_Pan_LFO_Type).c_str()),           strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Type).c_str()),           "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "声像 立体声深度",            strdup( NTS(MC_Pan_LFO_Stereo).c_str()),         strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Stereo).c_str()),         "0",     "1.0",
        "声像 额外立体声",            strdup( NTS(MC_Pan_Ex_St_Amt).c_str()),          strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_Ex_St_Amt).c_str()),          "0",     "1.0",

        "参数均衡 增益",       strdup( NTS(MC_Parametric_Gain).c_str()),        strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Gain).c_str()),        "0",     "1.0",
        "参数均衡 低频率",   strdup( NTS(MC_Parametric_Low_Freq).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Freq).c_str()),   "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "参数均衡 低增益",   strdup( NTS(MC_Parametric_Low_Gain).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Gain).c_str()),    "0",     "1.0",
        "参数均衡 低Q",      strdup( NTS(MC_Parametric_Low_Q).c_str()),       strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Q).c_str()),       "0",     "1.0",
        "参数均衡 中频率",   strdup( NTS(MC_Parametric_Mid_Freq).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Freq).c_str()),  "800",     strdup( NTS(C_MC_7200_RANGE).c_str()),
        "参数均衡 中增益",   strdup( NTS(MC_Parametric_Mid_Gain).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Gain).c_str()),    "0",     "1.0",
        "参数均衡 中Q",      strdup( NTS(MC_Parametric_Mid_Q).c_str()),       strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Q).c_str()),       "0",     "1.0",
        "参数均衡 高频率",  strdup( NTS(MC_Parametric_High_Freq).c_str()),   strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Freq).c_str()),"6000",     strdup( NTS(C_MC_20000_RANGE).c_str()),
        "参数均衡 高增益",  strdup( NTS(MC_Parametric_High_Gain).c_str()),   strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Gain).c_str()),   "0",     "1.0",
        "参数均衡 高Q",     strdup( NTS(MC_Parametric_High_Q).c_str()),      strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Q).c_str()),      "0",     "1.0",

        "移相 干湿",           strdup( NTS(MC_Phaser_DryWet).c_str()),          strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_DryWet).c_str()),        "127",     "-1.0",
        "移相 声像",               strdup( NTS(MC_Phaser_Pan).c_str()),             strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Pan).c_str()),             "0",     "1.0",
        "移相 速度",             strdup( NTS(MC_Phaser_LFO_Tempo).c_str()),       strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "移相 随机",            strdup( NTS(MC_Phaser_LFO_Random).c_str()),      strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Random).c_str()),      "0",     "1.0",
        "移相 LFO波形",          strdup( NTS(MC_Phaser_LFO_Type).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "移相 相位",             strdup( NTS(MC_Phaser_Phase).c_str()),           strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Phase).c_str()),           "0",     "1.0",
        "移相 立体声深度",         strdup( NTS(MC_Phaser_LFO_Stereo).c_str()),      strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Stereo).c_str()),      "0",     "1.0",
        "移相 深度",             strdup( NTS(MC_Phaser_Depth).c_str()),           strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Depth).c_str()),           "0",     "1.0",
        "移相 反馈",          strdup( NTS(MC_Phaser_Feedback).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Feedback).c_str()),        "0",     "1.0",
        "移相 左右交叉",         strdup( NTS(MC_Phaser_LR_Cross).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LR_Cross).c_str()),        "0",     "1.0",

        "程序切换表",     strdup( NTS(MC_Program_Table).c_str()),          strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "谐振器 干湿",      strdup( NTS(MC_Ressol_DryWet).c_str()),          strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_DryWet).c_str()),        "127",     "-1.0",
        "谐振器 速度",        strdup( NTS(MC_Ressol_LFO_Tempo).c_str()),       strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "谐振器 相位深度",     strdup( NTS(MC_Ressol_Depth).c_str()),           strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_Depth).c_str()),           "0",     "1.0",
        "谐振器 宽度",        strdup( NTS(MC_Ressol_Width).c_str()),           strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_Width).c_str()),           "0",     "1.0",
        "谐振器 反馈",     strdup( NTS(MC_Ressol_Feedback).c_str()),        strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_Feedback).c_str()),        "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "谐振器 失配",     strdup( NTS(MC_Ressol_Mismatch).c_str()),        strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_Mismatch).c_str()),        "0",     "1.0",
        "谐振器 失真",   strdup( NTS(MC_Ressol_Distortion).c_str()),      strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_Distortion).c_str()),      "0",     "1.0",
        "谐振器 立体声深度",   strdup( NTS(MC_Ressol_LFO_Stereo).c_str()),      strdup( NTS(EFX_RESSOLUTION).c_str()),   strdup( NTS(Ressol_LFO_Stereo).c_str()),      "0",     "1.0",
        
        "混响 干湿",           strdup( NTS(MC_Reverb_DryWet).c_str()),          strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_DryWet).c_str()),        "127",     "-1.0",
        "混响 声像",               strdup( NTS(MC_Reverb_Pan).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Pan).c_str()),             "0",     "1.0",
        "混响 时间",              strdup( NTS(MC_Reverb_Time).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Time).c_str()),            "0",     "1.0",
        "混响 初始延迟",     strdup( NTS(MC_Reverb_I_Delay).c_str()),         strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_I_Delay).c_str()),         "0",     "1.0",
        "混响 Del. E/R",          strdup( NTS(MC_Reverb_Delay_FB).c_str()),        strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Delay_FB).c_str()),        "0",     "1.0",
        "混响 房间大小",         strdup( NTS(MC_Reverb_Room).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Room).c_str()),            "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "混响 LPF",               strdup( NTS(MC_Reverb_LPF).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_LPF).c_str()),            "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "混响 HPF",               strdup( NTS(MC_Reverb_HPF).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_HPF).c_str()),            "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "混响 阻尼",           strdup( NTS(MC_Reverb_Damp).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Damp).c_str()),           "64",     strdup( NTS(C_MC_63_RANGE).c_str()),

        "混响矩阵 干湿",       strdup( NTS(MC_Revtron_DryWet).c_str()),         strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_DryWet).c_str()),       "127",     "-1.0",
        "混响矩阵 声像",           strdup( NTS(MC_Revtron_Pan).c_str()),            strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Pan).c_str()),            "0",     "1.0",
        "混响矩阵 电平",         strdup( NTS(MC_Revtron_Level).c_str()),          strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Level).c_str()),          "0",     "1.0",
        "混响矩阵 阻尼",          strdup( NTS(MC_Revtron_Damp).c_str()),           strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Damp).c_str()),           "0",     "1.0",
        "混响矩阵 反馈",      strdup( NTS(MC_Revtron_Feedback).c_str()),       strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Feedback).c_str()),     "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "混响矩阵 长度",        strdup( NTS(MC_Revtron_Length).c_str()),         strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Length).c_str()),        "20",     strdup( NTS(C_MC_1480_RANGE).c_str()),
        "混响矩阵 拉伸",       strdup( NTS(MC_Revtron_Stretch).c_str()),        strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Stretch).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "混响矩阵 初始延迟", strdup( NTS(MC_Revtron_I_Delay).c_str()),        strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_I_Delay).c_str()),        "0",     strdup( NTS(C_MC_500_RANGE).c_str()),
        "混响矩阵 淡出",          strdup( NTS(MC_Revtron_Fade).c_str()),           strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Fade).c_str()),           "0",     "1.0",
        "混响矩阵 扩散",     strdup( NTS(MC_Revtron_Diffusion).c_str()),      strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Diffusion).c_str()),      "0",     "1.0",
        "混响矩阵 LPF",           strdup( NTS(MC_Revtron_LPF).c_str()),            strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_LPF).c_str()),           "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),

        "环形 干湿",             strdup( NTS(MC_Ring_DryWet).c_str()),            strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_DryWet).c_str()),          "127",     "-1.0",
        "环形 左右交叉",           strdup( NTS(MC_Ring_LR_Cross).c_str()),          strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_LR_Cross).c_str()),        "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "环形 输入",               strdup( NTS(MC_Ring_Input).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Input).c_str()),             "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "环形 电平",               strdup( NTS(MC_Ring_Level).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Level).c_str()),             "0",     "1.0",
        "环形 声像",                 strdup( NTS(MC_Ring_Pan).c_str()),               strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Pan).c_str()),             "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "环形 深度",               strdup( NTS(MC_Ring_Depth).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Depth).c_str()),             "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "环形 频率",                strdup( NTS(MC_Ring_Freq).c_str()),              strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Freq).c_str()),              "1",     strdup( NTS(C_MC_19999_RANGE).c_str()),
        "环形 正弦",                strdup( NTS(MC_Ring_Sine).c_str()),              strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Sine).c_str()),              "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "环形 三角波",            strdup( NTS(MC_Ring_Triangle).c_str()),          strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Triangle).c_str()),          "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "环形 锯齿波",            strdup( NTS(MC_Ring_Saw).c_str()),               strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Saw).c_str()),               "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "环形 方波",              strdup( NTS(MC_Ring_Square).c_str()),            strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Square).c_str()),            "0",     strdup( NTS(C_MC_100_RANGE).c_str()),

        "序列 干湿",         strdup( NTS(MC_Sequence_DryWet).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_DryWet).c_str()),      "127",     "-1.0",
        "序列 1",               strdup( NTS(MC_Sequence_Step_1).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_1).c_str()),        "0",     "1.0",
        "序列 2",               strdup( NTS(MC_Sequence_Step_2).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_2).c_str()),        "0",     "1.0",
        "序列 3",               strdup( NTS(MC_Sequence_Step_3).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_3).c_str()),        "0",     "1.0",
        "序列 4",               strdup( NTS(MC_Sequence_Step_4).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_4).c_str()),        "0",     "1.0",
        "序列 5",               strdup( NTS(MC_Sequence_Step_5).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_5).c_str()),        "0",     "1.0",
        "序列 6",               strdup( NTS(MC_Sequence_Step_6).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_6).c_str()),        "0",     "1.0",
        "序列 7",               strdup( NTS(MC_Sequence_Step_7).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_7).c_str()),        "0",     "1.0",
        "序列 8",               strdup( NTS(MC_Sequence_Step_8).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_8).c_str()),        "0",     "1.0",
        "序列 速度",           strdup( NTS(MC_Sequence_Tempo).c_str()),         strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "序列 Q",               strdup( NTS(MC_Sequence_Resonance).c_str()),     strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Resonance).c_str()),     "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "序列 立体声深度",       strdup( NTS(MC_Sequence_Stdf).c_str()),          strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Stdf).c_str()),          "0",     strdup( NTS(C_MC_7_RANGE).c_str()),

        "搁架提升 增益",          strdup( NTS(MC_Shelf_Gain).c_str()),             strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Gain).c_str()),             "0",     "1.0",
        "搁架提升 电平",         strdup( NTS(MC_Shelf_Level).c_str()),            strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Level).c_str()),            "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "搁架提升 音色",          strdup( NTS(MC_Shelf_Tone).c_str()),             strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Tone).c_str()),           "220",     strdup( NTS(C_MC_15780_RANGE).c_str()),
        "搁架提升 存在感",      strdup( NTS(MC_Shelf_Presence).c_str()),         strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Presence).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "移调器 干湿",          strdup( NTS(MC_Shifter_DryWet).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_DryWet).c_str()),       "127",     "-1.0",
        "移调器 音程",         strdup( NTS(MC_Shifter_Interval).c_str()),       strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Interval).c_str()),       "0",     strdup( NTS(C_MC_12_RANGE).c_str()),
        "移调器 增益",             strdup( NTS(MC_Shifter_Gain).c_str()),           strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Gain).c_str()),           "0",     "1.0",
        "移调器 声像",              strdup( NTS(MC_Shifter_Pan).c_str()),            strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Pan).c_str()),            "0",     "1.0",
        "移调器 起音",           strdup( NTS(MC_Shifter_Attack).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Attack).c_str()),         "1",     strdup( NTS(C_MC_1999_RANGE).c_str()),
        "移调器 衰减",            strdup( NTS(MC_Shifter_Decay).c_str()),          strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Decay).c_str()),          "1",     strdup( NTS(C_MC_1999_RANGE).c_str()),
        "移调器 阈值",        strdup( NTS(MC_Shifter_Threshold).c_str()),      strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Threshold).c_str()),    "-70",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "移调器 Whammy",           strdup( NTS(MC_Shifter_Whammy).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Whammy).c_str()),         "0",     "1.0",

        "洗牌均衡 干湿",          strdup( NTS(MC_Shuffle_DryWet).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_DryWet).c_str()),       "127",     "-1.0",
        "洗牌均衡 低频率",         strdup( NTS(MC_Shuffle_Freq_L).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_L).c_str()),        "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "洗牌均衡 低增益",         strdup( NTS(MC_Shuffle_Gain_L).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_L).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "洗牌均衡 中低频率",        strdup( NTS(MC_Shuffle_Freq_ML).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_ML).c_str()),      "400",     strdup( NTS(C_MC_3600_RANGE).c_str()),
        "洗牌均衡 中低增益",        strdup( NTS(MC_Shuffle_Gain_ML).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_ML).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "洗牌均衡 中高频率",        strdup( NTS(MC_Shuffle_Freq_MH).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_MH).c_str()),     "1200",     strdup( NTS(C_MC_6800_RANGE).c_str()),
        "洗牌均衡 中高增益",        strdup( NTS(MC_Shuffle_Gain_MH).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_MH).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "洗牌均衡 高频率",        strdup( NTS(MC_Shuffle_Freq_H).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_H).c_str()),      "6000",     strdup( NTS(C_MC_20000_RANGE).c_str()),
        "洗牌均衡 高增益",        strdup( NTS(MC_Shuffle_Gain_H).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_H).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "洗牌均衡 Q",                strdup( NTS(MC_Shuffle_Width).c_str()),          strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Width).c_str()),        "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "立体声和声 干湿",       strdup( NTS(MC_Sharm_DryWet).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_DryWet).c_str()),         "127",     "-1.0",
        "立体声和声 左音程",         strdup( NTS(MC_Sharm_L_Interval).c_str()),       strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Interval).c_str()),       "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "立体声和声 左半音",        strdup( NTS(MC_Sharm_L_Chroma).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Chroma).c_str()),     "-2000",     strdup( NTS(C_MC_4000_RANGE).c_str()),
        "立体声和声 左增益",        strdup( NTS(MC_Sharm_L_Gain).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Gain).c_str()),           "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "立体声和声 右音程",         strdup( NTS(MC_Sharm_R_Interval).c_str()),       strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Interval).c_str()),       "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "立体声和声 右半音",        strdup( NTS(MC_Sharm_R_Chroma).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Chroma).c_str()),     "-2000",     strdup( NTS(C_MC_4000_RANGE).c_str()),
        "立体声和声 右增益",        strdup( NTS(MC_Sharm_R_Gain).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Gain).c_str()),           "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "立体声和声 左右交叉",     strdup( NTS(MC_Sharm_LR_Cross).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_LR_Cross).c_str()),         "0",     "1.0",
        "立体声和声 选择",        strdup( NTS(MC_Sharm_Select).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Select).c_str()),           "0",     "1.0",
        "立体声和声 音符",          strdup( NTS(MC_Sharm_Note).c_str()),             strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Note).c_str()),             "0",     strdup( NTS(C_MC_23_RANGE).c_str()),
        "立体声和声 和弦",         strdup( NTS(MC_Sharm_Chord).c_str()),            strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Chord).c_str()),            "0",     strdup( NTS(C_MC_33_RANGE).c_str()),

        "踏板箱 电平",           strdup( NTS(MC_Stomp_Level).c_str()),            strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Level).c_str()),            "0",     "1.0",
        "踏板箱 增益",            strdup( NTS(MC_Stomp_Gain).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Gain).c_str()),             "0",     "1.0",
        "踏板箱 偏置",            strdup( NTS(MC_Stomp_Bias).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Bias).c_str()),           "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "踏板箱 中频",             strdup( NTS(MC_Stomp_Mid).c_str()),              strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Mid).c_str()),            "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "踏板箱 音色",            strdup( NTS(MC_Stomp_Tone).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Tone).c_str()),           "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "踏板箱 模式",            strdup( NTS(MC_Stomp_Mode).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Mode).c_str()),             "0",     strdup( NTS(C_MC_8_RANGE).c_str()),

        "延音器 增益",           strdup( NTS(MC_Sustain_Gain).c_str()),           strdup( NTS(EFX_SUSTAINER).c_str()),     strdup( NTS(Sustain_Gain).c_str()),           "0",     "1.0",
        "延音器 延音",        strdup( NTS(MC_Sustain_Sustain).c_str()),        strdup( NTS(EFX_SUSTAINER).c_str()),     strdup( NTS(Sustain_Sustain).c_str()),        "1",     strdup( NTS(C_MC_126_RANGE).c_str()),

        "合成滤波 干湿",      strdup( NTS(MC_Synthfilter_DryWet).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_DryWet).c_str()),   "127",     "-1.0",
        "合成滤波 失真度",      strdup( NTS(MC_Synthfilter_Distort).c_str()),    strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Distort).c_str()),    "0",     "1.0",
        "合成滤波 速度",        strdup( NTS(MC_Synthfilter_LFO_Tempo).c_str()),  strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Tempo).c_str()),  "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "合成滤波 随机",       strdup( NTS(MC_Synthfilter_LFO_Random).c_str()), strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Random).c_str()), "0",     "1.0",
        "合成滤波 LFO波形",     strdup( NTS(MC_Synthfilter_LFO_Type).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Type).c_str()),   "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "合成滤波 立体声深度",    strdup( NTS(MC_Synthfilter_LFO_Stereo).c_str()), strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Stereo).c_str()), "0",     "1.0",
        "合成滤波 宽度",        strdup( NTS(MC_Synthfilter_Width).c_str()),      strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Width).c_str()),      "0",     "1.0",
        "合成滤波 反馈",     strdup( NTS(MC_Synthfilter_Feedback).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Feedback).c_str()), "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "合成滤波 深度",        strdup( NTS(MC_Synthfilter_Depth).c_str()),      strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Depth).c_str()),      "0",     "1.0",
        "合成滤波 包络灵敏度",       strdup( NTS(MC_Synthfilter_Env_Sens).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Env_Sens).c_str()), "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "合成滤波 起音时间",       strdup( NTS(MC_Synthfilter_Attack).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Attack).c_str()),     "5",     strdup( NTS(C_MC_995_RANGE).c_str()),
        "合成滤波 释放时间",       strdup( NTS(MC_Synthfilter_Release).c_str()),    strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Release).c_str()),    "5",     strdup( NTS(C_MC_495_RANGE).c_str()),
        "合成滤波 偏移",       strdup( NTS(MC_Synthfilter_Offset).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Offset).c_str()),     "0",     "1.0",

        "电子管 干湿",            strdup( NTS(MC_Valve_DryWet).c_str()),           strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_DryWet).c_str()),         "127",     "-1.0",
        "电子管 左右交叉",          strdup( NTS(MC_Valve_LR_Cross).c_str()),         strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_LR_Cross).c_str()),         "0",     "1.0",
        "电子管 声像",                strdup( NTS(MC_Valve_Pan).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Pan).c_str()),              "0",     "1.0",
        "电子管 电平",              strdup( NTS(MC_Valve_Level).c_str()),            strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Level).c_str()),            "0",     "1.0",
        "电子管 驱动",              strdup( NTS(MC_Valve_Drive).c_str()),            strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Drive).c_str()),            "0",     "1.0",
        "电子管 失真",               strdup( NTS(MC_Valve_Distortion).c_str()),       strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Distortion).c_str()),       "0",     "1.0",
        "电子管 存在感",           strdup( NTS(MC_Valve_Presence).c_str()),         strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Presence).c_str()),         "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "电子管 LPF",                strdup( NTS(MC_Valve_LPF).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_LPF).c_str()),             "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "电子管 HPF",                strdup( NTS(MC_Valve_HPF).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_HPF).c_str()),             "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "变频段 干湿",         strdup( NTS(MC_VaryBand_DryWet).c_str()),        strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_DryWet).c_str()),      "127",     "-1.0",
        "变频段 速度1",         strdup( NTS(MC_VaryBand_LFO_Tempo_1).c_str()),   strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Tempo_1).c_str()),   "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "变频段 LFO1波形",      strdup( NTS(MC_VaryBand_LFO_Type_1).c_str()),    strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Type_1).c_str()),    "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "变频段 立体声深度1",         strdup( NTS(MC_VaryBand_LFO_Stereo_1).c_str()),  strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Stereo_1).c_str()),  "0",     "1.0",
        "变频段 速度2",         strdup( NTS(MC_VaryBand_LFO_Tempo_2).c_str()),   strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Tempo_2).c_str()),   "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "变频段 LFO2波形",      strdup( NTS(MC_VaryBand_LFO_Type_2).c_str()),    strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Type_2).c_str()),    "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "变频段 立体声深度2",         strdup( NTS(MC_VaryBand_LFO_Stereo_2).c_str()),  strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Stereo_2).c_str()),  "0",     "1.0",
        "变频段 交叉1",         strdup( NTS(MC_VaryBand_Cross_1).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "变频段 交叉2",         strdup( NTS(MC_VaryBand_Cross_2).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_2).c_str()),    "1000",     strdup( NTS(C_MC_7000_RANGE).c_str()),
        "变频段 交叉3",         strdup( NTS(MC_VaryBand_Cross_3).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_3).c_str()),    "2000",     strdup( NTS(C_MC_24000_RANGE).c_str()),

        "颤音 干湿",             strdup( NTS(MC_Vibe_DryWet).c_str()),            strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_DryWet).c_str()),          "127",     "-1.0",
        "颤音 宽度",               strdup( NTS(MC_Vibe_Width).c_str()),             strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Width).c_str()),             "0",     "1.0",
        "颤音 深度",               strdup( NTS(MC_Vibe_Depth).c_str()),             strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Depth).c_str()),             "0",     "1.0",
        "颤音 速度",               strdup( NTS(MC_Vibe_LFO_Tempo).c_str()),         strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "颤音 随机",              strdup( NTS(MC_Vibe_LFO_Random).c_str()),        strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Random).c_str()),        "0",     "1.0",
        "颤音 LFO波形",            strdup( NTS(MC_Vibe_LFO_Type).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Type).c_str()),          "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "颤音 立体声深度",           strdup( NTS(MC_Vibe_LFO_Stereo).c_str()),        strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Stereo).c_str()),        "0",     "1.0",
        "颤音 反馈",            strdup( NTS(MC_Vibe_Feedback).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Feedback).c_str()),          "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "颤音 左右交叉",           strdup( NTS(MC_Vibe_LR_Cross).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LR_Cross).c_str()),          "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "颤音 声像",                 strdup( NTS(MC_Vibe_Pan).c_str()),               strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Pan).c_str()),               "0",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "声码器 干湿",          strdup( NTS(MC_Vocoder_DryWet).c_str()),         strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_DryWet).c_str()),       "127",     "-1.0",
        "声码器 声像",              strdup( NTS(MC_Vocoder_Pan).c_str()),            strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Pan).c_str()),            "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "声码器 输入",            strdup( NTS(MC_Vocoder_Input).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Input).c_str()),          "0",     "1.0",
        "声码器 涂抹",            strdup( NTS(MC_Vocoder_Smear).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Smear).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "声码器 Q",                strdup( NTS(MC_Vocoder_Q).c_str()),              strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Q).c_str()),             "40",     strdup( NTS(C_MC_130_RANGE).c_str()),
        "声码器 环形",             strdup( NTS(MC_Vocoder_Ring).c_str()),           strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Ring).c_str()),           "0",     "1.0",
        "声码器 电平",            strdup( NTS(MC_Vocoder_Level).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Level).c_str()),          "0",     "1.0",

        "哇音 干湿",           strdup( NTS(MC_WahWah_DryWet).c_str()),          strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_DryWet).c_str()),        "127",     "-1.0",
        "哇音 声像",               strdup( NTS(MC_WahWah_Pan).c_str()),             strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Pan).c_str()),             "0",     "1.0",
        "哇音 速度",             strdup( NTS(MC_WahWah_LFO_Tempo).c_str()),       strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "哇音 随机",            strdup( NTS(MC_WahWah_LFO_Random).c_str()),      strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Random).c_str()),      "0",     "1.0",
        "哇音 LFO波形",          strdup( NTS(MC_WahWah_LFO_Type).c_str()),        strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_LFO_RNGE).c_str()),
        "哇音 立体声深度",         strdup( NTS(MC_WahWah_LFO_Stereo).c_str()),      strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Stereo).c_str()),      "0",     "1.0",
        "哇音 深度",             strdup( NTS(MC_WahWah_Depth).c_str()),           strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Depth).c_str()),           "0",     "1.0",
        "哇音 振幅灵敏度",            strdup( NTS(MC_WahWah_Sense).c_str()),           strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Sense).c_str()),           "0",     "1.0",
        "哇音 振幅灵敏度反",          strdup( NTS(MC_WahWah_ASI).c_str()),             strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_ASI).c_str()),             "0",     "1.0",
        "哇音 平滑",            strdup( NTS(MC_WahWah_Smooth).c_str()),          strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Smooth).c_str()),          "0",     "1.0",
    };

    // If any additional parameters are added, then the constant
    // C_MC_PARAMETER_SIZE must be adjusted.
    for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
    {
        RKRP::strlcpy(mc_efx_params[i].Description, los_params[i * 6], sizeof(mc_efx_params[i].Description));
        sscanf(los_params[i * 6 + 1], "%d", &mc_efx_params[i].MC_params_index);
        sscanf(los_params[i * 6 + 2], "%d", &mc_efx_params[i].Effect_index);
        sscanf(los_params[i * 6 + 3], "%d", &mc_efx_params[i].Efx_param_index);
        sscanf(los_params[i * 6 + 4], "%d", &mc_efx_params[i].MC_offset);
        sscanf(los_params[i * 6 + 5], "%lf", &mc_efx_params[i].MC_range);
    }
}

/**
 *  Alsa MIDI connection.
 *  If auto connect is requested, then make the connections.
 */
void
RKR::ConnectMIDI()
{
    if (Config.aconnect_MI)
        Conecta();
}

void
RKR::InitMIDI()
{
#ifndef RKR_PLUS_LV2
#ifdef ALSA_SUPPORT
    // Open Alsa Seq
    int err = snd_seq_open(&midi_in, "default", SND_SEQ_OPEN_INPUT, 0);
    if (err < 0)
        printf("Cannot activate ALSA seq client\n");
    snd_seq_set_client_name(midi_in, jackcliname);
    snd_config_update_free_global();

    char portname[70];

    // Create Alsa Seq Client
    snprintf(portname, sizeof(portname), "%s IN", jackcliname);
    snd_seq_create_simple_port(midi_in, portname,
                               SND_SEQ_PORT_CAP_WRITE |
                               SND_SEQ_PORT_CAP_SUBS_WRITE,
                               SND_SEQ_PORT_TYPE_SYNTH);
#endif
#endif
}

void
RKR::miramidi()
{
#ifndef RKR_PLUS_LV2
#ifdef ALSA_SUPPORT
    if (snd_seq_event_input_pending(midi_in, 1))
    {
        do
        {
            midievents();
        }
        while (snd_seq_event_input_pending(midi_in, 0));
    }
#endif
#endif
}

void
RKR::midievents()
{
#ifndef RKR_PLUS_LV2
#ifdef ALSA_SUPPORT
    int i;
    snd_seq_event_t *midievent;
    midievent = NULL;
    snd_seq_event_input(midi_in, &midievent);
    
    if (midievent == NULL)
        return;
    
    if (midievent->type == 42)
        return;

    if ((Tap_Active) && (Tap_Selection == 3) && (midievent->type == SND_SEQ_EVENT_CLOCK))
    {
        mtc_counter++;
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((EFX_Active[EFX_LOOPER]) && (Tap_Selection == 3))
    {
        if (midievent->type == SND_SEQ_EVENT_START)
        {
            Rack_Effects[EFX_LOOPER]->changepar(Looper_Play, 1);
            Gui_Refresh = GUI_Refresh_Looper;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            Rack_Effects[EFX_LOOPER]->changepar(Looper_Stop, 1);
            Gui_Refresh = GUI_Refresh_Looper;
        }
    }

    if ((midievent->type == SND_SEQ_EVENT_NOTEON)
        || (midievent->type == SND_SEQ_EVENT_NOTEOFF))
    {
        int cmdnote = midievent->data.note.note;
        int cmdvelo = midievent->data.note.velocity;

        if ((Tap_Active) && (Tap_Selection == 1) && (midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
            Tap_TempoSet = TapTempo();

        if (midievent->data.note.channel == Config.Harmonizer_MIDI_Channel)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (midievent->data.note.channel == Config.StereoHarm_MIDI_Channel)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }


    if (midievent->type == SND_SEQ_EVENT_PGMCHANGE)
    {
        if (midievent->data.control.channel == Config.MIDI_In_Channel)
        {
            if (!Config.custom_midi_table)
            {
                if ((midievent->data.control.value > 0)
                    && (midievent->data.control.value < 61))
                    Change_Preset = midievent->data.control.value;

                if (midievent->data.control.value == 81)
                    if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;

                if (midievent->data.control.value == 82)
                    if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
            }
            else
            {
                int bank = MIDI_Table[midievent->data.control.value].bank;
                int preset = MIDI_Table[midievent->data.control.value].preset + 1;
                process_midi_controller_events(0, bank, preset);  // 0 is CC 0 Bank Select
            }
        }
    }

    if (midievent->type == SND_SEQ_EVENT_CONTROLLER)
    {
        if (midievent->data.control.channel == Config.MIDI_In_Channel)
        {
            if (RControl)
            {
                ControlGet = (int) midievent->data.control.param;
                return;
            }
            
            // Bank_Select = CC 0
            if(midievent->data.control.param == 0)
            {
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
            }
            else if (Config.MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (Active_Preset.XUserMIDI[(int) midievent->data.control.param][i])
                        process_midi_controller_events(Active_Preset.XUserMIDI[(int) midievent->data.control.param][i],
                                                       (int) midievent->data.control.value);
                    else break;
                }
            }
            else
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
        }
    }
#ifdef SYSEX_SUPPORT
    if (midievent->type == SND_SEQ_EVENT_SYSEX)
    {
        /* temp for midi data */
        unsigned char buffer[0x1000];
        bool sysex = false;
        
        snd_midi_event_t *midi_ev;
        snd_midi_event_new(sizeof(buffer), &midi_ev);
        long bytes = snd_midi_event_decode(midi_ev, buffer, sizeof(buffer), midievent);

        if (bytes <= 0)
        {
            snd_midi_event_free( midi_ev );
            return;
        }

        start_sysex( );
        sysex = append_sysex( buffer, bytes );

        /* sysex messages might be more than one message */
        while (sysex)
        {
            snd_seq_event_input(midi_in, &midievent);

            bytes = snd_midi_event_decode(midi_ev, buffer, sizeof(buffer), midievent);

            if (bytes > 0)
                sysex = append_sysex( buffer, bytes );
            else
                sysex = false;
        }

        snd_midi_event_free( midi_ev );

        parse_sysex();
    }
#endif // SYSEX_SUPPORT
#endif
#endif // #ifndef RKR_PLUS_LV2
}

void
RKR::ActOnOff()
{
    if (OnOffC < 63) OnOffC++;
}

void
RKR::ActiveUn(int value)
{
    int numef;
    int inoff = 0;
    int miraque = 0;

    if (value < 20)
    {
        numef = value / 2;
        inoff = checkonoff(efx_order[numef]); // value % 2;
        miraque = efx_order[numef];
        ActOnOff();
        Mnumeff[OnOffC] = numef;
    }
    else if (value < 121)
    {
        numef = value - 20;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 1000 + numef;
    }
    else
    {
        numef = value;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 2000 + numef;
    }
    
    if(miraque < EFX_NUMBER_EFFECTS)
    {
        for(int i = 0; i < EFX_NUMBER_EFFECTS; i++)
        {
            if(miraque == i)
            {
                if (inoff) EFX_Active[i] = 1;
                else EFX_Active[i] = 0;
                return;
            }
        }
    }
    else
    {
        switch (miraque)
        {
            case EFX_TAP_TEMPO_ON_OFF:
                if (inoff) Tap_Active = 1;
                else Tap_Active = 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (inoff) MIDIConverter_Active = 1;
                else MIDIConverter_Active = 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (inoff) Tuner_Active = 1;
                else Tuner_Active = 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (inoff) Active_Preset.FX_Master_Active = 1;
                else Active_Preset.FX_Master_Active = 0;
            break;
        }
    }
}

int
RKR::checkonoff(int miraque)
{
    if(miraque < EFX_NUMBER_EFFECTS)
    {
        for(int i = 0; i < EFX_NUMBER_EFFECTS; i++)
        {
            if(miraque == i)
            {
                if (EFX_Active[i])
                {
                    return (0);
                }
                else
                    return (1);
            }
        }
    }
    else
    {
        switch (miraque)
        {
            case EFX_TAP_TEMPO_ON_OFF:
                if (Tap_Active) return 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (MIDIConverter_Active) return 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (Tuner_Active) return 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (Active_Preset.FX_Master_Active) return 0;
            break;
        }
    }

    return (1);
}

void
RKR::Conecta()
{
    FILE *fp;

    int client = 0;
    int puerto = 0;

    if (IsCoIn)
        disconectaaconnect();

    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        char temp[128];
        char temp1[128];
        char temp2[128];
        char *nume;
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "Client") != NULL)
            {
                RKRP::strlcpy(temp1, temp, sizeof(temp1));
                strtok(temp1, " ");
                nume = strtok(NULL, "\"");
                sscanf(nume, "%d", &client);
            }

            if (strstr(temp, "Port") != NULL)
            {
                RKRP::strlcpy(temp2, temp, sizeof(temp2));
                strtok(temp2, " ");
                nume = strtok(NULL, "  ");
                sscanf(nume, "%d", &puerto);
                
                std::string s_name = jackcliname;
                s_name += " IN";
                
                if (strstr(temp, s_name.c_str()) != 0)
                {
                    Cyoin = client;
                    Pyoin = puerto;
                }
                
                if (strstr(temp, Config.MID) != 0)
                {
                    Ccin = client;
                    Pcin = puerto;
                }
            }
        }

        fclose(fp);
    }

    conectaaconnect();
};

void
RKR::conectaaconnect()
{
    if (strlen(Config.MID) != 0)
    {
        char tempi[128];
        memset(tempi, 0, sizeof (tempi));
        snprintf(tempi, sizeof(tempi), "aconnect %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        
        if (system(tempi) == -1)
        {
            Handle_Message(29);
        }
        else
            IsCoIn = 1;
    }
}

void
RKR::disconectaaconnect()
{
    if (strlen(Config.MID) != 0)
    {
        char tempi[128];
        memset(tempi, 0, sizeof (tempi));
        snprintf(tempi, sizeof(tempi), "aconnect -d %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        if (system(tempi) == -1)
        {
            Handle_Message(29);
        }
        else
            IsCoIn = 0;
    }
}

#ifndef RKR_PLUS_LV2
void
RKR::jack_process_midievents(jack_midi_event_t *midievent)
{
    int i;
    int type = midievent->buffer[0] >> 4;

    // 0xf8 is MIDI message clock
    if ((Tap_Active) && (Tap_Selection == 3) && (midievent->buffer[0] == 0xf8))
    {
        mtc_counter++;
        
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    // Note ON and Note OFF
    if ((type == 8) || (type == 9))
    {
        int cmdnote = midievent->buffer[1];
        int cmdvelo = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if ((Tap_Active) && (Tap_Selection == 1) && (type == 9) && (cmdvelo != 0)) Tap_TempoSet = TapTempo();

        if (cmdchan == Config.Harmonizer_MIDI_Channel)
        {
            for (i = 0; i < POLY; i++)
            {
                // Note ON with > 0 velocity
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                // Note ON with zero velocity, treat as note OFF
                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                // Note OFF
                if (type == 8)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (cmdchan == Config.StereoHarm_MIDI_Channel)
        {
            for (i = 0; i < POLY; i++)
            {
                // Note ON with > 0 velocity
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                // Note ON with zero velocity, treat as note OFF
                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                // Note OFF
                if (type == 8)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }

    // Program Change
    if (type == 12)
    {
        int cmdvalue = midievent->buffer[1];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == Config.MIDI_In_Channel)
        {
            if (!Config.custom_midi_table)
            {
                if ((cmdvalue > 0)
                    && (cmdvalue < 61))
                    Change_Preset = cmdvalue;

                if (cmdvalue == 81) if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;
                
                if (cmdvalue == 82) if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
            }
            else
            {
                int bank = MIDI_Table[cmdvalue].bank;
                int preset = MIDI_Table[cmdvalue].preset + 1;
                process_midi_controller_events(0, bank, preset);    // 0 is CC 0 Bank Select
            }
        }
    }

    // Control Change
    if (type == 11)
    {
        int cmdcontrol = midievent->buffer[1];
        int cmdvalue = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == Config.MIDI_In_Channel)
        {
            if (RControl)
            {
                ControlGet = cmdcontrol;
                return;
            }
            
            // Bank_Select = CC 0
            if(cmdcontrol == 0)
            {
                process_midi_controller_events(cmdcontrol, cmdvalue);
            }
            else if (Config.MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (Active_Preset.XUserMIDI[cmdcontrol][i])
                        process_midi_controller_events(Active_Preset.XUserMIDI[cmdcontrol][i], cmdvalue);
                    else break;
                }
            }
            else
                process_midi_controller_events(cmdcontrol, cmdvalue);
        }
    }
#ifdef SYSEX_SUPPORT
    if(type == 15)
    {
        start_sysex();
        append_sysex(midievent->buffer, midievent->size);
        parse_sysex();
    }
#endif
}
#endif  // #ifndef RKR_PLUS_LV2

#ifdef RKR_PLUS_LV2
static void* check_program_change(void * _RKR)
{
    RKR * rkr = (RKR *) _RKR;

    while (rkr->Exit_Program)
    {
        if (rkr->Change_Preset != C_CHANGE_PRESET_OFF)
        {
            if ((rkr->Change_Preset > 0) && (rkr->Change_Preset < 61))
            {
                rkr->active_bank_preset_to_main_window(rkr->Change_Preset);

                // reset these if volume changed from preset
                rkr->calculavol(1);
                rkr->calculavol(2);
                rkr->booster = 1.0f;
            }

            // hold the preset number so we can update the bank window highlight TODO
            rkr->hold_preset = rkr->Change_Preset;
            rkr->Change_Preset = C_CHANGE_PRESET_OFF;
        }
        usleep(1500);
    }

    return 0;
}

void
RKR::lv2_process_midi_program_changes()
{
    Exit_Program = 1;
    int result = pthread_create(&t_pgm, nullptr, check_program_change, this);
    if(result != 0)
    {
        Handle_Message (52, "pthread_create - at lv2_process_midi_program_changes().");
    }
}

void
RKR::lv2_join_thread()
{
    Exit_Program = 0;
    if(t_pgm)
    {
        int result = pthread_join(t_pgm, nullptr);
        if(result != 0)
        {
            Handle_Message (52, "pthread_join - at lv2_join_thread().");
        }
    }
}
#endif

void
RKR::lv2_process_midievents(const uint8_t* const msg)
{
#ifdef RKR_PLUS_LV2

    if ((Tap_Active) && (Tap_Selection == 3) && (msg[0] == LV2_MIDI_MSG_CLOCK))
    {
        mtc_counter++;

        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    int i;
    uint8_t statusByte = msg[0];
    int channel = 0;
    if ((statusByte & 0xF0) >= 0x80 && (statusByte & 0xF0) <= 0xEF)
    {
        channel = statusByte & 0x0F; // Extract channel from the status byte
       // printf("Channel = %d\n", channel);
    }

    switch (lv2_midi_message_type(msg))
    {
        case LV2_MIDI_MSG_NOTE_ON:
        {
            //printf("NOTE ON - msg[0] %hhu: NOTE-msg[1] %hhu: VELOCITY-msg[2] %hhu\n", msg[0], msg[1], msg[2]);
            // Set tap tempo based on any note on
            if ((Tap_Active) && (Tap_Selection == 1) &&  (msg[2] != 0))
                Tap_TempoSet = TapTempo();

            if (channel == Config.Harmonizer_MIDI_Channel)
            {
                for (i = 0; i < POLY; i++)
                {
                    // Note ON with > 0 velocity
                    if (msg[2] != 0)
                    {
                        if (RC_Harm->note_active[i] == 0)
                        {
                            RC_Harm->note_active[i] = 1;
                            RC_Harm->rnote[i] = msg[1];
                            RC_Harm->gate[i] = 1;
                            RC_Harm->MiraChord();
                            break;
                        }
                    }

                    // Note ON with zero velocity, treat as note OFF
                    if (msg[2] == 0)
                    {
                        if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == msg[1]))
                        {
                            RC_Harm->note_active[i] = 0;
                            RC_Harm->gate[i] = 0;
                            break;
                        }
                    }
                }
            }

            if (channel == Config.StereoHarm_MIDI_Channel)
            {
                for (i = 0; i < POLY; i++)
                {
                    // Note ON with > 0 velocity
                    if (msg[2] != 0)
                    {
                        if (RC_Stereo_Harm->note_active[i] == 0)
                        {
                            RC_Stereo_Harm->note_active[i] = 1;
                            RC_Stereo_Harm->rnote[i] = msg[1];
                            RC_Stereo_Harm->gate[i] = 1;
                            RC_Stereo_Harm->MiraChord();
                            break;
                        }
                    }

                    // Note ON with zero velocity, treat as note OFF
                    if (msg[2] == 0)
                    {
                        if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == msg[1]))
                        {
                            RC_Stereo_Harm->note_active[i] = 0;
                            RC_Stereo_Harm->gate[i] = 0;
                            break;
                        }
                    }
                }
            }
            break;
        }
        case LV2_MIDI_MSG_NOTE_OFF:
        {
           // printf("NOTE OFF - msg[0] %hhu: msg[1] %hhu: msg[2] %hhu\n", msg[0], msg[1], msg[2]);
            if (channel == Config.Harmonizer_MIDI_Channel)
            {
                for (i = 0; i < POLY; i++)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == msg[1]))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
            if (channel == Config.StereoHarm_MIDI_Channel)
            {
                for (i = 0; i < POLY; i++)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == msg[1]))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
            break;
        }
        case LV2_MIDI_MSG_PGM_CHANGE:
        {
//            printf("Program change %hhu\n", msg[1]);
            int cmdvalue = msg[1];

            if (channel == Config.MIDI_In_Channel)
            {
                if (!Config.custom_midi_table)
                {
                    if ((cmdvalue > 0)
                        && (cmdvalue < 61))
                        Change_Preset = cmdvalue;

                    if (cmdvalue == 81) if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;

                    if (cmdvalue == 82) if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
                }
                else
                {
                    int bank = MIDI_Table[cmdvalue].bank;
                    int preset = MIDI_Table[cmdvalue].preset + 1;
                    process_midi_controller_events(0, bank, preset);    // 0 is CC 0 Bank Select
                }
            }

            break;
        }
        case LV2_MIDI_MSG_CONTROLLER:
        {
 //           printf("Got CC %hhu: Control Change %hhu: Value %hhu\n", msg[0], msg[1], msg[2]);

            int cmdcontrol = msg[1];
            int cmdvalue = msg[2];

            if (channel == Config.MIDI_In_Channel)
            {
                if (RControl)
                {
                    ControlGet = cmdcontrol;
                    return;
                }

                // Bank_Select = CC 0
                if(cmdcontrol == 0)
                {
                    process_midi_controller_events(cmdcontrol, cmdvalue);
                }
                else if (Config.MIDIway)
                {
                    for (i = 0; i < 20; i++)
                    {
                        if (Active_Preset.XUserMIDI[cmdcontrol][i])
                            process_midi_controller_events(Active_Preset.XUserMIDI[cmdcontrol][i], cmdvalue);
                        else break;
                    }
                }
                else
                    process_midi_controller_events(cmdcontrol, cmdvalue);
            }
            break;
        }

        default: break;
    }

#endif  // #ifdef RKR_PLUS_LV2
}

void
RKR::lv2_set_bpm(float a_bpm)
{
#ifdef RKR_PLUS_LV2
    // no need to check active and selection here since it is done already before
    // it is sent.
//    if ((Tap_Active) && (Tap_Selection == 2))
//    {
        if ((a_bpm > 19) && (a_bpm < 360) && (a_bpm != Tap_TempoSet))
        {
            Tap_TempoSet = a_bpm;
            Update_tempo();
            Tap_Display = 1;
        }
//    }
#endif
}

/*
 * process MIDI controller events
 */
void
RKR::process_midi_controller_events(int parameter, int value, int preset)
{
    // Don't process MIDI control when updating quality since
    // the efx may be deleted
    if(quality_update)
        return;
    
    // Flags used for Gui update from MIDI control - used by RKRGUI::MIDI_control_gui_refresh()
    if (parameter > 0)
    {
        Mcontrol[parameter] = 1;    // The parameter number that needs updating
        Mvalue = 1;                 // Flag to indicate at least one parameter needs update
    }

    // Special cases
    switch (parameter)
    {
        // Bank Select is Hard coded to CC 0
        case MC_Bank_Select:    // CC 0
        {
            if(value < (int) Bank_Vector.size())
            {
                if(active_bank != value)
                {
                    copy_bank(Bank, Bank_Vector[value].Bank);
                    active_bank = Change_Bank = value;      // Change_Bank is for GUI update
                }
            }
            
            // From custom program change table.
            // We must set the preset after the bank change.
            if(preset != C_CHANGE_PRESET_OFF)
            {
                Change_Preset = preset;
            }
            return;
        }

        case MC_Unused_10:
        case MC_Unused_11:
        case MC_Unused_13:
        case MC_Unused_15:
        case MC_Unused_16:
        case MC_Unused_17:
        case MC_Unused_18:
        case MC_Unused_19:

        case MC_Unused_33:
        case MC_Unused_34:
        case MC_Unused_35:
        case MC_Unused_36:
        case MC_Unused_37:
        case MC_Unused_38:
        case MC_Unused_39:
        case MC_Unused_40:
        case MC_Unused_41:
        case MC_Unused_42:
        case MC_Unused_43:
        case MC_Unused_44:
        case MC_Unused_45:

        case MC_Unused_64:

        case MC_Unused_128:
        case MC_Unused_129:
            return;

        case MC_Program_Table:
        {
            if(set_midi_table (value))
                Change_MIDI_Table = value;  // GUI update

            return;
        }

        case MC_Output_Volume:
            Active_Preset.Master_Volume = (float) value / 127.0f;
            calculavol(2);
            return;

        case MC_Balance_FX:
            Active_Preset.Fraction_Bypass = (float) value / 127.0f;
            return;

        case MC_Input_Volume:
            Active_Preset.Input_Gain = (float) value / 127.0f;
            calculavol(1);
            return;

        case MC_Multi_On_Off:
            ActiveUn(value);
            return;
    }

    // Normal MIDI processing of rack effects
    for (int param_index = 0; param_index < C_MC_PARAMETER_SIZE; param_index++)
    {
        if(mc_efx_params[param_index].MC_params_index == parameter)
        {
            // Get the MIDI control item to change
            int effect_index = mc_efx_params[param_index].Effect_index;
            int efx_param_index = mc_efx_params[param_index].Efx_param_index;
            int mc_offset = mc_efx_params[param_index].MC_offset;
            double range = mc_efx_params[param_index].MC_range;
            
            // Apply the change to the effect
            Rack_Effects[effect_index]->changepar(efx_param_index, mc_offset + ( (int) ((float) value * range)));
            return;
        }
    }
}

#ifdef SYSEX_SUPPORT
void
RKR::start_sysex( void  )
{
    m_sysex.clear();
}

bool 
RKR::append_sysex( unsigned char *a_data, long a_size )
{
    bool ret = true;

    for ( int i=0; i<a_size; i++ )
    {

        m_sysex.push_back( a_data[i] );
        if ( a_data[i] == EVENT_SYSEX_END )
            ret = false;
    }

    return ret;
}

/**
 * http://www.indiana.edu/~emusic/etext/MIDI/chapter3_MIDI9.shtml
 * A System Exclusive code set begins with 11110000 (240 decimal or F0 hex),
 * followed by the manufacturer ID#, then by an unspecified number of
 * data bytes of any ranges from 0-127) and ends with 11110111
 * (decimal 247 or F7 hex), meaning End of SysEx message. No other coded
 * are to be transmitted during a SysEx message (except a system real time
 * message). Normally, after the manufacturer ID, each maker will have its
 * own instrument model subcode, so a Yamaha DX7 will ignore a Yamaha SY77's
 * patch dump. In addition, most instruments have a SysEx ID # setting so
 * more than one of the same instruments can be on a network but not
 * necessarily respond to a patch dump not intended for it.
 * 
 * Dec 125 = hex 7D is the Universal Non-Commercial identification byte,
 * designed for use by Universities, researchers, etc.
 *
 * 
 * * * * * *  Format of Rakarrack-plus sysex save message * * * * * *
    EVENT_SYSEX                                             // byte 0 0xF0
    const unsigned char c_non_commercial_ID = 0x7D;         // byte 1
    const unsigned long c_RKRP_subcode      = 0x524B5250;   // bytes 2 - 5
    const unsigned char c_sysex_type        = 0x01;         // byte 6 - (For sysex type - 0x01)
    Bank save location (Dec 3 - 127)                        // byte 7
    Preset location (Dec 1 - 60)                            // byte 8
    Preset name (ascii hex max 22 characters)               // bytes 9 to ??? max 31 variable size
    end sysex 0xF7                                          // byte ??  (End message)
 */
void RKR::parse_sysex()
{
    unsigned char *data = m_sysex.data();

    // Check the manufacturer ID
    const unsigned char c_non_commercial_ID = 0x7D;         // byte 1
    if(data[1] != c_non_commercial_ID)
        return;

    // Check the subcode
    const unsigned long c_RKRP_subcode      = 0x524B5250;   // bytes 2 - 5
    unsigned long subcode = 0;

    subcode += (data[2] << 24);
    subcode += (data[3] << 16);
    subcode += (data[4] << 8);
    subcode += (data[5]);

    if(subcode != c_RKRP_subcode)
        return;

    // We are good to go 
    std::string preset_name = "";
    unsigned bank_number = data[7];
    unsigned preset_number = data[8];

    for(unsigned i = 9; i < (m_sysex.size() - 1); i++)
    {
        preset_name += (char) m_sysex[i];
    }

    //printf("Preset name = %s: bank = %d: p_num = %d\n", preset_name.c_str(), bank_number, preset_number);

    m_preset_name = preset_name;
    m_bank_number = bank_number;
    m_preset_number = preset_number;
    m_have_sysex_message = data[6];
}

void RKR::sysex_save_preset()
{
    if((m_bank_number >= Bank_Vector.size()) || (m_bank_number < 3))
    {
        fprintf(stderr, "Invalid Bank save request!\n");
        return;
    }

    if((m_preset_number < 1) || (m_preset_number > 60))
    {
        fprintf(stderr, "Invalid preset number save request!\n");
        return;
    }
    
    if(m_preset_name.size() > 22 || m_preset_name.empty())
    {
        fprintf(stderr, "Invalid preset name size!\n");
        return;
    }

    PresetBankStruct Save_Bank[62];
    
    // Copy the requested bank to be saved
    copy_bank(Save_Bank, Bank_Vector[m_bank_number].Bank);
    
    // Update active preset for any user changes
    refresh_active_preset();
    
    // Set the preset name for the active preset
    memset(Active_Preset.Preset_Name, 0, sizeof (char) * 64);
    RKRP::strlcpy(Active_Preset.Preset_Name, m_preset_name.c_str(), sizeof(Active_Preset.Preset_Name));
    
    // Copy the active preset to the save bank
    Save_Bank[m_preset_number] = Active_Preset;
    
    // Get the filename for the requested bank
    std::string filename = Bank_Vector[m_bank_number].Bank_File_Name;
    
    // Save the bank
    save_bank(filename, Save_Bank);
    
    // Update the Bank Vector
    load_bank_vector();
}
#endif  // SYSEX_SUPPORT
