/*
  rakarrack - a guitar effects software

 varios.C  -  some extra functions
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

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Widget.H>
#include "icono_rakarrack_32x32.xpm"
#include "global.h"
#include "process.h"

int
RKR::Message(int prio, const std::string &labelwin, const std::string &message_text)
{
    if ((Config.Disable_Warnings) && (prio == 0)) return (0);
    
    if(Gui_Shown)
    {
        Fl_Widget *w = fl_message_icon();

        Fl_Image *img = new Fl_Pixmap(icono_rakarrack_32x32);
        w->color(FL_WHITE);
        w->label("");
        w->image(img);
        w->align(FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        if(w->parent())
            w->parent()->copy_label(labelwin.c_str());

        // Need to shut off below mouse or it tries to modify the fl_message widget and crash.
        Shut_Off_Below_Mouse = 1;
        fl_message("%s", message_text.c_str());
        Shut_Off_Below_Mouse = 0;
    }
    else
    {
        fprintf(stderr, "%s\n", message_text.c_str());
    }

    return (0);

};

void
RKR::Handle_Message(int num, std::string strMessage)
{
    // Flag to indicate message severity.
    // 1 = always show, even if messages are turned off in settings.
    // 0 = only show if messages are turned on in settings.
    int message_type = 1;

    char meslabel[70];
    char error_msg[256];
    memset(meslabel, 0, sizeof (meslabel));
    snprintf(meslabel, sizeof(meslabel), "%s %s", jackcliname, VERSION);

    global_error_number = 0;
    memset(error_msg, 0, sizeof (error_msg));

    switch (num)
    {
        case 1:
            snprintf(error_msg, sizeof(error_msg), "%s", "卷积混响无法打开音频 .wav 文件。");
            break;
        case 2:
            snprintf(error_msg, sizeof(error_msg), "%s", "混响矩阵无法打开 IR .rvb 文件");
            break;
        case 3:
            snprintf(error_msg, sizeof(error_msg), "%s", "写入文件出错。你是否有此目录的写入权限？");
            break;
        case Dly_Open:
            snprintf(error_msg, sizeof(error_msg), "%s", "回声矩阵无法打开 .dly 文件");
            break;
        case Dly_Pan:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个声像参数超出范围");
            break;
        case Dly_Time:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个时间参数超出范围");
            break;
        case Dly_Level:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个电平参数超出范围");
            break;
        case Dly_LP:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个低通参数超出范围");
            break;
        case Dly_BP:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个带通参数超出范围");
            break;
        case Dly_HP:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个高通参数超出范围");
            break;
        case Dly_Freq:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个频率参数超出范围");
            break;
        case Dly_Q:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个 Q 参数超出范围");
            break;
        case Dly_Stages:
            snprintf(error_msg, sizeof(error_msg), "%s", ".dly 文件中某个级数参数超出范围");
            break;
        case 14:
            snprintf(error_msg, sizeof(error_msg), "载入文件出错 %s", strMessage.c_str());
            break;
        case 15:
            snprintf(error_msg, sizeof(error_msg), "载入文件顺序出错 %s", strMessage.c_str());
            break;
        case 16:
            snprintf(error_msg, sizeof(error_msg), "载入文件版本出错 %s", strMessage.c_str());
            break;
        case 17:
            snprintf(error_msg, sizeof(error_msg), "载入文件作者出错 %s", strMessage.c_str());
            break;
        case 18:
            snprintf(error_msg, sizeof(error_msg), "载入文件预置名出错 %s", strMessage.c_str());
            break;
        case 19:
            snprintf(error_msg, sizeof(error_msg), "载入文件常规项出错 %s", strMessage.c_str());
            break;
        case 20:
            snprintf(error_msg, sizeof(error_msg), "载入文件 MIDI 出错 %s", strMessage.c_str());
            break;
        case 21:
            snprintf(error_msg, sizeof(error_msg), "%s", "load_names() 读取错误");
            break;
        case 22:
            snprintf(error_msg, sizeof(error_msg), "%s", "load_bank() 读取错误");
            break;
        case 23:
            snprintf(error_msg, sizeof(error_msg), "读取文件出错 %s", strMessage.c_str());
            break;
        case 24:
            snprintf(error_msg, sizeof(error_msg), "%s", "运行 rakconvert 出错！");
            break;
        case 25:
            snprintf(error_msg, sizeof(error_msg), "%s", "运行 rakverb 出错！");
            break;
        case 26:
            snprintf(error_msg, sizeof(error_msg), "%s", "移除内部预置出错！");
            break;
        case 27:
            snprintf(error_msg, sizeof(error_msg), "%s", "合并内部预置出错！");
            break;
        case 28:
            snprintf(error_msg, sizeof(error_msg), "add_bank_item() 读取错误 %s", strMessage.c_str());
            break;
        case 29:
            snprintf(error_msg, sizeof(error_msg), "%s", "运行 aconnect 出错！");
            break;
        case 30:
            snprintf(error_msg, sizeof(error_msg), "%s", "无法载入此音色库文件，它来自旧版 rakarrack，"
                    "\n 请使用音色库窗口中的“转换旧版音色库”菜单项。");
            break;
        case 31:
            snprintf(error_msg, sizeof(error_msg), "%s", "无法载入此音色库文件，它来自旧版 rakarrack git 版本，"
                    "\n 请使用 rakgit2new 工具转换。");
            break;
        case 32:
            snprintf(error_msg, sizeof(error_msg), "%s", "！！Rakarrack-plus CPU 占用警告！！\n"
                    "当前设置下你的 CPU 似乎难以轻松处理卷积运算。\n"
                    "请谨慎调整卷积混响效果的参数。\n"
                    "更多信息请阅读帮助 (F1)。");
            message_type = 0;
            break;
        case 33:
            snprintf(error_msg, sizeof(error_msg), "%s", "JACK 已关闭，请尝试保存你的工作");
            break;
        case 34:
            snprintf(error_msg, sizeof(error_msg), "%s", "无法创建 JACK 客户端，jackd 是否正在运行？");
            break;
        case 35:
            snprintf(error_msg, sizeof(error_msg),"请现在尝试载入新文件");
            break;
        case 36:
            snprintf(error_msg, sizeof(error_msg),"此文件已是新格式");
            break;
        case 37:
            snprintf(error_msg, sizeof(error_msg),"请现在用混响矩阵载入新的 '.rvb' 文件");
            break;
        case 38:
            snprintf(error_msg, sizeof(error_msg), "此设置将在下次运行 rakarrack-plus 时生效");
            break;
        case 39:
            snprintf(error_msg, sizeof(error_msg), "内部预置无法被删除 ");
            break;
        case 40:
            snprintf(error_msg, sizeof(error_msg), "在用户目录中找不到音色库文件 %s\n\n"
                    "所有用户音色库必须放在以下位置设置的用户目录中：\n"
                    "设置/首选项/用户 - 用户目录", strMessage.c_str());
            break;
        case 41:
            snprintf(error_msg, sizeof(error_msg),"User Directory is not set!\n\n"
                    "You must set a User Directory in :\n"
                    "Settings/Preferences/User - User Directory.");
            break;
        case 42:
            snprintf(error_msg, sizeof(error_msg), "在用户目录中找不到 MIDI 程序文件：\n%s\n\n"
                    "所有 MIDI 程序文件应放在以下位置设置的用户目录中：\n"
                    "设置/首选项/用户 - 用户目录", strMessage.c_str());
            break;
        case 43:
        {
            Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
            strMessage =  Efx_Convolotron->Filename;
            snprintf(error_msg, sizeof(error_msg), "在用户目录中找不到卷积混响用户文件：\n%s\n\n"
                "所有用户文件必须放在以下位置设置的用户目录中：\n"
                "设置/首选项/用户 - 用户目录", strMessage.c_str());
        }
            break;
        case 44:
        {
            Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
            strMessage =  Efx_Echotron->Filename;
            snprintf(error_msg, sizeof(error_msg), "在用户目录中找不到回声矩阵用户文件：\n%s\n\n"
                "所有用户文件必须放在以下位置设置的用户目录中：\n"
                "设置/首选项/用户 - 用户目录", strMessage.c_str());
        }
            break;
        case 45:
        {
            Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
            strMessage =  Efx_Reverbtron->Filename;
            snprintf(error_msg, sizeof(error_msg), "在用户目录中找不到混响矩阵用户文件：\n%s\n\n"
                "所有用户文件必须放在以下位置设置的用户目录中：\n"
                "设置/首选项/用户 - 用户目录", strMessage.c_str());
        }
            break;
        case 46:
        {
            snprintf(error_msg, sizeof(error_msg), "无法访问用户目录：\n%s\n"
                    "你是否有相应权限？\n"
                    "用户目录是否为可读写的有效文件夹？", strMessage.c_str());
        }
            break;
        case 47:
        {
            snprintf(error_msg, sizeof(error_msg), "无法访问数据目录：\n%s\n"
                    "必须安装 Rakarrack-plus 才能访问默认预置音色库。\n", strMessage.c_str());
        }
            break;
        case 48:
        {
            int effect_number = atoi( strMessage.c_str() );
            std::string effect_name = "";
            
            switch(effect_number)
            {
            case EFX_LOOPER:
                effect_name = "循环录音";
                break;
            case EFX_VOCODER:
                effect_name = "声码器";
                break;
            }
            
            snprintf(error_msg, sizeof(error_msg), "不支持导出 %s 插件。\n"
                    "导出时将被忽略...\n", effect_name.c_str());
            message_type = 0;
        }
            break;
        case 49:
        {
            snprintf(error_msg, sizeof(error_msg), "排除效果器数量无效 = %s\n"
                    "生成随机预置时最多只能排除 %d 个效果器\n"
                    "\n", strMessage.c_str(), C_MAX_EXCLUDED);
        }
            break;
        case 50:
        {
            snprintf(error_msg, sizeof(error_msg), "预置名称重复：%s\n"
                    "请为预置换一个名称。\n", strMessage.c_str());
            message_type = 0;
        }
            break;
        case 51:
        {
            snprintf(error_msg, sizeof(error_msg), "预置名称中不能使用逗号：%s\n", strMessage.c_str());
        }
            break;
        case 52:
        {
            snprintf(error_msg, sizeof(error_msg), "线程中发生错误：%s\n", strMessage.c_str());
        }
            break;
    }

    Message(message_type, meslabel, error_msg);

}

/**
 *  Checks CPU usage
 * 
 * @return 
 *  1 if /proc/cpuinfo can be opened.
 *  0 if cannot be opened.
 *  Currently not used.
 */
int
RKR::Get_Bogomips()
{
    char *tmp = NULL;
    char *tmp2 = NULL;
    FILE *fp;

    if ((fp = fopen("/proc/cpuinfo", "r")) != NULL)
    {
        char temp[256];
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "bogomips") != NULL)
            {
                tmp = strdup(temp);
                tmp2 = tmp; // needed since strsep alters original == memory leak
                strsep(&tmp2, ":");
                sscanf(tmp2, "%f", &bogomips);
                break;
            }
        }

        int maxx_len = lrintf(150.0f / 4800.0f * bogomips);
        
        if (Config.upsample)
        {
            maxx_len /= (Config.UpAmo + 8);
            maxx_len /= (6 - ((Config.UpQual + Config.DownQual) / 2));
            // printf("Max Len: %d\n",maxx_len);

        }
        
        if (maxx_len < 5)
        {
            // if (maxx_len < 2) maxx_len = 2;

            if(Re_init_in_progress || lv2_is_active)
                need_bogomips_message = true;
            else
                Handle_Message(32);
        }

        fclose(fp);
        free(tmp);
        return (1);
    }

    return (0);
}

int
RKR::TapTempo()
{
    gettimeofday(&timeA, NULL);

    double Aseconds = ((double) timeA.tv_sec + (double) timeA.tv_usec * 0.000001);

    if (Tap_Selection == 0)
    {
        double latency = 0.04;
        Aseconds += latency;
    }

    double timediff = Aseconds - Tap_timeB;

    if (timediff < 3.0f)
    {
        if (timediff > 0.01f)
        {
            tempobuf[tempocnt] = 60.0f / ((double) timediff);
        }

        if ((++tempocnt) >= 5)
        {
            tempocnt = 0;
        }
    }

    double AvTempo = 0.0;

    for (int i = 0; i < 4; i++)
    {
        AvTempo += tempobuf[i];
    }

    AvTempo *= 0.25;
    Tap_TempoSet = lrint(AvTempo);
    Tap_timeB = Aseconds;
    Tap_timeC = Aseconds;
    
    if (Tap_Selection)
    {
        Tap_Display = 1;
    }

    Update_tempo();

    return (Tap_TempoSet);
}

void
RKR::TapTempo_Timeout(int state)
{
    gettimeofday(&timeA, NULL);
    double Aseconds = (double) timeA.tv_sec + (double) timeA.tv_usec * 0.000001;
    
    if (!state)
    {
        Tap_timeC = Aseconds;
        return;
    }
    
    double timediff = Aseconds - Tap_timeC;
    
    if (timediff > 8.0f)
    {
        Tap_Display = 2;
    }
}

void
RKR::Update_tempo()
{
    if ((Tap_TempoSet < 1) || (Tap_TempoSet > 600))
        return;

    int Tap_TempoSetD = Tap_TempoSet;
    int Tap_TempoSetL = Tap_TempoSet;

    switch (Tap_SetValue)
    {
    case 0:
        Tap_TempoSetD = Tap_TempoSetL = Tap_TempoSet;
        break;
    case 1:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 2:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 3:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 4:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 5:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 6:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 7:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 8:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 9:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 10:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 11:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 12:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 13:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 14:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 15:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 16:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 17:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 18:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 19:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 20:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 21:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 22:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 23:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 24:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    }

    if ((Tap_TempoSetL < 1) || (Tap_TempoSetL > 600))
    {
        Tap_TempoSetL = Tap_TempoSet;
    }
    
    if ((Tap_TempoSetD < 1) || (Tap_TempoSetD > 600))
    {
        Tap_TempoSetD = Tap_TempoSet;
    }

    if (EFX_Active[EFX_LOOPER])
    {
        Looper *Efx_Looper = static_cast <Looper*> (Rack_Effects[EFX_LOOPER]);
        Efx_Looper->settempo(Tap_TempoSet);
    }

    if (EFX_Active[EFX_CHORUS]) Rack_Effects[EFX_CHORUS]->changepar(Chorus_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_FLANGER]) Rack_Effects[EFX_FLANGER]->changepar(Flanger_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_PHASER]) Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_PAN]) Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_WAHWAH]) Rack_Effects[EFX_WAHWAH]->changepar(WahWah_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_ALIENWAH]) Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_MUSICAL_DELAY]) Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Tempo, Tap_TempoSetD);
    if (EFX_Active[EFX_ANALOG_PHASER]) Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_DUAL_FLANGE]) Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_SYNTHFILTER]) Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_MUTROMOJO]) Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_VARYBAND]) Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Tempo_1, Tap_TempoSetL);
    if (EFX_Active[EFX_VARYBAND]) Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Tempo_2, Tap_TempoSetL);
    if (EFX_Active[EFX_ARPIE]) Rack_Effects[EFX_ARPIE]->changepar(Arpie_Tempo, Tap_TempoSetD);
    if (EFX_Active[EFX_ECHOVERSE]) Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Tempo, Tap_TempoSetD);
    if (EFX_Active[EFX_SEQUENCE]) Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Tempo, Tap_TempoSetD);
    if (EFX_Active[EFX_ECHOTRON]) Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Tempo, Tap_TempoSetD);
    if (EFX_Active[EFX_OPTICALTREM]) Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_VIBE]) Rack_Effects[EFX_VIBE]->changepar(Vibe_LFO_Tempo, Tap_TempoSetL);
    if (EFX_Active[EFX_INFINITY]) Rack_Effects[EFX_INFINITY]->changepar(Infinity_Tempo, Tap_TempoSetL);

    if (EFX_Active[EFX_ECHO])
    {
        Echo *Efx_Echo = static_cast<Echo*>(Rack_Effects[EFX_ECHO]);
        Efx_Echo->Tempo2Delay(Tap_TempoSetD);
    }

}

