//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "app.h"

void main(void)
{
    playerType player1, player2;
    player1 = Client;
    player2 = Host;

    G8RTOS_Init();

    for(int j = 16; j < 288-48; j+=16)
    {
        LCD_DrawRectangle(j+16, j+16, 0, 15, LCD_BLACK);
        LCD_DrawRectangle(j+16, j+16, 225, 240, LCD_BLACK);
    }

    for(int j = 16; j < 208; j+=16)
    {
        LCD_DrawRectangle(0, 15, j+16, j+16, LCD_BLACK);
        LCD_DrawRectangle(305-48, 320-48, j+16, j+16, LCD_BLACK);
    }

    LCD_DrawRectangle(0, 320-48, 15, 15, LCD_BLACK);
    LCD_DrawRectangle(0, 320-48, 225, 225, LCD_BLACK);
    LCD_DrawRectangle(15, 15, 0, 240, LCD_BLACK);
    LCD_DrawRectangle(305-48, 305-48, 0, 240, LCD_BLACK);
    LCD_DrawRectangle(305-32, 305-32, 0, 240, LCD_BLACK);

    LCD_DrawRectangle(16, 304-48, 16, 224, LCD_GREEN);
    LCD_DrawRectangle(16, 31, 16, 31, LCD_WHITE);
    //LCD_Bomberman(16, 31, 16, 31, table);

    for(int i = 32; i < 224; i+=32)
    {
        for(int j = 32; j < 250; j+=32)
        {
            LCD_DrawRectangle(j, j+14, i, i+14, LCD_GRAY);
            LCD_DrawRectangle(j, j+15, i+15, i+15, LCD_BLACK);
            if(j < 288)
            {
                LCD_DrawRectangle(j+15, j+15, i, i+15, LCD_BLACK);
            }
        }
    }

////    LCD_DrawRectangle(40, 60, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(80, 100, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(120, 140, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(160, 180, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(200, 220, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(240, 260, 40, 60, LCD_GRAY);
////    LCD_DrawRectangle(280, 300, 40, 60, LCD_GRAY);

//
//    initCC3100(player1);
//    G8RTOS_InitSemaphore(&wifi, 1);
//    G8RTOS_InitSemaphore(&globalsem, 1);
//    G8RTOS_InitSemaphore(&globalsemdata, 1);
//    G8RTOS_AddThread(&IdleThread, "idle", 255);
//    G8RTOS_AddThread(&SendDataToHost, "SendToHost", 1);
//    G8RTOS_AddThread(&ReceiveDataFromHost, "ReceiveFromHost", 1);
//    G8RTOS_AddThread(&DrawObjectsClient, "DrawObjects", 1);
//    Handshake(player1);
//
////    initCC3100(player2);
////    G8RTOS_InitSemaphore(&wifi, 1);
////    G8RTOS_InitSemaphore(&globalsem, 1);
////    G8RTOS_InitSemaphore(&globalsemdata, 1);
////    G8RTOS_AddThread(&IdleThread, "idle", 255);
////
////    G8RTOS_AddThread(&SendDataToClient, "SendToClient", 1);
////    G8RTOS_AddThread(&ReceiveDataFromClient, "RecvFromClient", 1);
////    G8RTOS_AddThread(&DrawObjectsHost, "DrawObjects", 1);
////    Handshake(player2);


    initCC3100(player1);
    G8RTOS_InitSemaphore(&wifi, 1);
    G8RTOS_InitSemaphore(&globalsem, 1);
    G8RTOS_InitSemaphore(&globalsemdata, 1);
    G8RTOS_AddThread(&SendDataToHost, "SendToHost", 1);
    G8RTOS_AddThread(&ReceiveDataFromHost, "ReceiveFromHost", 1);
    G8RTOS_AddThread(&DrawObjects, "DrawObjects", 1);
    G8RTOS_AddThread(&ReadJoystickClient, "joystick", 1);
    G8RTOS_AddThread(&IdleThread, "idle", 255);
    Handshake(player1);

//    initCC3100(player2);
//    G8RTOS_InitSemaphore(&wifi, 1);
//    G8RTOS_InitSemaphore(&globalsem, 1);
//    G8RTOS_InitSemaphore(&globalsemdata, 1);
//    G8RTOS_AddThread(&SendDataToClient, "SendToClient", 1);
//    G8RTOS_AddThread(&ReceiveDataFromClient, "RecvFromClient", 1);
//    G8RTOS_AddThread(&DrawObjects, "DrawObjects", 1);
//    G8RTOS_AddThread(&ReadJoystickHost, "joystick", 1);
//    G8RTOS_AddThread(&IdleThread, "idle", 255);
//    Handshake(player2);

    G8RTOS_Launch();
    while(1);
}
