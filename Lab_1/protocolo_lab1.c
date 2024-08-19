#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MSG_TYPE_CONTROL 0x00
#define MSG_TYPE_STATUS 0x01
#define MSG_TYPE_DATA 0x02
#define MSG_TYPE_ERROR 0x03

uint8_t Flag_MSG_Control = 0x00;
uint8_t Flag_MSG_Status = 0x00;
uint8_t Flag_MSG_Data = 0x00;
uint8_t Flag_ERROR = 0x00;

typedef struct
{
    uint8_t MSG_Type;
    void *data;
    int length;
}Message;

void send_Message()
{
    uint8_t type;
    char message[300];
    Message msg;

    printf("Tipo de mensaje a mandar: \n");
    scanf("%hhu",&type);
    getchar();

    switch (type)
    {
    case 0x00:
        if(Flag_MSG_Control == 0x01){
            printf("tipo de mensaje ya enviado y no leido \n");
            break;
        }
        if (Flag_ERROR == 0x00)
        {
            printf("mensaje a ingresar: \n");
            fgets(message,300,stdin);
            message[strcspn(message, "\n")] = '\0'; // Remove the newline character
            msg = (Message){type, &message, strlen(message)};
            Flag_MSG_Control = 0x01;
        }
        else
        {
            printf("error al mandar mensaje\n");
        }
        break;
    case 0x01:
        if (Flag_MSG_Status ==0x01)
        {
            printf("tipo de mensaje ya enviado y no leido \n");
            break;
        }
        if (Flag_ERROR == 0x00)
        {
            printf("mensaje a ingresar: \n");
            fgets(message,300,stdin);
            message[strcspn(message, "\n")] = '\0'; // Remove the newline character
            msg = (Message){type, &message, strlen(message)};
            Flag_MSG_Status = 0x01;
        }
        else
        {
            printf("error al mandar mensaje \n");
        }
        break;
    case 0x02:
    if (Flag_MSG_Data ==0x01)
        {
            printf("tipo de mensaje ya enviado y no leido \n");
            break;
        }
        if (Flag_ERROR == 0x00)
        {
            printf("mensaje a ingresar: \n");
            fgets(message,300,stdin);
            message[strcspn(message, "\n")] = '\0'; // Remove the newline character
            msg = (Message){type, &message, strlen(message)};
            Flag_MSG_Data = 0x01;
        }
        else
        {
            printf("error al mandar mensaje \n");
        }
        break;
    default:
        printf("Tipo de mensaje incorrecto \n");
        Flag_ERROR = 0x01;
        break;
    }
    
}

int main(int argc, char** argv)
{
    int cont = 10;
    while( cont != 0)
    {
        send_Message();
        printf("funcion corrida \n");
        cont--;
    }

    return 0;
}


