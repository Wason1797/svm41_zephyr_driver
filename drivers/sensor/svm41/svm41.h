
#ifndef ZEPHYR_DRIVERS_SENSOR_SVM41_H_
#define ZEPHYR_DRIVERS_SENSOR_SVM41_H_

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>

#define SVM41_CRC_POLY 0x31
#define SVM41_CRC_INIT 0xFF

#define SVM41_START_MEASUREMENT 0x0010
// #define SVM41_GET_SIGNALS 0x03D2
#define SVM41_GET_SIGNALS 0x0405
#define SVM41_STOP_MEASUREMENT 0x0104

enum svm41_model
{
	SVM41_MODEL_SVM41,
};

enum svm41_channel
{
	/** AQI_CHANNEL */
	SENSOR_CHAN_VOC_INDEX = SENSOR_CHAN_PRIV_START + 1,
	SENSOR_CHAN_NOX_INDEX,
};

struct svm41_config
{
	struct i2c_dt_spec bus;
	enum svm41_model model;
};

struct svm41_data
{
	int16_t humidity;
	int16_t temperature;
	int16_t voc_index;
	int16_t nox_index;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_SVM41_H_ */