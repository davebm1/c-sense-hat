/*
 *  C code to read pressure and temperature from the
 *  Raspberry Pi Sense HAT add-on board.
 *  
 *  sudo raspi-config --> advanced options --> enable i2c
 *
 *  sudo apt-get install libi2c-dev i2c-tools
 *
 *  Then build with:
 *
 *       gcc -Wall pressure.c -o pressure
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_ID 0x5c
#define DEV_PATH "/dev/i2c-1"
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define PRESS_OUT_XL 0x28
#define PRESS_OUT_L 0x29
#define PRESS_OUT_H 0x2A
#define TEMP_OUT_L 0x2B
#define TEMP_OUT_H 0x2C

int main(void)
{
    int fd = 0 ;
    uint8_t temp_out_l = 0, temp_out_h = 0 ;
    int16_t temp_out = 0 ;
    double t_c = 0.0 ;

    uint8_t press_out_xl = 0 ;
    uint8_t press_out_l = 0 ;
    uint8_t press_out_h = 0 ;

    int32_t press_out = 0;
    double pressure = 0.0 ;

    uint8_t status = 0 ;

    /* open i2c comms */
    if ((fd = open (DEV_PATH, O_RDWR)) < 0)
    {
      perror("Unable to open i2c device");
      exit (1) ;
    }

    /* configure i2c slave */
    if (ioctl (fd, I2C_SLAVE, DEV_ID) < 0)
    {
      perror("Unable to configure i2c slave device");
      close (fd) ;
      exit (1) ;
    }

    /* check we are who we should be */
    if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBD) {
	printf("%s\n", "who_am_i error") ;
	close (fd) ;
	exit (1) ;
    }

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data (fd, CTRL_REG1, 0x00) ;

    /* Turn on the pressure sensor analog front end in single shot mode  */
    i2c_smbus_write_byte_data (fd, CTRL_REG1, 0x84) ;

    /* Run one-shot measurement (temperature and pressure), the set bit will be reset by the
     * sensor itself after execution (self-clearing bit) */
    i2c_smbus_write_byte_data (fd, CTRL_REG2, 0x01) ;

    /* Wait until the measurement is complete */
    do {
      usleep (25000) ; /* 25 milliseconds) */
      status = i2c_smbus_read_byte_data (fd, CTRL_REG2) ;
    } while (status != 0) ;

    /* Read the temperature measurement (2 bytes to read) */
    temp_out_l = i2c_smbus_read_byte_data (fd, TEMP_OUT_L) ;
    temp_out_h = i2c_smbus_read_byte_data (fd, TEMP_OUT_H) ;

    /* Read the pressure measurement (3 bytes to read) */
    press_out_xl = i2c_smbus_read_byte_data (fd, PRESS_OUT_XL) ;
    press_out_l = i2c_smbus_read_byte_data (fd, PRESS_OUT_L) ;
    press_out_h = i2c_smbus_read_byte_data (fd, PRESS_OUT_H) ;

    /* make 16 and 24 bit values (using bit shift) */
    temp_out = temp_out_h << 8 | temp_out_l ;
    press_out = press_out_h << 16 | press_out_l << 8 |  press_out_xl ;

    /* calculate output values */
    t_c = 42.5 + (temp_out / 480.0) ;
    pressure = press_out / 4096.0 ; 

    /*output */
    printf ("Temp (from press) = %.2f°C\n", t_c) ;
    printf ("Pressure = %.0f hPa\n", pressure) ;

    /* Power down the device */
    i2c_smbus_write_byte_data (fd, CTRL_REG1, 0x00) ;

    close (fd) ;

    return(0) ;
}
