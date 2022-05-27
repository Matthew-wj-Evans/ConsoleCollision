#ifndef SCREEN_CPP
#define SCREEN_CPP

class Screen
{
private:
    wchar_t *_screen;
    int _width;
    int _height;

public:
    Screen();
    Screen(int, int);
    bool Add(int, wchar_t);
    wchar_t* GetScreenContent();
};

Screen::Screen() {}
Screen::Screen(int width, int height) : _width(width), _height(height), _screen(new wchar_t[height * width])
{
    for (int index = 0; index < _width * _height; index++)
        _screen[index] = L' ';
}

bool Screen::Add(int index, wchar_t charToAdd) {
    _screen[index] = charToAdd;
    return charToAdd <= 255;
}

wchar_t* Screen::GetScreenContent()
{
    return _screen;
}
#endif