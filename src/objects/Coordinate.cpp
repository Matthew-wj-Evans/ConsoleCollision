#ifndef COORDINATE_CPP
#define COORDINATE_CPP

class Coordinate
{
private:
    int _x;
    int _y;
public:
// Constructor
    Coordinate(int, int);
    Coordinate();
// Operator Overide
    bool operator==(const Coordinate &) const;
// Accessors
    int GetY();
    int GetX();
// Mutators
    void Reposition(int, int);
    void Reposition(Coordinate);
    void SetX(int);
    void SetY(int);
};

Coordinate::Coordinate(int x, int y)
{
    _x = x;
    _y = y;
}

bool Coordinate::operator==(const Coordinate &coord2) const
{
    return ((_y == coord2._y) && (_y == coord2._y));
}

int Coordinate::GetX()
{
    return _x;
}

int Coordinate::GetY()
{
    return _y;
}

void Coordinate::Reposition(int x, int y)
{
    _x = x;
    _y = y;
}

void Coordinate::Reposition(Coordinate coords)
{
    _x = coords.GetX();
    _y = coords.GetY();
}

void Coordinate::SetX(int x)
{
    _x = x;
}

void Coordinate::SetY(int y)
{
    _y = y;   
}


#endif