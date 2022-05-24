#ifndef BINARY_DECIMAL_CONVERTER
#define BINARY_DECIMAL_CONVERTER

#define BASE_TWO 2
#define BASE_TEN 10

#include <vector>
using namespace std;

class BinaryDecimalConverter
{
private:
    int GetPower(int, int);
public:
    int BinaryToDecimal(vector<bool>);
    vector<bool> DecimalToBinary(int, int);
};

/* 
The chunckCount dictates the size of the binary chunck.
With an assumed base of four. 
*/
vector<bool> BinaryDecimalConverter::DecimalToBinary(int decimal, int chunkSize)
{
    vector<bool> binary;
    int displaySize;
    while (decimal > 0)
    {
        binary.push_back(decimal % 2);
        decimal /= 2;
    };

    // 
    while (binary.size() % chunkSize != 0) {
        binary.push_back(0);
    }

    return binary;
}

int BinaryDecimalConverter::BinaryToDecimal(vector<bool> binary) {
    int decimal = 0;
    for (int index = 0; index < binary.size(); index++) {
        if (binary.at(index)) {
            decimal += GetPower(index, BASE_TWO);
        }
    }
    return decimal;
}

int BinaryDecimalConverter::GetPower(int power, int base) {
    int result = 1;
    for (int index = 0; index < power; index++) {
        result *= base;
    }
    return result;
}

#endif