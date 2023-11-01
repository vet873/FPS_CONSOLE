// FPS_CONSOLE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <Windows.h>
#include <string>
#include <chrono>

//removes the need to use std::
using namespace std;


int nScreenWidth = 120;
int nScreenHeight = 40;

//Player position
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
//Player angle
float fPlayerA = 0.0f;

//map width and height.
int nMapHeight = 16;
int nMapWidth = 16;

//field of view variable
float fFOV = 3.14159 / 4.0;
//max depth to search for a wall
float fDepth = 16.0f;

int main()
{

    //create screen buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //create the map using unicode std::wstring
    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..........#####";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#....######....#";
    map += L"#.........#....#";
    map += L"#..............#";
    map += L"################";


    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();


    //Game Loop
    while (1)
    {
        //take system time
        tp2 = chrono::system_clock::now();
        //calculate elaped time
        chrono::duration<float> elapsedTime = tp2 - tp1;
        //reset tp1
        tp1 = tp2;
        //create float since the game engine uses floating point variables.
        float fElapsedTime = elapsedTime.count();



        //Controls
        //Handle CCW Rotation
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            fPlayerA -= (0.8f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            fPlayerA += (0.8f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
        }

        for (int x = 0; x < nScreenWidth; x++)
        {
            //For each colomn, calculate the projected ray angle into world space
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float) nScreenWidth) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;

            //calculate direction the player is looking
            float fEyeX = sinf(fRayAngle); // unit vector for ray in player space
            float fEyeY = cosf(fRayAngle);


            while (!bHitWall && fDistanceToWall < fDepth)
            {

                fDistanceToWall += 0.1f;

                //draw line to check distance to wall from player. swapping value to int because we dont care about 1.5 or 1.01 when checking if there is a wall. we only want the 1.
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY+ fEyeY * fDistanceToWall);

                //Test if ray is out of bounds
                if (nTestX < 0 || nTestX > nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;    //just set distance to maximum depth
                    fDistanceToWall = fDepth;
                }
                else
                {
                    //Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                    }

                }

            }

            //Calculate distance to ceiling and floor
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';

            //using unicode to draw the wall as opposed to #
            if (fDistanceToWall <= fDepth / 4.0f)           nShade = 0x2588;  //very close
            else if (fDistanceToWall < fDepth / 3.0f)       nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)       nShade = 0x2592;
            else if (fDistanceToWall < fDepth)              nShade = 0x2591;
            else                                            nShade = ' ';     //Too far away


            //draw wall into the column
            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling)
                    screen[y * nScreenWidth + x] = ' ';
                else if (y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade;
                else
                    screen[y * nScreenWidth + x] = ' ';


            }
        }

        //Write to the screen
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    
    
    }

    /* THIS CODE IS FOR TESTING VECTORS


    float screenHeight = 20;
    float screenWidth = 40;
    int continueGame = 1;
    int addforrow = 0;

    //generate minimap design using a vector and screen dimensions
    std::vector <char> miniMap;
    for (int k = 0; k < screenHeight; k++)
    {

        for (int l = 0; l < screenWidth; l++)
        {
            if ((k == 0 || k == (screenHeight - 1)) || (l == 0 || l == (screenWidth - 1)))
                miniMap.push_back('#');
            else
                miniMap.push_back('.');
        }
        
    }


    while (continueGame == 1)
    {

        for (int i = 0; i < screenHeight; i++)
        {
            
            for (int j = 0; j < screenWidth; j++)
            {
                if (j + addforrow < miniMap.size())
                    std::cout << miniMap[j + addforrow];
                else
                    break;

            }
            std::cout << "\n";
            addforrow += screenWidth;
        }

        addforrow = 0;
        //std::cout << "Do you want to continue? 0 = no, 1 = yes: ";
        //std::cin >> continueGame;
        //std::cout << miniMap.size();
        //if (GetAsyncKeyState(VK_SPACE))
            continueGame = 0;
    }

    END OF VECTOR TESTING CODE*/
}
