#ifndef DEBUG_LINE_CPP
#define DEBUG_LINE_CPP

#include <string>

class DebugLine {
public:
    std::wstring Get() {return _msg + L' ' + _value; }
    DebugLine(std::wstring msg, std::wstring val) : _msg(msg), _value(val) {} 
private:
    std::wstring _msg;
    std::wstring _value;
};

#endif