/*  vfd_demo.c  – RC2014 VFD demo for HLOAD BASIC  */
#include <stdint.h>

/*
   Write an immediate 8-bit value to a Z80 I/O port.
   Usage: VFD_CMD(port_number, value);
   Expands to an inline-asm block:
       ld   a,value
       out  (port),a
*/
#define VFD_CMD(port,value)     \
    __asm                      \
        ld   a,value           \
        out  (port),a          \
    __endasm

/* Crude busy-wait for delay */
static void delay(uint16_t loops) {
    while (loops--) __asm nop __endasm;
}

/* Initialise VFD registers (BASIC lines 5–40 & 8000–8700) */
static void vfd_init(void) {
    VFD_CMD(1,0x00);
    VFD_CMD(1,0x01);
    VFD_CMD(0,0x3F);

    VFD_CMD(1,0x00);
    VFD_CMD(1,0x01);
    VFD_CMD(0,0x0F);

    VFD_CMD(1,0x00);
    VFD_CMD(1,0x01);
    VFD_CMD(0,0x01);

    VFD_CMD(1,0x00);
    VFD_CMD(1,0x01);
    VFD_CMD(0,0x06);
}

/* Send one data byte to VFD */
static void vfd_put(uint8_t b) {
    VFD_CMD(1,0x05);
    VFD_CMD(1,0x04);
    VFD_CMD(0,b);
}

/* Send an array of bytes */
static void vfd_block(const uint8_t *p, uint8_t len) {
    while (len--) vfd_put(*p++);
}

/* Main loop */
void main(void) {
    /* 40-character message */
    static const uint8_t msg1[] =
        "  COLEMANZ80"   /* 12 chars */
        "          "     /* +28 spaces */
        "          "     /* pad */
        "          ";    /* total 40 */

    static const uint8_t msg2[] = " MICRO COMPUTER ";
    static const uint8_t msg3[] = " 8-BIT FOREVER! ";

    for (;;) {
        vfd_init();                vfd_block(msg1, sizeof msg1 - 1); delay(200);
        vfd_block(msg2, sizeof msg2 - 1);                     delay(4000);
        vfd_init();                vfd_block(msg3, sizeof msg3 - 1); delay(4000);
    }
}
