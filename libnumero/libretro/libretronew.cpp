//#define OLDCORE

#ifdef OLDCORE
#include "libretro.cpp"
#else


//using STB library to load the PNG files
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "calcimage.h"

#include <stdlib.h>
#include <libretro.h>
#include <libretro_core_options_new.h>

#if defined(__DJGPP__) && defined(__STRICT_ANSI__)
/* keep this above libretro-common includes */
#undef __STRICT_ANSI__
#endif

#include <streams/file_stream.h>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <cmath>

#include "../../ezdib/ezdib.h"

#include "stdafx.h"
#include "corecalc.h"
#include "83phw.h"
#include "device.h"
#include "var.h"
#include "lcd.h"
#include "neil_controller.h"

std::string debugText;
calc_t mycalc;
bool debugMode = false;
#define FRAME_SUBDIVISIONS 1024

#ifdef _3DS
extern "C" void* linearMemAlign(size_t size, size_t alignment);
extern "C" void linearFree(void* mem);
#endif

retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

void toast_message(char* message);
char toast_buffer[250];

static uint32_t* video_buf;
static bool libretro_supports_option_categories = false;
static std::string rom_path;
bool buttonPressed = false;
int virtualMouseSpeed = 1;
int virtualMouseX = 50;
int virtualMouseY = 270;
bool virtualMouseMode = false;
int frameCounter = 0;
bool bigMode = false;
bool lastBigModePressed = false;
bool gamingButtons = false;
bool hasBios = false;
unsigned char* calcScreen = NULL;

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_BUFF_SIZE (VIDEO_WIDTH * VIDEO_HEIGHT * sizeof(uint32_t))
#define VIDEO_PITCH (VIDEO_WIDTH * sizeof(uint32_t))
#define CALC_WIDTH 128
#define CALC_HEIGHT 64

HEZDIMAGE hDib;
HEZDFONT hFont;
char user_header[EZD_HEADER_SIZE];


struct NeilButtons neilbuttons;
struct NeilButtons lastButtons;
unsigned char screen_converted[128 * 64 * 4];
bool has_ti_rom = false;
char savetempdir[400];
char saveprogressdir[400];
void setSaveDir();
void setProgressDir(char* rom_name);

typedef struct {
    const char* text;
    int group;
    int bit;
} key_string_t;

#include <vector>
std::vector<NeilVirtualButton> virtualButtons;

#include <stdarg.h>

// LIBRETRO CALLBACKS
void PrintDebugOutput(const char* text, ...)
{
    static char buf[1024];
    va_list va;
    va_start(va, text);
    //vfprintf(stderr, fmt, va);
    vsnprintf(buf, sizeof(buf), text, va);
    va_end(va);


#ifdef UWPDEBUG
    wchar_t wtext[1024];
    mbstowcs(wtext, buf, strlen(buf) + 1);//Plus null
    LPWSTR ptr = wtext;
    OutputDebugString(ptr);
#else
    printf("%s", buf);
#endif
}

void drawButtonHover(struct NeilVirtualButton* button)
{

    if (button->hover)
    {

        int color = 50 << 16 | 255 << 8 | 50;

        ezd_rect(hDib, button->x, button->y,
            button->x + button->width, button->y + button->height, color);


        if (button->pressed)
        {
            //TODO alpha channel for ezdib
            //SDL_SetRenderDrawColor(renderer, 50, 230, 50, 150);
            //SDL_RenderFillRect(renderer, &menuRect);
        }
    }

}

NeilVirtualButton make_virtual_button(int x,int y,int width,int height,bool hover,bool pressed,int* key) {
    NeilVirtualButton button = { x, y, width, height, hover, pressed, key };
    return button;
}

void initVirtualButtons()
{

 /*int x;
   int y;
   int width;
   int height;
   bool hover;
   bool pressed;
   int* key;*/

    virtualButtons.push_back(make_virtual_button(510, 88, 544, 136, false, false, &neilbuttons.leftKey));
    virtualButtons.push_back(make_virtual_button(545,130,594,161,false,false,&neilbuttons.downKey ));
    virtualButtons.push_back(make_virtual_button(596,88,630,136,false,false,&neilbuttons.rightKey ));
    virtualButtons.push_back(make_virtual_button(545,62,595,94,false,false,&neilbuttons.upKey ));

    virtualButtons.push_back(make_virtual_button(331,23,379,45,false,false,&neilbuttons.yequals ));
    virtualButtons.push_back(make_virtual_button(391,23,441,45,false,false,&neilbuttons.window ));
    virtualButtons.push_back(make_virtual_button(453,23,501,45,false,false,&neilbuttons.zoom ));
    virtualButtons.push_back(make_virtual_button(512,23,562,45,false,false,&neilbuttons.trace ));
    virtualButtons.push_back(make_virtual_button(573,23,623,45,false,false,&neilbuttons.graph ));

    virtualButtons.push_back(make_virtual_button(331, 87, 380, 119,false,false,&neilbuttons.second ));
    virtualButtons.push_back(make_virtual_button(390, 87, 442, 119,false,false,&neilbuttons.mode ));
    virtualButtons.push_back(make_virtual_button(451, 87, 502, 119,false,false,&neilbuttons.del ));

    virtualButtons.push_back(make_virtual_button(330,130,380,162,false,false,&neilbuttons.alpha ));
    virtualButtons.push_back(make_virtual_button(389,130,442,162,false,false,&neilbuttons.xt ));
    virtualButtons.push_back(make_virtual_button(448,130,501,162,false,false,&neilbuttons.stat ));

    virtualButtons.push_back(make_virtual_button(327,172,379,206,false,false,&neilbuttons.math ));
    virtualButtons.push_back(make_virtual_button(390,172,440,206,false,false,&neilbuttons.apps ));
    virtualButtons.push_back(make_virtual_button(450,172,501,204,false,false,&neilbuttons.prgm ));
    virtualButtons.push_back(make_virtual_button(510,172,562,204,false,false,&neilbuttons.vars ));
    virtualButtons.push_back(make_virtual_button(570,172,622,204,false,false,&neilbuttons.clear ));

    virtualButtons.push_back(make_virtual_button(328,216,379,248,false,false,&neilbuttons.xneg1 ));
    virtualButtons.push_back(make_virtual_button(388,216,441,248,false,false,&neilbuttons.sin ));
    virtualButtons.push_back(make_virtual_button(450,216,501,248,false,false,&neilbuttons.cos ));
    virtualButtons.push_back(make_virtual_button(510,216,561,248,false,false,&neilbuttons.tan ));
    virtualButtons.push_back(make_virtual_button(571,216,622,250,false,false,&neilbuttons.triangle ));

    virtualButtons.push_back(make_virtual_button(328,260,379,292,false,false,&neilbuttons.xsquared ));
    virtualButtons.push_back(make_virtual_button(389,260,440,292,false,false,&neilbuttons.comma ));
    virtualButtons.push_back(make_virtual_button(450,260,501,292,false,false,&neilbuttons.parenopen ));
    virtualButtons.push_back(make_virtual_button(510,260,562,292,false,false,&neilbuttons.parenclose ));
    virtualButtons.push_back(make_virtual_button(571,260,622,292,false,false,&neilbuttons.divide ));

    virtualButtons.push_back(make_virtual_button(327,302,379,337,false,false,&neilbuttons.log ));
    virtualButtons.push_back(make_virtual_button(388,302,441,337,false,false,&neilbuttons.key7 ));
    virtualButtons.push_back(make_virtual_button(449,302,501,337,false,false,&neilbuttons.key8 ));
    virtualButtons.push_back(make_virtual_button(510,302,561,337,false,false,&neilbuttons.key9 ));
    virtualButtons.push_back(make_virtual_button(570,302,622,337,false,false,&neilbuttons.mul ));

    virtualButtons.push_back(make_virtual_button(328,347,380,380,false,false,&neilbuttons.ln ));
    virtualButtons.push_back(make_virtual_button(388,344,439,380,false,false,&neilbuttons.key4 ));
    virtualButtons.push_back(make_virtual_button(449,344,501,380,false,false,&neilbuttons.key5 ));
    virtualButtons.push_back(make_virtual_button(510,344,561,380,false,false,&neilbuttons.key6 ));
    virtualButtons.push_back(make_virtual_button(570,347,621,380,false,false,&neilbuttons.minus ));

    virtualButtons.push_back(make_virtual_button(328,389,380,423,false,false,&neilbuttons.store ));
    virtualButtons.push_back(make_virtual_button(389,387,439,423,false,false,&neilbuttons.key1 ));
    virtualButtons.push_back(make_virtual_button(449,387,501,423,false,false,&neilbuttons.key2 ));
    virtualButtons.push_back(make_virtual_button(510,387,561,423,false,false,&neilbuttons.key3 ));
    virtualButtons.push_back(make_virtual_button(570,390,621,423,false,false,&neilbuttons.plus ));

    virtualButtons.push_back(make_virtual_button(328,433,379,466,false,false,&neilbuttons.on ));
    virtualButtons.push_back(make_virtual_button(388,430,440,469,false,false,&neilbuttons.key0 ));
    virtualButtons.push_back(make_virtual_button(449,430,500,469,false,false,&neilbuttons.period ));
    virtualButtons.push_back(make_virtual_button(510,430,561,469,false,false,&neilbuttons.dash ));
    virtualButtons.push_back(make_virtual_button(570,435,621,472,false,false,&neilbuttons.enter ));

    int i = 0;
    int arrSize = virtualButtons.size();
    for (i = 0; i < arrSize; i++)
    {
        virtualButtons[i].width = virtualButtons[i].width - virtualButtons[i].x;
        virtualButtons[i].height = virtualButtons[i].height - virtualButtons[i].y;
    }
}


void convert_to_rgba(unsigned char* screen, unsigned char* converted)
{
    int i = 0;
    int cursor = 0;
    unsigned char byte = 0;
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    for (i = 0; i < 128 * 64; i++)
    {
        byte = screen[i];

        //the lightest shade is a little too dark
        if (byte == 102)
            byte = 30;

        r = 255 - byte;
        g = 255 - byte;
        b = 255 - byte;

        // make green a little more pronounced
        if (r >= 30) r -= 30;
        if (b >= 30) b -= 30;

        converted[cursor + 0] = r;
        converted[cursor + 1] = g;
        converted[cursor + 2] = b;
        converted[cursor + 3] = 255;

        cursor += 4;
    }
}


void resetNeilButtons()
{
    neilbuttons.upKey = 0;
    neilbuttons.downKey = 0;
    neilbuttons.leftKey = 0;
    neilbuttons.rightKey = 0;
    neilbuttons.startKey = 0;
    neilbuttons.selectKey = 0;
    neilbuttons.lKey = 0;
    neilbuttons.rKey = 0;
    neilbuttons.aKey = 0;
    neilbuttons.bKey = 0;
    neilbuttons.xKey = 0;
    neilbuttons.yKey = 0;
    neilbuttons.axis0 = 0;
    neilbuttons.axis1 = 0;
    neilbuttons.axis2 = 0;
    neilbuttons.axis3 = 0;
    neilbuttons.touch = 0;

    neilbuttons.key0 = 0;
    neilbuttons.key1 = 0;
    neilbuttons.key2 = 0;
    neilbuttons.key3 = 0;
    neilbuttons.key4 = 0;
    neilbuttons.key5 = 0;
    neilbuttons.key6 = 0;
    neilbuttons.plus = 0;

    neilbuttons.yequals = 0;
    neilbuttons.window = 0;
    neilbuttons.zoom = 0;
    neilbuttons.trace = 0;
    neilbuttons.graph = 0;

    neilbuttons.second = 0;
    neilbuttons.mode = 0;
    neilbuttons.del = 0;

    neilbuttons.alpha = 0;
    neilbuttons.xt = 0;
    neilbuttons.stat = 0;

    neilbuttons.math = 0;
    neilbuttons.apps = 0;
    neilbuttons.prgm = 0;
    neilbuttons.vars = 0;
    neilbuttons.clear = 0;

    neilbuttons.xneg1 = 0;
    neilbuttons.sin = 0;
    neilbuttons.cos = 0;
    neilbuttons.tan = 0;
    neilbuttons.triangle = 0;

    neilbuttons.xsquared = 0;
    neilbuttons.comma = 0;
    neilbuttons.parenopen = 0;
    neilbuttons.parenclose = 0;
    neilbuttons.divide = 0;

    neilbuttons.log = 0;
    neilbuttons.key7 = 0;
    neilbuttons.key8 = 0;
    neilbuttons.key9 = 0;
    neilbuttons.mul = 0;

    neilbuttons.ln = 0;
    neilbuttons.key4 = 0;
    neilbuttons.key5 = 0;
    neilbuttons.key6 = 0;
    neilbuttons.minus = 0;

    neilbuttons.store = 0;
    neilbuttons.key1 = 0;
    neilbuttons.key2 = 0;
    neilbuttons.key3 = 0;
    neilbuttons.plus = 0;

    neilbuttons.on = 0;
    neilbuttons.key0 = 0;
    neilbuttons.period = 0;
    neilbuttons.dash = 0;
    neilbuttons.enter = 0;

}

void checkButtons(int* key, int* lastkey, int num1, int num2) {
    if (*key != *lastkey)
    {
        if (*key) keypad_press(&mycalc.cpu, num1, num2); else keypad_release(&mycalc.cpu, num1, num2);
        *lastkey = *key;
    }
}

void processInput() {

    checkButtons(&neilbuttons.upKey, &lastButtons.upKey, 0, 3);
    checkButtons(&neilbuttons.downKey, &lastButtons.downKey, 0, 0);
    checkButtons(&neilbuttons.leftKey, &lastButtons.leftKey, 0, 1);
    checkButtons(&neilbuttons.rightKey, &lastButtons.rightKey, 0, 2);

    checkButtons(&neilbuttons.yequals, &lastButtons.yequals, 6, 4);
    checkButtons(&neilbuttons.window, &lastButtons.window, 6, 3);
    checkButtons(&neilbuttons.zoom, &lastButtons.zoom, 6, 2);
    checkButtons(&neilbuttons.trace, &lastButtons.trace, 6, 1);
    checkButtons(&neilbuttons.graph, &lastButtons.graph, 6, 0);

    checkButtons(&neilbuttons.second, &lastButtons.second, 6, 5);
    checkButtons(&neilbuttons.mode, &lastButtons.mode, 6, 6);
    checkButtons(&neilbuttons.del, &lastButtons.del, 6, 7);

    checkButtons(&neilbuttons.alpha, &lastButtons.alpha, 5, 7);
    checkButtons(&neilbuttons.xt, &lastButtons.xt, 4, 7);
    checkButtons(&neilbuttons.stat, &lastButtons.stat, 3, 7);

    checkButtons(&neilbuttons.math, &lastButtons.math, 5, 6);
    checkButtons(&neilbuttons.apps, &lastButtons.apps, 4, 6);
    checkButtons(&neilbuttons.prgm, &lastButtons.prgm, 3, 6);
    checkButtons(&neilbuttons.vars, &lastButtons.vars, 2, 6);
    checkButtons(&neilbuttons.clear, &lastButtons.clear, 1, 6);

    checkButtons(&neilbuttons.xneg1, &lastButtons.xneg1, 5, 5);
    checkButtons(&neilbuttons.sin, &lastButtons.sin, 4, 5);
    checkButtons(&neilbuttons.cos, &lastButtons.cos, 3, 5);
    checkButtons(&neilbuttons.tan, &lastButtons.tan, 2, 5);
    checkButtons(&neilbuttons.triangle, &lastButtons.triangle, 1, 5);

    checkButtons(&neilbuttons.xsquared, &lastButtons.xsquared, 5, 4);
    checkButtons(&neilbuttons.comma, &lastButtons.comma, 4, 4);
    checkButtons(&neilbuttons.parenopen, &lastButtons.parenopen, 3, 4);
    checkButtons(&neilbuttons.parenclose, &lastButtons.parenclose, 2, 4);
    checkButtons(&neilbuttons.divide, &lastButtons.divide, 1, 4);

    checkButtons(&neilbuttons.log, &lastButtons.log, 5, 3);
    checkButtons(&neilbuttons.key7, &lastButtons.key7, 4, 3);
    checkButtons(&neilbuttons.key8, &lastButtons.key8, 3, 3);
    checkButtons(&neilbuttons.key9, &lastButtons.key9, 2, 3);
    checkButtons(&neilbuttons.mul, &lastButtons.mul, 1, 3);

    checkButtons(&neilbuttons.ln, &lastButtons.ln, 5, 2);
    checkButtons(&neilbuttons.key4, &lastButtons.key4, 4, 2);
    checkButtons(&neilbuttons.key5, &lastButtons.key5, 3, 2);
    checkButtons(&neilbuttons.key6, &lastButtons.key6, 2, 2);
    checkButtons(&neilbuttons.minus, &lastButtons.minus, 1, 2);

    checkButtons(&neilbuttons.store, &lastButtons.store, 5, 1);
    checkButtons(&neilbuttons.key1, &lastButtons.key1, 4, 1);
    checkButtons(&neilbuttons.key2, &lastButtons.key2, 3, 1);
    checkButtons(&neilbuttons.key3, &lastButtons.key3, 2, 1);
    checkButtons(&neilbuttons.plus, &lastButtons.plus, 1, 1);

    checkButtons(&neilbuttons.on, &lastButtons.on, 5, 0);
    checkButtons(&neilbuttons.key0, &lastButtons.key0, 4, 0);
    checkButtons(&neilbuttons.period, &lastButtons.period, 3, 0);
    checkButtons(&neilbuttons.dash, &lastButtons.dash, 2, 0);
    checkButtons(&neilbuttons.enter, &lastButtons.enter, 1, 0);

}

unsigned char* get_video_buffer()
{
    LCDBase_t* lcd = mycalc.cpu.pio.lcd;
    unsigned char* image = lcd->image(lcd);

    return image;
}


bool file_present_in_system(std::string fname)
{
    const char* systemdirtmp = NULL;
    bool worked = environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemdirtmp);
    if (!worked)
        return false;

    std::string fullpath = systemdirtmp;
    fullpath += "/";
    fullpath += fname;

    RFILE* fp = filestream_open(fullpath.c_str(), RETRO_VFS_FILE_ACCESS_READ,
        RETRO_VFS_FILE_ACCESS_HINT_NONE);

    if (fp)
    {
        filestream_close(fp);
        return true;
    }

    return false;
}

void retro_get_system_info(struct retro_system_info* info)
{
    info->library_name = "Numero";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
    info->library_version = "1.1" GIT_VERSION;
    info->need_fullpath = false;
    info->block_extract = false;
    info->valid_extensions = "8xp|8xk|8xg";
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
    info->geometry.base_width = VIDEO_WIDTH;
    info->geometry.base_height = VIDEO_HEIGHT;
    info->geometry.max_width = VIDEO_WIDTH;
    info->geometry.max_height = VIDEO_HEIGHT;
    info->geometry.aspect_ratio = (float)VIDEO_WIDTH / (float)VIDEO_HEIGHT;
    info->timing.fps = 60;
    info->timing.sample_rate = 0;
}

static void log_null(enum retro_log_level level, const char* fmt, ...) {}

struct retro_perf_callback perf_cb;
bool hasPerf = false;

void retro_init(void)
{
    debugText = "";
    hasPerf = environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb);

    struct retro_log_callback log;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        log_cb = log.log;
    else
        log_cb = log_null;

    video_buf = (uint32_t*)malloc(VIDEO_BUFF_SIZE);

    calcScreen = (unsigned char*)malloc(GRAY_DISPLAY_SIZE);
    ZeroMemory(calcScreen, GRAY_DISPLAY_SIZE);

    initVirtualButtons();

    setSaveDir();

    // Create image with providing my own buffer
    hDib = ezd_initialize(user_header, sizeof(user_header), 640, -480, 32,
        EZD_FLAG_USER_IMAGE_BUFFER);

    // Load medium size font (large doesn't work)
    hFont = ezd_load_font(EZD_FONT_TYPE_MEDIUM, 0, 0);

    // Set custom image buffer
    if (!ezd_set_image_buffer(hDib, video_buf, VIDEO_BUFF_SIZE))
    {
        printf("error initializing ezd buffer\n");
    }

    retro_reset();

}

void retro_set_environment(retro_environment_t cb)
{
    struct retro_vfs_interface_info vfs_iface_info;
    environ_cb = cb;

    /* Set core options */
    libretro_supports_option_categories = false;
    libretro_set_core_options(environ_cb,
        &libretro_supports_option_categories);

    vfs_iface_info.required_interface_version = 1;
    vfs_iface_info.iface = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_iface_info))
        filestream_vfs_init(&vfs_iface_info);

    bool no_content = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }


void retro_reset()
{
    hasBios = false;
    calc_slot_free(&mycalc);
    memset(&mycalc, 0, sizeof(calc_t));
    mycalc.active = TRUE;
    mycalc.speed = 100;
    mycalc.slot = 0;

    //look for silver edition
    char* rom_name = "ti83se.rom";
    has_ti_rom = file_present_in_system(rom_name);
    setProgressDir("ti83se");

    //look for plus
    if (!has_ti_rom)
    {
        rom_name = "ti83plus.rom";
        has_ti_rom = file_present_in_system(rom_name);
        setProgressDir("ti83plus");
    }

    //look for ti83
    if (!has_ti_rom)
    {
        rom_name = "ti83.rom";
        has_ti_rom = file_present_in_system(rom_name);
        setProgressDir("ti83");
    }

    if (has_ti_rom)
    {
        debugText += rom_name;
        debugText += " ";

        const char* systemdirtmp = NULL;
        environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemdirtmp);

        std::string fullpath = systemdirtmp;
        fullpath += "/";
        fullpath += rom_name;

        bool loaded = rom_load(&mycalc, fullpath.c_str());


        calc_turn_on(&mycalc);

        calc_set_running(&mycalc, TRUE);
        hasBios = true;
    }
}

static size_t serialize_size = 0;
size_t retro_serialize_size(void)
{
    return 2500000;
}

const char* getSaveDir()
{
    return savetempdir;
}

const char* getProgressDir()
{
    return saveprogressdir;
}

void setSaveDir()
{
    const char* tmp = NULL;
    environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &tmp);
    std::string fullpath = tmp;
    fullpath += "/";
    fullpath += "tisavestatetemp.sav";
    sprintf(savetempdir, "%s", fullpath.c_str());
}


void setProgressDir(char* rom_name)
{
    const char* tmp = NULL;
    environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &tmp);
    std::string fullpath = tmp;
    fullpath += "/";
    fullpath += "tisavestateprogress";
    fullpath += rom_name;
    fullpath += ".sav";
    sprintf(saveprogressdir, "%s", fullpath.c_str());
}

void saveState(bool progress)
{
    const char* savepath = progress ? getProgressDir() : getSaveDir();

    SAVESTATE_t* savestate = SaveSlot(&mycalc, "calc", "savestate comment");
    WriteSave(savepath, savestate, 0);
    FreeSave(savestate);
}

void loadState(bool progress)
{
    const char* savepath = progress ? getProgressDir() : getSaveDir();
    rom_load(&mycalc, savepath);
}


bool retro_serialize(void* data, size_t size)
{
    saveState(false);

    const char* savepath = getSaveDir();
    RFILE* ofile = filestream_open(savepath,
        RETRO_VFS_FILE_ACCESS_READ,
        RETRO_VFS_FILE_ACCESS_HINT_NONE);
    filestream_seek(ofile, 0, SEEK_END);
    int fsize = filestream_tell(ofile);
    filestream_seek(ofile, 0, SEEK_SET);
    filestream_read(ofile, data, fsize);
    filestream_close(ofile);
    
    return true;
}

bool retro_unserialize(const void* data, size_t size)
{
    const char* savepath = getSaveDir();
    RFILE* ofile = filestream_open(savepath,
        RETRO_VFS_FILE_ACCESS_WRITE,
        RETRO_VFS_FILE_ACCESS_HINT_NONE);
    filestream_write(ofile, data, size);
    filestream_close(ofile);

    loadState(false);

    return true;
}

//this is where we check if stuff changed
static void check_variables(void)
{
    struct retro_variable var = { 0 };
    var.key = "mouse_speed";
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        virtualMouseSpeed = atoi(var.value);
    }

    struct retro_variable var2 = { 0 };
    var2.key = "debug_mode";
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var2) && var2.value)
    {
        debugMode = atoi(var2.value);
    }
}

#define RETRO_DEVICE_JOYPAD_ALT  RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)

unsigned char* pngBuffer;
int pngWidth = 0;
int pngHeight = 0;
int pngBpp = 0;
char retro_save_directory[4096];
char rom_dir[4096];

LINK_ERR SendFile(const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination);

bool retro_load_game(const struct retro_game_info* info)
{
    if (hasBios)
    {
        if (filestream_exists(getProgressDir()))
        {
           loadState(true);
        }

        //we support no rom file
        if (info)
        {
            sprintf(rom_dir, "%s", info->path);
            LINK_ERR err = SendFile(&mycalc, rom_dir, SEND_RAM);
            if (err != LERR_SUCCESS)
            {
                if (err == LERR_MEM)
                {
                    //not enough memory so load it to archive
                    err = SendFile(&mycalc, rom_dir, SEND_ARC);
                }
                else
                {
                    //just try it again because sometimes it throws an error?
                    err = SendFile(&mycalc, rom_dir, SEND_RAM);
                    if (err == LERR_MEM)
                        err = SendFile(&mycalc, rom_dir, SEND_ARC);
                }
            }

            if (err == LERR_SUCCESS)
            {
                sprintf(toast_buffer, "Import Success: %s", info->path);
                toast_message(toast_buffer);
            }

            rom_path = info->path ? info->path : "";
            log_cb(RETRO_LOG_INFO, "Got rom path: %s.\n", rom_path.c_str());

        }
        
    }
    
    //load image bytes from header file
    pngBuffer = stbi_load_from_memory((const stbi_uc*)buttons, 348657, &pngWidth, &pngHeight, &pngBpp, 4);

   
    static const struct retro_controller_description port_1[] = {
        { "Joypad", RETRO_DEVICE_JOYPAD },
        { "Gaming Buttons", RETRO_DEVICE_JOYPAD_ALT },
        { "None", RETRO_DEVICE_NONE },
        { 0, 0 },
    };

    static const struct retro_controller_info ports[] = {
       { port_1, 3 },
       { 0, 0 },
    };

    environ_cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
    
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        log_cb(RETRO_LOG_ERROR, "XRGB8888 is not supported.\n");
        return false;
    }
    
    check_variables();

    bool no = false;
    environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS, &no);

    return true;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
    struct retro_input_descriptor descJoypad[] = {
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Mouse Left" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Mouse Up" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Mouse Down" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Mouse Right" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Mouse Press" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Button 2ND" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Button UP" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Button DOWN" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Button ALPHA" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Button ENTER" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,  "Button LEFT" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,  "Button RIGHT" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,  "Toggle Big Mode" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,  "Mouse Press" },
               { 0 },
    };

    struct retro_input_descriptor descGaming[] = {
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Button LEFT" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Button UP" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Button DOWN" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Button RIGHT" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Button 2ND" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Button ALPHA" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Button MODE" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Button PRGM" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Button CLEAR" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Button ENTER" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,  "Button LINK" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,  "Button STAT" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,  "Toggle Big Mode" },
               { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,  "Mouse Press" },
               { 0 },
    };

    if (port == 0)
    {
        switch (device)
        {
        case RETRO_DEVICE_JOYPAD:
            gamingButtons = false;
            environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, descJoypad);
            break;
        case RETRO_DEVICE_JOYPAD_ALT:
            gamingButtons = true;
            environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, descGaming);
            break;
        }
    }
}

void retro_unload_game()
{
}

unsigned retro_get_region()
{
    return RETRO_REGION_NTSC;
}

void determineVirtualOrRegularMouseMode()
{

    int16_t mouseX = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
    int16_t mouseY = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

    if (mouseX > 0 || mouseY > 0 || mouseX < 0 || mouseY < 0)
    {
        virtualMouseMode = false;
    }

    int16_t gamepadX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
    int16_t gamepadY = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
    bool left = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
    bool right = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
    bool up = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
    bool down = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
    if (gamingButtons)
    {
        left = false;
        right = false;
        up = false;
        down = false;
    }
    if (gamepadX > 4000 || gamepadX < -4000 || gamepadY > 4000 || gamepadY < -4000 ||
        left || right || up || down)
    {
        virtualMouseMode = true;
    }
}

char textBuffer[500];

void drawVirtualMouse()
{
    ezd_line(hDib, virtualMouseX,virtualMouseY,virtualMouseX + 10
        ,virtualMouseY + 10,0xFFFFFF);
    ezd_line(hDib, virtualMouseX, virtualMouseY, virtualMouseX + 5
        , virtualMouseY, 0xFFFFFF);
    ezd_line(hDib, virtualMouseX, virtualMouseY, virtualMouseX
        , virtualMouseY + 5, 0xFFFFFF);
}

void drawScreen()
{

    // Fill in the background
    ezd_fill(hDib, 0x444444);

    if (hasBios)
    {
        //draw calculator buttons
        int cursor = 0;
        if (!bigMode)
        {
            for (int y = 0; y < pngHeight; y++)
            {
                for (int x = 0; x < pngWidth; x++)
                {
                    video_buf[y * VIDEO_WIDTH + x + (640 - pngWidth)] = pngBuffer[cursor] << 16 | pngBuffer[cursor + 1] << 8 | pngBuffer[cursor + 2];
                    cursor += 4;
                }
            }
        }


        //draw calculator screen
        int sourceX = 0;
        int sourceY = 0;
        float scaleFactor = 3.0f;
        if (bigMode)
        {
            scaleFactor = ((float)VIDEO_WIDTH / (float)96);
        }
        int scaledWidth = (int)((float)CALC_WIDTH * scaleFactor);
        int scaledHeight = (int)((float)CALC_HEIGHT * scaleFactor); ;
        for (int y = 0; y < scaledHeight; y++)
        {
            for (int x = 0; x < scaledWidth; x++)
            {
                if (x < 96 * scaleFactor)
                {
                    sourceX = (int)(((float)x) * ((float)CALC_WIDTH / float(scaledWidth)));
                    sourceY = (int)(((float)y) * ((float)CALC_HEIGHT / float(scaledHeight)));
                    cursor = ((sourceY * CALC_WIDTH) + sourceX) * 4;
                    video_buf[y * VIDEO_WIDTH + x] = screen_converted[cursor] << 16 | screen_converted[cursor + 1] << 8 | screen_converted[cursor + 2];
                }
            }
        }

        if (!bigMode)
        {
            //draw button hovers
            int arrSize = virtualButtons.size();
            for (int i = 0; i < arrSize; i++)
            {
                drawButtonHover(&virtualButtons[i]);
            }
        }


        if (!bigMode)
        {
            int16_t mouseX = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
            int16_t mouseY = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
            int16_t mousePressed = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
            
            int16_t mousePointerX = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
            int16_t mousePointerY = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
            int16_t mousePointerPressed = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
            
            int16_t gamepadX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
            int16_t gamepadY = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);

            if (!gamingButtons)
            {
                bool leftPressed = false;
                bool rightPressed = false;
                bool upPressed = false;
                bool downPressed = false;

                if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)) leftPressed = true;
                if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)) rightPressed = true;
                if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)) upPressed = true;
                if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)) downPressed = true;

                if (leftPressed) gamepadX = -15000;
                if (rightPressed) gamepadX = 15000;
                if (upPressed) gamepadY = -15000;
                if (downPressed) gamepadY = 15000;
            }

            if (debugMode)
            {
                sprintf(textBuffer, "DEBUG: %s", debugText.c_str());
                ezd_text(hDib, hFont, textBuffer, -1, 10, 210, 0xffffff);
            }
            

            //deadzone
            if (gamepadX > 4000 || gamepadX < -4000)
                virtualMouseX += ((int)(gamepadX / 6000.0f)) * virtualMouseSpeed;
            if (gamepadY > 4000 || gamepadY < -4000)
                virtualMouseY += ((int)(gamepadY / 6000.0f)) * virtualMouseSpeed;

            //bounds
            if (virtualMouseX < 0) virtualMouseX = 0;
            if (virtualMouseY < 0) virtualMouseY = 0;
            if (virtualMouseX > 640) virtualMouseX = 640;
            if (virtualMouseY > 480) virtualMouseY = 480;

            drawVirtualMouse();
        }

    }
    else
    {
        int xAdjust = 40;
        ezd_fill_rect(hDib, 90 + xAdjust, 140, 470 + xAdjust, 230, 0x222222);

        sprintf(textBuffer, "Please add one of the following rom files to your System Directory");
        ezd_text(hDib, hFont, textBuffer, -1, 100 + xAdjust, 150, 0xffffff);

        sprintf(textBuffer, "ti83se.rom (recommended)");
        ezd_text(hDib, hFont, textBuffer, -1, 120 + xAdjust, 170, 0xffffff);

        sprintf(textBuffer, "ti83plus.rom");
        ezd_text(hDib, hFont, textBuffer, -1, 120 + xAdjust, 190, 0xffffff);

        sprintf(textBuffer, "ti83.rom");
        ezd_text(hDib, hFont, textBuffer, -1, 120 + xAdjust, 210, 0xffffff);

    }
    
}

int printCounter = 0;

void progressSave()
{
    frameCounter++;

    //save every 10 seconds
    if (frameCounter == 600)
    {
        frameCounter = 0;
        saveState(true);

        //toast_message("Progress Saved");

    }
}

void toast_message(char* message)
{
    struct retro_message msg;
    msg.msg = message;
    msg.frames = 60;
    environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
}

double fpsInterval = 1000.0 / 60.0;
uint64_t fpsThen = 0; //time in milliseconds
uint64_t fpsNow = 0; //time in milliseconds
int fpsFrameCounter = 0;
int fpsAudioRemaining = 0; //used to keep relieve some audio when emulator is running too fast

bool IsFrameReady() {

    retro_time_t time = perf_cb.get_time_usec(); //get time in microseconds
    fpsNow = time / 1000;
    retro_time_t elapsed = fpsNow - fpsThen;
    bool ready = true;

    int shouldBeFrames = (int)((double)elapsed / fpsInterval);
    if (fpsFrameCounter > shouldBeFrames)
    {
        ready = false;
    }

    if (ready)
        fpsFrameCounter++;

    //reset
    if (elapsed > 1000)
    {
        fpsThen = fpsNow;
        fpsFrameCounter = 0;
    }

    return ready;
}


void retro_run()
{
    //this would enforce 60FPS
    if (hasPerf && !IsFrameReady())
       return;

    resetNeilButtons();

    if (hasBios)
    {
        progressSave();
    }


    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
        check_variables();

    input_poll_cb();

    if (hasBios)
    {
        determineVirtualOrRegularMouseMode();

        int mouseX = 0;
        int mouseY = 0;
        int mousePressed = 0;
        if (virtualMouseMode)
        {
            mouseX = virtualMouseX;
            mouseY = virtualMouseY;
            if (!gamingButtons)
            {
                if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
                {
                    mousePressed = 1;
                }
            }
        }
        else
        {
            int16_t mousePointerX = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
            int16_t mousePointerY = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
            mouseX = (int)((mousePointerX + 0x7fff) / (float)(0x7fff * 2) * VIDEO_WIDTH);
            mouseY = (int)((mousePointerY + 0x7fff) / (float)(0x7fff * 2) * VIDEO_HEIGHT);

            int16_t mouseDevicePressed = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
            int16_t mousePointerPressed = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
            mousePressed = mouseDevicePressed | mousePointerPressed;
        }

        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2))
        {
            mousePressed = true;
        }


        int i = 0;
        int arrSize = virtualButtons.size();
        for (i = 0; i < arrSize; i++)
        {
            virtualButtons[i].hover = false;
            virtualButtons[i].pressed = false;

            if (mouseX >= virtualButtons[i].x && mouseX <= virtualButtons[i].x + virtualButtons[i].width &&
                mouseY >= virtualButtons[i].y && mouseY <= virtualButtons[i].y + virtualButtons[i].height)
            {
                virtualButtons[i].hover = true;
                if (mousePressed)
                {
                    printCounter++;
                    virtualButtons[i].pressed = true;
                    *(virtualButtons[i].key) = true;
                }
            }

            virtualButtons[i].lastPressed = virtualButtons[i].pressed;

        }


        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START)) neilbuttons.enter = true;

        if (gamingButtons)
        {
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A)) neilbuttons.second = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)) neilbuttons.leftKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)) neilbuttons.rightKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)) neilbuttons.upKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)) neilbuttons.downKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L)) neilbuttons.xt = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R)) neilbuttons.stat = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B)) neilbuttons.alpha = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X)) neilbuttons.mode = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y)) neilbuttons.prgm = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT)) neilbuttons.clear = true;
        }
        else
        {
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B)) neilbuttons.second = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X)) neilbuttons.upKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y)) neilbuttons.downKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L)) neilbuttons.leftKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R)) neilbuttons.rightKey = true;
            if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT)) neilbuttons.alpha = true;
        }

        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2))
        {
            if (!lastBigModePressed)
            {
                bigMode = !bigMode;
            }
            lastBigModePressed = true;
        }
        else
            lastBigModePressed = false;


        //process calculator buttons
        processInput();

        //TODO this is not the exact timing need to fix
        int time = (&mycalc)->cpu.timer_c->freq / 50;
        calc_run_tstates(&mycalc, time);

        unsigned char* screen = get_video_buffer();
        convert_to_rgba(screen, screen_converted);
    }

    

    drawScreen();

    video_cb(video_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PITCH);

}



unsigned retro_api_version()
{
    return RETRO_API_VERSION;
}

void retro_deinit(void)
{

    saveState(true);
    calc_slot_free(&mycalc);


#ifdef _3DS
    linearFree(video_buf);
    linearFree(calcScreen);
#else
    free(video_buf);
    free(calcScreen);
#endif

    ezd_destroy(hDib);
    ezd_destroy_font(hFont);
    stbi_image_free(pngBuffer);

    virtualButtons.clear();

    video_buf = NULL;
}

void cartridge_set_rumble(unsigned active){}
void retro_cheat_reset(){}
void retro_cheat_set(unsigned index, bool enabled, const char* code){}
bool retro_load_game_special(unsigned, const struct retro_game_info*, size_t) { return false; }
void* retro_get_memory_data(unsigned id){return 0;}
size_t retro_get_memory_size(unsigned id){return 0;}

#endif //ifdef OLDCORE
