// WindowsHookTest1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WindowsHookTest1.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HANDLE outputFile;
LPDWORD writeSize;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    LowLevelMouseProc(int, WPARAM, LPARAM);
void consolePrint(const char*, size_t);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow) {
  
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_WINDOWSHOOKTEST1, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);
  
  AllocConsole();
  AttachConsole(ATTACH_PARENT_PROCESS);

  outputFile = CreateFileA((LPCSTR)"testFile.txt", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
  
  WriteFile((HANDLE)STD_OUTPUT_HANDLE, "\ntesting...", sizeof("\ntesting..."), writeSize, NULL);
  WriteFile(outputFile, "\ntesting...", sizeof("\ntesting..."), writeSize, NULL);

  SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, hInstance, NULL);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSHOOKTEST1));

  MSG msg;

  // Main message loop:
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int) msg.wParam;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSHOOKTEST1));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;/*MAKEINTRESOURCEW(IDC_WINDOWSHOOKTEST1);*/
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}





//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  
  hInst = hInstance; // Store instance handle in our global variable

  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) {
    return FALSE;
  }

  //ShowWindow(hWnd, nCmdShow);
  //UpdateWindow(hWnd);

  return TRUE;
}




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  
  static char x[4];
  static char y[4];

  switch (message) {

    case WM_CREATE:
      
      WriteFile((HANDLE) STD_OUTPUT_HANDLE, "\nCreating Window...", sizeof("\nCreating Window..."), writeSize, NULL);
      WriteFile(outputFile, L"\nCreating Window...", sizeof(L"\nCreating Window..."), NULL, NULL);
      break;

    case WM_COMMAND:
    {
      int wmId = LOWORD(wParam);
      // Parse the menu selections:
      switch (wmId) {

        case IDM_EXIT:
          DestroyWindow(hWnd);
          break;
        
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
    }
    break;
    
    case WM_MOUSEMOVE:
      memset(x, 0, 4);
      memset(y, 0, 4);
      memmove(x, std::to_string(GET_X_LPARAM(lParam)).c_str(), 4);
      memmove(y, std::to_string(GET_Y_LPARAM(lParam)).c_str(), 4);

      WriteFile((HANDLE) STD_OUTPUT_HANDLE, "\nWM_MOUSEMOVE: ", sizeof("\nWM_MOUSEMOVE: "), NULL, NULL);
      WriteFile((HANDLE) STD_OUTPUT_HANDLE, x, 4, NULL, NULL);
      WriteFile((HANDLE) STD_OUTPUT_HANDLE, y, 4, NULL, NULL);
      break;
    
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // TODO: Add any drawing code that uses hdc here...
      EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}





LRESULT LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  
  static char x[4];
  static char y[4];
  static LPPOINT point;
  static size_t sizeofcoordinate = sizeof(LONG);

  switch (wParam) {    

    case WM_LBUTTONDOWN:
      ShowCursor(0);
      consolePrint("\nWM_LBUTTONDOWN", sizeof("\nWM_LBUTTONDOWN"));
      break;
    
    case WM_LBUTTONUP:
      ShowCursor(1);
      consolePrint("\nWM_LBUTTONUP", sizeof("\nWM_LBUTTONUP"));
      break;
    
    case WM_MOUSEMOVE:
      
      GetCursorPos(point);                        

      consolePrint("\nWM_MOUSEMOVE", sizeof("\nWM_MOUSEMOVE"));
      consolePrint(std::to_string(point->x).c_str(), 4);
      consolePrint(" ", 1);
      consolePrint(std::to_string(point->y).c_str(), 4);
      
      
      break;

    default:
      break;
  }
  
  return LRESULT();
}

void consolePrint(const char * msg, size_t msg_size) {
  WriteFile((HANDLE) STD_OUTPUT_HANDLE, msg, msg_size, NULL, NULL);
}
