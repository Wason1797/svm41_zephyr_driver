#define DT_DRV_COMPAT sensirion_svm41

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/crc.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

#include "svm41.h"

LOG_MODULE_REGISTER(SVM41, CONFIG_SENSOR_LOG_LEVEL);

static uint8_t svm41_compute_crc(uint16_t value)
{
	uint8_t buf[2];

	sys_put_be16(value, buf);

	return crc8(buf, 2, SVM41_CRC_POLY, SVM41_CRC_INIT, false);
}

static int svm41_write_command(const struct device *dev, uint16_t cmd)
{

	const struct svm41_config *cfg = dev->config;
	uint8_t tx_buf[2] = {0, 0};

	sys_put_be16(cmd, tx_buf);

	return i2c_write_dt(&cfg->bus, tx_buf, sizeof(tx_buf));
}

static int svm41_start_measurement(const struct device *dev)
{
	int result = svm41_write_command(dev, SVM41_START_MEASUREMENT);
	if (result == 0)
	{
		k_sleep(K_MSEC(100));
		return result;
	}
	return result;
}

static int svm41_stop_measurement(const struct device *dev)
{

	int result = svm41_write_command(dev, SVM41_STOP_MEASUREMENT);
	if (result == 0)
	{
		k_sleep(K_MSEC(100));
		return result;
	}
	return result;
}

static int svm41_sample_fetch(const struct device *dev, enum sensor_channel chan)
{

	struct svm41_data *data = dev->data;
	const struct svm41_config *cfg = dev->config;

	uint8_t rx_buff[12];

	int result = svm41_write_command(dev, SVM41_GET_SIGNALS);

	if (result != 0)
	{
		LOG_ERR("Error when sending get_signals cmd");
		return -1;
	}

	k_sleep(K_MSEC(1));

	result = i2c_read_dt(&cfg->bus, rx_buff, sizeof(rx_buff));

	if (result < 0)
	{
		LOG_ERR("Error while reading sample");
	}
	uint8_t crc = rx_buff[2];
	data->humidity = sys_get_be16(rx_buff);

	// if (crc != svm41_compute_crc(data->humidity))
	// {
	// 	LOG_ERR("CRC8 for humidity does not match");
	// }

	crc = rx_buff[5];
	uint8_t t_buff[] = {rx_buff[3], rx_buff[4]};
	data->temperature = sys_get_be16(t_buff);

	// if (crc != svm41_compute_crc(data->temperature))
	// {
	// 	LOG_ERR("CRC8 for temperature does not match");
	// }

	crc = rx_buff[8];
	uint8_t nox_buff[] = {rx_buff[6], rx_buff[7]};
	data->nox_index = sys_get_be16(nox_buff);

	// if (crc != svm41_compute_crc(data->nox_index))
	// {
	// 	LOG_ERR("CRC8 for nox_index does not match");
	// }

	crc = rx_buff[11];
	uint8_t voc_buff[] = {rx_buff[9], rx_buff[10]};
	data->voc_index = sys_get_be16(voc_buff);

	// if (crc != svm41_compute_crc(data->voc_index))
	// {
	// 	LOG_ERR("CRC8 for voc_index does not match");
	// }

	return 0;
}

static int svm41_channel_get(const struct device *dev,
							 enum sensor_channel chan,
							 struct sensor_value *val)
{

	const struct svm41_data *data = dev->data;

	if (chan == SENSOR_CHAN_HUMIDITY)
	{
		sensor_value_from_float(val, (float)((int)data->humidity) / 100);
	}

	if (chan == SENSOR_CHAN_AMBIENT_TEMP)
	{
		sensor_value_from_float(val, (float)((int)data->temperature) / 200);
	}

	if (chan == SENSOR_CHAN_VOC_INDEX)
	{
		sensor_value_from_float(val, (float)((int)data->voc_index) / 10);
	}

	if (chan == SENSOR_CHAN_NOX_INDEX)
	{
		sensor_value_from_float(val, (float)((int)data->nox_index) / 10);
	}

	return 0;
}

#if defined(CONFIG_PM_DEVICE)
static int svm41_pm_action(const struct device *dev,
						   enum pm_device_action action)
{

	return -ENOTSUP;
}
#endif /* CONFIG_PM_DEVICE */

static int svm41_init(const struct device *dev)
{
	k_sleep(K_MSEC(10));
	int result = svm41_stop_measurement(dev);
	result = svm41_start_measurement(dev);
	if (result != 0)
	{
		LOG_ERR("Error in start_measurement");
		return result;
	}
	return 0;
}

static const struct sensor_driver_api svm41_api = {
	.sample_fetch = svm41_sample_fetch,
	.channel_get = svm41_channel_get,
};

#define SVM41_INIT(n)                                     \
	static struct svm41_data svm41_data_##n;              \
                                                          \
	static const struct svm41_config svm41_config_##n = { \
		.bus = I2C_DT_SPEC_INST_GET(n),                   \
		.model = DT_INST_ENUM_IDX(n, model)};             \
                                                          \
	PM_DEVICE_DT_INST_DEFINE(n, svm41_pm_action);         \
                                                          \
	DEVICE_DT_INST_DEFINE(n,                              \
						  svm41_init,                     \
						  NULL,                           \
						  &svm41_data_##n,                \
						  &svm41_config_##n,              \
						  POST_KERNEL,                    \
						  CONFIG_SENSOR_INIT_PRIORITY,    \
						  &svm41_api);

DT_INST_FOREACH_STATUS_OKAY(SVM41_INIT)