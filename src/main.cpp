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
#define SPAWN_INTERVAL 2

// Debugging container

int consoleHeight = 50,
    consoleWidth = 175;

int fieldHeight = 28,
    fieldWidth = 75;
// Remaining Width = 175 - 75 = 100
// Remaining Height = 50 - 28 = 22
int debugHeight = 28,
    debugWidth = 75;

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

unsigned char *fieldPointer = nullptr;
unsigned char *messagePointer = nullptr;
unsigned char *debugPointer = nullptr;

VecWrapper<Asteroid> vAsteroid;
std::vector<int> indexMoveQueue;

DebugContainer metrics;

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

std::wstring GetBinaryString(vector<bool> binary)
{
    std::wstring binaryString = L"";

    for (int index = 0; index < binary.size(); index++)
    {
        binaryString += binary[index] ? L"1" : L"0";
    }
    return binaryString;
}

void WipeAsteroid(Coordinate coords, int height, int width)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            fieldPointer[(y + coords.GetY()) * fieldWidth + (x + coords.GetX())] = 0;
        }
}

bool Compare(Asteroid obj1, Asteroid obj2)
{
    return (obj1.coordinate.GetY() < obj2.coordinate.GetY());
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

    // Write out data for each asteroid
    for (int y = 0; y < vAsteroid.size(); y++)
    {
        if ((lineNumber + y) < debugHeight)
        {
            int xOffset = 0;
            // Draw out the shapemap
            for (int x = 0; x < vAsteroid[y].GetShapeMap().size(); x++)
            {
                debugPointer[((/* Y */ lineNumber + y) * debugWidth + rowStart + x)] = (vAsteroid[y].GetShapeMap()[x] ? L'1' : L'0');
            }
            xOffset += vAsteroid[y].GetShapeMap().size();
            // Add the address
            std::wostringstream woStringStream;
            woStringStream << L" 0x" << &vAsteroid[y];
            std::wstring stringAddress = woStringStream.str();
            for (int x = 0; x < stringAddress.size(); x++)
            {
                debugPointer[(lineNumber + y) * debugWidth + (rowStart + x + xOffset)] = stringAddress[x];
            }
            xOffset += stringAddress.size();
            // Add the Y coordinate
            std::wstring coord = L" Y:" + std::to_wstring(vAsteroid[y].coordinate.GetY());
            for (int x = 0; x < coord.size(); x++)
            {
                debugPointer[(lineNumber + y) * debugWidth + (rowStart + x + xOffset)] = coord[x];
            }
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

    int ticks = 20; // 1 second
    int tickCount = 0;
    int time = 0;

    // Game Loop
    while (!bGamerOver)
    {
        // Timing
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tickCount++;
        bool update = (tickCount == ticks);

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
                    /*
                        Need to account for the aseroid at the last position
                        being skipped over for movement when a removal occurs.
                    */
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

            if (!(time % SPAWN_INTERVAL)) // if time is evenly divisible by 5, spawn a rock
            {
                SpawnAsteroid();
                std::sort(vAsteroid.begin(), vAsteroid.end(), Compare);
            }

            time++;
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
        swprintf_s(&screen[iTimerY * consoleWidth + iTimerX], 16, L"Time: %d", time);

        // CMD Buffer variables
        WriteConsoleOutputCharacterW(consoleHandle, screen, consoleWidth * consoleHeight, {
                                                                                              0,
                                                                                              0,
                                                                                          },
                                     &bytesWritten);
    }
    return 0;
}
#endif