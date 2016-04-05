# Raspberry Pi Sense-HAT add-on board

### C language demonstration code


pressure.c  : Get pressure and temperature readings from the LPS25H pressure sensor.


humidity.c  : Get relative humidity and temperature readings from the HTS221 humidity sensor.

              This is a more involved procedure than for the pressure sensor, above. Each HTS221 sensor is
              individually calibrated and the results are stored in designated registers. These calibration
              registers must be read and the straight line equations for calibration (y = mx + c) solved.
	      The measured ambient values are then converted to give the output humidity and temperature,
	      by reference to these linear equation solutions.


led_matrix.c  : Demonstrate control of the LED matrix using the mmap method to map the led device into memory.
