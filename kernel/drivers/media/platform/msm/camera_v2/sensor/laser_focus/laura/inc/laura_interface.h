/*
*
*	Author:	Jheng-Siou, Cai
*	Time:	2015-08
*
*/

#ifndef __LINUX_SHOW_LAURA_SENSOR_INTERFACE_H
#define __LINUX_SHOW_LAURA_SENSOR_INTERFACE_H

#include "msm_laser_focus.h"
#include "laura_shipping_func.h"
#include "laura_factory_func.h"
#include "laura_i2c.h"

/* Laura tof configure size */
#define LAURA_CONFIG_SIZE 6

/* Laura configuration control */
enum laura_configuration_ctrl {
        LAURA_READ_CONFIG,
        LAURA_CALIBRATION_10_CONFIG, /* Calibration 10 cm */
        LAURA_CALIBRATION_40_CONFIG, /* Calibration 40 cm */
        LAURA_CALIBRATION_INF_CONFIG, /* Calibration infinity */
};

/* Laura calibration interface */
int Laura_device_clibration_interface(struct msm_laser_focus_ctrl_t *dev_t, bool load_cal, bool *calibration_flag, int ctrl);
/* Laura get calibration input data interface */
int Laura_get_calibration_input_data_interface(struct seq_file *buf, void *v);
/* Laura read range interface */
int Laura_device_read_range_interface(struct msm_laser_focus_ctrl_t *dev_t, bool load_cal, bool *calibration_flag);
/* Laura power up initialization interface (verify firmware) */
int Laura_device_power_up_init_interface(struct msm_laser_focus_ctrl_t *dev_t, bool load_cal, bool *cal_flag, bool do_measure);
/* Laura power up initialization interface (non-verify firmware) */
int Laura_device_wake_up_interface(struct msm_laser_focus_ctrl_t *dev_t, bool load_cal, bool *cal_flag, bool do_measure);
/* Laura configuration interface */
int Laura_device_tof_configuration_interface(struct msm_laser_focus_ctrl_t *dev_t, int ctrl);

#endif
