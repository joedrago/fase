#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"

#define FASE_WINDOW_CLASS "FASE"
#define FASE_TIMER_ID 5
#define FASE_TIMER_UPDATE_MS 10
#define FASE_OPACITY 75

typedef enum AnimationState
{
    AS_ENTERING = 0,
    AS_SHOWING,
    AS_LEAVING,
    AS_WAITING,

    AS_COUNT
} AnimationState;

typedef struct Animation
{
    unsigned int start;
    int duration;
    int srcX;
    int srcY;
    int dstX;
    int dstY;
    AnimationState state;
} Animation;

static HINSTANCE shInstance;
static HBITMAP shFaseBitmap;
static Animation sAnim = {0};
static int sVisitOnlyOnce = 0;

static void getBMPSize(HBITMAP pBitmap, int *w, int *h);
static HRGN ScanRegion(HBITMAP pBitmap, BYTE jTranspR, BYTE jTranspG, BYTE jTranspB);
static void faseThink();

static void fasePaint(HWND hWnd, LPPAINTSTRUCT ps)
{
    RECT r;
    HDC dc = GetDC(hWnd);
    HDC bmpDC = CreateCompatibleDC(dc);
    GetClientRect(hWnd, &r);
    SelectObject(bmpDC, shFaseBitmap);
    BitBlt(dc, 0, 0, r.right, r.bottom, bmpDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, dc);
    DeleteDC(bmpDC);
}

static void faseAnimate(int X, int fromY, int toY, int duration)
{
    sAnim.srcX = X;
    sAnim.srcY = fromY;
    sAnim.dstX = X;
    sAnim.dstY = toY;

    sAnim.duration = duration;
    sAnim.start = GetTickCount();
    faseThink();
}

static void faseStartAnimation(HWND hWnd)
{
    sAnim.state = AS_WAITING;
    faseThink(hWnd);
}

static void faseThink(HWND hWnd)
{
    int now = (int)GetTickCount();
    int dt = now - sAnim.start;
    if(sAnim.duration && (dt <= sAnim.duration))
    {
        // continue animation

        if((sAnim.srcX != sAnim.dstX) || (sAnim.srcY != sAnim.dstY))
        {
            int x = sAnim.srcX + (int)((sAnim.dstX - sAnim.srcX) * ((float)dt / sAnim.duration));
            int y = sAnim.srcY + (int)((sAnim.dstY - sAnim.srcY) * ((float)dt / sAnim.duration));
            SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
        }
    }
    else
    {
        // setup next animation

        AnimationState nextState = (sAnim.state + 1) % AS_COUNT;
        int firstMonitorW = GetSystemMetrics(SM_CXSCREEN);
        int firstMonitorH = GetSystemMetrics(SM_CYSCREEN);
        int faseW, faseH;
        int hidePosY, showPosY;

        getBMPSize(shFaseBitmap, &faseW, &faseH);
        hidePosY = firstMonitorH;
        showPosY = firstMonitorH - (faseH / 3);

        switch(nextState)
        {
        case AS_ENTERING:
            faseAnimate(rand() % (firstMonitorW - faseW), hidePosY, showPosY, 2000);
            break;
        case AS_SHOWING:
            faseAnimate(sAnim.dstX, sAnim.dstY, sAnim.dstY, 4000);
            break;
        case AS_LEAVING:
            faseAnimate(sAnim.dstX, showPosY, hidePosY, 1000);
            break;
        case AS_WAITING:
            if(sVisitOnlyOnce)
                PostQuitMessage(0);
            faseAnimate(sAnim.dstX, sAnim.dstY, sAnim.dstY, 2000);
            break;
        }
        sAnim.state = nextState;
    }
}

static LRESULT CALLBACK faseWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_ERASEBKGND:
        hdc = BeginPaint(hWnd, &ps);
        fasePaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;
        break;
    case WM_PAINT:
        break;
    case WM_TIMER:
        faseThink(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

static ATOM faseRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = faseWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FASE));
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = 0;//(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName  = 0;
    wcex.lpszClassName = FASE_WINDOW_CLASS;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassEx(&wcex);
}

static void faseRegion(HWND hWnd)
{
    HRGN rgn = ScanRegion(shFaseBitmap, 255, 0, 255);
    SetWindowRgn(hWnd, rgn, TRUE);
    DeleteObject(rgn);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HWND hWnd;
    const char *c;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    c = lpCmdLine;
    for(; *c; ++c)
    {
        if(*c == '-')
        {
            for(; *c && (*c != ' ') && (*c != '\t'); ++c)
            {
                switch(*c)
                {
                case 'o':
                    sVisitOnlyOnce = 1;
                };
            }
        }
    }

    shInstance = hInstance;
    shFaseBitmap = LoadBitmap(shInstance, MAKEINTRESOURCE(IDB_FASE));
    faseRegisterClass(hInstance);

    hWnd = CreateWindow(FASE_WINDOW_CLASS, "fase", WS_BORDER, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    SetWindowLong(hWnd, GWL_STYLE, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
    SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hWnd, 0, (255 * FASE_OPACITY) / 100, LWA_ALPHA);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, -1000, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    faseRegion(hWnd);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    InvalidateRect(hWnd, 0, TRUE);
    faseStartAnimation(hWnd);
    SetTimer(hWnd, FASE_TIMER_ID, FASE_TIMER_UPDATE_MS, NULL);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

static void getBMPSize(HBITMAP pBitmap, int *w, int *h)
{
    BITMAP bmpBmp;
    LPBITMAPINFO pbmiInfo;

    GetObject(pBitmap, sizeof(bmpBmp),&bmpBmp);
    pbmiInfo = (LPBITMAPINFO)&bmpBmp;

    *w = (int)pbmiInfo->bmiHeader.biWidth;
    *w -= (*w % 4);
    *h = pbmiInfo->bmiHeader.biHeight;
}

static BYTE* Get24BitPixels(HBITMAP pBitmap, WORD *pwWidth, WORD *pwHeight)
{
    BITMAP bmpBmp;
    LPBITMAPINFO pbmiInfo;
    BITMAPINFO bmiInfo;
    WORD wBmpWidth, wBmpHeight;
    BYTE *pPixels;
    HDC hDC;
    int iRes;

    // ---------------------------------------------------------
    // get some info from the bitmap
    // ---------------------------------------------------------
    GetObject(pBitmap, sizeof(bmpBmp),&bmpBmp);
    pbmiInfo = (LPBITMAPINFO)&bmpBmp;

    // get width and height
    wBmpWidth  = (WORD)pbmiInfo->bmiHeader.biWidth;
    wBmpWidth -= (wBmpWidth%4);                       // width is 4 byte boundary aligned.
    wBmpHeight = (WORD)pbmiInfo->bmiHeader.biHeight;

    // copy to caller width and height parms
    *pwWidth  = wBmpWidth;
    *pwHeight = wBmpHeight;
    // ---------------------------------------------------------

    // allocate width * height * 24bits pixels
    pPixels = malloc(wBmpWidth*wBmpHeight*3);
    if (!pPixels) return NULL;

    // get user desktop device context to get pixels from
    hDC = GetWindowDC(NULL);

    // fill desired structure
    bmiInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiInfo.bmiHeader.biWidth = wBmpWidth;
    bmiInfo.bmiHeader.biHeight = -wBmpHeight;
    bmiInfo.bmiHeader.biPlanes = 1;
    bmiInfo.bmiHeader.biBitCount = 24;
    bmiInfo.bmiHeader.biCompression = BI_RGB;
    bmiInfo.bmiHeader.biSizeImage = wBmpWidth*wBmpHeight*3;
    bmiInfo.bmiHeader.biXPelsPerMeter = 0;
    bmiInfo.bmiHeader.biYPelsPerMeter = 0;
    bmiInfo.bmiHeader.biClrUsed = 0;
    bmiInfo.bmiHeader.biClrImportant = 0;

    // get pixels from the original bitmap converted to 24bits
    iRes = GetDIBits(hDC,pBitmap,0,wBmpHeight,(LPVOID)pPixels,&bmiInfo,DIB_RGB_COLORS);

    // release the device context
    ReleaseDC(NULL,hDC);

    // if failed, cancel the operation.
    if (!iRes)
    {
        free(pPixels);
        return NULL;
    };

    // return the pixel array
    return pPixels;
}

static HRGN ScanRegion(HBITMAP pBitmap, BYTE jTranspR, BYTE jTranspG, BYTE jTranspB)
{
  WORD wBmpWidth,wBmpHeight;
  HRGN hRgn, hTmpRgn;
  DWORD p;
  WORD x, y;

  BYTE *pPixels = Get24BitPixels(pBitmap, &wBmpWidth, &wBmpHeight);
  if (!pPixels) return NULL;

  // create our working region
  hRgn = CreateRectRgn(0,0,wBmpWidth,wBmpHeight);
  if (!hRgn) { free(pPixels); return NULL; }

  // ---------------------------------------------------------
  // scan the bitmap
  // ---------------------------------------------------------
  p=0;
  for (y=0; y<wBmpHeight; y++)
  {
    for (x=0; x<wBmpWidth; x++)
    {
      BYTE jRed   = pPixels[p+2];
      BYTE jGreen = pPixels[p+1];
      BYTE jBlue  = pPixels[p+0];

      if (jRed == jTranspR && jGreen == jTranspG && jBlue == jTranspB)
      {
        // remove transparent color from region
        hTmpRgn = CreateRectRgn(x,y,x+1,y+1);
        CombineRgn(hRgn, hRgn, hTmpRgn, RGN_XOR);
        DeleteObject(hTmpRgn);
      }

      // next pixel
      p+=3;
    }
  }

  // release pixels
  free(pPixels);

  // return the region
  return hRgn;
}