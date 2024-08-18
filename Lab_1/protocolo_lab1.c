#include <stdio.h>
#include <stdint.h>

#define MSG_TYPE_CONTROL 0x00
#define MSG_TYPE_STATUS 0x01
#define MSG_TYPE_DATA 0x02
#define MSG_TYPE_ERROR 0x03

int Flag_MSG_Control = 0;
int Flag_MSG_Status = 0;
int Flag_MSG_Data = 0;
int Flag_ERROR = 0;

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

    printf("Tipo de mensaje a mandar: ");
    scanf("%hhu", &type);
    printf("mensaje a ingresar: ");
    fgets(message,300,stdin);
    
    switch (type)
    {
    case 0x00:
        Flag_MSG_Control = 1;
        break;
    case 0x01:
        Flag_MSG_Status = 1;
        break;
    case 0x02:
        Flag_MSG_Data = 1;
        break;
    default:
        printf("Tipo de mensaje incorrecto");
        break;
    }
    

}




