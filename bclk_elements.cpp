//***********************************************************************
//  adding new graphics sets to binclock
//  - add sprite name
//  - LoadImage / DestroyObject
//  - define WIDTH/HEIGHT/OFFSET/MASK
//  - add masking function
//  - add drawing function
//  - build SystemTray menu dynamically
//***********************************************************************

#include <windows.h>
#include <utility>   //  for std::move

#include "common.h"
#include "bclk_elements.h"

//lint -esym(1714, bclock_element::bclock_element, bclock_element::get_on_color)
//lint -esym(1714, bclock_element::get_off_color, bclock_element::next_led_color)
//lint -esym(1714, bclock_element::get_menu_code, bclock_element::get_flags)

//lint -esym(1762, bclock_element::Box, bclock_element::Solid_Rect, bclock_element::select_color)

//  main menu number
static unsigned be_object_num = 0 ;

//***********************************************************************
char *bclock_element::get_system_message(void) const
{
#define  GSM_LEN     1024
   static char msg[GSM_LEN+1] ;
   // int slen ;
   uint errcode = 0 ;
   int result = (int) GetLastError() ;
   if (result < 0) {
      result = -result ;
      errcode = (DWORD) result ;
      // wsprintfA(msg, "Win32: unknown error code %d", result) ;
      // return msg;
   }
   LPVOID lpMsgBuf;
   DWORD dresult = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errcode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (char *) &lpMsgBuf, //lint !e740
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   if (dresult == 0) {
      DWORD glError = GetLastError() ;
      if (glError == 317) {   //  see comment at start of function
         sprintf(msg, "FormatMessage(): no message for error code %d", result) ;
      } else {
         sprintf(msg, "FormatMessage() failed: [%u], errcode %d", (uint) GetLastError(), result) ;
      }
      
   } else
   if (lpMsgBuf == NULL) {
      sprintf(msg, "NULL buffer in response from FormatMessage() [%u]", (uint) GetLastError()) ;
   } else 
   {
      strncpy((char *) msg, (char *) lpMsgBuf, GSM_LEN) ;
      // Free the buffer.
      LocalFree( lpMsgBuf );
   }

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;

   return msg;
}

//***********************************************************************
//  this function is not currently in use
//***********************************************************************
unsigned bclock_element::next_led_color(void)
{
   if (flags & BE_DRAWN) {
      
   } else {
      curr_element++ ;
      while (LOOP_FOREVER) {
         if (curr_element == num_elements) {
            curr_element = 0 ;
            continue;
         }
         if (skip_elementsv[curr_element] != 0) {
            curr_element++ ;
            continue;
         }
         break;
      }
      if (flags & BE_PAIRS) 
         off_idx = curr_element - 1 ;
   }

   return curr_element ;
}

//***********************************************************************
bclock_element::bclock_element(HINSTANCE g_hInst, char *name, unsigned width, 
   unsigned be_flags, int mask_index, unsigned off_index, unsigned start_element) :
   bm_name(""),
   el_width(width),
   el_height(0),
   flags(be_flags),
   mask_idx(mask_index), //  negative means no mask is used
   off_idx(off_index),
   x_offset(0),
   y_offset(0),
   // skip_elements(nullptr),
   skip_elementsv(),
   num_elements(0),
   menu_code(0),
   curr_element(start_element),
   hSpriteBitmap(0),
   menu_hdl(0),
   // object_code(0),
   object_code(be_object_num++),
   color_menu_str_list(),
   menu_str(""),
   attr_lhigh(0),
   attr_llow(0),
   attr_high(0),
   attr_low(0)
{
   BITMAP bm;
   // object_code = be_object_num++ ;

   //  if no filename provided, assume BE_DRAWN format
   if (name == 0) {
      // bm_name[0] = 0 ;
      // hSpriteBitmap = 0 ;

      el_height = width ;
      //  for drawn types, num_elements (which is an overloaded variable
      //  which represents *both* number of drawable elements in the bitmap,
      //  and number of string elements in the menu-string list
      num_elements = 4 ;   //  hard-coded for now

      //  BE_DRAWN attributes
      attr_lhigh = GetSysColor(COLOR_BTNHIGHLIGHT) ;
      attr_llow  = GetSysColor(COLOR_BTNSHADOW) ;
      attr_high  = RGB(0, 255, 0) ;
      attr_low   = RGB(63, 63, 63) ;
   } 
   //  open the bitmap file, read sprite images into memory
   else {
      strncpy(bm_name, name, sizeof(bm_name)-1) ;
      hSpriteBitmap = (HBITMAP) LoadImage (g_hInst, bm_name, 
         IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
      if (hSpriteBitmap == NULL) {
         syslog("%s: LoadImage: %s\n", bm_name, get_system_message()) ;
      }
      if (GetObject ((HGDIOBJ) hSpriteBitmap, sizeof (BITMAP), &bm) == 0) {
         syslog("%s: GetObject: %s\n", bm_name, get_system_message()) ;
      }
      el_height = bm.bmHeight ;
      if (el_width == 0) 
         el_width = bm.bmHeight ;
      num_elements = (unsigned) bm.bmWidth / el_width ;
   }
   
   // syslog("%s: loaded, width=%u, elements=%u\n", bm_name, el_width, num_elements) ;
   unsigned j ;
   for (j=0; j<num_elements; j++) {
      char errstr[81] ;
      wsprintf(errstr, "color %u", j) ;
      color_menu_str_list.emplace_back(errstr);
      
      skip_elementsv.emplace_back(0) ;
   }
   // syslog("sizeof skip_elementsv [a]: %u elements, num_elements: %u\n", skip_elementsv.size(), num_elements);

   // skip_elements = new u8[num_elements] ;
   // ZeroMemory(skip_elements, num_elements) ;

   if (off_idx < num_elements) {
      // skip_elements[off_idx] = 1 ;
      skip_elementsv[off_idx] = 1 ;
   }
   if (mask_idx >= 0  &&  (unsigned) mask_idx < num_elements) {
      // skip_elements[mask_idx] = 1 ; //lint !e661 !e662
      skip_elementsv[mask_idx] = 1 ;
   }
}

//***********************************************************************
//  create a move constructor
//***********************************************************************
// bclock_element::bclock_element(bclock_element&& obj) noexcept
// {
//    if (this != &obj) {
//       //  *this = std::move(obj);   //  I'm not sure about this
//       hSpriteBitmap = std::exchange(obj.hSpriteBitmap, nullptr) ; // HBITMAP 
//       menu_hdl = std::exchange(obj.menu_hdl, nullptr) ;  // HMENU 
//    }
// }

//***********************************************************************
//  create a move assignment operator
//***********************************************************************
// bclock_element &bclock_element::operator=(bclock_element &&obj) noexcept
// {
//    if (this != &obj) {
//       hSpriteBitmap = std::exchange(obj.hSpriteBitmap, nullptr) ; // HBITMAP 
//       menu_hdl = std::exchange(obj.menu_hdl, nullptr) ;  // HMENU 
//    }
//    return *this;
// }

//***********************************************************************
// pointer member not directly freed or zeroed by destructor
//lint -esym(1740, bclock_element::skip_elements, bclock_element::hSpriteBitmap)
//lint -esym(1740, bclock_element::menu_hdl, bclock_element::color_menu_str)

bclock_element::~bclock_element()
{
   DeleteObject ((HGDIOBJ) hSpriteBitmap);
   if (menu_hdl != 0) {
      DestroyMenu(menu_hdl);
   }
}

//******************************************************************
//  these are used only for BE_DRAWN option
//******************************************************************
void bclock_element::set_element_attr(COLORREF fgnd, COLORREF bgnd)
{
   attr_high = fgnd ;
   attr_low  = bgnd ;
}

//******************************************************************
void bclock_element::set_image_offsets(int dx, int dy)
{
   x_offset = dx ;
   y_offset = dy ;
}

//******************************************************************
//  this function sets aside menuID codes for all of its 
//  sub-menu elements.  So sub-menu item I = umenu_code + I
//  It then returns the next valid menuID
//******************************************************************
unsigned bclock_element::add_menu_data(unsigned umenu_code, char *mstr)
{
   menu_code = umenu_code ;
   strncpy(menu_str, mstr, sizeof(menu_str)) ;
   menu_str[sizeof(menu_str) - 1] = 0 ; //  make sure line is NULL-term
   umenu_code += num_elements ;  //  reserve menu/message numbers for all colors
   return umenu_code ;
}

//******************************************************************
void bclock_element::add_color_menu_str(unsigned menu_idx, char *mstr)
{
   //  for BE_DRAWN, num_elements is currently hard-coded in constructor...
   //  There's gotta be a better way to handle that...
   if (menu_idx >= num_elements)
      return ;

   // color_menu_str_list.emplace_back(mstr);
   color_menu_str_list[menu_idx] = mstr ;
}

//****************************************************************
void bclock_element::check_menu_item(HMENU hPopMenu, uint checked_state)
{
   CheckMenuItem (hPopMenu, (UINT) menu_hdl, checked_state);
}

//****************************************************************
void bclock_element::check_sub_menu_item(HMENU hPopMenu, uint checked_state)
{
   CheckMenuItem (hPopMenu, (UINT) menu_code + curr_element, checked_state);
}

//****************************************************************
COLORREF bclock_element::select_color(COLORREF init_attr)
{
   static CHOOSECOLOR cc ;
   static COLORREF    crCustColors[16] ;

   ZeroMemory(&cc, sizeof(cc));
   cc.lStructSize    = sizeof (CHOOSECOLOR) ;
   // cc.rgbResult      = RGB (0x80, 0x80, 0x80) ; // set initial color in dialog
   cc.rgbResult      = init_attr ; // set initial color in dialog
   cc.lpCustColors   = crCustColors ;
   cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;

   if (ChooseColor(&cc) == TRUE) {
      return cc.rgbResult ;
   } else {
      return 0 ;
   }
}

//*********************************************************************************
//  Okay, this is going a little awry...
//  For BE_DRAWN, which has SetColor functions in the
//  color-selection menu, this function needs to act differently;
//  
//  For BE_DRAWN, what this function needs is some way to communicate
//  the fgnd/bgnd colors to the parent code, to save in config file.
//*********************************************************************************
int bclock_element::get_menu_id(unsigned menu_idx) 
{
   if (menu_idx < menu_code) 
      return -1 ;
   if (menu_idx >= (menu_code+num_elements)) 
      return -1 ;
   //  otherwise, it's to us
   curr_element = menu_idx - menu_code ;
   if ((flags & BE_DRAWN)  &&  curr_element > 1) { // Bound Boxes
      if (curr_element == 2) {   //  set foreground color
         attr_high = select_color(attr_high) ;
      } else
      if (curr_element == 3) {   //  set background color
         attr_low = select_color(attr_low) ;
      } 
      //  -1 is returned if color change was selected
      return -1;
   } 
   if (flags & BE_PAIRS) {
      off_idx = curr_element - 1 ;
   }
   return object_code ;
}

//******************************************************************
char *bclock_element::get_menu_str(void) 
{
   return menu_str;  //lint !e1536  Exposing low access member 'bclock_element::menu_str'
}

//******************************************************************
//  mask the target array (do this ONCE)
//******************************************************************
void bclock_element::mask_the_source(HDC hdc)
{
   if (mask_idx < 0)
      return ;
   unsigned xmask  = (unsigned) mask_idx * el_width  ;

   HDC hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hSpriteBitmap);

   unsigned j ;
   for (j=0; j<num_elements; j++) {
      //  don't mask the mask image!!
      if (mask_idx >= 0  &&  j == (unsigned) mask_idx)
         continue;

      unsigned xsrc = j * el_width  ;
      if (!BitBlt (hdcMem, xsrc, 0, el_width, el_height, hdcMem, xmask, 0, SRCINVERT)) {
         syslog("BitBlt (source mask): %s", get_system_message()) ;
      }
   }

   DeleteDC (hdcMem);
}

//******************************************************************
// this builds the second-level menus below the style menus
//******************************************************************
void bclock_element::debug_dump_data(void)
{
   char msg[81] ;
   uint element_idx = 0 ;
   unsigned j ;
   uint uoffset = 0 ;
   uoffset += (uint) sprintf(msg, "%2u [%2u] ", element_idx, num_elements) ;
   
   for (j=0; j<num_elements; j++) {
      uoffset += (uint) sprintf(msg+uoffset, "%u ", skip_elementsv[j]) ;
   }
   syslog("%s\n", msg);
}

//******************************************************************
// this builds the second-level menus below the style menus
//******************************************************************
HMENU bclock_element::build_options_menu(void)
{
   unsigned j ;
   // char mmsg[40] ;
   HMENU hMenuOptions = CreateMenu() ;

   // AppendMenu(hMenuOptions, MF_STRING, menu_code, "switch to ME!") ;
   for (j=0; j<num_elements; j++) {
      //  don't mask the mask image!!
      if (skip_elementsv[j])
         continue;

      // AppendMenu(hMenuOptions, MF_STRING, menu_code+j, color_menu_str[j]) ;
      AppendMenu(hMenuOptions, MF_STRING, menu_code+j, color_menu_str_list[j].c_str()) ;
   }
   menu_hdl = hMenuOptions ;
   return hMenuOptions;
}

//************************************************************************
//  WARNING: THIS FUNCTION IS FLAWED !!
//  The act of calling CreatePen() and SelectObject() once,
//  then calling them a second time and over-writing the previous resources,
//  causes the graphic environment to become corrupted after a few minutes,
//  probably due to running out of one of the resources.
//************************************************************************
void bclock_element::BoxDoNotUse(HDC hdc, int x0, int y0, int x1, int y1, unsigned style, COLORREF fgattr, COLORREF bgattr)
{
   HPEN hPen = 0 ;

   switch (style) {
   case BX_SHADOW_OUTER:
      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y1, NULL) ;
      LineTo  (hdc, x0, y0) ;
      LineTo  (hdc, x1, y0) ;

      hPen = CreatePen(PS_SOLID, 1, bgattr) ;
      SelectObject(hdc, hPen) ;

      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      SetPixel(hdc, x0, y1, bgattr) ;
      break;

   case BX_SHADOW_INNER:
      hPen = CreatePen(PS_SOLID, 1, bgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y1, NULL) ;
      LineTo  (hdc, x0, y0) ;
      LineTo  (hdc, x1, y0) ;

      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      SetPixel(hdc, x0, y1, fgattr) ;
      break;

   case BX_SOLID:
   default:
      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y0, NULL) ;
      LineTo  (hdc, x1, y0) ;
      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      LineTo  (hdc, x0, y0) ;
      break;

   }

   if (hPen != 0) {
      SelectObject(hdc, GetStockObject(BLACK_PEN)) ;  //  deselect my pen
      DeleteObject (hPen) ;
   }
}

/************************************************************************/
void bclock_element::Solid_Box(HDC hdc, int x0, int y0, int x1, int y1, COLORREF Color)
{
   HPEN hPen = CreatePen(PS_SOLID, 1, Color) ;
   SelectObject(hdc, hPen) ;

   MoveToEx(hdc, x0, y0, NULL) ;
   LineTo  (hdc, x1, y0) ;
   LineTo  (hdc, x1, y1) ;
   LineTo  (hdc, x0, y1) ;
   LineTo  (hdc, x0, y0) ;

   SelectObject(hdc, GetStockObject(BLACK_PEN)) ;  //  deselect my pen
   DeleteObject (hPen) ;
}

//************************************************************************
void bclock_element::Solid_Rect(HDC hdc, int xl, int yu, int xr, int yl, COLORREF Color)
{
   HBRUSH hBrush ;
   RECT   rect ;

   if (SetRect (&rect, xl, yu, xr, yl) == 0) {
      syslog("SetRect: %s\n", get_system_message());
   }
   hBrush = CreateSolidBrush (Color) ;
   FillRect (hdc, &rect, hBrush) ;
   DeleteObject (hBrush) ;
}

//*********************************************************************
//  this draws the clock frame (for Bound Boxes style)
//*********************************************************************
void bclock_element::draw_frame(HDC hdc, unsigned x, unsigned y, unsigned on_noff)
{
   unsigned xl = x ;
   unsigned yt = y ;

   // syslog("x=%u, y=%u, th=Tu, pad=%u, fw=%u\n",
   //    x, y, fthickness, fpadding) ;
   // unsigned xr = x + get_frame_width() ;
   // unsigned yb = y + get_frame_height() ;
   // unsigned frame_edge = fthickness + fpadding ;
   unsigned xr = x + el_width ;
   unsigned yb = y + el_height ;

   Solid_Rect(hdc, xl, yt, xr, yb, (on_noff) ? attr_high : attr_low) ;

   // Box(hdc, xl, yt, xr, yb, BX_SHADOW_OUTER, attr_lhigh, attr_llow) ;
   Solid_Box(hdc, xl, yt, xr, yb, attr_lhigh) ;
   xl++ ;  yt++ ;  xr-- ;  yb-- ;
   // Box(hdc, xl, yt, xr, yb, BX_SHADOW_INNER, attr_lhigh, attr_llow) ;
   Solid_Box(hdc, xl, yt, xr, yb, attr_llow) ;
}

//******************************************************************
void bclock_element::draw_sprite(HDC hdc, unsigned on_noff, unsigned xidest, unsigned yidest)
{
   unsigned xmask, xsrc ;
   int xdest, ydest ;
   HDC hdcMem ;

   unsigned scol = (on_noff == 0) ? off_idx : curr_element ;
   xsrc  = scol * el_width  ;
   // ysrc  = srow * el_height ;
   xdest = (int) xidest + x_offset ;
   ydest = (int) yidest + y_offset ;
   // syslog("mask_idx=%d\n", mask_idx) ;

   if (flags & BE_DRAWN) {
      draw_frame(hdc, xidest, yidest, on_noff) ;
   } else
   if (mask_idx < 0) {
      hdcMem = CreateCompatibleDC (hdc);
      SelectObject (hdcMem, (HGDIOBJ) hSpriteBitmap);

      if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xsrc, 0, SRCCOPY)) {
         syslog("BitBlt (copy): %s", get_system_message()) ;
      }
      DeleteDC (hdcMem);
   }
   else {
      hdcMem = CreateCompatibleDC (hdc);
      SelectObject (hdcMem, (HGDIOBJ) hSpriteBitmap);

      xmask  = (unsigned) mask_idx * el_width  ;
      if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xmask, 0, SRCAND)) {
         syslog("BitBlt (mask): %s", get_system_message()) ;
      }
      if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xsrc, 0, SRCPAINT)) {
         syslog("BitBlt (image): %s", get_system_message()) ;
      }
      DeleteDC (hdcMem);
   }
}

