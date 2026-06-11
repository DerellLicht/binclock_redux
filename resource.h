//***********************************************************************
//  Window-control access codes
//  Windows resource conventions (plus some DDM additions)
//    
//    IDM_  Menu ID
//    IDI_  Icon ID
//    IDT_  Timer ID
//    IDD_  Dialog ID
//    IDS_  Static control ID
//    IDC_  Control ID (generic)
//    IDB_  Button ID
//    
//    WM_   Windows Message
//    
//***********************************************************************

//  main-window resources
#define  IDC_STATIC          16383
#define  IDI_APPICON           300

#define  IDD_MAIN_DIALOG      1000
#define  IDD_ABOUT            1001

#define  IDC_WEBLINK          1010
#define  IDC_WEBLINK2         1011
#define  IDC_VERNUM           1012
#define  IDB_ABOUT            1013
#define  IDB_CLOSE            1014

// #define  IDD_MAIN_DIALOG      300
#define  BCICON               301
#define  IDT_TIMER            302
#define  BM_BALLS             303

#define  IDS_HOURS            400
#define  IDC_HOURS            401
#define  IDS_MINS             402
#define  IDC_MINS             403
#define  IDS_SECS             404
#define  IDC_SECS             405

#define  ID_TRAYMENU          406
#define  ID_TRAYOPEN          407
#define  ID_MINIMIZE          408
#define  ID_TOGGLE_LAYOUT     409
                              
#define  ID_NEXT_COLOR        410
#define  ID_TRAYEXIT          411
#define  ID_UNUSED            412
                              
#define  ID_TOGGLE_WINMSGS    413

//  This must be larger than all other IDs
#define  ID_LAMPS0           2000



