/*
 * @file main.c
 * @brief Main file for the project
 */

#include <stdio.h>
#include <stdint.h>
#include "../include/lab1_protocol.h"

Message a;
int b = a.message_type;

if(a.message_type == MSG_TYPE_CONTROL)
{
    // send_control o handle_control ... 
}
// 1 funcion para cada tipo de mensaje



void setMessageData(Message* msg, void* data, uint16_t length) 
{
    msg->data = malloc(length);
    if (msg->data != NULL) {
        memcpy(msg->data, data, length); // 
        msg->length = length;
    }
}

void setMessageType(Message* msg, uint8_t message_type)   
{
    msg->message_type = message_type;
} 

void setMessageFlag(Message* msg, uint8_t flags)
{
    msg->flags |= flags;
}
