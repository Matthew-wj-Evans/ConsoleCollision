#ifndef SCREEN_CPP
#define SCREEN_CPP

#include <memory>
#include <vector>
#include <stdexcept>

class Screen
{
public:
    typedef std::vector<wchar_t>::size_type size_type;
    Screen(int, int);
    size_type size() const {return data->size();}
    bool empty () const {return data->empty();}
    wchar_t& front();
    wchar_t& back();
    wchar_t& operator[](size_type i);
    void Add(size_type, wchar_t);
    std::vector<wchar_t> GetScreenContent();

private:
    std::shared_ptr<std::vector<wchar_t>> data;
    void check(size_type i, const std::string &msg) const;
    void check(wchar_t wc, const std::string &msg) const;
    int _width;
    int _height;
};

// C++ Primer 263, a default constructor is only generated if no constructer is defined. Screen will always have a height and width. (Will cause this being a class' member to require extra steps)
Screen::Screen(int width, int height) : _width(width), _height(height), data(std::make_shared<std::vector<wchar_t>> (height*width, L' ')) {}

void Screen::check(size_type i, const std::string &msg) const
{
    if (i >= data->size())
        throw std::out_of_range(msg);
}

void Screen::check(wchar_t wc, const std::string &msg) const
{
    if (wc > 255 || wc == 35) 
        throw std::out_of_range(msg);
}

wchar_t &Screen::front()
{
    check((size_type)0, "front on empty Screen");
    return data->front();
}

wchar_t &Screen::back()
{
    check((size_type)0, "back on empty Screen");
    return data->back();
}

wchar_t& Screen::operator[](size_type i)
{
    check(i, "subscript out of range on screen");
    return (*data)[i];
}

void Screen::Add(size_type i, wchar_t charToAdd) {
    check((size_type)i, "subscript out of range on screen");
    check(charToAdd, "Invalid char written to Screen");
    (*data)[i] = charToAdd;
}

std::vector<wchar_t> Screen::GetScreenContent()
{
    return (*data);
}
#endif