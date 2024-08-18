
#include <stdint.h>

#define MSG_TYPE_CONTROL 0x00
#define MSG_TYPE_DATA    0x01
#define MSG_TYPE_ERROR   0x02 
#define MSG_TYPE_STATUS  0x03

#define FLAG_ACK   0x01  // 0000 0001
#define FLAG_ERROR 0x02  // 0000 0010
#define FLAG_READY 0x04  // 0000 0100
#define FLAG_BUSY  0x08  // 0000 1000

typedef struct // tipos? --- control, datos, error y state
{
    uint8_t message_type; // 0x00 = control, 0x01 .............

    void *data; // en la funcion send y recieve voy a usar este puntero para referirme al mensaje. Ir incrementando el puntero para ir enviando cada caracter consecutivamente hatsa que termine (usar length)
    
    uint8_t flags; //para avisar que ya se termino de enviar el mensaje y 
                   //el receptor puede enviarle un mensaje en respuesta al emisor en base a ese flag, por ejemplo para "avisar" que se recibio el mensaje

    uint16_t length; // para la longitud del mensaje      
             
} Message; 


/**
 * @brief Allocates memory for the message data and copies the provided data into the message structure.
 * 
 * This function allocates a block of memory to store the data for a given message and copies the
 * provided data into this allocated space. The length of the data is recorded in the message structure.
 * If the memory allocation fails, the message's data remains unmodified.
 *
 * @param msg Pointer to a `Message` structure that will hold the data.
 * @param data Pointer to the data that needs to be copied into the message structure.
 * @param length The size of the data to be copied, in bytes.
 *
 * @note Ensure that the `msg` pointer is not `NULL` and that `length` is greater than 0. The function 
 *       does not free any previously allocated memory for `msg->data`.
 */
 void setMessageData(Message* msg, void* data, uint16_t length); 




 /**
 * @brief Sets the message type in the provided message structure.
 * 
 * This function assigns the given message type to the `type` field of the specified 
 * message structure.
 *
 * @param msg Pointer to a `Message` structure where the message type will be set.
 * @param message_type The type of the message, represented as an unsigned 8-bit integer.
 *
 * @note Ensure that the `msg` pointer is not `NULL` before calling this function.
 */
void setMessageType(Message* msg, uint8_t message_type);




/**
 * @brief Sets or updates the flags in the provided message structure.
 * 
 * This function applies the specified flags to the `flags` field of the given message structure 
 * using a bitwise OR operation. This allows multiple flags to be set without overwriting the existing ones.
 *
 * @param msg Pointer to a `Message` structure where the flags will be set or updated.
 * @param flags The flags to be applied, represented as an unsigned 8-bit integer.
 *
 * @note Ensure that the `msg` pointer is not `NULL` before calling this function.
 */
void setMessageFlag(Message* msg, uint8_t flags);