#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

#include "resource.h"
#include "sprite.h"
#include "animation.h"

#define FASE_WINDOW_CLASS "FASE"
#define FASE_OPACITY 75

static HINSTANCE shInstance;

LRESULT CALLBACK faseSpriteWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HRGN ScanRegion(HBITMAP pBitmap, BYTE jTranspR, BYTE jTranspG, BYTE jTranspB);
static BYTE* Get24BitPixels(HBITMAP pBitmap, WORD *pwWidth, WORD *pwHeight);
static void getBMPSize(HBITMAP pBitmap, int *w, int *h);

void faseSpriteStartup(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    shInstance = hInstance;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = faseSpriteWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = shInstance;
    wcex.hIcon         = LoadIcon(shInstance, MAKEINTRESOURCE(IDI_FASE));
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = 0;//(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName  = 0;
    wcex.lpszClassName = FASE_WINDOW_CLASS;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassEx(&wcex);

    srand((unsigned)time(NULL));
}

faseSprite *faseSpriteCreate(int res, const faseMovement *moves, int count, faseSprite *top)
{
    HRGN rgn;
    int i;
    faseSprite *sprite = (faseSprite *)malloc(sizeof(faseSprite));

    sprite->anim = NULL;
    sprite->moves = moves;
    sprite->count = count;
    sprite->duration = 0;
    for(i = 0; i < sprite->count; ++i)
    {
        sprite->duration += sprite->moves[i].duration;
    }
    sprite->hbmp = LoadBitmap(shInstance, MAKEINTRESOURCE(res));
    getBMPSize(sprite->hbmp, &sprite->bmpw, &sprite->bmph);
    sprite->hwnd = CreateWindow(FASE_WINDOW_CLASS, "fase", WS_BORDER, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, top ? top->hwnd : NULL, NULL, shInstance, NULL);
    SetWindowLong(sprite->hwnd, GWL_USERDATA, (LONG)(intptr_t)sprite);
    SetWindowLong(sprite->hwnd, GWL_STYLE, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
    SetWindowLong(sprite->hwnd, GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(sprite->hwnd, 0, (255 * FASE_OPACITY) / 100, LWA_ALPHA);
    SetWindowPos(sprite->hwnd, HWND_TOPMOST, 0, -1000, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
    SetWindowPos(sprite->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

    rgn = ScanRegion(sprite->hbmp, 255, 0, 255);
    SetWindowRgn(sprite->hwnd, rgn, TRUE);
    DeleteObject(rgn);

    ShowWindow(sprite->hwnd, SW_SHOW);
    UpdateWindow(sprite->hwnd);
    InvalidateRect(sprite->hwnd, 0, TRUE);

    return sprite;
}

void faseSpriteDestroy(faseSprite *sprite)
{
    free(sprite);
}

void faseSpriteThink(faseSprite *sprite, int dt)
{
    while(dt > 0)
    {
        if(sprite->currentMove < sprite->count)
        {
            int firstMonitorW = GetSystemMetrics(SM_CXSCREEN);
            int firstMonitorH = GetSystemMetrics(SM_CYSCREEN);
            const faseMovement *move = &sprite->moves[sprite->currentMove];
            int moveX, moveY;

            // Calculate X
            if(move->x == LAST_DST)
            {
                moveX = sprite->x;
            }
            else if((move->x >= RANDOM0) && (move->x <= RANDOM9))
            {
                moveX = sprite->anim->randX[move->x - RANDOM0] % (firstMonitorW - sprite->anim->bigw);
            }
            else
            {
                if(move->anchor & ANCHOR_XL)
                {
                    moveX = move->x;
                }
                else if(move->anchor & ANCHOR_XC)
                {
                    moveX = move->x + (firstMonitorW >> 1);
                }
                else if(move->anchor & ANCHOR_XR)
                {
                    moveX = firstMonitorW - move->x - sprite->anim->bigw;
                }
            }

            // Calculate Y
            if(move->y == LAST_DST)
            {
                moveY = sprite->y;
            }
            else if((move->y >= RANDOM0) && (move->y <= RANDOM9))
            {
                moveY = sprite->anim->randY[move->y - RANDOM0] % (firstMonitorH - sprite->anim->bigh);
            }
            else
            {
                if(move->anchor & ANCHOR_YT)
                {
                    moveY = move->y;
                }
                else if(move->anchor & ANCHOR_YC)
                {
                    moveY = move->y + (firstMonitorH >> 1);
                }
                else if(move->anchor & ANCHOR_YB)
                {
                    moveY = firstMonitorH - move->y - sprite->anim->bigh;
                }
            }

            if((sprite->t + dt) < move->duration)
            {
                sprite->t += dt;
                dt = 0;
            }
            else
            {
                dt -= (move->duration - sprite->t);
                sprite->t = move->duration;
                sprite->x = moveX;
                sprite->y = moveY;

                if((sprite->currentMove + 1) < sprite->count)
                {
                    ++sprite->currentMove;
                    sprite->t = 0;
                }
                else
                {
                    dt = 0;
                }
            }

            //if((sprite->x != moveX) || (sprite->y != moveY))
            {
                int x, y;
                if(move->duration)
                {
                    x = sprite->x + (int)((moveX - sprite->x) * ((float)sprite->t / move->duration));
                    y = sprite->y + (int)((moveY - sprite->y) * ((float)sprite->t / move->duration));
                }
                else
                {
                    x = moveX;
                    y = moveY;
                }
                if(dt == 0)
                {
                    if(sprite->t == move->duration)
                    {
                        SetWindowPos(sprite->hwnd, HWND_TOPMOST, 0, -1000, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
                        SetWindowPos(sprite->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        ShowWindow(sprite->hwnd, SW_HIDE);
                    }
                    else
                    {
                        SetWindowPos(sprite->hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
                        SetWindowPos(sprite->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        ShowWindow(sprite->hwnd, SW_SHOW);
                    }
                }
            }
        }
    }
}

void faseSpriteHide(faseSprite *sprite)
{
    ShowWindow(sprite->hwnd, SW_HIDE);
}

void faseSpriteReset(faseSprite *sprite, int now)
{
    sprite->currentMove = 0;
    sprite->x = -1000000;
    sprite->y = -1000000;
    sprite->t = 0;
    faseSpriteThink(sprite, 0);
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

static void faseSpritePaint(faseSprite *sprite, LPPAINTSTRUCT ps)
{
    RECT r;
    HDC dc = GetDC(sprite->hwnd);
    HDC bmpDC = CreateCompatibleDC(dc);
    GetClientRect(sprite->hwnd, &r);
    SelectObject(bmpDC, sprite->hbmp);
    BitBlt(dc, 0, 0, r.right, r.bottom, bmpDC, 0, 0, SRCCOPY);
    ReleaseDC(sprite->hwnd, dc);
    DeleteDC(bmpDC);
}

LRESULT CALLBACK faseSpriteWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    faseSprite *sprite = (faseSprite *)(intptr_t)GetWindowLong(hWnd, GWL_USERDATA);

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
        faseSpritePaint(sprite, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;
        break;
    case WM_PAINT:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
