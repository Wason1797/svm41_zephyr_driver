#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include "sensor/svm41/svm41.h"

#if !DT_HAS_COMPAT_STATUS_OKAY(sensirion_svm41)
#error "No sensirion,svm41 compatible node found in the device tree"
#endif

int main(void)
{

        const struct device *svm41 = DEVICE_DT_GET_ANY(sensirion_svm41);

        if (!device_is_ready(svm41))
        {
                printk("Device Ready!");
                return 1;
        }
        struct sensor_value humidity, temperature, voc_index, nox_index;

        while (true)
        {
                sensor_sample_fetch(svm41);
                sensor_channel_get(svm41, SENSOR_CHAN_HUMIDITY, &humidity);
                printk("Humidity from sensor %f\n", sensor_value_to_double(&humidity));
                sensor_channel_get(svm41, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
                printk("Temperature from sensor %f\n", sensor_value_to_double(&temperature));
                sensor_channel_get(svm41, SENSOR_CHAN_VOC_INDEX, &voc_index);
                printk("VOC Index from sensor %f\n", sensor_value_to_double(&voc_index));
                sensor_channel_get(svm41, SENSOR_CHAN_NOX_INDEX, &nox_index);
                printk("NOX Index from sensor %f\n", sensor_value_to_double(&nox_index));
                k_sleep(K_MSEC(10000));
        }
        return 0;
}

// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/i2c.h>
// #include <string.h>
// #include <stdlib.h>
// #include <math.h>
// #include <zephyr/sys/byteorder.h>

// #define I2C_ACCEL_WRITE_ADDR 0x32
// #define I2C_ACCEL_READ_ADDR 0x75

// struct device *i2c_accel;
// uint8_t WhoAmI = 0u;

// #define I2C_NODE DT_NODELABEL(i2c0) //DT_N_S_soc_S_i2c_40003000

// static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

// void main(void)
// {
//         k_sleep(K_MSEC(500));

//         printk("Starting i2c scanner...\n");
//         if (!i2c_dev)
//         {
//                 printk("I2C: Device driver not found.\n");
//                 return;
//         }

//         uint8_t error = 0u;

//         i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));

//         printk("Value of NRF_TWI0->PSEL.SCL: %ld \n", NRF_TWI0->PSEL.SCL);
//         printk("Value of NRF_TWI0->PSEL.SDA: %ld \n", NRF_TWI0->PSEL.SDA);
//         printk("Value of NRF_TWI0->FREQUENCY: %ld \n", NRF_TWI0->FREQUENCY);
//         printk("26738688 -> 100k\n");

//         for (uint8_t i = 4; i <= 0x77; i++)
//         {
//                 struct i2c_msg msgs[1];
//                 uint8_t dst = 1;

//                 /* Send the address to read from */
//                 msgs[0].buf = &dst;
//                 msgs[0].len = 1U;
//                 msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

//                 error = i2c_transfer(i2c_dev, &msgs[0], 1, i);
//                 if (error == 0)
//                 {
//                         printk("0x%2x FOUND\n", i);
//                         uint8_t tx_buf[2] = {0, 0};

//                         sys_put_be16(0x104, tx_buf);
//                         msgs[0].buf = &tx_buf;
//                         msgs[0].len = 2U;
//                         error = i2c_transfer(i2c_dev, &msgs[0], 1, i);

//                         k_sleep(K_MSEC(100));

//                         printk("error: %d", error);

//                         sys_put_be16(0x10, tx_buf);
//                         msgs[0].buf = &tx_buf;
//                         msgs[0].len = 2U;
//                         error = i2c_transfer(i2c_dev, &msgs[0], 1, i);

//                         printk("error: %d", error);
//                 }
//                 else
//                 {
//                         // printk("error %d \n", error);
//                 }
//         }
// }