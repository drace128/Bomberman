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


    //initCC3100(player1);
    G8RTOS_InitSemaphore(&wifi, 1);
    G8RTOS_InitSemaphore(&globalsem, 1);
    G8RTOS_InitSemaphore(&globalsemdata, 1);
    //G8RTOS_AddThread(&SendDataToHost, "SendToHost", 1);
    //G8RTOS_AddThread(&ReceiveDataFromHost, "ReceiveFromHost", 1);
    G8RTOS_AddThread(&DrawObjects, "DrawObjects", 1);
    G8RTOS_AddThread(&ReadJoystickClient, "joystick", 1);
    G8RTOS_AddThread(&IdleThread, "idle", 255);
    //Handshake(player1);
    DrawArena();

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
//    DrawArena();

    G8RTOS_Launch();
    while(1);
}
