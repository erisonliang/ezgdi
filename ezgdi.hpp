/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ezgdi.hpp

 2020-01-01 16:37:22

 EZGDI��һ���Ƚϼ򵥡���ѧ�����õ�C++�⣬���Ŀ��ּ�ڰ�����ѧ��ѧϰʹ��C++��
 ��ӭ����EZGDI��Ҳ��ӭ�����ҵ�GITHUB������������
 https://github.com/sdragonx/ezgdi

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

#include <map>
#include <sstream>
#include <string>
#include <vector>

//---------------------------------------------------------------------------
//
// ͨ��
//
//---------------------------------------------------------------------------

enum{
    ezNone,
    ezFixed,
    ezSizeable,
    ezFullScreen,

    ezLeft   = 1,
    ezRight  = 2,
    ezUp     = 4,
    ezDown   = 8,
    ezMiddle = 16,
};


typedef void (*EZGDI_KEYEVENT)(int key);                    //�����¼�
typedef void (*EZGDI_MOUSEEVENT)(int x, int y, int button); //����¼�


template<typename T, typename _char_t>
inline T string_cast(const std::basic_string<_char_t>& str)
{
    std::basic_stringstream<_char_t> stm(str);
    T n;
    stm>>n;
    return n;
}

template<typename _char_t, typename T>
std::basic_string<_char_t> to_string(const T& value)
{
    std::basic_stringstream<_char_t> stm;
    stm<<value;
    return stm.str();
}

//Unicode�ַ�����
class ezstring : public std::wstring
{
public:
    ezstring() : std::wstring() { }
    ezstring(const char* str) : std::wstring() { assign(str, strlen(str)); }
    ezstring(const char* str, size_t size) : std::wstring() { assign(str, size); }
    ezstring(const wchar_t* str) : std::wstring(str) { }
    ezstring(const wchar_t* str, size_t size) : std::wstring(str, size) { }

    //����ת�ַ���
    ezstring(int n) : std::wstring(to_string<wchar_t>(n)) { }

    //��ʵ��ת�ַ���
    ezstring(float n) : std::wstring(to_string<wchar_t>(n)) { }

    //˫ʵ��ת�ַ���
    ezstring(double n) : std::wstring(to_string<wchar_t>(n)) { }

    ezstring(const ezstring& str) : std::wstring(str.c_str(), str.length()) { }

    using std::wstring::assign;
    ezstring& assign(const char* str, size_t size = -1)
    {
        std::vector<wchar_t> buf;
        int n = MultiByteToWideChar(CP_ACP, 0, str, size, 0, 0);
        buf.resize(n);
        MultiByteToWideChar(CP_ACP, 0, str, size, &buf[0], n);
        std::wstring::assign(&buf[0], n);
        return *this;
    }

    int    to_int()    { return string_cast<int>(*this);    }//�ַ���ת����
    float  to_float()  { return string_cast<float>(*this);  }//�ַ���ת��ʵ��
    double to_double() { return string_cast<double>(*this); }//�ַ���ת˫ʵ��
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

#ifdef _MSC_VER
int random(int n)
{
    return rand() % n;
}
#endif

//---------------------------------------------------------------------------
//
// ͼƬ��
//
//---------------------------------------------------------------------------

class ezImage
{
private:
    Gdiplus::Bitmap* m_handle;

public:
    ezImage() : m_handle() { }
    ~ezImage() { this->dispose(); }

    Gdiplus::Bitmap* handle()const { return m_handle; }

    //����һ��ͼƬ��Ĭ��Ϊ32λɫ
    int create(int width, int height, int format = PixelFormat32bppARGB)
    {
        this->dispose();
        m_handle = new Gdiplus::Bitmap(width, height, format);
        return 0;
    }

    //��һ��ͼƬ��֧��bmp��jpg��png����̬gif�ȳ�����ʽ
    int open(const ezstring& filename)
    {
        this->dispose();
        m_handle = Gdiplus::Bitmap::FromFile(filename.c_str());
        return 0;
    }

    //�Զ��ͷ�ͼƬ
    void dispose()
    {
        if(m_handle){
            delete m_handle;
            m_handle = NULL;
        }
    }

    //����ͼƬ�Ŀ��
    int width()const
    {
        return m_handle ? m_handle->GetWidth() : 0;
    }

    //����ͼƬ�ĸ߶�
    int height()const
    {
        return m_handle ? m_handle->GetHeight() : 0;
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
    EZGDI_QUALITY,      //��������
    ezSpeed,
    ezQuality,
};

int effect_level(EFFECT_LEVEL level);

//����
void clear(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���Ļ�����ɫ
void pen_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���������ɫ
void fill_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//���ʿ��
void pen_width(float width);

//�����߶�
void draw_line(float x1, float y1, float x2, float y2);

//����һ�����ľ���
void draw_rect(float x, float y, float width, float height);

//���һ������
void fill_rect(float x, float y, float width, float height);

//���ƿ�����Բ��xyΪԲ��
void draw_ellipse(float x, float y, float cx, float cy);

//�����Բ
void fill_ellipse(float x, float y, float cx, float cy);

//���ƿ���Բ��xyΪԲ��
void draw_circle(float x, float y, float r);

//���Բ
void fill_circle(float x, float y, float r);

//---------------------------------------------------------------------------
//
// ���庯��
//
//---------------------------------------------------------------------------

enum EZGDI_FONTSTYLE{
    ezNormal     = 0,
    ezBold       = 1,
    ezItalic     = 2,
    ezBoldItalic = 3,
    ezUnderline  = 4,
    ezStrikeout  = 8
};

//�������塣�������֡���С�����
void ezgdi_font(const ezstring& name, float size, EZGDI_FONTSTYLE style = ezNormal);
void font_name(const ezstring& name);
void font_size(float size);
void font_style(int style);


//������ɫ
void text_color(BYTE r, BYTE g, BYTE b, BYTE a = 255);

//�������
void text_out(float x, float y, const ezstring& text);

//---------------------------------------------------------------------------
//
// ͼƬ����
//
//---------------------------------------------------------------------------

//����ͼƬ�����ù����ͷ�
ezImage* loadimage(const ezstring& filename);

//��xyλ�û���ͼƬ��ԭʼ��С
void draw_image(ezImage* image, float x, float y);

//��xyλ�û���ͼƬ������
void draw_image(ezImage* image, float x, float y, float width, float height);

//��xyλ�û���ͼƬ�����ţ�����תһ���Ƕ�
void point_image(ezImage* image, float x, float y, float width, float height, float rotate);

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

//---------------------------------------------------------------------------
//
// ����
//
//---------------------------------------------------------------------------

//��ʾ��Ϣ
void showmessage(const ezstring& msg);

//��ʾ�����
ezstring inputbox(const ezstring& title, const ezstring& message, const ezstring value = ezstring());

//
// ������
//

class ezWindow
{
public:
    HWND m_handle;

public:
    ezWindow() : m_handle() { }

    int create(PCWSTR className, PCWSTR title, int x, int y, int width, int height, int style = WS_OVERLAPPEDWINDOW);

    HWND create_button(PCWSTR title, int x, int y, int width, int height, int id)
    {
        DWORD style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;//BS_DEFPUSHBUTTON;
        return CreateWindowExW(0, L"button", title, style, x, y, width, height, m_handle, (HMENU)id, GetModuleHandle(NULL), 0);
    }

    HWND create_label(PCWSTR text, int x, int y, int width, int height, int id)
    {
        DWORD style = WS_CHILD | WS_VISIBLE | SS_EDITCONTROL;
        return CreateWindowExW(0, L"static", text, style, x, y, width, height, m_handle, (HMENU)id, GetModuleHandle(NULL), 0);
    }

    HWND create_edit(PCWSTR text, int x, int y, int width, int height, int id)
    {
        DWORD style = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
        return CreateWindowExW(WS_EX_CLIENTEDGE, L"edit", text, style, x, y, width, height, m_handle, (HMENU)id, GetModuleHandle(NULL), 0);
    }

    void set_bounds(int x, int y, int width, int height) { MoveWindow(m_handle, x, y, width, height, TRUE); }
    void show() { ShowWindow(m_handle, SW_SHOW); }
    int show_model(HWND parent);

protected:
    virtual int wndproc(UINT msg, WPARAM wparam, LPARAM lparam) { return DefWindowProc(m_handle, msg, wparam, lparam); }
    friend LRESULT CALLBACK basic_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

namespace ezgdi{

//---------------------------------------------------------------------------
//
// ��Դ������
//
//---------------------------------------------------------------------------

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

class ezResource
{
private:
    std::map<ezstring, ezImage*> images;

public:
    //����һ��ͼƬ
    ezImage* loadimage(const ezstring& name)
    {
        ezImage* bmp = NULL;
        std::map<ezstring, ezImage*>::iterator itr;
        itr = images.find(name);
        if(itr == images.end()){
            bmp = new ezImage;
            bmp->open(name);
            images[name] = bmp;
        }
        else{
            bmp = itr->second;
        }
        return bmp;
    }

    //�ͷ�������Դ
    void dispose()
    {
        delete_all(images);
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

    Gdiplus::Graphics* graphics;    //GDIPlus�豸
    Gdiplus::Bitmap* colorbuf;      //������������ʹ��˫�����ֹ��˸
    HBITMAP pixelbuf;               //���ػ�����

    Gdiplus::Pen* pen;
    Gdiplus::SolidBrush* brush;

    Gdiplus::Font* font;
    Gdiplus::SolidBrush* font_color;
    ezstring fontName;
    float    fontSize;
    int      fontStyle;
    bool     fontIsChange;

    ezResource resource;

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

    static ezInstance instance;
};

};//end namespace ezgdi

#include "ezgdi.inl"

#endif //EZGDI_HPP
