//  config.cpp
extern uint x_pos ;
extern uint y_pos ;
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

