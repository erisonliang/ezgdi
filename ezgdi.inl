#pragma once

#include "ezgdi.hpp"
#include <mmsystem.h>

#pragma comment(lib,"Winmm.lib")

#define EZGDI_CLASS_NAME (L"EZGDI_WINDOW")

//����ezgdi��ʵ��
EZGDI_PUBLIC ezgdi::ezInstance<>& __ezgdi_instance = ezgdi::ezInstance<>::instance;

//������Ϣѭ��
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

//����������
void SwapBuffers(HDC hDC, Gdiplus::Image* image)
{
    if(image){
        Gdiplus::Graphics g(hDC);
        g.DrawImage(image, 0, 0, image->GetWidth(), image->GetHeight());
    }
}

//�����ػ��¼�
int OnWindowPaint(HWND hwnd)
{
    HDC hDC = GetDC(hwnd);
    SwapBuffers(hDC, __ezgdi_instance.colorbuf);
    ReleaseDC(hwnd, hDC);
    return 0;
}

//�������ں���
HWND InitWindow(
    LPCWSTR title,      //����
    int     x,          //λ��x
    int     y,          //λ��y
    int     width,      //���
    int     height,     //�߶�
    DWORD   style,      //���ڷ��
    DWORD   styleEx     //��չ���
    )
{
    WNDCLASSEXW wc;
    HWND hwnd;
    HINSTANCE hInstance = GetModuleHandle(NULL);//����ʵ�����

    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WindowProc; //��Ϣ������
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = EZGDI_CLASS_NAME; //��������
    wc.hIcon         = NULL;//LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm       = NULL;//LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassExW(&wc)){//ע�ᴰ����
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"), MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    hwnd = CreateWindowExW(
        styleEx,
        EZGDI_CLASS_NAME,   //����
        title,              //����
        style,              //���
        x,                  //���λ��
        y,                  //����λ��
        width,              //���
        height,             //�߶�
        NULL,
        NULL,
        hInstance,
        NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"), MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    ezgdi_loop();

    return hwnd;
}

//windows����Ϣ������
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message){
    case WM_CREATE:
        __ezgdi_instance.gdiplusInit();
        __ezgdi_instance.initGraphics(1920, 1080);
        break;
    case WM_DESTROY:
        __ezgdi_instance.closeGraphics();
        __ezgdi_instance.gdiplusShutdown();
        PostQuitMessage(0);
        break;
    case WM_WINDOWPOSCHANGING:
        break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_SHOWWINDOW:
        __ezgdi_instance.repaint();
        break;
    case WM_SIZE:
        break;
    case WM_PAINT:
        OnWindowPaint(hWnd);
        break;

    case WM_KEYDOWN:
        if(__ezgdi_instance.OnKeyDown)__ezgdi_instance.OnKeyDown(wParam);
        break;
    case WM_KEYUP:
        if(__ezgdi_instance.OnKeyUp)__ezgdi_instance.OnKeyUp(wParam);
        break;
    case WM_CHAR:
        if(__ezgdi_instance.OnKeyPress)__ezgdi_instance.OnKeyPress(wParam);
        break;

    case WM_COMMAND:
        switch(wParam){
        case IDOK:
            show_message("OK");
            break;
        }
        break;

    case WM_MOUSEMOVE:
        if(__ezgdi_instance.OnMouseMove)__ezgdi_instance.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
        break;
    case WM_LBUTTONDOWN:
        if(__ezgdi_instance.OnMouseDown)__ezgdi_instance.OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam & (MK_LBUTTON|MK_SHIFT|MK_CONTROL));
        break;
    case WM_LBUTTONUP:
        if(__ezgdi_instance.OnMouseUp)__ezgdi_instance.OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MK_LBUTTON);
        break;
    case WM_RBUTTONDOWN:
        if(__ezgdi_instance.OnMouseDown)__ezgdi_instance.OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam & (MK_RBUTTON|MK_SHIFT|MK_CONTROL));
        break;
    case WM_RBUTTONUP:
        if(__ezgdi_instance.OnMouseUp)__ezgdi_instance.OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MK_RBUTTON);
        break;
    case WM_MBUTTONDOWN:
        if(__ezgdi_instance.OnMouseDown)__ezgdi_instance.OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam & (MK_MBUTTON|MK_SHIFT|MK_CONTROL));
        break;
    case WM_MBUTTONUP:
        if(__ezgdi_instance.OnMouseUp)__ezgdi_instance.OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MK_MBUTTON);
        break;

    default:
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

namespace ezgdi{

template<typename T>
ezInstance<T> ezInstance<T>::instance = ezInstance<T>();

template<typename T>
ezInstance<T>::ezInstance() :
    window(), hdc(),
    OnKeyDown(), OnKeyUp(), OnKeyPress(), OnMouseDown(), OnMouseUp(), OnMouseMove(),
    graphics(), colorbuf(), pen(), brush(),
    font(),
    fontName(L"΢���ź�"),
    fontSize(12),
    fontStyle(ezfsNormal)
{

}

template<typename T>
ezInstance<T>::~ezInstance()
{
    closeGraphics();
}

template<typename T>
int ezInstance<T>::initGraphics(int width, int height)
{
    closeGraphics();

    colorbuf = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
    graphics = new Gdiplus::Graphics(colorbuf);
    pen = new Gdiplus::Pen(Gdiplus::Color::Black);
    brush = new Gdiplus::SolidBrush(Gdiplus::Color::White);
    font = new Gdiplus::Font(L"΢���ź�", 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint, NULL);
    font_color = new Gdiplus::SolidBrush(Gdiplus::Color::Black);

    return true;
}

template<typename T>
void ezInstance<T>::closeGraphics()
{
    safe_delete(graphics);
    safe_delete(colorbuf);
    safe_delete(pen);
    safe_delete(brush);
    safe_delete(font);
    safe_delete(font_color);
}

template<typename T>
void ezInstance<T>::repaint()
{
    RECT rc;
    GetClientRect(window, &rc);
    RedrawWindow(window, &rc, 0, RDW_UPDATENOW|RDW_INVALIDATE|RDW_NOERASE);
}

template<typename T>
void ezInstance<T>::gdiplusInit()
{
    Gdiplus::GdiplusStartup(&token, &input, NULL);
}

template<typename T>
void ezInstance<T>::gdiplusShutdown()
{
    Gdiplus::GdiplusShutdown(token);
}

}//end namespace ezgdi


int ezgdi_init(const ezstring& title, int width, int height, int style)
{
    //�����Ļ��С
    int cx = GetSystemMetrics( SM_CXFULLSCREEN );
    int cy = GetSystemMetrics( SM_CYFULLSCREEN );

    //����һ������
    HWND hwnd = InitWindow(
        title.c_str(),
        (cx - width) / 2,
        (cy - height) / 2,
        width,
        height,
        WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_MINIMIZEBOX|style,
        WS_EX_CLIENTEDGE
        //|WS_EX_TOPMOST    //�ö�
        );

    //��ʾ����
    ShowWindow(hwnd, SW_SHOW);

    __ezgdi_instance.window = hwnd;

    return 0;
}

void ezgdi_close()
{
    SendMessage(__ezgdi_instance.window, WM_CLOSE, 0, 0);
}

HWND ezgdi_window()
{
    return __ezgdi_instance.window;
}

void ezgdi_execute()
{
    MSG Msg;
    while(GetMessage(&Msg, NULL, 0, 0) > 0){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
        Sleep(1);
    }
}

//��Ϣѭ������
bool ezgdi_loop()
{
    MSG msg;
    bool run = true;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
        if(msg.message == WM_QUIT){
            run = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    __ezgdi_instance.repaint(); //ˢ�´���
    return run;
}

void show_message(const ezstring& msg)
{
    MessageBoxW(ezgdi_window(), msg.c_str(), L"��Ϣ", IDOK);
}

//---------------------------------------------------------------------------
//
// ��ͼ����
//
//---------------------------------------------------------------------------

//���GDI+��ͼ�豸
Gdiplus::Graphics* ezgdi_canvas()
{
    return __ezgdi_instance.graphics;
}

//������ʾ����
int ezgdi_effect_level(EFFECT_LEVEL level)
{
    if(__ezgdi_instance.graphics){
        switch(level){
        case EZGDI_SPEED:
            __ezgdi_instance.graphics->SetSmoothingMode( Gdiplus::SmoothingModeHighSpeed );
            __ezgdi_instance.graphics->SetInterpolationMode( Gdiplus::InterpolationModeNearestNeighbor );
            __ezgdi_instance.graphics->SetPixelOffsetMode( Gdiplus::PixelOffsetModeHalf );
            break;
        case EZGDI_QUALITY:
            __ezgdi_instance.graphics->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
            __ezgdi_instance.graphics->SetInterpolationMode( Gdiplus::InterpolationModeBilinear );
            __ezgdi_instance.graphics->SetPixelOffsetMode( Gdiplus::PixelOffsetModeHighQuality );
            break;
        default:
            break;
        }
    }
    return 0;
}

//����
void clear(BYTE r, BYTE g, BYTE b, BYTE a)
{
    if(ezgdi_canvas()){
        ezgdi_canvas()->Clear(Gdiplus::Color(a, r, g, b));
    }
}

//���Ļ�����ɫ
void pen_color(BYTE r, BYTE g, BYTE b, BYTE a)
{
    if(__ezgdi_instance.pen){
        __ezgdi_instance.pen->SetColor(Gdiplus::Color(a, r, g, b));
    }
}

//���������ɫ
void fill_color(BYTE r, BYTE g, BYTE b, BYTE a)
{
    if(__ezgdi_instance.brush){
        __ezgdi_instance.brush->SetColor(Gdiplus::Color(a, r, g, b));
    }
}

//���ʿ��
void pen_width(float width)
{
    __ezgdi_instance.pen->SetWidth(width);
}

//�����߶�
void draw_line(int x1, int y1, int x2, int y2)
{
    if(__ezgdi_instance.graphics){
        __ezgdi_instance.graphics->DrawLine(__ezgdi_instance.pen, x1, y1, x2, y2);
    }
}

//����һ�����ľ���
void draw_rect(int x, int y, int width, int height)
{
    if(__ezgdi_instance.graphics){
        __ezgdi_instance.graphics->DrawRectangle(__ezgdi_instance.pen, x, y, width, height);
    }
}

//���һ������
void fill_rect(int x, int y, int width, int height)
{
    if(__ezgdi_instance.graphics){
        __ezgdi_instance.graphics->FillRectangle(__ezgdi_instance.brush, x, y, width, height);
    }
}

//������Բ��xyΪԲ��
void draw_ellipse(float x, float y, float cx, float cy)
{
    if(__ezgdi_instance.graphics){
        __ezgdi_instance.graphics->DrawEllipse(__ezgdi_instance.pen, x - cx * 0.5f, y - cy * 0.5f, cx, cy);
    }
}

//�����Բ
void fill_ellipse(float x, float y, float cx, float cy)
{
    if(__ezgdi_instance.graphics){
        __ezgdi_instance.graphics->FillEllipse(__ezgdi_instance.brush, x - cx * 0.5f, y - cy * 0.5f, cx, cy);
    }
}

//---------------------------------------------------------------------------
//
// ���庯��
//
//---------------------------------------------------------------------------

//�������塣�������֡���С�����
void ezgdi_font(const ezstring& name, int size, EZGDI_FONTSTYLE style)
{
    if(__ezgdi_instance.font){
        delete __ezgdi_instance.font;
        __ezgdi_instance.font = new Gdiplus::Font(name.c_str(), size, style, Gdiplus::UnitPoint, NULL);
    }
}

void font_name(const ezstring& name)
{
    __ezgdi_instance.fontName = name;
    __ezgdi_instance.fontIsChange = true;
}

void font_size(int size)
{
    __ezgdi_instance.fontSize = size;
    __ezgdi_instance.fontIsChange = true;
}

void font_style(int style)
{
    __ezgdi_instance.fontStyle = style;
    __ezgdi_instance.fontIsChange = true;
}

//������ɫ
void text_color(BYTE r, BYTE g, BYTE b, BYTE a)
{
    if(__ezgdi_instance.font_color){
        __ezgdi_instance.font_color->SetColor(Gdiplus::Color(a, r, g, b));
    }
}

//�������
void text_out(int x, int y, const ezstring& text)
{
    if(__ezgdi_instance.graphics){
        if(__ezgdi_instance.fontIsChange){
            ezgdi_font(__ezgdi_instance.fontName, __ezgdi_instance.fontSize, EZGDI_FONTSTYLE(__ezgdi_instance.fontStyle));
            __ezgdi_instance.fontIsChange = false;
        }

        Gdiplus::StringFormat fmt;
        __ezgdi_instance.graphics->DrawString(text.c_str(), text.length(), __ezgdi_instance.font,
            Gdiplus::PointF(x, y), &fmt, __ezgdi_instance.font_color);
    }
}

//---------------------------------------------------------------------------
//
// �������
//
//---------------------------------------------------------------------------

//�жϰ����Ƿ���
bool key_state(int key)
{
    return GetAsyncKeyState(key) & 0x8000;
}

//�����¼�ӳ��
void ezgdi_keyup(EZGDI_KEYEVENT function)
{
    __ezgdi_instance.OnKeyDown = function;
}

void ezgdi_keydown(EZGDI_KEYEVENT function)
{
    __ezgdi_instance.OnKeyDown= function;
}

void ezgdi_keypress(EZGDI_KEYEVENT function)
{
    __ezgdi_instance.OnKeyPress = function;
}

//����¼�ӳ��
void ezgdi_mousedown(EZGDI_MOUSEEVENT function)
{
    __ezgdi_instance.OnMouseDown = function;
}

void ezgdi_mouseup(EZGDI_MOUSEEVENT function)
{
    __ezgdi_instance.OnMouseUp = function;
}

void ezgdi_mousemove(EZGDI_MOUSEEVENT function)
{
    __ezgdi_instance.OnMouseMove = function;
}

//---------------------------------------------------------------------------
//
// ��ý��
//
//---------------------------------------------------------------------------

//��������
int playmusic(PCTSTR filename)
{
    if(!filename){
        mciSendString(TEXT("stop background"), NULL, 0, NULL);
    }
    else{
        TCHAR command[256] = {0};
        _tcscpy(command, TEXT("open "));
        _tcscat(command, filename);
        _tcscat(command, TEXT(" alias background"));

        mciSendString(command, NULL, 0, NULL);
        mciSendString(TEXT("play background repeat"), NULL, 0, NULL);
    }
    return 0;
}

//����wav�ļ�
int playsound(PCTSTR filename)
{
    return PlaySound(filename, 0, SND_FILENAME|SND_ASYNC);
}
