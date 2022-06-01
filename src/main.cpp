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

#include "./objects/Screen.cpp"
#include "./objects/Asteroid.cpp"
#include "./objects/Coordinate.cpp"
#include "./objects/DebugContainer.cpp"
#include "./objects/DebugLine.cpp"
#include "./templates/VecWrapper.cpp"

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
int ticks = 20,
    tickCount = 0;
int timer = 0;
int secondTicks = 20,
    secondTicksCount = 0;
int asteroidSpawnRate = 2;

/* Containers */
Screen fieldScreen = Screen(fieldWidth, fieldHeight);
Screen debugScreen = Screen(debugWidth, debugHeight);
Screen consoleScreen = Screen(consoleWidth, consoleHeight);
VecWrapper<Asteroid> vAsteroid;

std::vector<int> indexMoveQueue;

DebugContainer metrics;

/* function prototypes */
std::wstring GetBinaryString(vector<bool> binary);
void WipeAsteroid(Coordinate coords, int height, int width);
bool Compare(Asteroid obj1, Asteroid obj2);
void AddAsteroidToField(Asteroid asteroid);
void SpawnAsteroid();
void PopulateDebug(std::vector<DebugLine> displayLines);
void SanitizeDebug();
template <typename T>
bool AtBoundry(T obj);
template <typename T>
void Move(T &obj);

/* functions */
int main()
{
    std::srand(std::time(0));

    for (int x = 0; x < debugWidth; x++)
        for (int y = 0; y < debugHeight; y++)
            debugScreen[y * debugWidth + x] =
                ((x == 0) || (y == 0) || (x == (debugWidth - 1)) || (y == (debugHeight - 1))) ? L'-' : L' '; // 20 = space, 126 = squiggle

    // Create the screen buffer
    // wchar_t *screen = new wchar_t[consoleHeight * consoleWidth];
    // for (int index = 0; index < consoleHeight * consoleWidth; index++)
    //     screen[index] = L' ';
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
                    vAsteroid.RemoveElement(i);
                    SanitizeDebug();
                    metrics.DeleteInc();
                }
            };

            while (indexMoveQueue.size())
            {
                Move(vAsteroid[indexMoveQueue.back()]);
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
        std::vector<DebugLine> debugLine;
        debugLine.push_back(DebugLine(L"Value at screen[0]", std::to_wstring(consoleScreen.GetScreenContent()[2 * consoleWidth + 99])));
        PopulateDebug(debugLine);

        // Draw Border
        // Left & Right Borders
        for (int y = 0; y < fieldHeight + 2; y++)
        {
            consoleScreen.Add((y + iFieldBorderY) * (consoleWidth) + iFieldBorderX, L'|');
            consoleScreen.Add((y + iFieldBorderY) * (consoleWidth) + (iFieldBorderX + (fieldWidth + 1)), L'|');
        }
        // Top & Bottom Borders
        for (int x = 1; x < fieldWidth + 1; x++)
        {
            consoleScreen.Add(iFieldBorderY * (consoleWidth) + (iFieldBorderX + x), L'=');
            consoleScreen.Add((iFieldBorderY + (fieldHeight + 1)) * (consoleWidth) + (iFieldBorderX + x), L'=');
        }

        // Draw field
        for (int x = 0; x < fieldWidth; x++)
        {
            for (int y = 0; y < fieldHeight; y++)
            {
                consoleScreen.Add((y + iFieldY) * consoleWidth + (x + iFieldX), fieldScreen[y * fieldWidth + x]);
            }
        }

        // Draw Debug window
        for (int x = 0; x < debugWidth; x++)
        {
            for (int y = 0; y < debugHeight; y++)
            {
                consoleScreen.Add((debugY + y) * consoleWidth + (debugX + x), debugScreen[y * debugWidth + x]);
            }
        }

        // Draw Timer
        swprintf_s(&consoleScreen[iTimerY * consoleWidth + iTimerX], 16, L"Time: %d", timer);
        // CMD Buffer variables
        WriteConsoleOutputCharacterW(consoleHandle, consoleScreen.GetScreenContent().data(), consoleWidth * consoleHeight, {
                                                                                                                               0,
                                                                                                                               0,
                                                                                                                           },
                                     &bytesWritten);
    }
    return 0;
}

void PopulateDebug(std::vector<DebugLine> displayLines)
{
    /*
        debugHeight = 28,
        debugWidth = 75;
    */
    std::vector<std::wstring> lines;
    int lineNumber = 1;
    // Write header
    lines.push_back(
        L"Asteroids Created/Destroyed: " + std::to_wstring(metrics.GetCreateCount()) + L"\\" + std::to_wstring(metrics.GetDeleteCount()));
    lineNumber++;
    // Write tick speed
    lines.push_back(L"Tick Speed: " + std::to_wstring(ticks * 50) + L"ms");
    lineNumber++;
    lines.push_back(L" ");
    // Asteroid Count
    lines.push_back(L"Asteroid Count: " + std::to_wstring(metrics.GetCreateCount() - metrics.GetDeleteCount()));
    lineNumber++;
    lines.push_back(L" ");
    // Array values
    int charValue = debugScreen[0];
    lines.push_back(L"Screen value at Debug[0]: " + std::to_wstring(charValue));
    lineNumber++;
    lines.push_back(L" ");

    for (int i = 0; i < displayLines.size() && i + lineNumber < debugHeight; i++)
    {
        lines.push_back(displayLines[i].Get());
    }

    // Iterate through Lines changing debugPointer
    for (int y = 0; y < lines.size(); y++)
    {
        for (int x = 0; x < lines[y].size(); x++)
        {
            if ((int)lines[y][x] == 771)
            {
                throw std::out_of_range("kek");
            }
            debugScreen[(y + 1) * debugWidth + (x + 1)] = (unsigned char)lines[y][x];
        }
    }
} // End PopulateDebug()

void SanitizeDebug()
{
    int lineNumber = 2;
    int rowStart = 1;

    for (int x = 1; x < debugWidth - 1; x++)
        for (int y = 1; y < debugHeight - 1; y++)
            debugScreen[y * debugWidth + x] = L' ';
    // {
    //     debugPointer[y * debugWidth + x] = ' ';
    // }
}

void SpawnAsteroid()
{
    int xMax = fieldWidth - 3,
        xMin = 1,
        yMax = fieldHeight / 2 - 1,
        yMin = 1;
    int xCoord = rand() % (xMax - xMin) + xMin,
        yCoord = rand() % (yMax - yMin) + yMin;
    Asteroid asteroid(xCoord, yCoord);
    vAsteroid.push_back(asteroid);

    metrics.CreateInc();
    AddAsteroidToField(asteroid);
}

void AddAsteroidToField(Asteroid asteroid)
{
    // Add the Asteroid to the field
    for (int xA = 0; xA < asteroid.GetWidth(); xA++)
        for (int yA = 0; yA < asteroid.GetHeight(); yA++)
        {
            int index = (asteroid.coordinate.GetY() + yA) * fieldWidth + (asteroid.coordinate.GetX() + xA);
            if (asteroid.GetShapeMap()[(yA * asteroid.GetWidth() + xA)] && index >= 1)
            {
                // Add the current
                fieldScreen[(asteroid.coordinate.GetY() + yA) * fieldWidth + (asteroid.coordinate.GetX() + xA)] = L'0';
            }
        }
}

bool Compare(Asteroid obj1, Asteroid obj2)
{
    return (obj1.coordinate.GetY() < obj2.coordinate.GetY());
}

void WipeAsteroid(Coordinate coords, int height, int width)
{
    for (int x = 0; x < width && coords.GetX() + width < fieldWidth; x++)
        for (int y = 0; y <= height && coords.GetY() + height <= fieldHeight; y++)
        {
            fieldScreen[(y + coords.GetY()) * fieldWidth + (x + coords.GetX())] = L' ';
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
    return !(obj.coordinate.GetY() <= fieldHeight);
}

template <typename T>
void Move(T &obj)
{
    // Starting at the bottom
    // X is decreased first to move from the bottom-right to the top-left
    for (int yA = 0; yA < obj.GetHeight(); yA++)
    {
        for (int xA = 0; xA < obj.GetWidth(); xA++)
        {
            if (obj.coordinate.GetY() < 0)
            {
                int index = (yA + obj.coordinate.GetY()) * fieldWidth + (xA + obj.coordinate.GetX());
                if (index >= 0 && index < fieldScreen.size() && obj.GetShapeMap()[yA * obj.GetWidth() + xA])
                {
                    fieldScreen.Add(index, L'0');
                }
            } else {
                int index = (yA + obj.coordinate.GetY()) * fieldWidth + (xA + obj.coordinate.GetX());
                if (index >= 0 && index < fieldScreen.size() && obj.GetShapeMap()[yA * obj.GetWidth() + xA])
                {
                    fieldScreen.Add(index, L' ');
                    fieldScreen.Add(index + fieldWidth, L'0');
                }
            }
        }
    }
    obj.MoveY(); // Change the Y value
}

// template <typename T>
// vector<Coordinate> GetCollisionSet(const T &v)
// {

// }
#endif