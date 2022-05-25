#ifndef MAIN
#define MAIN

#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "./objects/Asteroid.cpp"
#include "./objects/Coordinate.cpp"
#include "./objects/DebugContainer.cpp"
#include "./templates/VecWrapper.cpp"

#define ASCII_TILDE L'~'
#define ASCII_SPACE L' '
#define SPAWN_INTERVAL 1

/* Buffer widths and heights */
int consoleHeight = 50,
    consoleWidth = 175;

int fieldHeight = 28,
    fieldWidth = 75;

int debugHeight = 47,
    debugWidth = 75;

/* Buffer positional values */
int iFieldX = 3,
    iFieldY = 3; // Top left corner

int iFieldBorderX = iFieldX - 1,
    iFieldBorderY = iFieldY - 1;

int debugX = (consoleWidth - debugWidth) - 2,
    debugY = 2; // Top Right corner

int iMessagesX = fieldWidth + 6,
    iMessagesY = 3; // Left side, under field

int iTimerX = (fieldWidth / 2) - 2,
    iTimerY = 1; // above field, centered on field X

/* Game Logic/Timer variables */
int ticks = 1,
    tickCount = 0;
int timer = 0;
int secondTicks = 20,
    secondTicksCount = 0;
int asteroidSpawnRate = 20;

/* Containers */
unsigned char *fieldPointer = nullptr;
unsigned char *messagePointer = nullptr;
unsigned char *debugPointer = nullptr;
VecWrapper<Asteroid> vAsteroid;
std::vector<int> indexMoveQueue;

DebugContainer metrics;

/* function prototypes */
std::wstring GetBinaryString(vector<bool> binary);
void WipeAsteroid(Coordinate coords, int height, int width);
bool Compare(Asteroid obj1, Asteroid obj2);
void AddAsteroid(Asteroid asteroid);
void SpawnAsteroid();
void PopulateDebug();
void SanitizeDebug();
template <typename T>
bool AtBoundry(T obj);
template <typename T>
void Move(T &obj);

/* functions */
int main()
{
    srand(std::time(0)); // Setting here will set the rand() seed globally.
    // (If set in the asteroid class EVERY created asteroid will overwrite the seed, thus requiring a 1 second pause between rand() uses, otherwise the seed will not change.)

    fieldPointer = new unsigned char[fieldWidth * fieldHeight];
    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            fieldPointer[y * fieldWidth + x] = 0;

    debugPointer = new unsigned char[debugWidth * debugHeight];
    for (int x = 0; x < debugWidth; x++)
        for (int y = 0; y < debugHeight; y++)
            debugPointer[y * debugWidth + x] =
                ((x == 0) || (y == 0) || (x == (debugWidth - 1)) || (y == (debugHeight - 1))) ? ASCII_TILDE : ASCII_SPACE; // 20 = space, 126 = squiggle

    // Create the screen buffer
    wchar_t *screen = new wchar_t[consoleHeight * consoleWidth];
    for (int index = 0; index < consoleHeight * consoleWidth; index++)
        screen[index] = L' ';
    HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(consoleHandle);
    DWORD bytesWritten = 0;

    bool bGamerOver = false;

    // Game Loop
    while (!bGamerOver)
    {
        // Timing
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tickCount++;
        secondTicksCount++;
        bool update = (tickCount == ticks);
        bool timerUpdate = (secondTicks == secondTicksCount);

        if (timerUpdate)
        {
            secondTicksCount = 0;
            timer++;
            /*
                Need to adjust my approach so timer is independent of the adjustable tick rate
            */
            if (timer % 5 == 0 && ticks > 1)
            {
                ticks -= 1;
            }
        }
        if (update)
        {
            tickCount = 0;
            for (int i = vAsteroid.size() - 1; i >= 0; i--)
            {
                if (!AtBoundry(vAsteroid[i]))
                {
                    Move(vAsteroid[i]);
                }
                else
                {
                    WipeAsteroid(vAsteroid[i].coordinate, vAsteroid[i].GetHeight(), vAsteroid[i].GetWidth());
                    vAsteroid.pop_back();
                    SanitizeDebug();
                    metrics.DeleteInc();
                }
            };

            while (indexMoveQueue.size())
            {
                Move(vAsteroid[indexMoveQueue.at(indexMoveQueue.size() - 1)]);
                indexMoveQueue.pop_back();
            }

            if (!(timer % SPAWN_INTERVAL)) // if time is evenly divisible by 5, spawn a rock
            {
                for (int n = 0; n < asteroidSpawnRate; n++)
                {
                    SpawnAsteroid();
                }
                std::sort(vAsteroid.begin(), vAsteroid.end(), Compare);
            }
        }

        // Draw the debug window
        PopulateDebug();

        // Draw Border
        // Left & Right Borders
        for (int y = 0; y < fieldHeight + 2; y++)
        {
            screen[(y + iFieldBorderY) * (consoleWidth) + iFieldBorderX] = L'|';
            screen[(y + iFieldBorderY) * (consoleWidth) + (iFieldBorderX + (fieldWidth + 1))] = L'|';
        }
        // Top & Bottom Borders
        for (int x = 1; x < fieldWidth + 1; x++)
        {
            screen[iFieldBorderY * (consoleWidth) + (iFieldBorderX + x)] = L'=';
            screen[(iFieldBorderY + (fieldHeight + 1)) * (consoleWidth) + (iFieldBorderX + x)] = L'=';
        }

        // Draw field
        for (int x = 0; x < fieldWidth; x++)
        {
            for (int y = 0; y < fieldHeight; y++)
            {
                screen[(y + iFieldX) * consoleWidth + (x + iFieldY)] = L" |=0*"[fieldPointer[y * fieldWidth + x]];
            }
        }

        // Draw Debug window
        for (int x = 0; x < debugWidth; x++)
        {
            for (int y = 0; y < debugHeight; y++)
            {
                screen[(debugY + y) * consoleWidth + (debugX + x)] = debugPointer[y * debugWidth + x];
            }
        }

        // Draw Timer
        swprintf_s(&screen[iTimerY * consoleWidth + iTimerX], 16, L"Time: %d", timer);

        // CMD Buffer variables
        WriteConsoleOutputCharacterW(consoleHandle, screen, consoleWidth * consoleHeight, {
                                                                                              0,
                                                                                              0,
                                                                                          },
                                     &bytesWritten);
    }
    return 0;
}

void PopulateDebug()
{
    /*
        debugHeight = 28,
        debugWidth = 75;

    */

    // Header (w34) = Asteroids Created/Destroyed: NN/NN
    int lineNumber = 1;
    int rowStart = 2;
    std::wstring header = L"Asteroids Created/Destroyed: " + std::to_wstring(metrics.GetCreateCount()) + L"\\" + std::to_wstring(metrics.GetDeleteCount());
    // Write header to line 1
    for (int index = 0; index < header.size(); index++)
    {
        debugPointer[(lineNumber * debugWidth + rowStart + index)] = header[index];
    }
    lineNumber++;

    // Write tick speed
    std::wstring speed = L"Tick Speed: " + std::to_wstring(ticks * 50) + L"ms";
    for (int index = 0; index < speed.size(); index++)
    {
        debugPointer[(lineNumber * debugWidth + rowStart + index)] = speed[index];
    }
    lineNumber += 2;

    // Write out data for each asteroid
    for (int y = 0; y < vAsteroid.size() && lineNumber + y < debugHeight - 1; y++)
    {
        int xOffset = 0;
        // Draw out the shapemap
        for (int x = 0; x < vAsteroid[y].GetShapeMap().size(); x++)
        {
            debugPointer[((/* Y */ lineNumber + y) * debugWidth + rowStart + x)] = (vAsteroid[y].GetShapeMap()[x] ? L'1' : L'0');
        }
        xOffset += vAsteroid[y].GetShapeMap().size();
        // Add the address, padding to overrite any leftover characters
        std::wostringstream woStringStream;
        woStringStream << L" 0x" << &vAsteroid[y];
        std::wstring stringAddress = woStringStream.str();
        for (int x = 0; x < stringAddress.size(); x++)
        {
            debugPointer[(lineNumber + y) * debugWidth + (rowStart + x + xOffset)] = stringAddress[x];
        }
        xOffset += stringAddress.size();
        // Add the Y coordinate
        int value = vAsteroid[y].coordinate.GetY();
        std::wstring coordValue = value < 10 ? L"0" + to_wstring(value) : to_wstring(value);
        std::wstring coord = L" Y:" + coordValue;
        for (int x = 0; x < coord.size(); x++)
        {
            debugPointer[(lineNumber + y) * debugWidth + (rowStart + x + xOffset)] = coord[x];
        }
    }

} // End PopulateDebug()

void SanitizeDebug()
{
    int lineNumber = 2;
    int rowStart = 1;

    for (int x = rowStart; x < debugWidth - 1; x++)
        for (int y = lineNumber; y < debugHeight - 1; y++)
        {
            debugPointer[y * debugWidth + x] = ' ';
        }
}

void SpawnAsteroid()
{
    int xMax = fieldWidth - 3,
        xMin = 1,
        yMax = fieldHeight / 2 - 1,
        yMin = 1;
    int xCoord = rand() % (xMax - xMin) + xMin,
        yCoord = rand() % (yMax - yMin) + yMin;
    Asteroid toAdd(xCoord, yCoord);
    AddAsteroid(toAdd);
    vector<Asteroid> stroids;
}

void AddAsteroid(Asteroid asteroid)
{

    vAsteroid.push_back(asteroid);
    metrics.CreateInc();
    // Add the Asteroid to the field
    for (int xA = 0; xA < asteroid.GetWidth(); xA++)
        for (int yA = 0; yA < asteroid.GetHeight(); yA++)
            if (asteroid.GetShapeMap()[(yA * asteroid.GetWidth() + xA)])
            {
                // Add the current
                fieldPointer[(asteroid.coordinate.GetY() + yA) * fieldWidth + (asteroid.coordinate.GetX() + xA)] = 3;
            }
}

bool Compare(Asteroid obj1, Asteroid obj2)
{
    return (obj1.coordinate.GetY() < obj2.coordinate.GetY());
}

void WipeAsteroid(Coordinate coords, int height, int width)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            fieldPointer[(y + coords.GetY()) * fieldWidth + (x + coords.GetX())] = 0;
        }
}

std::wstring GetBinaryString(vector<bool> binary)
{
    std::wstring binaryString = L"";

    for (int index = 0; index < binary.size(); index++)
    {
        binaryString += binary[index] ? L"1" : L"0";
    }
    return binaryString;
}

template <typename T>
bool AtBoundry(T obj)
{
    return !(obj.coordinate.GetY() + obj.GetHeight() < fieldHeight - 1);
}

template <typename T>
void Move(T &obj)
{
    // Starting at the bottom
    // X is decreased first to move from the bottom-right to the top-left
    for (int yA = obj.GetHeight() - 1; yA >= 0; yA--)
        for (int xA = obj.GetWidth() - 1; xA >= 0; xA--)
            if (obj.GetShapeMap()[(yA * obj.GetWidth() + xA)])
            {
                fieldPointer[(obj.coordinate.GetY() + yA) * fieldWidth + (obj.coordinate.GetX() + xA)] = 0;
                fieldPointer[(obj.coordinate.GetY() + yA) * fieldWidth + (obj.coordinate.GetX() + xA) + fieldWidth] = 3;
            }

    obj.MoveY(); // Change the Y value
}

// template <typename T>
// vector<Coordinate> GetCollisionSet(const T &v)
// {

// }
#endif