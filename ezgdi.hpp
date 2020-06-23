/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ezgdi.hpp

 2020-01-01 16:37:22

 �򵥵�win32���ڡ���ͼ������װ

*/
#ifndef EZGDI_HPP
#define EZGDI_HPP

#define NO_WIN32_LEAN_AND_MEAN
#ifndef STRICT
    #define STRICT
#endif

#if defined(__GNUC__)
	#define EZGDI_PUBLIC __attribute__((weak))
#else
	#define EZGDI_PUBLIC __declspec(selectany)
#endif

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <map>
#include <sstream>
#include <string>

typedef void (*EZGDI_KEYEVENT)(int key);
typedef void (*EZGDI_MOUSEEVENT)(int x, int y, int button);

//---------------------------------------------------------------------------
//
// ͨ��
//
//---------------------------------------------------------------------------

//Unicode�ַ�����
class ezstring : public std::wstring
{
public:
    ezstring() : std::wstring() { }
    ezstring(const char* str, size_t size = -1) : std::wstring()
    {
        assign(str, size);
    }

    ezstring(int n) : std::wstring()
    {
        std::wstringstream stm;
        stm<<n;
        this->assign(stm.str());
    }

    ezstring(const wchar_t* str) : std::wstring(str) { }
    ezstring(const wchar_t* str, size_t size) : std::wstring(str, size) { }

    using std::wstring::assign;
    void assign(const char* str, size_t size = -1)
    {
        wchar_t *buf;
        int n = MultiByteToWideChar(CP_ACP, 0, str, size, 0, 0);
        buf = new wchar_t[n];
        MultiByteToWideChar(CP_ACP, 0, str, size, buf, n);
        delete buf;
        this->assign(buf, n - 1);
    }
};

//��
struct point_t
{
    float x, y;
};

//����
struct rect_t
{
    float x, y, width, height;
};

namespace ezgdi{

//��ȫɾ��ָ��
template<typename T>
void safe_delete(T* &p)
{
    if(p){
        delete p;
        p = NULL;
    }
}

//ɾ��stl�������������
template<typename T>
void delete_all(T& obj)
{
    typename T::iterator itr = obj.begin();
    for( ; itr != obj.end(); ++itr){
        delete itr->second;
    }
    obj.clear();
}

//---------------------------------------------------------------------------
//
// ͼƬ��Դ������
//
//---------------------------------------------------------------------------

class ImageResource
{
public:
    typedef std::wstring string_t;

public:
    std::map<string_t, Gdiplus::Image*> resource;

    //����һ��ͼƬ
    Gdiplus::Image* load_image(const string_t& name)
    {
        Gdiplus::Image* bmp = NULL;
        std::map<string_t, Gdiplus::Image*>::iterator itr;
        itr = resource.find(name);
        if(itr == resource.end()){
            bmp = new Gdiplus::Image(name.c_str());
            resource[name] = bmp;
        }
        else{
            bmp = itr->second;
        }

        return bmp;
    }

    //�ͷ�����ͼƬ��Դ
    void dispose()
    {
        delete_all(resource);
    }
};

//---------------------------------------------------------------------------
//
// EZGDI ʵ����
//
//---------------------------------------------------------------------------

template<typename T = int>
class ezInstance
{
public:
    HWND window;                    //������
    HDC  hdc;                       //GDI�豸

    EZGDI_KEYEVENT OnKeyDown;
    EZGDI_KEYEVENT OnKeyUp;
    EZGDI_KEYEVENT OnKeyPress;

    EZGDI_MOUSEEVENT OnMouseDown;
    EZGDI_MOUSEEVENT OnMouseUp;
    EZGDI_MOUSEEVENT OnMouseMove;

    Gdiplus::Graphics* graphics;    //
    Gdiplus::Bitmap* colorbuf;      //������������ʹ��˫�����ֹ��˸

    Gdiplus::Pen* pen;
    Gdiplus::SolidBrush* brush;

    Gdiplus::Font* font;
    Gdiplus::SolidBrush* font_color;
    ezstring fontName;
    int      fontSize;
    int      fontStyle;
    bool     fontIsChange;

    ImageResource images;

public:
    static ezInstance instance;

private:
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput input;

public:
    ezInstance();
    ~ezInstance();

    void gdiplusInit();
    void gdiplusShutdown();

    int initGraphics(int width, int height);
    void closeGraphics();

    void repaint();
};

};

//---------------------------------------------------------------------------
//
// ͼƬ��
//
//---------------------------------------------------------------------------

class ezBitmap
{
public:
    Gdiplus::Bitmap* handle;

public:
    ezBitmap() : handle() { }

    ~ezBitmap()
    {
        this->dispose();
    }

    //����һ��ͼƬ��Ĭ��Ϊ32λɫ
    int create(int width, int height, int format = PixelFormat32bppARGB)
    {
        this->dispose();
        handle = new Gdiplus::Bitmap(width, height, format);
        return 0;
    }

    //��һ��ͼƬ��֧��bmp��jpg��png����̬gif�ȳ�����ʽ
    int open(const ezstring& filename)
    {
        this->dispose();
        handle = Gdiplus::Bitmap::FromFile(filename.c_str());
        return 0;
    }

    //�Զ��ͷ�ͼƬ
    void dispose()
    {
        if(handle){
            delete handle;
            handle = NULL;
        }
    }

    //����ͼƬ�Ŀ��
    int width()const
    {
        return handle->GetWidth();
    }

    //����ͼƬ�ĸ߶�
    int height()const
    {
        return handle->GetHeight();
    }
};

//---------------------------------------------------------------------------
//
// ������
//
//---------------------------------------------------------------------------

//ͼ�ο��ʼ��
int ezgdi_init(const ezstring& name, int width, int height, int style = 0);

//ͼ�ο�ر�
void ezgdi_close();

//������ִ�к���
void ezgdi_execute();

//��������ھ��
HWND ezgdi_window();

//��Ϣѭ������
bool ezgdi_loop();

//��ʾ��Ϣ
void show_message(const ezstring& msg);

//---------------------------------------------------------------------------
//
// ��ͼ����
//
//---------------------------------------------------------------------------

//���GDI+��ͼ�豸
Gdiplus::Graphics* ezgdi_canvas();

//������ʾ����
enum EFFECT_LEVEL{
    EZGDI_SPEED,        //�ٶ�����
    EZGDI_QUALITY       //��������
};

int ezgdi_effect_level(EFFECT_LEVEL level);

//����
void clear(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���Ļ�����ɫ
void pen_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���������ɫ
void fill_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���ʿ��
void pen_width(float width);

//�����߶�
void draw_line(int x1, int y1, int x2, int y2);

//����һ�����ľ���
void draw_rect(int x, int y, int width, int height);

//���һ������
void fill_rect(int x, int y, int width, int height);

//���ƿ�����Բ��xyΪԲ��
void draw_ellipse(float x, float y, float cx, float cy);

//�����Բ
void fill_ellipse(float x, float y, float cx, float cy);

//---------------------------------------------------------------------------
//
// ���庯��
//
//---------------------------------------------------------------------------

enum EZGDI_FONTSTYLE{
    ezfsNormal     = 0,
    ezfsBold       = 1,
    ezfsItalic     = 2,
    ezfsBoldItalic = 3,
    ezfsUnderline  = 4,
    ezfsStrikeout  = 8
};

//�������塣�������֡���С�����
void ezgdi_font(const ezstring& name, int size, EZGDI_FONTSTYLE style = ezfsNormal);
void font_name(const ezstring& name);
void font_size(int size);
void font_style(int style);


//������ɫ
void text_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//�������
void text_out(int x, int y, const ezstring& text);

//---------------------------------------------------------------------------
//
// �������
//
//---------------------------------------------------------------------------

//�����¼�ӳ��
void ezgdi_keyup(EZGDI_KEYEVENT function);
void ezgdi_keydown(EZGDI_KEYEVENT function);
void ezgdi_keypress(EZGDI_KEYEVENT function);

//����¼�ӳ��
void ezgdi_mousedown(EZGDI_MOUSEEVENT function);
void ezgdi_mouseup(EZGDI_MOUSEEVENT function);
void ezgdi_mousemove(EZGDI_MOUSEEVENT function);

//�жϰ����Ƿ���
bool key_state(int key);

//---------------------------------------------------------------------------
//
// ��ý��
//
//---------------------------------------------------------------------------

//���ű�������
int playmusic(PCTSTR filename);

//����wav�ļ�
int playsound(PCTSTR filename);

#include "ezgdi.inl"

#endif //EZGDI_HPP
