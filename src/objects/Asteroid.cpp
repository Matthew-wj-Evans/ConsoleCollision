#ifndef ASTEROID_CPP
#define ASTEROID_CPP

#include <iostream>
#include <string>
#include <vector>

#include "./../utility/BinaryDecimalConverter.cpp"
#include "./Coordinate.cpp"

// Class
class Asteroid
{
private:
    // Constants
    static const int DEFAULT_WIDTH = 3;
    static const int DEFAULT_HEIGHT = 3;
    static const int MIN_MASS = 3;
    static const int MAX_MASS = 9;
    static const int START_Y = -3;

    // private members
    short int _width;
    short int _height;
    short int _mass;
    // North = 1, East = 2, South = 3, West = 4;
    short int _angle = 3; // Default will be moving south
    vector<bool> _shapeMap;
    std::wstring _shapeAssets = L" 0";

private: // Methods
    bool SetRotation();
    bool SetSpeed();
    vector<bool> GeneratePattern();

public: // Members
    Coordinate coordinate;
    float _velocity = -1.00f;
    float _rotationalVelocity = 0.00f;

public: // Methods
    Asteroid(int x, int y, int width, int height);
    Asteroid(int x, int y);
    std::vector<bool> GetShapeMap();
    int GetChunck(int x, int y);
    bool Rotate(); // Returns true if rotation was possible
    bool CausesCollision();
    void MoveY();
    int GetHeight();
    int GetWidth();
    unsigned char GetAsset(int index);
};

// Constructors
Asteroid::Asteroid(int x, int y, int width, int height) : coordinate(x, START_Y), _width(width), _height(height), _shapeMap(GeneratePattern())
{
}

Asteroid::Asteroid(int x, int y) : coordinate(x, START_Y), _width(DEFAULT_WIDTH), _height(DEFAULT_HEIGHT), _shapeMap(GeneratePattern())
{
}

// Getters & Setters

int Asteroid::GetHeight()
{
    return _height;
}

int Asteroid::GetWidth()
{
    return _width;
}

unsigned char Asteroid::GetAsset(int index)
{
    return _shapeAssets[index];
}

// Methods

void Asteroid::MoveY()
{
    coordinate.SetY(coordinate.GetY() - -1);
}

bool Asteroid::Rotate()
{
    return false;
}

vector<bool> Asteroid::GeneratePattern()
{
    int maxValue = 8,
        minValue = 1;
    int stackCount = 3;
    int randValue;
    BinaryDecimalConverter converter;

    vector<bool> shape;
    vector<bool> toAdd;
    for (int stack = 0; stack < stackCount; stack++)
    {
        randValue = rand() % (maxValue - minValue) + minValue;
        toAdd = converter.DecimalToBinary(randValue, this->_width);
        for (int index = 0; index < toAdd.size(); index++)
            shape.push_back(index);
    }

    return shape;
}

// std::wstring Asteroid::ValidateShape(std::wstring pattern)
// {
//     // validate that the shape is congruous
//     for (int x = 0; x < _width; x++)
//     {
//         for (int y = 0; y < _height; y++)
//         {
//             int adjCount = 0;
//             if (pattern[(y * _width) + x] == 'x')
//             {
//                 // Check the adjacent faces
//
//                 // if (y-1) > 0, then if at (x, y-1) == 'x', add one, else do not add anything
//                 adjCount += (y - 1 >= 0) ? pattern[((y - 1) * _width) + x] == 'x' ? 1 : 0 : 0;
//                 adjCount += (y + 1 < _height) ? pattern[((y + 1) * _width) + x] == 'x' ? 1 : 0 : 0;
//                 adjCount += (x - 1 >= 0) ? pattern[((x - 1) * _width) + x] == 'x' ? 1 : 0 : 0;
//                 adjCount += (x + 1 > _width) ? pattern[((x + 1) * _width) + x] == 'x' ? 1 : 0 : 0;
//
//                 if (!adjCount)
//                 { // if adjCount has not been incremeneted, overwrite the lone piece.
//                     pattern[(y * _width) + x] = '.';
//                 }
//             }
//         }
//     }
//     return pattern;
// }

int Asteroid::GetChunck(int x, int y)
{
    int index = y * _width + x;
    return _shapeMap[index] ? 3 : 0;
}

std::vector<bool> Asteroid::GetShapeMap()
{
    return _shapeMap;
}

#endif