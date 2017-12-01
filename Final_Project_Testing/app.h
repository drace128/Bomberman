/*
 * app.h
 *
 *  Created on: Nov 28, 2017
 *      Author: brons
 */

#ifndef APP_H_
#define APP_H_

#include "G8RTOS.h"
#include <stdbool.h>

#define MAX_NUM_OF_BOMBS     8
#define MAX_NUM_OF_BRICKS    174


#pragma pack ( push, 1)

/* Data Structures */
typedef struct
{
    uint16_t Xindex;
    uint16_t Yindex;
    uint8_t countdown;
    uint8_t animation;
    bool alive;
} Bomb_t;

typedef struct
{
    uint16_t xpos;
    uint16_t ypos;
    uint8_t speedlevel;
    uint8_t maxbombs;
    uint8_t bombstrength;
    bool bombplaced;
    bool alive;
} Player_t;

typedef struct
{
    uint16_t Xindex;
    uint16_t Yindex;
    bool alive;
} Brick_t;

typedef struct
{
    Player_t player;
    Bomb_t bombs[MAX_NUM_OF_BOMBS];
    Brick_t bricks[MAX_NUM_OF_BRICKS];
    uint8_t gametime;
    bool winner;
    bool gameover;
} gamestate_t;
#pragma pack ( pop )


typedef struct
{
    uint16_t xpos;
    uint16_t ypos;
} Prevplayer_t;

/* Buffers */
uint8_t bufferToClient[30];
uint8_t bufferToHost[30];
uint8_t bufferIP[4];

/* Semaphores */
semaphore_t wifi;
semaphore_t globalsem;
semaphore_t globalsemdata;

/* Client Threads */
void SendDataToHost();
void ReceiveDataFromHost();
void ReadJoystickClient();

/* Host Threads */
void SendDataToClient();
void ReceiveDataFromClient();
void ReadJoystickHost();
void DrawPlayer();

/* Common Threads */
void Handshake(bool player);
void DrawObjectsClient();
void DrawObjects();
void IdleThread();

#endif /* APP_H_ */
