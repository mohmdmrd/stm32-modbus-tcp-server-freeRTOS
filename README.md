# STM32 Modbus TCP Server (FreeRTOS + LwIP)

A lightweight and efficient **Modbus TCP Server** library for **STM32H7 microcontrollers**, built on top of **FreeRTOS** and **LwIP**. This library is designed for embedded systems that require fast and reliable Modbus communication over Ethernet in real-time environments.

---

## 🚀 Features

- 📡 Supports **Modbus TCP Protocol** (based on standard Modbus Spec)
- 🧠 Designed for **FreeRTOS**-based applications
- 🌐 Uses **LwIP** (RAW or netconn API)
- 🧰 Written in **C** for performance and portability
- 🛠 Fully customizable and hardware-independent
- 🔌 Handles multiple simultaneous connections (configurable)
- 📦 Lightweight memory footprint (suitable for embedded systems)

How to use libary:
- Enable Ethernet and set pins.
- Enable freeRTOS
- Enable LWIP and set a desire IP
- Set MEM_SIZE to 10*1024 Byte
- Enable Ethernet interrupt and set priority to last number (in this exmple 15)
- if use cortex m7 must configuration MPU unit. do it like exmaple. 0x30000000 is Ethernet rx and tx dma buffer and 0x30004000 is LWIP_RAM_HEAP_POINTER
- if use MPU and use another SRAM (like example) must add add address section in STM32xx_FLASH.id
- before MX_LWIP_Inut(); always use delay for prevent some bugs.(in exmaple uses 1s delay in main function)
- dont use osdelay.
- in FreeRTOSConfig.h delete "#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}" and write this "#define configASSERT( x ) if ((x) == 0) {}" for prevet some error.
- insert tcpserver_modbus.h and tcpserver_modbus.c in your project
- at the end initial tcpserver modbus in main file (like exmaple)
