#include <windows.h>
#include <CommCtrl.h>
#include <cstdint>
#include <string>
#include "resource.h"

static const char *CLS_NAME = "DFSimWin";

struct Ctx {
    HINSTANCE instance;

    struct Channel {
        double dir, rssi, squelsh;
        uint32_t status;

        Channel (): dir (0.0), squelsh (0.0), rssi (0.0), status (0) {}
    } channels [3];

    Ctx (HINSTANCE _inst): instance (_inst) {}
};

std::string formatFloat (double val, const char *format = "%.1f") {
    char buffer [100];
    sprintf (buffer, format, val);
    return std::string (buffer);
};

void doCommand (HWND wnd, uint16_t command) {
    Ctx *ctx = (Ctx *) GetWindowLongPtr (wnd, GWLP_USERDATA);

    switch (command) {
        default: {

        }
    }
}

void initWindow (HWND wnd, void *data) {
    Ctx *ctx = (Ctx *) data;
    RECT client;

    GetClientRect (wnd, & client);

    auto createControl = [&wnd, &ctx] (const char *className, const char *text, uint32_t style, bool visible, int x, int y, int width, int height, uint64_t id) {
        style |= WS_CHILD;

        if (visible) style |= WS_VISIBLE;
        
        CreateWindow (className, text, style, x, y, width, height, wnd, (HMENU) id, ctx->instance, 0);
    };

    SetWindowLongPtr (wnd, GWLP_USERDATA, (LONG_PTR) data);

    auto createLabeledEdit = [wnd, createControl] (const char *label, int x, int y, int width, uint16_t id, const char *value, uint32_t style = 0, const char *ctlClass = "EDIT", int height = 22) {
        createControl ("STATIC", label, SS_LEFT, true, x, y, 100, 25, IDC_STATIC);
        createControl (ctlClass, value, WS_TABSTOP | WS_BORDER | style, true, x + 70, y, width, height, id);
    };
    auto createChannelFrame = [createLabeledEdit, createControl] (Ctx::Channel *channel, char *title, int x, uint16_t startID) {
        createControl ("BUTTON", title, BS_GROUPBOX, true, x, 20, 240, 150, IDC_STATIC);
        createLabeledEdit ("Direction", x + 10, 40, 70, startID, formatFloat (channel->dir, "%05.1f").c_str ());
        createLabeledEdit ("RSSI", x + 10, 70, 70, startID + 1, formatFloat (channel->rssi, "%.2f").c_str ());
        createLabeledEdit ("Squelsh", x + 10, 100, 70, startID + 2, formatFloat (channel->squelsh).c_str ());
        createLabeledEdit ("Status", x + 10, 130, 150, startID + 3, "Ok", CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, "COMBOBOX", 80);
    };
    createChannelFrame (ctx->channels, "121.5mhz", 20, IDC_121_5_DIR);
    createChannelFrame (ctx->channels, "156.800mhz", 270, IDC_156_800_DIR);
    createChannelFrame (ctx->channels, "243mhz", 520, IDC_243_DIR);
    createControl ("BUTTON", "Connection", BS_GROUPBOX, true, 20, 170, 190, 120, IDC_STATIC);
    createLabeledEdit ("Port", 30, 190, 100, IDC_PORT, "COM1", CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, "COMBOBOX", 80);
    createLabeledEdit ("Baud", 30, 220, 100, IDC_BAUD, "COM1", CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, "COMBOBOX", 80);
    createControl ("BUTTON", "Open", BS_CHECKBOX | BS_PUSHLIKE | BS_AUTOCHECKBOX, true, 30, 250, 100, 25, IDC_OPEN_CLOSE);
    /*createControl ("BUTTON", "121.5mhz", BS_GROUPBOX, true, 20, 20, 275, 150, IDC_STATIC);
    createLabeledEdit ("Direction", 30, 40, 70, IDC_121_5_DIR, "0.0");
    createLabeledEdit ("RSSI", 30, 70, 70, IDC_121_5_RSSI, "1.23");
    createLabeledEdit ("Squelsh", 30, 100, 70, IDC_121_5_SQUELSH, "92.0");
    createLabeledEdit ("Status", 30, 130, 150, IDC_121_5_STATUS, "Ok", CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, "COMBOBOX", 80);*/
    /*createControl ("STATIC", "Direction", SS_LEFT, true, 30, 40, 100, 25, IDC_STATIC);
    createControl ("EDIT", "0.0", WS_TABSTOP | WS_BORDER, true, 100, 40, 100, 22, IDC_121_5_DIR);*/
    /*char title [256];
    sprintf (title, "%s %sinstallation", cfg.getString (SECTION_MAIN, "appName").c_str (), ctx->uinstallMode ? "un" : "");
    SetWindowText (wnd, title);
    createControl ("BUTTON", ctx->uinstallMode ? "&Uninstall" : "&Install", WS_TABSTOP | BS_PUSHBUTTON | BS_DEFPUSHBUTTON, true, client.right - 220, client.bottom - 45, 100, 35, IDC_INSTALL);
    createControl ("BUTTON", "&Cancel", WS_TABSTOP | BS_PUSHBUTTON, true, client.right - 110, client.bottom - 45, 100, 35, IDC_CANCEL);
    createControl ("LISTBOX", "", WS_BORDER, false, 320, 10, client.right - 330, client.bottom - 55, IDC_LOG);
    createControl ("EDIT", cfg.getString (SECTION_SHORTCUTS, "DefProgramGroup").c_str (), WS_TABSTOP | WS_BORDER | ES_LEFT, !ctx->uinstallMode, 450, 10, client.right - 460, 25, IDC_PRG_GROUP);
    createControl ("STATIC", "Program group", SS_LEFT, !ctx->uinstallMode, 320, 10, 100, 25, IDC_PRG_GROUP_LBL);
    createControl ("STATIC", "Run after instalation the following tasks:", SS_LEFT, !ctx->uinstallMode, 320, 50, 400, 25, IDC_RUN_LBL);

    auto numOfTasks = cfg.getInt (SECTION_TASKS, "number");

    for (auto i = 1, y = 80; i <= numOfTasks; ++ i, y += 40) {
        auto info = cfg.getString (SECTION_TASKS, std::to_string (i).c_str ());

        std::vector<std::string> parts;

        splitLine (info, parts, ',');

        if (parts.size () > 0) {
            createControl ("BUTTON", parts [0].c_str (), WS_TABSTOP | BS_CHECKBOX | BS_AUTOCHECKBOX, !ctx->uinstallMode, 320, y, 400, 35, IDC_FIRST_TASK + i - 1);
        }
    }
    */
}

bool queryExit (HWND wnd) {
    return MessageBox (wnd, "Do you want to exit?", "Exit app", MB_YESNO | MB_ICONQUESTION) == IDYES;
}

bool doSysCommand (HWND wnd, WPARAM param1, LPARAM param2) {
    bool processed = false;
    switch (param1) {
        case SC_CLOSE: {
            if (queryExit (wnd)) DestroyWindow (wnd);
            processed = true;
            break;
        }
    }
    if (!processed) DefWindowProc (wnd, WM_SYSCOMMAND, param1, param2);
    return processed;
}

LRESULT wndProc (HWND wnd, UINT msg, WPARAM param1, LPARAM param2) {
    LRESULT result = 0;

    switch (msg) {
        case WM_COMMAND:
            doCommand (wnd, LOWORD (param1)); break;
        case WM_SYSCOMMAND:
            doSysCommand (wnd, param1, param2); break;
        case WM_CREATE:
            initWindow (wnd, ((CREATESTRUCT *) param2)->lpCreateParams); break;
        case WM_DESTROY:
            PostQuitMessage (9); break;
        default:
            result = DefWindowProc (wnd, msg, param1, param2);
    }
    
    return result;
}

void registerClass (HINSTANCE instance) {
    WNDCLASS classInfo;

    memset (&classInfo, 0, sizeof (classInfo));

    classInfo.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
    classInfo.hCursor = (HCURSOR) LoadCursor (nullptr, IDC_ARROW);
    classInfo.hIcon = (HICON) LoadIcon (nullptr, IDI_APPLICATION); //(instance, MAKEINTRESOURCE (IDI_SETUP));
    classInfo.hInstance = instance;
    classInfo.lpfnWndProc = wndProc;
    classInfo.lpszClassName = CLS_NAME;
    classInfo.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass (&classInfo);
}

void initCommonControls () {
    INITCOMMONCONTROLSEX data { sizeof (INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
	
    CoInitialize (0);
    InitCommonControlsEx (& data);
}


int APIENTRY WinMain (HINSTANCE instance, HINSTANCE prev, char *cmdLine, int showCmd) {
    Ctx ctx (instance);

    initCommonControls ();
    registerClass (instance);

    auto mainWnd = CreateWindow (
        CLS_NAME,
        "DFSim Tool",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        900,
        600,
        0,
        0,
        instance,
        & ctx
    );

    ShowWindow (mainWnd, SW_SHOW);
    UpdateWindow (mainWnd);

    MSG msg;

    while (GetMessage (&msg, 0, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
}
