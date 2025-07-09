/*
 * tcpserver.h
 *
 *  Created on: Apr 20, 2022
 *      Author: controllerstech
 */

#ifndef INC_TCPSERVER_MODBUS_H_
#define INC_TCPSERVER_MODBUS_H_

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "string.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "queue.h"

typedef struct
{

    uint16_t *u16reg;     /*!< memory buffer */
    uint16_t u16regsize;  //buffer size
    uint16_t u16Port;
    uint8_t  u8slaveID;
    uint16_t Timeout;
}
modbus_t;

typedef enum MB_FC
{
    MB_FC_READ_REGISTERS           = 3,	 /*!< FCT=3 -> read registers or analog outputs */
    MB_FC_READ_INPUT_REGISTER      = 4,	 /*!< FCT=4 -> read analog inputs */
    MB_FC_WRITE_REGISTER           = 6,	 /*!< FCT=6 -> write single register */
    MB_FC_WRITE_MULTIPLE_REGISTERS = 16,	 /*!< FCT=16 -> write multiple registers */
}mb_functioncode_t;

typedef enum ERR_OP_LIST
{
    // Errors
	ERR_Illegal_Function =1,
	ERR_Illegal_Data_Address =2,
//	ERR_Illegal_Data_Value =3,
//	ERR_Slave_Device_Failure =4,
	ERR_Slave_Device_Busy	 =6,
//	ERR_Memory_Parity_Error =8,
	ERR_WRONG_ANWSER=16,//slave send invalid answer
}ERR_OP_LIST_t;
void tcpserver_init (modbus_t* usermodbus);


#endif /* INC_TCPSERVER_MODBUS_H_ */
