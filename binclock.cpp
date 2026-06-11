//**********************************************************************
//  Copyright (c) 2009-2026  Derell Licht
//  about_hlinks - example program - About dialog with hyperlinks
//**********************************************************************

//  This conversion from C array to <vector> class,
//  change executable size from 32KB to 144KB
#define  USE_VECTOR_CLASS
// #undef  USE_VECTOR_CLASS

static char szClassName[] = "about_hlinks" ;

#include <windows.h>
#ifdef USE_VECTOR_CLASS
#include <vector>
#endif
#include <commctrl.h>           //  link to comctl32.lib
// #include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <tchar.h>

// #define  READY_FOR_BCLK_ELEMENTS
#undef  READY_FOR_BCLK_ELEMENTS

//lint -esym(715, hwnd, private_data, message, wParam, lParam)

#include "version.h"
#include "resource.h"
#include "common.h"
#include "binclock.h"
#include "bclk_elements.h"

//*************************************************************************

UINT  timerID = 0 ;

HINSTANCE g_hInst;
static NOTIFYICONDATA NotifyIconData;

//*************************************
#define  STD_SPACE      20
//  LABEL_LEN is starting X position for LEDs
#define  LABEL_LEN      70

// #define  FIELD_LEN      400
#define  SPRITE_WIDTH   22

#define  FIELD_HEIGHT   20
#define  TEXT_HEIGHT    20
// #define  BUTTON_HEIGHT  (FIELD_HEIGHT + 10)
#define  NEXT_FIELD     (FIELD_HEIGHT + 4)

#define  START_ROW      10
#define  BLANK_ROW      (START_ROW)
#define  HOURS_ROW      (BLANK_ROW + NEXT_FIELD)
#define  MINS_ROW       (HOURS_ROW + NEXT_FIELD)
#define  SECS_ROW       (MINS_ROW + NEXT_FIELD)
#define  BTN_ROW        (SECS_ROW + NEXT_FIELD + 10)

#define  LABEL_COL      STD_SPACE

#define  NUM_COL       (LABEL_COL + LABEL_LEN + STD_SPACE)
#define  NUMBER_LEN     30
#define  LED_COL       (NUM_COL + NUMBER_LEN + STD_SPACE)

//*************************************

static HWND hwndHours = NULL;
static HWND hwndMins  = NULL;
static HWND hwndSecs  = NULL;

HMENU hPopMenu = 0 ;

//***********************************************************************
unsigned bitmap_idx = 2 ;
unsigned bit_menu = 2 ;
unsigned layout_method = 0 ;

//  These color fields are used *only* for the "Bound Boxes" LED style
unsigned crfg = RGB(128, 255, 0) ;
unsigned crbg = RGB(128, 64, 0) ;

//*********************************************************************
void load_bitmap_files(HWND hwnd)
{
#ifdef  READY_FOR_BCLK_ELEMENTS
   HDC hdc = GetDC(hwnd) ;
   unsigned menu_code = ID_LAMPS0 ;
   bclock_element *be_temp ;
   unsigned idx = 0 ;
   unsigned j ;
   char msg[81] ;
   unsigned start_element ;

   // bclock_element(HINSTANCE g_hInst, char *name, 
   //          unsigned width, unsigned flags,
   //          int mask_index, unsigned off_index, unsigned start_element);
   start_element = (bitmap_idx == idx) ? bit_menu : 10 ;
   be_temp = new bclock_element(g_hInst, "ledarray.bmp", 22, BE_LINEAR, 3, 4, start_element);
   be_temp->add_skip_element(0) ;
   be_temp->add_skip_element(1) ;
   be_temp->add_color_menu_str(2, "green") ;
   be_temp->add_skip_element(5) ;
   be_temp->add_skip_element(6) ;
   be_temp->add_color_menu_str(7, "orange") ;
   be_temp->add_skip_element(8) ;
   be_temp->add_skip_element(9) ;
   be_temp->add_color_menu_str(10, "red") ;
   be_temp->add_skip_element(11) ;
   be_temp->add_skip_element(12) ;
   be_temp->add_color_menu_str(13, "yellow") ;
   be_temp->mask_the_source(hdc) ;
   // be_temp->add_menu_data(menu_code++, "Large lamps") ;
   menu_code = be_temp->add_menu_data(menu_code, "Large lamps") ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 3 ;
   be_temp = new bclock_element(g_hInst, "square2.bmp", 0, BE_LINEAR, -1, 0, start_element);
   be_temp->set_image_offsets(0, 2) ;
   menu_code = be_temp->add_menu_data(menu_code, "Square lamps") ;
   static char * const square_colors[10] = {
      "grey", "dark purple", "cyan", "green", "blue", "orange", "magenta", "purple", "red", "yellow" } ;
   for (j=0; j<10; j++)
      be_temp->add_color_menu_str(j, square_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 3 ;
   be_temp = new bclock_element(g_hInst, "balls.bmp", 0, BE_LINEAR, 0, 1, start_element);
   // be_temp = new bclock_element(g_hInst, BM_BALLS, 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Small lamps") ;
   static char * const balls_colors[7] = {
      " ", " ", "blue", "green", "orange", "yellow", "red" } ;
   for (j=0; j<7; j++)
      be_temp->add_color_menu_str(j, balls_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 5 ;
   be_temp = new bclock_element(g_hInst, "ceramics.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->set_image_offsets(-5, -5) ;
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Ceramic lamps") ;
   static char * const ceramics_colors[] = {
      " ", " ", "purple", "ecru", "verdant", "green", "tan", "fuchsia", "purple2", "red", "cyan", "yellow", 0 } ;
   for (j=0; ceramics_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, ceramics_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "accent.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Accent balls") ;
   static char * const accent_colors[] = {
      " ", " ", "green", "cyan", "blue", "pink", "red", "brown", "orange", "yellow", 0 } ;
   for (j=0; accent_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, accent_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "marbles.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Glass marbles") ;
   static char * const marble_colors[] = {
      " ", " ", "grey", "blue", "cyan", "yellow", "green", "purple", "red", 0 } ;
   for (j=0; marble_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, marble_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 3 ;
   be_temp = new bclock_element(g_hInst, "leds.tiny.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Tiny leds") ;
   static char * const tiny_leds_colors[] = {
      " ", " ", "blue", "green", "red", "yellow", 0 } ;
   for (j=0; tiny_leds_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, tiny_leds_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 2 ;
   be_temp = new bclock_element(g_hInst, "leds.bmp", 0, BE_PAIRS, 0, start_element-1, start_element);
   be_temp->set_image_offsets(-2, -5) ;
   be_temp->add_skip_element(1) ;
   be_temp->add_color_menu_str(2, "green") ;
   be_temp->add_skip_element(3) ;
   be_temp->add_color_menu_str(4, "red") ;
   be_temp->add_skip_element(5) ;
   be_temp->add_color_menu_str(6, "yellow") ;
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "LEDs") ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "lights.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Glass lights") ;
   static char * const light_colors[] = {
      " ", "blue", "red", "brown", "orange", "yellow", "green", "cyan", "blue", "fuchsia", 0 } ;
   for (j=0; light_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, light_colors[j]) ;
   element_list[idx++] = be_temp ;

   //*********************************************************
   //                 width         mask offset start_el
   be_temp = new bclock_element(g_hInst, (char *) NULL, 14, BE_DRAWN, 0,   0,     0);
   menu_code = be_temp->add_menu_data(menu_code, "Bound Boxes") ;
   static char * const drawn_colors[] = { " ", "select this element", "change ON color", "change OFF color", 0 } ; 
   for (j=0; drawn_colors[j] != 0; j++) {
      // OutputDebugString(drawn_colors[j]) ;
      be_temp->add_color_menu_str(j, drawn_colors[j]) ;
   }
   be_temp->set_element_attr(crfg, crbg) ;
   element_list[idx++] = be_temp ;

   //  this is an insufficient test; the program may abort
   //  before we can get to this test, if overrun occurs...
   if (idx > NUM_ELEMENTS) {
      wsprintf(msg, "too many elements created (%u vs %u)\n", idx, NUM_ELEMENTS) ;
      OutputDebugString(msg) ;
      MessageBox(NULL, msg, "DANGER!!", MB_OK) ;
   }

   ReleaseDC (hwnd, hdc) ;
#endif   
}

//*********************************************************************
#ifdef  READY_FOR_BCLK_ELEMENTS
static void draw_horiz_binary_time(HDC hdc, unsigned row, unsigned tvalue)
{
   unsigned mask = 0x20 ;
   unsigned idx = LED_COL ;
   // for (unsigned j=0; j<8; j++) {
   for (unsigned j=0; j<6; j++) {
      element_list[bitmap_idx]->draw_sprite(hdc, (tvalue & mask), idx, row);
      idx += SPRITE_WIDTH + 4 ;
      mask >>= 1 ;
   }
}

//*********************************************************************
static void draw_bcd_time(HDC hdc, unsigned row, unsigned time_seg, unsigned draw_flags)
{
   if (draw_flags & 8)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 8), row, BLANK_ROW);
   if (draw_flags & 4)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 4), row, HOURS_ROW);
   if (draw_flags & 2)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 2), row, MINS_ROW);
   if (draw_flags & 1)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 1), row, SECS_ROW);
}
#endif
//*********************************************************************
void update_timer_count(HWND hwnd)
{
   time_t ttm ;
   struct tm *gtm ;
   char tstr[30] ;
   time(&ttm) ;
   gtm = localtime(&ttm) ;

   wsprintf(tstr, " %02d", gtm->tm_hour) ;
   SetWindowText(hwndHours, tstr) ;
   wsprintf(tstr, " %02d", gtm->tm_min) ;
   SetWindowText(hwndMins, tstr) ;
   wsprintf(tstr, " %02d", gtm->tm_sec) ;
   SetWindowText(hwndSecs, tstr) ;
   
#ifdef  READY_FOR_BCLK_ELEMENTS
   HDC hdc = GetDC(hwnd) ;
   //  method 0 shows hours, mins, secs as 8-bit binary values, horizontally
   if (layout_method == 0) {
      draw_horiz_binary_time(hdc, HOURS_ROW, gtm->tm_hour) ;
      draw_horiz_binary_time(hdc, MINS_ROW,  gtm->tm_min) ;
      draw_horiz_binary_time(hdc, SECS_ROW,  gtm->tm_sec) ;
   }
   //  method 1 shows hours, mins, secs in BCD format, vertically
   else {
      unsigned hoursh = gtm->tm_hour / 10 ;
      unsigned hoursl = gtm->tm_hour % 10 ;
      unsigned minsh  = gtm->tm_min  / 10 ;
      unsigned minsl  = gtm->tm_min  % 10 ;
      unsigned secsh  = gtm->tm_sec  / 10 ;
      unsigned secsl  = gtm->tm_sec  % 10 ;
      unsigned idx = LED_COL ;

      draw_bcd_time(hdc, idx, hoursh, 0x3) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, hoursl, 0xF) ;

      idx += SPRITE_WIDTH + 16 ;
      draw_bcd_time(hdc, idx, minsh, 0x7) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, minsl, 0xF) ;

      idx += SPRITE_WIDTH + 16 ;
      draw_bcd_time(hdc, idx, secsh, 0x7) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, secsl, 0xF) ;
   }

   ReleaseDC (hwnd, hdc) ;
#endif   
}

//*******************************************************************
static bool do_init_dialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   char msgstr[81] ;
   wsprintf(msgstr, "%s %s", szClassName, VerNum) ;
   SetWindowText(hwnd, msgstr) ;

   SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM) LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_APPICON)));
   SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_APPICON)));

   // load_bitmap_files(hwnd) ;

   hwndHours = GetDlgItem(hwnd, IDC_HOURS) ;
   hwndMins  = GetDlgItem(hwnd, IDC_MINS) ;
   hwndSecs  = GetDlgItem(hwnd, IDC_SECS) ;

   //***********************************************
   // create tray menu
   //***********************************************
   // hMenu = LoadMenu (g_hInst, MAKEINTRESOURCE (ID_TRAYMENU));
   //  for some reason, I cannot simply create a menu here,
   //  and then attach it to the system tray...
   //  Niether AppendMenu nor InsertMenu actually do anything...

   // put the icon into a system tray
   NotifyIconData.cbSize = sizeof (NOTIFYICONDATA);
   NotifyIconData.hWnd = hwnd;
   NotifyIconData.uID = 0;
   NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
   NotifyIconData.uCallbackMessage = WM_USER; // tray events will generate WM_USER event
   // NotifyIconData.hIcon = (HICON) LoadImage (g_hInst, MAKEINTRESOURCE (IDAPPLICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); // load 16 x 16 pixels icon
   NotifyIconData.hIcon = (HICON) LoadIcon (g_hInst, MAKEINTRESOURCE (IDI_APPICON));
   lstrcpy (NotifyIconData.szTip, szClassName); // max 64 characters

   Shell_NotifyIcon (NIM_ADD, &NotifyIconData);

   timerID = SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC) NULL) ;
   return true ;
}

//*******************************************************************
static bool do_timer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   switch (wParam) {
   case IDT_TIMER:
      update_timer_count(hwnd) ;
      break;
      
   default:
      return false ;
   }
   return true ;
}

//*******************************************************************
static bool do_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   DWORD cmd = HIWORD (wParam) ;
   DWORD target = LOWORD(wParam) ;
   // putf(&this_term, "WM_COMMAND: cmd=%u, target=%u", cmd, target) ;
   // If a button is clicked...
   if (cmd == BN_CLICKED) {
      switch (target) {
      case IDB_ABOUT:
         CmdAbout(hwnd);
         return true;

      case IDB_CLOSE:
         PostMessageA(hwnd, WM_CLOSE, 0, 0);
         return true;
      }  //lint !e744
   } 
   return false ;
}

//*******************************************************************
static bool do_user(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   POINT lpClickPoint;
      // event genereted by a system tray - the type of tray event that
      // generated the message can be found in lParam
      switch (lParam)   {
      case WM_RBUTTONUP:
      case WM_LBUTTONUP:
         GetCursorPos(&lpClickPoint);
         if (hPopMenu == 0) {
            hPopMenu = CreatePopupMenu();

            AppendMenu(hPopMenu, MF_STRING, ID_UNUSED, _T("derelict's binary clock")) ;
            AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL) ;

#ifdef  READY_FOR_BCLK_ELEMENTS
            for (j=0; j<NUM_ELEMENTS; j++) {
               // AppendMenu(hPopMenu, MF_STRING, 
               //       element_list[j]->get_menu_id(),
               //    _T(element_list[j]->get_menu_str())) ;
               HMENU hMenuTemp = (HMENU) element_list[j]->build_options_menu() ;
               // menu_handles[j] = hMenuTemp ;
               AppendMenu(hPopMenu, MF_POPUP, (UINT) hMenuTemp, _T(element_list[j]->get_menu_str())) ;
            }
#endif            

            AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL) ;
            // AppendMenu(hPopMenu, MF_STRING, ID_NEXT_COLOR,    _T("Select next color"));
            AppendMenu(hPopMenu, MF_STRING, ID_TOGGLE_LAYOUT,  _T("Toggle time format"));
            AppendMenu(hPopMenu, MF_STRING, ID_TRAYOPEN,       _T("Open clock window"));
            AppendMenu(hPopMenu, MF_STRING, ID_MINIMIZE,       _T("Minimize window"));
            AppendMenu(hPopMenu, MF_STRING, ID_TOGGLE_WINMSGS, _T("Toggle Winmsgs"));
            AppendMenu(hPopMenu, MF_STRING, ID_TRAYEXIT,       _T("Exit from program"));
            // CheckMenuItem (hPopMenu, (UINT) menu_handles[bitmap_idx], MF_CHECKED);
#ifdef  READY_FOR_BCLK_ELEMENTS
            CheckMenuItem (hPopMenu, (UINT) element_list[bitmap_idx]->get_menu_handle(), MF_CHECKED);
#endif            
         }

         SetForegroundWindow(hwnd);
         TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,
            lpClickPoint.x, lpClickPoint.y, 0, hwnd, NULL);
         break;

      // case WM_RBUTTONUP:
      //    // show window as response to right-clicking the tray icon
      //    ShowWindow (hwnd, SW_SHOWNORMAL);
      //    SetForegroundWindow (hwnd);
      //    break;
      }
   return true ;
}

//*******************************************************************
static bool do_close(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   DestroyWindow(hwnd);
   return true ;
}

//*******************************************************************
static bool do_destroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
{
   PostQuitMessage(0);
   return true ;
}

//*******************************************************************
struct winproc_table_s {
   uint win_code ;
   bool (*winproc_func)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data) ;
#ifdef USE_VECTOR_CLASS
   winproc_table_s (
      uint iwin_code,
      bool (*iwinproc_func)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)) ;
#endif      
} ;

#ifdef USE_VECTOR_CLASS
winproc_table_s::winproc_table_s (
   uint iwin_code,
   bool (*iwinproc_func)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data)
) :
   win_code(iwin_code),
   winproc_func(iwinproc_func)
{}
#endif

//  This conversion from C array to <vector> class,
//  change executable size from 32KB to 144KB
#ifdef USE_VECTOR_CLASS
static std::vector<winproc_table_s> winproc_table = {
#else
static winproc_table_s const winproc_table[] = {
#endif
{ WM_INITDIALOG,     do_init_dialog },
{ WM_TIMER,          do_timer },
{ WM_COMMAND,        do_command },
{ WM_USER,           do_user },
{ WM_CLOSE,          do_close },
{ WM_DESTROY,        do_destroy }
#ifndef USE_VECTOR_CLASS
,
{ 0, NULL } 
#endif
} ;

//*******************************************************************
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef USE_VECTOR_CLASS
   for(auto &winproc_element : winproc_table) {
      if (winproc_element.win_code == message) {
         return (*winproc_element.winproc_func)(hwnd, message, wParam, lParam, NULL) ;
      }
   }
#else
   uint idx ;
   for (idx=0; winproc_table[idx].win_code != 0; idx++) {
      if (winproc_table[idx].win_code == message) {
         return (*winproc_table[idx].winproc_func)(hwnd, message, wParam, lParam, NULL) ;
      }
   }
#endif   
   
   return false;
}  //lint !e715

//*********************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszArgument, int nFunsterStil)
{
   g_hInst = hInstance;
   //  create the main application
   HWND hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC) WndProc);
   if (hwnd == NULL) {
      return FALSE;
   }
   ShowWindow (hwnd, SW_SHOW) ;
   UpdateWindow(hwnd);

   MSG msg ;
   while (GetMessage (&msg, NULL, 0, 0)) {
      if (!IsDialogMessage(hwnd, &msg)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
      }
   }
   return (int) msg.wParam ;
}  //lint !e715

