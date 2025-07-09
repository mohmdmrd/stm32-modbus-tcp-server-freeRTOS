/*
 * tcpserver.c
 *
 *  Rebuilt on: Jul 4, 2025
 *  Author: optimized by ChatGPT
 */

#include <tcpserver_modbus.h>

/////////////////////////// Variables
static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;
char msgc[100];
char smsgc[100];
modbus_t modbus;
uint16_t length_send = 0;
struct netconn *clientconn = NULL;
////////////////////////// Prototypes
static void copyString(char* dest, char* src, unsigned num);
static void modbus_parse(struct netconn *client);
static void readInputRegs(void);
static void eror_handler(uint8_t Exception_code);
static void writeInputReg(void);
static void writeInputMultiRegs(void);



//////////////////////////Function


//a task for init server and wait for massage
static void tcp_thread(void *arg)
{

    err_t err, accept_err;

    conn = netconn_new(NETCONN_TCP);
    if (conn != NULL)
    {
        err = netconn_bind(conn, IP_ADDR_ANY, modbus.u16Port);
        if (err == ERR_OK)
        {
            netconn_listen(conn);

            while (1)
            {

                accept_err = netconn_accept(conn, &clientconn);
                if (accept_err == ERR_OK && clientconn != NULL)
                {

                    while (netconn_recv(clientconn, &buf) == ERR_OK)
                    {
                        addr = netbuf_fromaddr(buf);
                        port = netbuf_fromport(buf);

                        do
                        {
                        	memset (msgc, '\0', sizeof(smsgc));  // clear the buffer
                        	memset (smsgc, '\0', sizeof(smsgc));  // clear the buffer

                            copyString(msgc, (char *)buf->p->payload, buf->p->len > sizeof(msgc) ? sizeof(msgc) : buf->p->len);

                            modbus_parse(clientconn);
                        }
                        while (netbuf_next(buf) > 0);

                        netbuf_delete(buf);
                    }

                    netconn_close(clientconn);
                    netconn_delete(clientconn);
                }
            }
        }
        else
        {
            netconn_delete(conn);
            conn = NULL;
        }
    }
}



//parse data from master(client)
static void modbus_parse(struct netconn *client)
{
    if (msgc[2] == 0 && msgc[3] == 0 && msgc[6] == modbus.u8slaveID)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            smsgc[i] = msgc[i];
        }

        switch (msgc[7])
        {
            case MB_FC_READ_INPUT_REGISTER:
            case MB_FC_READ_REGISTERS:
                readInputRegs();
                break;

            case MB_FC_WRITE_REGISTER:
                writeInputReg();
                break;

            case MB_FC_WRITE_MULTIPLE_REGISTERS:
                writeInputMultiRegs();
                break;

            default:
                eror_handler(ERR_Illegal_Function);
                break;
        }

        netconn_write(client, smsgc, length_send, NETCONN_COPY);
    }
}


//make tcp task
void tcpserver_init(modbus_t* usermodbus)
{
    modbus = *usermodbus;
    sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}


static void copyString(char* dest, char* src, unsigned num)
{
    for (unsigned i = 0; i < num; ++i)
    {
        dest[i] = src[i];
    }
}

//0x03 and 0x04 Function code
static void readInputRegs()
{
    uint16_t count = (msgc[10] << 8) + msgc[11];
    uint16_t start_address = (msgc[8] << 8) + msgc[9];

    if (start_address + count > modbus.u16regsize)
    {
        eror_handler(ERR_Illegal_Data_Address);
        return;
    }

    smsgc[4] = ((3 + count * 2) >> 8);
    smsgc[5] = ((3 + count * 2) & 0xFF);
    smsgc[6] = modbus.u8slaveID;
    smsgc[7] = msgc[7];
    smsgc[8] = count * 2;

    for (uint8_t i = 0; i < count; i++)
    {
        smsgc[9 + i * 2] = modbus.u16reg[start_address + i] >> 8;
        smsgc[10 + i * 2] = modbus.u16reg[start_address + i] & 0xFF;
    }

    length_send = 9 + smsgc[8];
}

//error response
static void eror_handler(uint8_t Exception_code)
{
    smsgc[4] = 0;
    smsgc[5] = 3;
    smsgc[6] = modbus.u8slaveID;
    smsgc[7] = 0x80 + msgc[7];
    smsgc[8] = Exception_code;
    length_send = 9;
}


//0x06 Function code
static void writeInputReg()
{
    uint16_t start_address = (msgc[8] << 8) + msgc[9];
    if (start_address >= modbus.u16regsize)
    {
        eror_handler(ERR_Illegal_Data_Address);
        return;
    }
    modbus.u16reg[start_address] = (msgc[10] << 8) + msgc[11];
    copyString(smsgc, msgc, 12);
    length_send = 12;
}


//0x10 Function code
static void writeInputMultiRegs()
{
    uint16_t start_address = (msgc[8] << 8) + msgc[9];
    uint16_t count = (msgc[10] << 8) + msgc[11];

    if (start_address + count > modbus.u16regsize)
    {
        eror_handler(ERR_Illegal_Data_Address);
        return;
    }

    for (uint8_t i = 0; i < count; i++)
    {
        modbus.u16reg[start_address + i] = (msgc[13 + i * 2] << 8) + msgc[14 + i * 2];
    }

    smsgc[4] = 0;
    smsgc[5] = 6;
    smsgc[6] = modbus.u8slaveID;
    smsgc[7] = msgc[7];
    smsgc[8] = msgc[8];
    smsgc[9] = msgc[9];
    smsgc[10] = msgc[10];
    smsgc[11] = msgc[11];

    length_send = 12;
}
