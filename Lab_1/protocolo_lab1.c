/*
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
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MSG_TYPE_CONTROL 0x00
#define MSG_TYPE_STATUS 0x01
#define MSG_TYPE_DATA 0x02
#define MSG_TYPE_ERROR 0x03

bool Flag_MSG_Control = false;
bool Flag_MSG_Status = false;
bool Flag_MSG_Data = false;
bool Flag_ERROR = false;

typedef struct
{
    uint8_t MSG_Type;
    char *data;
    int length;
} Message;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear the buffer until newline or EOF
}

void send_Message()
{
    uint8_t type = 0xFF; // Initialize with an invalid value
    char input[300];
    Message msg;
    msg.data = NULL; // Initialize the message data pointer

    // Reset the flags if necessary
    if (Flag_ERROR == true) {
        Flag_ERROR = false;
    }

    printf("Tipo de mensaje a mandar (0: Control, 1: Status, 2: Data): \n");

    // Read the type
    if (scanf("%hhu", &type) != 1) {
        printf("Error leyendo el tipo de mensaje\n");
        clear_input_buffer(); // Clear the buffer if scanf fails
        return;
    }
    clear_input_buffer(); // Clear the buffer to remove the newline character after scanf

    // Debug: Print the type read
    printf("Tipo de mensaje leído: 0x%02X\n", type);

    // Initialize the message data
    msg.data = NULL;

    switch (type)
    {
    case MSG_TYPE_CONTROL:
        if (Flag_MSG_Control == true)
        {
            printf("Tipo de mensaje (control) ya enviado y no leído \n");
            break;
        }
        if (Flag_ERROR == false)
        {
            printf("Mensaje a ingresar: \n");
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strcspn(input, "\n")] = '\0'; // Remove the newline character

                msg.data = malloc(strlen(input) + 1); // Allocate memory for message data
                if (msg.data == NULL)
                {
                    printf("Error al asignar memoria\n");
                    Flag_ERROR = true;
                    return;
                }
                strcpy(msg.data, input);
                msg.MSG_Type = type;
                msg.length = strlen(input);
                Flag_MSG_Control = true;
                printf("Mensaje de control enviado\n");
            }
            else
            {
                printf("Error leyendo el mensaje\n");
            }
        }
        else
        {
            printf("Error al mandar mensaje\n");
        }
        break;

    case MSG_TYPE_STATUS:
        if (Flag_MSG_Status == true)
        {
            printf("Tipo de mensaje (status) ya enviado y no leído \n");
            break;
        }
        if (Flag_ERROR == false)
        {
            printf("Mensaje a ingresar: \n");
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strcspn(input, "\n")] = '\0'; // Remove the newline character

                msg.data = malloc(strlen(input) + 1); // Allocate memory for message data
                if (msg.data == NULL)
                {
                    printf("Error al asignar memoria\n");
                    Flag_ERROR = true;
                    return;
                }
                strcpy(msg.data, input);
                msg.MSG_Type = type;
                msg.length = strlen(input);
                Flag_MSG_Status = true;
                printf("Mensaje de estatus enviado\n");
            }
            else
            {
                printf("Error leyendo el mensaje\n");
            }
        }
        else
        {
            printf("Error al mandar mensaje \n");
        }
        break;

    case MSG_TYPE_DATA:
        if (Flag_MSG_Data == true)
        {
            printf("Tipo de mensaje (data) ya enviado y no leído \n");
            break;
        }
        if (Flag_ERROR == false)
        {
            printf("Mensaje a ingresar: \n");
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strcspn(input, "\n")] = '\0'; // Remove the newline character

                msg.data = malloc(strlen(input) + 1); // Allocate memory for message data
                if (msg.data == NULL)
                {
                    printf("Error al asignar memoria\n");
                    Flag_ERROR = true;
                    return;
                }
                strcpy(msg.data, input);
                msg.MSG_Type = type;
                msg.length = strlen(input);
                Flag_MSG_Data = true;
                printf("Mensaje de datos enviado\n");
            }
            else
            {
                printf("Error leyendo el mensaje\n");
            }
        }
        else
        {
            printf("Error al mandar mensaje \n");
        }
        break;

    default:
        printf("Tipo de mensaje incorrecto \n");
        Flag_ERROR = true;
        break;
    }

    // Free allocated memory for message data
    if (msg.data != NULL)
    {
        free(msg.data);
    }
}

int main(int argc, char **argv)
{
    int cont = 10;
    while (cont != 0)
    {
        send_Message();
        printf("Función corrida \n");
        cont--;
    }

    return 0;
}



