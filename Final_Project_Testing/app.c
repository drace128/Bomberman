/*
 * app.c
 *
 *  Created on: Nov 28, 2017
 *      Author: brons
 */
#include "msp.h"
#include "BSP.h"
#include "cc3100_usage.h"
#include <stdbool.h>
#include "app.h"

/*************** Global Variables **************/
_u32 ClientIP;
uint8_t display = 0;
uint8_t displayarr[3];
uint8_t fill = 0;
uint8_t data = 0;
uint16_t counterSend = 0;
uint16_t counterRecv = 0;
uint16_t counterIdle = 0;

int16_t xCoord, yCoord;
Player_t host, client;
Prevplayer_t oldhost, oldclient;

uint16_t tablehost[256] = {   960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          576,
                          768,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                         8384,
                         53254,
                         28675,
                          704,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                            0,
                         12289,
                         12483,
                         16516,
                           64,
                          512,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          448,
                         41546,
                         54158,
                         62415,
                         62415,
                         58255,
                         24838,
                          384,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          832,
                         20613,
                         62415,
                         62283,
                         58122,
                         58122,
                         62283,
                         49932,
                         12355,
                          832,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          832,
                         33225,
                         62216,
                         16513,
                         62151,
                         62151,
                         16513,
                         62152,
                         20742,
                          832,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          832,
                         12420,
                         62151,
                         8256,
                         62151,
                         62151,
                         8256,
                         62151,
                         8259,
                          832,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          896,
                           64,
                         41545,
                         62153,
                         58056,
                         58056,
                         62153,
                         41545,
                           64,
                          896,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          896,
                         25030,
                         4161,
                         4226,
                         4161,
                         8257,
                         8322,
                         4097,
                         16708,
                          896,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                         4225,
                         54029,
                         4358,
                         41871,
                         25359,
                         17103,
                         21135,
                         8454,
                         58254,
                         4225,
                          960,
                          960,
                          960,
                          960,
                          960,
                          576,
                         4096,
                            0,
                           66,
                           66,
                         62210,
                         58050,
                            2,
                            2,
                         4096,
                         4096,
                          576,
                          960,
                          960,
                          960,
                          960,
                          768,
                         32772,
                         12288,
                         37581,
                         25230,
                         16975,
                         16975,
                         29391,
                         33420,
                         12288,
                         32772,
                          768,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          320,
                         49868,
                         20870,
                            0,
                            0,
                         49997,
                         16644,
                          320,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          384,
                         32771,
                         40964,
                          320,
                          320,
                         40964,
                         36868,
                          384,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          512,
                          128,
                          128,
                          448,
                          448,
                          128,
                          128,
                          512,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960,
                          960};


/*************** Common Threads ****************/
void Handshake(bool player)
{
    int8_t retval = -1;
    if(player)      //host
    {
        while(retval != 0)
        {
            G8RTOS_WaitSemaphore(&wifi);
            retval = ReceiveData(bufferIP, sizeof(bufferIP));
            G8RTOS_SignalSemaphore(&wifi);
        }
        ClientIP = ((bufferIP[3]& 0x000000ff) << 24 )|((bufferIP[2]& 0x000000ff) << 16)|((bufferIP[1]& 0x000000ff) << 8)|((bufferIP[0]& 0x000000ff) );

    }
    else            //client
    {
        ClientIP = getLocalIP();
        bufferIP[0] = (ClientIP & 0x000000ff);
        bufferIP[1] = (ClientIP & 0x0000ff00) >> 8;
        bufferIP[2] = (ClientIP & 0x00ff0000) >> 16;
        bufferIP[3] = (ClientIP & 0xff000000) >> 24;
        G8RTOS_WaitSemaphore(&wifi);
        SendData(bufferIP, HOST_IP_ADDR, sizeof(bufferIP));
        G8RTOS_SignalSemaphore(&wifi);
    }
}

void IdleThread()
{
    while(1)
    {
        counterIdle++;
    }
}

void fillBuffer(uint8_t * buffer, uint8_t numOfBytes, int data, int index)
{
    if(numOfBytes == 1)
    {
        buffer[index] = data;
    }
    else if(numOfBytes == 2)
    {
        buffer[index+1] = data >> 8;
        buffer[index] = data;
    }
    else if(numOfBytes == 3)
    {
        buffer[index] = data >> 16;
        buffer[index+1] = data >> 8;
        buffer[index+2] = data;
    }
    else if(numOfBytes == 4)
    {
        buffer[index] = data >> 24;
        buffer[index+1] = data >> 16;
        buffer[index+2] = data >> 8;
        buffer[index+3] = data;
    }
}


/*************** Client Threads ****************/
void DrawObjectsClient()
{
    uint8_t tempfill;
    uint8_t tempfill2;
    while(1)
    {
        G8RTOS_WaitSemaphore(&globalsem);
        tempfill = fill;
        G8RTOS_SignalSemaphore(&globalsem);

        if(tempfill != tempfill2)
        {
            LCD_DrawRectangle(100, 150, 40, 55, LCD_BLACK);
            displayarr[2] = tempfill % 10 + '0';
            display = tempfill / 10;

            displayarr[1] = display % 10 + '0';
            display = display / 10;

            displayarr[0] = display + '0';

            LCD_Text(100, 40, displayarr, LCD_WHITE);
            tempfill2 = tempfill;
        }
        G8RTOS_OS_Sleep(21);
    }
}


void SendDataToHost()
{
    Player_t tempclient;
    while(1)
    {
        G8RTOS_WaitSemaphore(&globalsemdata);
        tempclient = client;
        G8RTOS_SignalSemaphore(&globalsemdata);

        //fillBuffer(bufferToHost, 1, temp, 0);

        G8RTOS_WaitSemaphore(&wifi);
        SendData((_u8 *)&tempclient, HOST_IP_ADDR, sizeof(Player_t));
        G8RTOS_SignalSemaphore(&wifi);

        counterSend++;
        G8RTOS_OS_Sleep(5);
    }
}

void ReceiveDataFromHost()
{
    int8_t retval = -1;
    Player_t temphost;
    while(1)
    {
        while(retval != 0)
        {
            G8RTOS_WaitSemaphore(&wifi);
            retval = ReceiveData((_u8 *)&temphost, sizeof(Player_t));
            G8RTOS_SignalSemaphore(&wifi);
            G8RTOS_OS_Sleep(1);
        }
        counterRecv++;
        retval = -1;

        G8RTOS_WaitSemaphore(&globalsem);
        host = temphost;
        G8RTOS_SignalSemaphore(&globalsem);

        G8RTOS_OS_Sleep(2);
    }
}


/*************** Host Threads   ****************/
void DrawObjects()
{
    while(1)
    {
        DrawPlayer(&host, &oldhost, tablehost);
        DrawPlayer(&client, &oldclient, tablehost);
        G8RTOS_OS_Sleep(15);
    }
}


void SendDataToClient()
{
    Player_t temphost;
    while(1)
    {
        G8RTOS_WaitSemaphore(&globalsem);
        temphost = host;
        G8RTOS_SignalSemaphore(&globalsem);

        //fillBuffer(bufferToClient, 1, temp, 0);

        counterSend++;
        G8RTOS_WaitSemaphore(&wifi);
        SendData((_u8 *)&temphost, ClientIP, sizeof(Player_t));
        G8RTOS_SignalSemaphore(&wifi);

        G8RTOS_OS_Sleep(5);
    }
}

void ReceiveDataFromClient()
{
    int32_t retval = -1;
    Player_t tempclient;
    while(1)
    {
        while(retval != 0)
        {
            G8RTOS_WaitSemaphore(&wifi);
            retval = ReceiveData((_u8 *)&tempclient, sizeof(Player_t));
            G8RTOS_SignalSemaphore(&wifi);
            G8RTOS_OS_Sleep(1);
        }
        counterRecv++;
        retval = -1;

        G8RTOS_WaitSemaphore(&globalsemdata);
        client = tempclient;
        G8RTOS_SignalSemaphore(&globalsemdata);

        G8RTOS_OS_Sleep(2);
    }

}

void ReadJoystickHost()
{
    host.xpos = 16;
    host.ypos = 16;
    host.speedlevel = 1;
    oldhost.xpos = 16;
    oldhost.ypos = 16;
    while(1)
    {
        GetJoystickCoordinates(&xCoord, &yCoord);
        if(xCoord > 4000 || (xCoord <-4000)){
        for(int i = 16; i < 210; i+=32)
        {
            if(host.ypos < i+4 && host.ypos > i-4)
            {
                host.ypos = i;
                if(xCoord > 4000)
                {
                    host.xpos -= host.speedlevel;
                }
                if(xCoord < -4000)
                {
                    host.xpos += host.speedlevel;
                }
            }
        }
        }
        if(yCoord > 4000 || (yCoord <-4000)){
        for(int i = 16; i < 270; i+=32)
        {
            if(host.xpos < i+4 && host.xpos > i-4)
            {
                host.xpos = i;
                if(yCoord < -4000)
                {
                    host.ypos -= host.speedlevel;
                }
                if(yCoord > 4000)
                {
                    host.ypos += host.speedlevel;
                }
            }
        }
        }
        G8RTOS_OS_Sleep(20);
    }
}


void ReadJoystickClient()
{
    client.xpos = 240;
    client.ypos = 208;
    client.speedlevel = 1;
    oldclient.xpos = 240;
    oldclient.ypos = 208;
    while(1)
    {
        GetJoystickCoordinates(&xCoord, &yCoord);
        if(xCoord > 4000 || (xCoord <-4000)){
        for(int i = 16; i < 210; i+=32)
        {
            if(client.ypos < i+4 && client.ypos > i-4)
            {
                client.ypos = i;
                if(xCoord > 4000)
                {
                    client.xpos -= client.speedlevel;
                }
                if(xCoord < -4000)
                {
                    client.xpos += client.speedlevel;
                }
            }
        }
        }
        if(yCoord > 4000 || (yCoord <-4000)){
        for(int i = 16; i < 270; i+=32)
        {
            if(client.xpos < i+4 && client.xpos > i-4)
            {
                client.xpos = i;
                if(yCoord < -4000)
                {
                    client.ypos -= client.speedlevel;
                }
                if(yCoord > 4000)
                {
                    client.ypos += client.speedlevel;
                }
            }
        }
        }
        G8RTOS_OS_Sleep(20);
    }
}

void DrawPlayer(Player_t* player, Prevplayer_t* oldplayer, uint16_t* tablePlayer)
{
    if(oldplayer->xpos != player->xpos || oldplayer->ypos != player->ypos)
    {
        LCD_DrawRectangle(oldplayer->xpos, oldplayer->xpos+15, oldplayer->ypos, oldplayer->ypos+15, LCD_GREEN);
        LCD_Bomberman(player->xpos, player->xpos+15, player->ypos, player->ypos+15, tablePlayer);
        oldplayer->xpos = player->xpos;
        oldplayer->ypos = player->ypos;
    }
}

