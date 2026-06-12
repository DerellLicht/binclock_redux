// extern HINSTANCE g_hInst ;

#define  WM_DO_COMM_TASK    (WM_USER + 101)
#define  WM_COMM_TASK_DONE  (WM_USER + 102)

//  config.cpp
extern unsigned bitmap_idx ;
extern unsigned bit_menu ;
extern unsigned layout_method ;
extern bool show_winmsgs ;

extern unsigned crfg ;
extern unsigned crbg ;

LRESULT read_config_file(void);
LRESULT save_cfg_file(void);

//  about.cpp
BOOL CmdAbout(HWND hwnd);

