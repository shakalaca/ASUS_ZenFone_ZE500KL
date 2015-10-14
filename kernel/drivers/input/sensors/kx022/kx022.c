/*
 * Copyright (C) 2012 Kionix, Inc.
 * Written by Chris Hudson <chudson@kionix.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/input/kionix_gsensor.h>
#include <linux/input-polldev.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/proc_fs.h>
#include "ASUS_Gsensor.h"
#include "sysfs/Gsensor_sysfs.h"
#include "property/Gsensor_property.h"
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

int build_version = 0;
bool bypass_for_eng_mode = true ;

/************************************************
 * Use to open/close the debugmessage
 *************************************************/
int KX022_DEBUG_MESSAGE = 0;
int KX022_REG_MESSAGE = 0;
int KX022_CALIBRATED_MESSAGE = 0;

#define KX022_RESUME_DISABLE		0
#define KX022_RESUME_ENABLE		1
#define KX022_RESUME_MISSDISABLE	2
#define KX022_RESUME_MISSENABLE	3

static int Gsensor_status = 0;

static struct workqueue_struct *Gsensor_data_ready_workqueue;
static struct work_struct Gsensor_data_ready_isr_work;

/******
 * 	The following table lists the maximum appropriate poll interval for each available output data rate.
 *	Default Setting :
 *	  kxtj9_odr_table[] = {
 *		{ 1,		ODR1600F },
 *		{ 3,		ODR800F },
 *		{ 5,		ODR400F },
 *		{ 10,	ODR200F },
 *		{ 20,	ODR100F },
 *		{ 40,	ODR50F  },
 *		{ 80,	ODR25F  },
 *		{ 0xFFFFFFFF,	ODR12_5F},
 *	//	{ 0xFFFFFFFF,		ODR3_125F,	KXTJ9_RES_8BIT},				// 320,	251~max	NO POLL
 *	};
 */

struct kionix_odr_table {
	unsigned int cutoff;
	u8 mask;
	int RES;
};
static const struct kionix_odr_table kx022_odr_table[] = {
																/*  ms,	range,	mode */
	{ 15,	 			ODR200F,		KX022_RES_16BIT},			/*  10,	0~ 10	FASTEST MODE , full power mode */
	{ 35, 			ODR100F,		KX022_RES_16BIT},			/*  20,	20~66	GAME MODE */
	{ 70, 			ODR50F,			KX022_RES_16BIT},			/*  66,	100~200	UI MODE */
	{ 250,			ODR25F,			KX022_RES_16BIT},			/*  100,	1000~	NORMAL MODE */
	{ 0xFFFFFFFF,		ODR12_5F,		KX022_RES_16BIT},			/*  1000,	max		SLOW MODE */
};

/*************************************************
 * 	kionix kernel driver data struture
 *************************************************/
struct ASUS_Gsensor_data	{
	struct i2c_client		*client;
	struct input_dev		*input_dev;
	struct mutex			lock;				/* For muxtex lock */
	unsigned int			last_poll_interval;
	atomic_t		enabled;
	u8		ctrl_reg1;
	u8		data_ctrl;
	u8		int_ctrl;
	int		suspend_resume_state;
	int		resume_enable;
	int		irq_status;
	int		irq;
	int		event_irq_status;
	int		event_irq;

	int		tap_state;
	int		hands_flip_state;
	int		flick_state;

#ifdef CONFIG_INPUT_KXTJ9_POLLED_MODE
	struct input_polled_dev *poll_dev;
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND	
	struct early_suspend gsensor_early_suspendresume;
#endif
// for enable motion detect , added by cheng_kao 2014.02.12 ++
	u8 wufe_rate;
	u8 wufe_timer;
	u8 wufe_thres;
// for enable motion detect , added by cheng_kao 2014.02.12 --
// added by cheng_kao 2013.06.01  for sensors calibration ++
	int accel_cal_data[6];
	int accel_cal_offset[3];
	int accel_cal_sensitivity[3];
// added by cheng_kao 2013.06.01  for sensors calibration --
// for enable motion detect , added by cheng_kao 2014.02.12 ++
	int motion_detect_threshold_x;
	int motion_detect_threshold_y;
	int motion_detect_threshold_z;
	int motion_detect_timer;
	int chip_interrupt_mode;
// for enable motion detect , added by cheng_kao 2014.02.12 --
	struct calidata asus_gsensor_cali_data;
	int x_gain_pos_data;
	int y_gain_pos_data;
	int z_gain_pos_data;
	int x_gain_neg_data;
	int y_gain_neg_data;
	int z_gain_neg_data;
	int data_report_count;
};

static struct ASUS_Gsensor_data *kionix_Gsensor_data;

static int kx022_into_drdy_function(void)
{
	int result = 0;
	// turn off power
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (result < 0){
		printk("[Gsensor] alp : drdy motion_detect_power_off_fail !\n");
		return result;
	}

	// turn on power
	kionix_Gsensor_data->ctrl_reg1 |= DRDYE;
	kionix_Gsensor_data->ctrl_reg1 &= WUFE_OFF;
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : kx022_into_drdy_function (%d)\n", kionix_Gsensor_data->ctrl_reg1);
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1); // enable motion detect
	if (result < 0){
		printk("[Gsensor] alp : kx022_into_drdy_function : into fail !\n");
		return result;
	}
	mdelay(50); // 50 ms

	return result;
}
/*
static int kx022_into_motion_detect_function(void)
{
	int result = 0;
	// turn off power
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (result < 0){
		printk("[Gsensor] alp : motion_detect_power_off_fail !\n");
		return result;
	}

	// set the chip timer
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, WAKEUP_TIMER, kionix_Gsensor_data->wufe_timer );
	if (result < 0){
		printk("[Gsensor] alp : set_timer_fail !\n");
		return result;
	}

	// set the chip threshold
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, WAKEUP_THRES, kionix_Gsensor_data->wufe_thres);
	if (result < 0){
		printk("[Gsensor] alp : set_chip_threshold_fail !\n");
		return result;
	}

	// set the chip wufe rate
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG2, kionix_Gsensor_data->wufe_rate);
	if (result < 0){
		printk("[Gsensor] alp : set_chip_wufe_rate_fail !\n");
		return result;
	}

	// turn on power
	kionix_Gsensor_data->ctrl_reg1 |= WUFE_ON;
	kionix_Gsensor_data->ctrl_reg1 &= DRDYE_OFF;
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : kx022_into_motion_detect_function (%d)\n", kionix_Gsensor_data->ctrl_reg1);
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1); // enable motion detect
	if (result < 0){
		printk("[Gsensor] alp : kx022_into_motion_detect_function : into fail !\n");
		return result;
	}
	mdelay(50); // 50 ms

	return result;
}
*/

static int kx022_reset_function(struct i2c_client *client)
{
	int result = 0;
	printk("[Gsensor] alp : kx022_reset_function!!!!!!\n");
	result = i2c_smbus_write_byte_data(client, CTRL_REG2, 0x84); // SRST set to 1
	if (result < 0){
		printk("[Gsensor] alp : kx022_reset_function : reset fail (%d)\n",result);
		result = 1;
		return result;
	}
	mdelay(50); // 50 ms

	/* ensure that PC1 is cleared before updating control registers */
	result = i2c_smbus_write_byte_data(client, CTRL_REG1, 0);
	if (result < 0){
		result = 3;
		return result;
	}
	/* only write INT_CTRL_REG1 if in irq mode */
	if (kionix_Gsensor_data->irq) {
		printk("[Gsensor] kx022_reset_function : Old INT_CTRL1(0x%x)\n", kionix_Gsensor_data->int_ctrl);
		result = i2c_smbus_write_byte_data(client,INT_CTRL1, kionix_Gsensor_data->int_ctrl);
		result = i2c_smbus_write_byte_data(client,INT_CTRL4, KX022_DRDYI1);
		if (result < 0){
			result = 4;
			return result;
		}
	}

	/* turn on outputs */
	kionix_Gsensor_data->ctrl_reg1 |= (PC1_ON | RES_16bit | DRDYE | GRP4_G_4G);
	result = i2c_smbus_write_byte_data(client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
	if (result < 0){
		result = 5;
		return result;
	}

	/* clear initial interrupt if in irq mode */
	if (kionix_Gsensor_data->irq) {
		result = i2c_smbus_read_byte_data(client, INT_REL);
		if (result < 0){
			result = 6;
			return result;
		}
	}
	atomic_set(&kionix_Gsensor_data->enabled, KX022_ACC_ENABLE);
		
	return result;
}

static int kx022_i2c_read(u8 addr, u8 *data, int len)
{
	struct i2c_msg msgs[] = {
		{
			.addr = kionix_Gsensor_data->client->addr,
			.flags = kionix_Gsensor_data->client->flags,
			.len = 1,
			.buf = &addr,
		},
		{
			.addr = kionix_Gsensor_data->client->addr,
			.flags = kionix_Gsensor_data->client->flags | I2C_M_RD,
			.len = len,
			.buf = data,
		},
	};

	return i2c_transfer(kionix_Gsensor_data->client->adapter, msgs, 2);
}

static void kx022_report_acceleration_data(void)
{
	unsigned char acc_data[6];
	int rawdata_x=0,rawdata_y=0,rawdata_z=0,err=0, ireset=0;
	int reportdata_x=0,reportdata_y=0,reportdata_z=0;
	err = kx022_i2c_read(XOUT_L, (u8 *)acc_data, 6);
	if (err < 0){
		dev_err(&kionix_Gsensor_data->client->dev, "accelerometer data read failed\n");
		return;
	}
	rawdata_x = (acc_data[0] | (acc_data[1]<<8));
	rawdata_y = (acc_data[2] | (acc_data[3]<<8));
	rawdata_z = (acc_data[4] | (acc_data[5]<<8));

	if( (rawdata_x==0)&&(rawdata_y==0)&&(rawdata_z==0) ){
		printk("[Gsensor] alp : the zero rawdata!!!\n");
		ireset = KX022_RESET_FOR_ZERO_RAWDATA;
	}

	if(ireset!=KX022_VALUE_FOR_NOT_NEED_RESET){
		err=kx022_reset_function(kionix_Gsensor_data->client);
		printk("[Gsensor] alp :  kx022_reset_function : %d\n",err);		
	}

	if(rawdata_x > 16383)
		rawdata_x = rawdata_x-65536;
	if(rawdata_y > 16383)
		rawdata_y = rawdata_y-65536;
	if(rawdata_z > 16383)
		rawdata_z = rawdata_z-65536;
	
	// transfromed by chip location
	switch(g_ilocation){
		case KX022_CHIP_LOCATION_SR_ZC500KL:
			if ((kionix_Gsensor_data->accel_cal_sensitivity[0] == 0) || (kionix_Gsensor_data->accel_cal_sensitivity[1] == 0) || (kionix_Gsensor_data->accel_cal_sensitivity[2] == 0)) {
				reportdata_x = rawdata_y*(-1);
				reportdata_y = rawdata_x;
				reportdata_z = rawdata_z;
				if (KX022_CALIBRATED_MESSAGE) {
					printk("[Gsensor] alp : LOCATION_ZC500KL default\n");
				}
			} else	{
				if (kionix_Gsensor_data->asus_gsensor_cali_data.version == 3)	{
					if (g_ASUS_hwID >= ZE500KL_SR1)	{
						reportdata_x = ((-1)*rawdata_y - kionix_Gsensor_data->asus_gsensor_cali_data.x_offset);

						reportdata_y = (rawdata_x - kionix_Gsensor_data->asus_gsensor_cali_data.y_offset);

						reportdata_z = (rawdata_z - kionix_Gsensor_data->asus_gsensor_cali_data.z_offset);
					} else	{
						reportdata_x = 16384*((-1)*rawdata_y - kionix_Gsensor_data->asus_gsensor_cali_data.x_offset)
											/kionix_Gsensor_data->x_gain_pos_data;

						reportdata_y = 16384*(rawdata_x - kionix_Gsensor_data->asus_gsensor_cali_data.y_offset)
											/kionix_Gsensor_data->y_gain_pos_data;

						reportdata_z = 16384*(rawdata_z - kionix_Gsensor_data->asus_gsensor_cali_data.z_offset)
											/kionix_Gsensor_data->z_gain_pos_data;
					}
				} else	{
					if (rawdata_y > 0)
						reportdata_x = GSENSOR_MAX_RESOLUTION*((-1)*rawdata_y - kionix_Gsensor_data->asus_gsensor_cali_data.x_offset)
										/kionix_Gsensor_data->x_gain_pos_data;
					else
						reportdata_x = GSENSOR_MAX_RESOLUTION*((-1)*rawdata_y - kionix_Gsensor_data->asus_gsensor_cali_data.x_offset)
										/kionix_Gsensor_data->x_gain_neg_data;
					
					if (rawdata_x > 0)
						reportdata_y = GSENSOR_MAX_RESOLUTION*(rawdata_x - kionix_Gsensor_data->asus_gsensor_cali_data.y_offset)
										/kionix_Gsensor_data->y_gain_pos_data;
					else
						reportdata_y = GSENSOR_MAX_RESOLUTION*(rawdata_x - kionix_Gsensor_data->asus_gsensor_cali_data.y_offset)
										/kionix_Gsensor_data->y_gain_neg_data;
					
					if (rawdata_z > 0)
						reportdata_z = GSENSOR_MAX_RESOLUTION*(rawdata_z - kionix_Gsensor_data->asus_gsensor_cali_data.z_offset)
										/kionix_Gsensor_data->z_gain_pos_data;
					else
						reportdata_z = GSENSOR_MAX_RESOLUTION*(rawdata_z - kionix_Gsensor_data->asus_gsensor_cali_data.z_offset)
										/kionix_Gsensor_data->z_gain_neg_data;

				}
				if (KX022_CALIBRATED_MESSAGE || (kionix_Gsensor_data->data_report_count)%2000== 0) {
					printk("[Gsensor] ZC500KL raw    data : (%d), (%d), (%d)\n",rawdata_x, rawdata_y, rawdata_z); 
					printk("[Gsensor] ZC500KL report data : (%d), (%d), (%d)\n",reportdata_x, reportdata_y, reportdata_z);
					kionix_Gsensor_data->data_report_count = 0;
				}
			}
		break;
	}

	kionix_Gsensor_data->data_report_count++;
	kionix_Gsensor_data->accel_cal_data[0] = reportdata_x;
	kionix_Gsensor_data->accel_cal_data[1] = reportdata_y;
	kionix_Gsensor_data->accel_cal_data[2] = reportdata_z;
	input_report_abs(kionix_Gsensor_data->input_dev, ABS_X, reportdata_x);
	input_report_abs(kionix_Gsensor_data->input_dev, ABS_Y, reportdata_y);
	input_report_abs(kionix_Gsensor_data->input_dev, ABS_Z, reportdata_z);
	input_sync(kionix_Gsensor_data->input_dev);
}

static void kx022_asseleration_ist(struct work_struct *work)
{
	int err;

	mutex_lock(&kionix_Gsensor_data->lock);
	/* data ready is the only possible interrupt type for interrupt1*/
	kx022_report_acceleration_data();
	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
	if (err < 0)
		printk("[Gsensor] alp : kx022_asseleration_ist err(%x)!!\n", err);
	mutex_unlock(&kionix_Gsensor_data->lock);
}

static irqreturn_t kx022_isr(int irq, void *dev)
{
	queue_work(Gsensor_data_ready_workqueue, &Gsensor_data_ready_isr_work);
	return IRQ_HANDLED;
}
//Added by Eason: event report function
static void report_gesture_event(int event_type)
{
	switch(event_type){
		case FLICK_TWICE: 
			printk("[Gsensor] Gesture FLICK_TWICE event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);			
		break;
		case HANDS_UP: 
			printk("[Gsensor] Gesture HANDS_UP event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);			
		break;
		case HANDS_DOWN: 
			printk("[Gsensor] Gesture HANDS_DOWN event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);
		break;
		case FACE_UP: 
			printk("[Gsensor] Gesture FACE_UP event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);			
		break;
		case FACE_DOWN: 
			printk("[Gsensor] Gesture FACE_DOWN event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);
		break;
		case DOUBLE_TAP: 
			printk("[Gsensor] Gesture DOUBLE_TAP event : (%d) !!!\n", event_type);
			input_report_abs(kionix_Gsensor_data->input_dev, ABS_THROTTLE, event_type);
			input_sync(kionix_Gsensor_data->input_dev);			
		break;

		default:
			printk("[Gsensor] Gesture event : error input !!!\n");
		break;

	}
	return;
}

//Added by Eason
int wupredir=0;
int check_loop=0;
int YNWU_flag=0, YPWU_flag=0, ZNWU_flag=0, ZPWU_flag=0;
#define FLICK_CHECK 4

static int kx022_gesture_check(int event_source)
{
	//check tap event
	u8 doutap_event=0x08;
	u8 flip_event=0x01; //also for HANDS UP/HANDS DOWN
	u8 wuf_event=0x02;//WUFS
	int tiltcurrpos = 0, tiltprepos=0; 
	int wucurrdir = 0;
	int event_report = 0;

	if (doutap_event == event_source){
		//ToDo for double tap event
		printk("[Gsensor] KIONIX Gesture - double tap gesture triggered\n");
		printk("[Gsensor] KIONIX Gesture - Double tap gesture confirmed\n");	
		event_report = DOUBLE_TAP;
		report_gesture_event(event_report);
		
	}
	else if (flip_event == event_source){
		//ToDo for flip event
		//check face up or face down from reading Tilt Position Registers
		//printk("[Gsensor] 2222 alp : kx022_isr flip/HANDs gesture triggered \n");
		tiltcurrpos = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, TSCP);
		tiltprepos = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, TSPP);
		//printk("[Gsensor] 2222 alp : kx022_isr flip - current pos(%x) previous pos(%x)\n",tiltcurrpos,tiltprepos);
		if (((0x20 == tiltprepos) && (0x10 == tiltcurrpos)) || ((0x01 == tiltprepos) && (0x10 == tiltcurrpos)))
		{
			printk("[Gsensor] KIONIX Gesture - HANDs Up gesture confirmed \n");
			event_report = HANDS_UP;
			report_gesture_event(event_report);
		}	
		else if (((0x10 == tiltprepos) && (0x20 == tiltcurrpos))||((0x01 == tiltprepos) && (0x20 == tiltcurrpos)))
		{
			printk("[Gsensor] KIONIX Gesture - HANDs Down gesture confirmed \n");
			event_report = HANDS_DOWN;
			report_gesture_event(event_report);
		}	
		else if ((0x02 == tiltprepos) && (0x01 == tiltcurrpos))
		{
			printk("[Gsensor] KIONIX Gesture - Face Up gesture confirmed \n");
			event_report = FACE_UP;
			report_gesture_event(event_report);
		}	
		else if ((0x01 == tiltprepos) && (0x02 == tiltcurrpos))
		{
			printk("[Gsensor] KIONIX Gesture - Face Down gesture confirmed \n");
			event_report = FACE_DOWN;
			report_gesture_event(event_report);
		}	
	}
	else if (wuf_event == event_source){
		//ToDo for double tap event
		printk("[Gsensor] KIONIX Gesture - Flick gesture triggered\n");
		//ToDo for flick twice event
		//printk("[Gsensor] KIONIX Gesture - Flick gesture triggered\n");
		wucurrdir = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INS3);
		printk("[Gsensor] 2222 alp : kx022_isr Flick - current axis(%x) previous axis(%x)\n",wucurrdir, wupredir);
		//ToDo - state machine
		wupredir = wucurrdir;
		printk("[Gsensor] 2222 alp : kx022_isr Flick - loop(%x) current axis(%x) previous axis(%x)\n",check_loop,wucurrdir, wupredir);
		//ToDo - state machine for verifing flick twice
		//wupredir = wucurrdir;
		//check_loop++;
		if (check_loop < 4){
			if ((wupredir & 0x08) || (wucurrdir & 0x08)){
				//Y- triggered
				printk("[Gsensor] KIONIX Gesture - Flick Y- event ++\n");
				YNWU_flag++;
				if ((ZNWU_flag != 0) || (ZPWU_flag != 0))
				{//clear other direction flag and loop numbers
					ZNWU_flag = ZPWU_flag = 0;
					check_loop = 0;
				}
				if ((YNWU_flag+YPWU_flag) >= FLICK_CHECK){
					printk("[Gsensor] KIONIX Gesture - Flick twice Y-(Right) confirmed\n");
					event_report = FLICK_TWICE;
					report_gesture_event(event_report);
					YNWU_flag = YPWU_flag = 0;
					check_loop=0;
					wupredir = 0;
				}
				else{//still counting
					wupredir = wucurrdir;//setup predir before leaving
					check_loop++;
				}
			
			}
			else if ((wupredir & 0x04) || (wucurrdir & 0x04)){
				//Y+ triggered
				printk("[Gsensor] KIONIX Gesture - Flick Y+ event ++\n");
				YPWU_flag++;
				if ((ZNWU_flag != 0) || (ZPWU_flag != 0))
				{//clear other direction flag and loop numbers
					ZNWU_flag = ZPWU_flag = 0;
					check_loop = 0;
				}
				if ((YPWU_flag+YNWU_flag) >= FLICK_CHECK){
					printk("[Gsensor] KIONIX Gesture - Flick twice Y+(Left) confirmed\n");
					event_report = FLICK_TWICE;
					report_gesture_event(event_report);

					YPWU_flag = YNWU_flag = 0;
					check_loop=0;
					wupredir = 0;
				}
				else{//still counting
					wupredir = wucurrdir;//setup predir before leaving
					check_loop++;
				}				
			}
			else if ((wupredir & 0x02) || (wucurrdir & 0x02)){
				//Z- triggered
				printk("[Gsensor] KIONIX Gesture - Flick Z- event ++\n");
				ZNWU_flag++;
				if ((YPWU_flag != 0) || (YNWU_flag != 0))
				{//clear other direction flag and loop numbers
					YPWU_flag = YNWU_flag = 0;
					check_loop = 0;
				}
				if ((ZNWU_flag+ZPWU_flag) >= FLICK_CHECK){
					printk("[Gsensor] KIONIX Gesture - Flick twice Z-(Down) confirmed\n");
					event_report = FLICK_TWICE;
					report_gesture_event(event_report);
					
					ZNWU_flag = ZPWU_flag =0;
					check_loop=0;
					wupredir = 0;
				}
				else{//still counting
					wupredir = wucurrdir;//setup predir before leaving
					check_loop++;
				}			
			}
			else if ((wupredir & 0x01) || (wucurrdir & 0x01)){
				//Z+ triggered
				printk("[Gsensor] KIONIX Gesture - Flick Z+ event ++\n");
				ZPWU_flag++;
				if ((YPWU_flag != 0) || (YNWU_flag != 0))
				{//clear other direction flag and loop numbers
					YPWU_flag = YNWU_flag = 0;
					check_loop = 0;
				}
				if ((ZPWU_flag+ZNWU_flag) >= FLICK_CHECK){
					printk("[Gsensor] KIONIX Gesture - Flick twice Z+(Up) confirmed\n");
					event_report = FLICK_TWICE;
					report_gesture_event(event_report);
					
					ZPWU_flag = ZNWU_flag = 0;
					check_loop=0;
					wupredir = 0;
				}
				else{//still counting
					wupredir = wucurrdir;//setup predir before leaving
					check_loop++;
				}
			}
		}
		else{//clear all variables
			check_loop=0;
			YNWU_flag = 0;
			YPWU_flag = 0;
			ZNWU_flag = 0;
			ZPWU_flag = 0;
			wupredir = 0;
		}
	}
	return 0;

}

static irqreturn_t kx022_event_isr(int irq, void *dev)
{
	int err;
	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INS2);
	if (err < 0){
		printk("[Gsensor] alp : kx022_isr read INS2 err(%x)\n",err);
	}
	printk("[Gsensor] ++++ kx022_isr recognize gesture triggered (%x)\n",err);
	if ( err != INS2_DRDY){//go to check which gesture triggered
		kx022_gesture_check(err);
	}
	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
	if (err < 0){
		printk("[Gsensor] alp : kx022_isr err(%x)\n",err);
	}

	return IRQ_HANDLED;
}

static int kx022_update_odr(unsigned int poll_interval)
{
	int err;
	int i;

	/* Use the lowest ODR that can support the requested poll interval */
	for (i = 0; i < ARRAY_SIZE(kx022_odr_table); i++) {
		if (poll_interval < kx022_odr_table[i].cutoff)
			break;
	}
	kionix_Gsensor_data->data_ctrl = kx022_odr_table[i].mask;
	kionix_Gsensor_data->data_ctrl |= LPRO_9;
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : kx022_update_odr  i(%d), cutoff(%d), mask(%d), poll(%d)\n", i, kx022_odr_table[i].cutoff, kx022_odr_table[i].mask, poll_interval);

	/* backup CTRL_REG1 value before clear */
	kionix_Gsensor_data->ctrl_reg1 = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, CTRL_REG1);
	printk("[Gsensor] alp : data_ctrl(0x%x), ctrl_reg1(0x%x)\n", kionix_Gsensor_data->data_ctrl, kionix_Gsensor_data->ctrl_reg1);

	printk("[Gsensor] alp : Old INT_CTRL1(0x%x)\n", kionix_Gsensor_data->int_ctrl);
	kionix_Gsensor_data->int_ctrl = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_CTRL1);
	printk("[Gsensor] alp : New INT_CTRL1(0x%x)\n", kionix_Gsensor_data->int_ctrl);

	kionix_Gsensor_data->ctrl_reg1 |= (PC1_ON | RES_16bit | DRDYE | GRP4_G_4G);

	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (err < 0)
		return err;

	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, DATA_CTRL, kionix_Gsensor_data->data_ctrl);
	if (err < 0)
		return err;

	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
	if (err < 0)
		return err;

	return 0;
}

/*
static int kx022_device_power_on(void)
{
	if (kionix_Gsensor_data->pdata.power_on)
		return kionix_Gsensor_data->pdata.power_on();
	return 0;
}
*/

static void check_gsensor_calibtarion_data (void)
{	
	if (kionix_Gsensor_data->asus_gsensor_cali_data.version == 3)	{
		printk("[Gsensor] Calibration by version 03 (%d)\n", kionix_Gsensor_data->asus_gsensor_cali_data.version);
		if (g_ASUS_hwID >= ZE500KL_SR1)	{
			kionix_Gsensor_data->asus_gsensor_cali_data.x_offset = kionix_Gsensor_data->asus_gsensor_cali_data.x_offset*2;
			kionix_Gsensor_data->asus_gsensor_cali_data.y_offset = kionix_Gsensor_data->asus_gsensor_cali_data.y_offset*2;
			kionix_Gsensor_data->asus_gsensor_cali_data.z_offset = 
				kionix_Gsensor_data->asus_gsensor_cali_data.z_max - GSENSOR_MAX_RESOLUTION;
		}	
		kionix_Gsensor_data->x_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.x_max - kionix_Gsensor_data->asus_gsensor_cali_data.x_min;
		kionix_Gsensor_data->y_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.y_max - kionix_Gsensor_data->asus_gsensor_cali_data.y_min;
		kionix_Gsensor_data->z_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.z_max - kionix_Gsensor_data->asus_gsensor_cali_data.z_min;

		kionix_Gsensor_data->x_gain_neg_data = kionix_Gsensor_data->x_gain_pos_data;
		kionix_Gsensor_data->y_gain_neg_data = kionix_Gsensor_data->y_gain_pos_data;
		kionix_Gsensor_data->z_gain_neg_data = kionix_Gsensor_data->z_gain_pos_data;
	} else	{
		printk("[Gsensor] Calibration by Old version (%d)\n", kionix_Gsensor_data->asus_gsensor_cali_data.version);
		kionix_Gsensor_data->x_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.x_max - kionix_Gsensor_data->asus_gsensor_cali_data.x_offset;
		kionix_Gsensor_data->y_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.y_max - kionix_Gsensor_data->asus_gsensor_cali_data.y_offset;
		kionix_Gsensor_data->z_gain_pos_data = 
			kionix_Gsensor_data->asus_gsensor_cali_data.z_max - kionix_Gsensor_data->asus_gsensor_cali_data.z_offset;

		kionix_Gsensor_data->x_gain_neg_data =
			kionix_Gsensor_data->asus_gsensor_cali_data.x_offset- kionix_Gsensor_data->asus_gsensor_cali_data.x_min;
		kionix_Gsensor_data->y_gain_neg_data =
			kionix_Gsensor_data->asus_gsensor_cali_data.y_offset- kionix_Gsensor_data->asus_gsensor_cali_data.y_min;
		kionix_Gsensor_data->z_gain_neg_data =
			kionix_Gsensor_data->asus_gsensor_cali_data.z_offset- kionix_Gsensor_data->asus_gsensor_cali_data.z_min;
		if (kionix_Gsensor_data->x_gain_pos_data == 0)
			kionix_Gsensor_data->x_gain_pos_data = GSENSOR_MAX_RESOLUTION;
		if (kionix_Gsensor_data->y_gain_pos_data == 0)
			kionix_Gsensor_data->y_gain_pos_data = GSENSOR_MAX_RESOLUTION;
		if (kionix_Gsensor_data->z_gain_pos_data == 0)
			kionix_Gsensor_data->z_gain_pos_data = GSENSOR_MAX_RESOLUTION;
		if (kionix_Gsensor_data->x_gain_neg_data == 0)
			kionix_Gsensor_data->x_gain_neg_data = GSENSOR_MAX_RESOLUTION;
		if (kionix_Gsensor_data->y_gain_neg_data == 0)
			kionix_Gsensor_data->y_gain_neg_data = GSENSOR_MAX_RESOLUTION;
		if (kionix_Gsensor_data->z_gain_neg_data == 0)
			kionix_Gsensor_data->z_gain_neg_data = GSENSOR_MAX_RESOLUTION;
	}
	printk("[Gsensor] Get offset from cali data: %d %d %d \n",
					kionix_Gsensor_data->asus_gsensor_cali_data.x_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data.y_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data.z_offset);
	
/*
	printk("[Gsensor] offset =%d %d %d positive gain = %f %f %f\n",
					kionix_Gsensor_data->asus_gsensor_cali_data->x_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->y_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->z_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->x_gain_pos,
					kionix_Gsensor_data->asus_gsensor_cali_data->y_gain_pos,
					kionix_Gsensor_data->asus_gsensor_cali_data->z_gain_pos);
	printk("[Gsensor] offset =%d %d %d negative gain = %f %f %f\n",
					kionix_Gsensor_data->asus_gsensor_cali_data->x_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->y_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->z_offset,
					kionix_Gsensor_data->asus_gsensor_cali_data->x_gain_neg,
					kionix_Gsensor_data->asus_gsensor_cali_data->y_gain_neg,
					kionix_Gsensor_data->asus_gsensor_cali_data->z_gain_neg);
*/
}

static void kx022_device_power_off(void)
{
	int err;
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : kx022_device_power_off ++\n");

	kionix_Gsensor_data->ctrl_reg1 = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, CTRL_REG1);
	printk("[Gsensor] alp : kx022_device_power_off ctrl_reg1(0x%X)\n", kionix_Gsensor_data->ctrl_reg1);

	kionix_Gsensor_data->ctrl_reg1 &= PC1_OFF;
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
	if (err < 0)
		dev_err(&kionix_Gsensor_data->client->dev, "soft power off failed\n");

		printk("[Gsensor] alp : kx022_device_power_off --\n");
}

static int kx022_enable(void)
{
	int err;
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp:kx022_enable ++\n");

	if (kionix_Gsensor_data->suspend_resume_state == 1)	{
		printk("[Gsensor] alp : kx022_enable  already suspend return !\n");
		kionix_Gsensor_data->resume_enable = KX022_RESUME_MISSENABLE;
		return 0;
	}

	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_ACC_ENABLE)	{
		printk("[Gsensor] alp : kx022_enable  already enable , return 0\n");
		return 0;
	}
	
	if (kionix_Gsensor_data->irq_status == 0)	{
		enable_irq(kionix_Gsensor_data->irq);
		printk("[Gsensor] alp : kx022_enable  irq (%d)\n", kionix_Gsensor_data->irq);
		kionix_Gsensor_data->irq_status = 1;
	} else
		printk("[Gsensor] alp : kx022_enable  irq already been enabled (irq_status : %d)\n", kionix_Gsensor_data->irq_status);

	/* backup CTRL_REG1 value before clear */
	kionix_Gsensor_data->ctrl_reg1 = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, CTRL_REG1);
	printk("[Gsensor] alp : kx022_enable ctrl_reg1(0x%X)\n", kionix_Gsensor_data->ctrl_reg1);

	/* ensure that PC1 is cleared before updating control registers */
	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (err < 0)
		return err;

	/* only write INT_CTRL_REG1 if in irq mode */
	if (kionix_Gsensor_data->irq) {
		err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, INT_CTRL1, kionix_Gsensor_data->int_ctrl);
		err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, INT_CTRL4, KX022_DRDYI1);
		if (err < 0){
			printk("[Gsensor] alp : kx022_enable fail 1 irq = %d\n", kionix_Gsensor_data->irq);
			return err;
		}
		if (KX022_DEBUG_MESSAGE)
			printk("[Gsensor] alp : kx022_enable  irq = %d(0x%x)\n", kionix_Gsensor_data->irq, kionix_Gsensor_data->int_ctrl);
	}

	/* turn on outputs */
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : turn on ++\n");
	kionix_Gsensor_data->ctrl_reg1 |= (PC1_ON | RES_16bit | DRDYE | GRP4_G_4G);
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : turn on -- (0x%X)\n", kionix_Gsensor_data->ctrl_reg1);
	if (err < 0)	{
		printk("[Gsensor] alp : turn on fail -- (%d)\n", err);
		return err;
	}

	err = kx022_update_odr(kionix_Gsensor_data->last_poll_interval);
	if (err < 0)
		return err;

	/* clear initial interrupt if in irq mode */
	if (kionix_Gsensor_data->irq) {
		err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, STATUS_REG);
		if(KX022_DEBUG_MESSAGE)
			printk("[Gsensor]  STATUS_REG(%x)\n", err);
		err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
		if (err < 0) {
			if (KX022_DEBUG_MESSAGE)
				printk("[Gsensor] alp : error clearing interrupt: %d\n", err)	;
			goto fail;
		}
	}

	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_DEVICE_DISABLE)	{
		printk("[Gsensor] alp : kx022 trun on device by acc function(%d) \n", atomic_read(&kionix_Gsensor_data->enabled));
		atomic_set(&kionix_Gsensor_data->enabled, KX022_ACC_ENABLE);
	} else	{
		printk("[Gsensor] alp : kx022 already enable by ori function(%d) \n", atomic_read(&kionix_Gsensor_data->enabled));
		atomic_set(&kionix_Gsensor_data->enabled, KX022_BOTH_ENABLE);
	}

	return 0;

fail:
	kx022_device_power_off();
	return err;
}

static void kx022_disable(void)
{
	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_DEVICE_DISABLE)	{
		printk("[Gsensor] alp : kx022_disable  already disable , return !!\n");
		return ;
	} else if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_ORI_ENABLE)	{
		printk("[Gsensor] kx022_disable_orientation Warning!Only ORI function working , return !!\n");
		return ;
	}

	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_BOTH_ENABLE)	{
		printk("[Gsensor] alp : kx022_disable  ORI function still working , return !!\n");
		atomic_set(&kionix_Gsensor_data->enabled, KX022_ORI_ENABLE);
		return ;
	} else	{
		if (kionix_Gsensor_data->irq_status==1)	{
			disable_irq(kionix_Gsensor_data->irq);
			kionix_Gsensor_data->irq_status = 0;
		}

		kx022_device_power_off();
		atomic_set(&kionix_Gsensor_data->enabled, KX022_DEVICE_DISABLE);
	}
}

static int kx022_enable_by_orientation(void)
{
	int err;
	if(KX022_DEBUG_MESSAGE) printk("[Gsensor] kx022_enable_by_orientation ++\n");

	if (kionix_Gsensor_data->irq_status == 0)	{
		enable_irq(kionix_Gsensor_data->irq);
		printk("[Gsensor] kx022_enable_by_orientation  irq (%d)\n", kionix_Gsensor_data->irq);
		kionix_Gsensor_data->irq_status = 1;
	} else
		printk("[Gsensor] kx022_enable_by_orientation  irq already been enabled (irq_status : %d)\n", kionix_Gsensor_data->irq_status);

	/* backup CTRL_REG1 value before clear */
	kionix_Gsensor_data->ctrl_reg1 = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, CTRL_REG1);
	printk("[Gsensor] alp : kx022_enable_by_orientation ctrl_reg1(0x%X)\n", kionix_Gsensor_data->ctrl_reg1);
	
	/* ensure that PC1 is cleared before updating control registers */
	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (err < 0)
		return err;

	/* only write INT_CTRL_REG1 if in irq mode */
	if (kionix_Gsensor_data->irq) {
		err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, INT_CTRL1, kionix_Gsensor_data->int_ctrl);
		err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, INT_CTRL4, KX022_DRDYI1);
		if (err < 0){
			if(KX022_DEBUG_MESSAGE) printk("[Gsensor] alp : kx022_enable fail 1 irq = %d\n", kionix_Gsensor_data->irq);
			return err;
		}
		if(KX022_DEBUG_MESSAGE) printk("[Gsensor] alp : kx022_enable  irq = %d\n", kionix_Gsensor_data->irq);
	}

	/* turn on outputs */
	kionix_Gsensor_data->ctrl_reg1 |= (PC1_ON | RES_16bit | DRDYE | GRP4_G_4G);
	err = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
	if (err < 0)
		return err;

	err = kx022_update_odr(ODR_GAME_MODE);
	if (err < 0)
		return err;

	/* clear initial interrupt if in irq mode */
	if (kionix_Gsensor_data->irq) {
		err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
		if (err < 0) {
			if(KX022_DEBUG_MESSAGE) printk("[Gsensor] alp : error clearing interrupt: %d\n",err)	;
			goto fail;
		}
	}

	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_DEVICE_DISABLE)	{
		printk("[Gsensor] alp : kx022 trun on device by ori function(%d) \n", atomic_read(&kionix_Gsensor_data->enabled));
		atomic_set(&kionix_Gsensor_data->enabled, KX022_ORI_ENABLE);
	} else	{
		printk("[Gsensor] alp : kx022 already enable by acc function(%d) \n", atomic_read(&kionix_Gsensor_data->enabled));
		atomic_set(&kionix_Gsensor_data->enabled, KX022_BOTH_ENABLE);
	}
	return 0;

fail:
	kx022_device_power_off();
	return err;
}

static void kx022_disable_orientation(void)
{
	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_DEVICE_DISABLE)	{
		printk("[Gsensor] kx022_disable_orientation  already disable , return !!\n");
		return ;
	} else if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_ACC_ENABLE)	{
		printk("[Gsensor] kx022_disable_orientation Warning!Only ACC function working , return !!\n");
		return ;
	}

	if (atomic_read(&kionix_Gsensor_data->enabled) == KX022_BOTH_ENABLE)	{
		printk("[Gsensor] kx022_disable_orientation  ACC function still working , return !!\n");
		atomic_set(&kionix_Gsensor_data->enabled, KX022_ACC_ENABLE);
		return ;
	} else	{
		if (kionix_Gsensor_data->irq_status == 1)	{
			disable_irq(kionix_Gsensor_data->irq);
			kionix_Gsensor_data->irq_status = 0;
		}

		kx022_device_power_off();
		atomic_set(&kionix_Gsensor_data->enabled, KX022_DEVICE_DISABLE);
	}
}

static int kx022_setup_input_device(void)
{
	int err;
	printk("[Gsensor] alp :  kx022_init_input_device ++\n");

	kionix_Gsensor_data->input_dev = input_allocate_device();
	if (!kionix_Gsensor_data->input_dev) {
		dev_err(&kionix_Gsensor_data->client->dev, "input device allocate failed\n");
		return -ENOMEM;
	}

	set_bit(EV_ABS, kionix_Gsensor_data->input_dev->evbit);
	set_bit(EV_SYN, kionix_Gsensor_data->input_dev->evbit);
	set_bit(EV_SW, kionix_Gsensor_data->input_dev->evbit);
	input_set_abs_params(kionix_Gsensor_data->input_dev, ABS_X, -G_MAX, G_MAX, FUZZ, FLAT);
	input_set_abs_params(kionix_Gsensor_data->input_dev, ABS_Y, -G_MAX, G_MAX, FUZZ, FLAT);
	input_set_abs_params(kionix_Gsensor_data->input_dev, ABS_Z, -G_MAX, G_MAX, FUZZ, FLAT);
	input_set_abs_params(kionix_Gsensor_data->input_dev, ABS_THROTTLE, -G_MAX, G_MAX, FUZZ, FLAT);

	kionix_Gsensor_data->input_dev->name = "kx022_accel";
	kionix_Gsensor_data->input_dev->id.bustype = BUS_I2C;
	kionix_Gsensor_data->input_dev->dev.parent = &kionix_Gsensor_data->client->dev;

	err = input_register_device(kionix_Gsensor_data->input_dev);
	if (err) {
		dev_err(&kionix_Gsensor_data->client->dev,
			"unable to register input polled device %s: %d\n",
			kionix_Gsensor_data->input_dev->name, err);
		input_free_device(kionix_Gsensor_data->input_dev);
		return err;
	}
	printk("[Gsensor] alp :  kx022_init_input_device --\n");

	return 0;
}

/*
 * When IRQ mode is selected, we need to provide an interface to allow the user
 * to change the output data rate of the part.  For consistency, we are using
 * the set_poll method, which accepts a poll interval in milliseconds, and then
 * calls update_odr() while passing this value as an argument.  In IRQ mode, the
 * data outputs will not be read AT the requested poll interval, rather, the
 * lowest ODR that can support the requested interval.  The client application
 * will be responsible for retrieving data from the input node at the desired
 * interval.
 */
ssize_t gsensor_chip_id_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int retval=0;

	retval = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, WHO_AM_I);

	printk("[Gsensor] alp : whoami = %d\n", retval);

	if(retval == 0x09)
		printk("[Gsensor] alp : use kxtj2!!\n");
	else if(retval == 0x08)
		printk("[Gsensor] alp : use kxtj9!!\n");
	else if(retval < 0)
		printk("[Gsensor] alp : read fail!!\n");

	return sprintf(buf, "%d\n", retval);
}

ssize_t gsensor_status_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int err=0;

	err = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, WHO_AM_I);
	if (err < 0)
		Gsensor_status = 0;
	else
		Gsensor_status = 1;

	return sprintf(buf, "%d\n", Gsensor_status);
}

ssize_t gsensor_dump_reg(struct device *dev, struct device_attribute *attr, char *buf)	
{
	int i = 0, val = 0;

	for(i = 0; i <0x3b; i++)
	{
		val = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, i);
		printk("[Gsensor] cmd =%02x value = %02x\n", i, val);
	}
	val = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, 0x6A);
	printk("[Gsensor] cmd = 6A value = %02x\n", val);
	
	return sprintf(buf, "%d\n", val);
}

ssize_t gsensor_read_raw(struct device *dev, struct device_attribute *attr, char *buf)
{
	int retval = 0,rawdata_x=0,rawdata_y=0,rawdata_z=0 ;
	unsigned char acc_data[6];
	static bool user_enable_sensor = false;

	mutex_lock(&kionix_Gsensor_data->lock);
	retval = atomic_read(&kionix_Gsensor_data->enabled);

	printk("[Gsensor] alp : get_rawdata enable state=(%d)\n",retval);
	if(retval==0){
		retval = kx022_enable();
		mdelay(100);
		if (retval < 0){
			printk("[Gsensor] ATTR power on fail\n");
			return retval;
		}
		user_enable_sensor = true;
	}

	retval = kx022_i2c_read(XOUT_L, (u8 *)acc_data, 6);
	if (retval < 0){
		dev_err(&kionix_Gsensor_data->client->dev, "accelerometer data read failed\n");
		return sprintf(buf, "get no data\n");
	}

	/* ZE500KL Project */
	rawdata_x = (acc_data[2] | (acc_data[3]<<8));
	rawdata_y = (acc_data[0] | (acc_data[1]<<8));
	rawdata_z = (acc_data[4] | (acc_data[5]<<8));

	if (rawdata_x > 16383)
		rawdata_x = rawdata_x - 65536;
	if (rawdata_y > 16383)
		rawdata_y = rawdata_y - 65536;
	if (rawdata_z > 16383)
		rawdata_z = rawdata_z - 65536;

	rawdata_x = rawdata_x*(-1);

	if (user_enable_sensor == true)
	{
		kx022_disable();
		user_enable_sensor = false;
	}
	mutex_unlock(&kionix_Gsensor_data->lock);

	return sprintf(buf, "%d %d %d\n", rawdata_x, rawdata_y, rawdata_z);
}

ssize_t gsensor_r_en_mt(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = (bypass_for_eng_mode == true) ? (0) : (1);

	return sprintf(buf, "en(%d)  bypass(%d) \n", ret, bypass_for_eng_mode);
}

ssize_t gsensor_w_en_mt(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val = simple_strtoul(buf, NULL, 10);

	if (val == 1) { /* enable mt => disable bypass mode */
		bypass_for_eng_mode = false;
	} else {
		bypass_for_eng_mode = true;
		kx022_into_drdy_function();
	}
	printk(KERN_INFO "[KXTJ9] en(%d)  bypass(%d) \n", val, bypass_for_eng_mode);

	return count;
}

ssize_t gsensor_show_message(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk("[Gsensor] alp : kx022_message DEBUG(%d), REG(%d), CALI(%d)\n",
			KX022_DEBUG_MESSAGE, KX022_REG_MESSAGE, KX022_CALIBRATED_MESSAGE);
	return sprintf(buf, "\n[Gsensor] alp : kx022_message DEBUG(%d), REG(%d), CALI(%d)\n",
			KX022_DEBUG_MESSAGE, KX022_REG_MESSAGE, KX022_CALIBRATED_MESSAGE);
}

ssize_t gsensor_set_message(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val = simple_strtoul(buf, NULL, 10);
	switch(val){
		case 0:
			KX022_DEBUG_MESSAGE = 0;
			KX022_REG_MESSAGE = 0;
			KX022_CALIBRATED_MESSAGE = 0;
			printk("[Gsensor] alp : disable all message !!!\n");
		break;

		case 1:
			KX022_DEBUG_MESSAGE = 1;
			KX022_REG_MESSAGE = 1;
			printk("[Gsensor] alp : enable all message without CALIBRATED_MESSAG !!!\n");
		break;

		case 2:
			KX022_REG_MESSAGE = 1;
			printk("[Gsensor] alp : enable  REG_MESSAGE !!!\n");
		break;

		case 3:
			KX022_CALIBRATED_MESSAGE = 1;
			printk("[Gsensor] alp : enable  CALIBRATED_MESSAG !!!\n");
		break;

		case 4:
			KX022_DEBUG_MESSAGE = 1;
			printk("[Gsensor] alp : enable  DEBUG_MESSAGE !!!\n");
		break;

		default:
			printk("[Gsensor] alp : error input !!!\n");
		break;
	}
	return count;
}

/* Returns currently selected poll interval (in ms) */
ssize_t gsensor_get_poll(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	int gsensor_ctrl = 0;
	gsensor_ctrl = i2c_smbus_read_byte_data(client, DATA_CTRL);
	printk("[Gsensor] alp : gsensor_get_poll (%d) (%d)\n", kionix_Gsensor_data->last_poll_interval, gsensor_ctrl);
	return sprintf(buf, "%d , %d\n", kionix_Gsensor_data->last_poll_interval, gsensor_ctrl);
}

/* Allow users to select a new poll interval (in ms) */
ssize_t gsensor_set_poll(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int interval = 0;

	// Lock the device to prevent races with open/close (and itself)
	mutex_lock(&kionix_Gsensor_data->lock);
	interval = simple_strtoul(buf, NULL, 10);
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : interval (%u)\n", interval);
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : gsensor_set_poll buf (%s)\n", buf);
	if (KX022_DEBUG_MESSAGE)
		printk("[Gsensor] alp : last_poll_interval (%u), New_poll(%d)\n", kionix_Gsensor_data->last_poll_interval, interval);

	 //Set current interval to the greater of the minimum interval or the requested interval
	kx022_update_odr(interval);
	kionix_Gsensor_data->last_poll_interval = interval;
	mutex_unlock(&kionix_Gsensor_data->lock);

	return count;
}

ssize_t gsensor_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk("[Gsensor] alp : kx022_enable_show (%d)\n", atomic_read(&kionix_Gsensor_data->enabled));
	return sprintf(buf, "%d\n", atomic_read(&kionix_Gsensor_data->enabled));
}

ssize_t gsensor_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val = 0;
	mutex_lock(&kionix_Gsensor_data->lock);
	val = simple_strtoul(buf, NULL, 10);
	switch(val)	{
		case 0:
			printk("[Gsensor] alp : kx022_disable_by_acceleation !!!\n");
			kx022_disable();
			break;
		case 1:
			printk("[Gsensor] alp : kx022_enable_by_acceleation !!!\n");
			kionix_Gsensor_data->data_report_count = 0;
			Gsensor_sysfs_read_calibration_data(&kionix_Gsensor_data->asus_gsensor_cali_data);
			check_gsensor_calibtarion_data();
			kx022_enable();
			break;
		case 2:
			printk("[Gsensor] alp : kx022_disable_by_orientation !!!\n");
			kx022_disable_orientation();
			break;
		case 3:
			printk("[Gsensor] alp : kx022_enable_by_orientation !!!\n");
			kionix_Gsensor_data->data_report_count = 0;
			Gsensor_sysfs_read_calibration_data(&kionix_Gsensor_data->asus_gsensor_cali_data);
			check_gsensor_calibtarion_data();
			kx022_enable_by_orientation();
			break;
	}
	mutex_unlock(&kionix_Gsensor_data->lock);
	return count;
}

ssize_t get_gsensor_state(struct device *dev, struct device_attribute *devattr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	int who_am_i_data = 0 , ret=0;
	who_am_i_data = i2c_smbus_read_byte_data(client, WHO_AM_I);
	printk("[Gsensor] get_Gsensor_state wia : %d\n", who_am_i_data);

	if ( (who_am_i_data == WHOAMI_VALUE_FOR_KXTJ9) || 
			(who_am_i_data == WHOAMI_VALUE_FOR_KXTJ2) || 
			(who_am_i_data == WHOAMI_VALUE_FOR_KX022) )
		ret = 1;
	else
		ret = 0;
	return sprintf(buf, "%d\n",ret);

}

ssize_t get_gsensor_data(struct device *dev, struct device_attribute *devattr, char *buf)
{
	int rawdata_x = 0, rawdata_y = 0, rawdata_z = 0;

	rawdata_x = kionix_Gsensor_data->accel_cal_data[0];
	rawdata_y = kionix_Gsensor_data->accel_cal_data[1];
	rawdata_z = kionix_Gsensor_data->accel_cal_data[2];
/*
	int retval = 0, rawdata_x = 0, rawdata_y = 0, rawdata_z = 0;
	unsigned char acc_data[6];

	retval = atomic_read(&kionix_Gsensor_data->enabled);
	printk("[Gsensor] alp : get_rawdata enable state=(%d)\n",retval);
	if(retval==0){
		retval = kx022_enable();
		mdelay(100);
		if (retval < 0){
			printk("[Gsensor] ATTR power on fail\n");
			return retval;
		}
	}
	retval = kx022_i2c_read(XOUT_L, (u8 *)acc_data, 6);
	if (retval < 0){
		dev_err(&kionix_Gsensor_data->client->dev, "accelerometer data read failed\n");
		return sprintf(buf, "get no data\n");
	}

	rawdata_x = ((acc_data[2]>>8) | (acc_data[3]<<8));
	rawdata_y = ((acc_data[0]>>8) | (acc_data[1]<<8));
	rawdata_z = ((acc_data[4]>>8) | (acc_data[5]<<8));

	if (rawdata_x > 16383)
		rawdata_x = rawdata_x - 65536;
	if (rawdata_y > 16383)
		rawdata_y = rawdata_y - 65536;
	if (rawdata_z > 16383)
		rawdata_z = rawdata_z - 65536;

	rawdata_x = rawdata_x*(-1);
*/
	return sprintf(buf, "%d %d %d\n", rawdata_x, rawdata_y, rawdata_z);
}

ssize_t read_gsensor_resolution(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned char data;
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	printk("[Gsensor] alp : read_gsensor_resolution (%d)\n",data);
	return sprintf(buf, "%d\n", data);
}

ssize_t write_gsensor_resolution(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int val = simple_strtoul(buf, NULL, 10);
	switch(val){
		case 0:
			printk("[Gsensor] alp : 8-bit !!!\n");
			kionix_Gsensor_data->ctrl_reg1 &=RES_8bit;
			i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
		break;
		case 1:
			printk("[Gsensor] alp : 16-bit !!!\n");
			kionix_Gsensor_data->ctrl_reg1 |=RES_16bit;
			i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1);
		break;

		default:
			printk("[Gsensor] alp : error input !!!\n");
		break;
	}
	return count;
}

ssize_t read_gsensor_wufe(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned char data[2]={0};
	data[0] = i2c_smbus_read_byte_data(client, WAKEUP_TIMER);
	data[1] = i2c_smbus_read_byte_data(client, WAKEUP_THRES);
	printk("[Gsensor] alp : read_gsensor_wufe time(%d) , thres(%d)\n",data[0],data[1]);
	return sprintf(buf, "%d & %d\n",data[0],data[1]);
}

ssize_t write_gsensor_wufe(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	u8 wufe_data[2]={0,0};
	int result = 0;
	int val = simple_strtoul(buf, NULL, 10);

	// turn off power
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (result < 0){
		printk("[Gsensor] alp : motion_detect_power_off_fail !\n");
		return result;
	}
	wufe_data[0] = 0x01; 
	switch(val){
		case 0:
			wufe_data[0] = 0x00; wufe_data[1] = 0x01;
		break;

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			wufe_data[1] = val;
		break;

		default:
			printk("[Gsensor] alp : not support !!!\n");
		break;
	}
	kionix_Gsensor_data->wufe_timer = wufe_data[0];
	kionix_Gsensor_data->wufe_thres = wufe_data[1];

	// set the chip timer
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, WAKEUP_TIMER, wufe_data[0]);
	if (result < 0){
		printk("[Gsensor] alp : ATTR set_timer_fail !\n");
		return result;
	}

	// set the chip threshold
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, WAKEUP_THRES, wufe_data[1]);
	if (result < 0){
		printk("[Gsensor] alp : ATTR set_chip_threshold_fail !\n");
		return result;
	}

	// turn on power
	kionix_Gsensor_data->ctrl_reg1 |= DRDYE;
	kionix_Gsensor_data->ctrl_reg1 &= WUFE_OFF;
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1); // enable motion detect
	if (result < 0){
		printk("[Gsensor] alp : write_gsensor_wufe into DRDY fail (%d)\n",result);
		return result;
	}
	mdelay(50); // 50 ms
	
	return count;
}

ssize_t read_gsensor_reg2_rate(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned char data;
	data = i2c_smbus_read_byte_data(client, CTRL_REG2);
	printk("[Gsensor] alp : read_gsensor_reg2_rate reg2 = %d\n",data);
	return sprintf(buf, "%d\n",data);
}

ssize_t write_gsensor_reg2_rate(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int result = 0;
	int val = simple_strtoul(buf, NULL, 10);
	unsigned char data;

	// turn off power
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, 0);
	if (result < 0)	{
		printk("[Gsensor] alp : motion_detect_power_off_fail !\n");
		return result;
	}
	switch(val){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			data = val;
		break;

		default:
			printk("[Gsensor] alp : not support !!!\n");
		break;
	}
	kionix_Gsensor_data->wufe_rate = data;
	// set the chip wufe rate
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG2, data);
	if (result < 0){
		printk("[Gsensor] alp : ATTR write_gsensor_reg2_rate !\n");
		return result;
	}

	// turn on power
	kionix_Gsensor_data->ctrl_reg1 |= DRDYE;
	kionix_Gsensor_data->ctrl_reg1 &= WUFE_OFF;
	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, CTRL_REG1, kionix_Gsensor_data->ctrl_reg1); // enable motion detect
	if (result < 0){
		printk("[Gsensor] alp : write_gsensor_reg2_rate power on fail ! (%d)\n", result);
		return result;
	}
	mdelay(50); // 50 ms
	
	return count;
}

ssize_t reset_gsensor(struct device *dev, struct device_attribute *attr, char *buf)
{
	int res=0;
	kx022_reset_function(kionix_Gsensor_data->client);
	return sprintf(buf, "%d\n", res);
}
// Kionix/Oulu - double tap code starts
int kx022_i2c_write_byte_and_trace(struct i2c_client *client, u8 reg, u8 value)
{
	int result;

	printk(" ----- reg 0x%x - value 0x%x\n",  reg, value);

	result = i2c_smbus_write_byte_data(kionix_Gsensor_data->client, reg, value);
	if (result < 0){
		printk("### %s - reg = 0x%X value = 0x%X \n", __FUNCTION__, reg, value);
	}

	return result;
}

//Added by Eason
// Kionix/Oulu - double tap code ends
ssize_t init_gsensor_double_tap(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	//u8 wufe_data[2]={0,0};
	int result = 0;
	int enable = simple_strtoul(buf, NULL, 10);
	//read CNTL1 data and set KX022 as standby-mode
	u8 data = 0;
	u8 init_data = 0;

	printk("[Gsensor] alp : init gsensor double tap +++\n");
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);
	data &= PC1_OFF;
	// set KX022 into standby mode
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, data);
	if (result < 0){
		printk("[Gsensor] alp : double_tap_power_off_fail !\n");
		return result;
	}
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);

	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);	
	if(enable){//do double tap init procedure here
		//init CNTL1 register
		init_data = 0b01000100;
		//check if other two states are ON
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPE_ON;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFE_ON;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_cntl1_fail !\n");
			return result;
		}
		//kionix_Gsensor_data->ctrl_reg1 = init_data;
		//init CNTL3 register, tilt ODR is 12.5Hz, tap is 400Hz, wakeup is 50Hz
		init_data = 0b10011110;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG3, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_cntl3_fail !\n");
			return result;
		}
		//init ODCNTL register, acceleration ODR is 100Hz
		init_data = 0b00000011;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, DATA_CTRL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_ODCNTL_fail !\n");
			return result;
		}
		//INC5: init register setup for INT2
		init_data = 0b00110000; // irq handler is active high. done in probe.
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL5, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_inc1_fail !\n");
			return result;
		}
		//INC3: init register setup for INT2
		init_data = 0b00111111;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL3, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_inc3_fail !\n");
			return result;
		}
		//INC6: init register setup for INT2
		init_data = 0b00000100;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPI2_EN;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFI2_EN;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_inc4_fail !\n");
			return result;
		}
		//TDTRC, only enable double tap interrupt
		init_data = 0b00000010;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TDTRC, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_inc4_fail !\n");
			return result;
		}

		//TDTC, additional register setup for tap event
		init_data = 0x78;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TDTC, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_TDTC_fail !\n");
			return result;
		}
		//TTH, additional register setup for tap event
		init_data = 0xCB;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TTH, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_TTH_fail !\n");
			return result;
		}
		//TTL, additional register setup for tap event
		init_data = 0x1a;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TTL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_TTL_fail !\n");
			return result;
		}
		//FTD, additional register setup for tap event
		init_data = 0xA2;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, FTD, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_FTD_fail !\n");
			return result;
		}
		//STD, additional register setup for tap event
		init_data = 0x24;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, STD, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_STD_fail !\n");
			return result;
		}
		//TLT, additional register setup for tap event
		init_data = 0x28;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TLT, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_TLT_fail !\n");
			return result;
		}
		//TWS, additional register setup for tap event
		init_data = 0xA0;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TWS, init_data);
		if (result < 0){
			printk("[Gsensor] alp : double_tap_TWS_fail !\n");
			return result;
		}

		/* Clean interrupt bit */
		i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);

		if(kionix_Gsensor_data->event_irq_status == 0){
			enable_irq(kionix_Gsensor_data->event_irq);
			printk("[Gsensor] alp : kx022_enable event irq (%d)\n", kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 1;
		}
		kionix_Gsensor_data->tap_state = FUNSTAT_ON;
	}	
	else	{
		//TODO here
		printk("[Gsensor] alp : double_tap_disable function !\n");
		//change CNTL1 register
		init_data = 0b01001000; //RES, DRDYE and 4g
		//check if other two states are ON
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPE_ON;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFE_ON;
		if ((kionix_Gsensor_data->hands_flip_state == FUNSTAT_OFF) && (kionix_Gsensor_data->flick_state == FUNSTAT_OFF))
			init_data |= DRDYE;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable double_tap_cntl1_fail !\n");
			return result;
		}
		//change INC6: init register setup for INT2
		init_data = 0b00000000;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPI2_EN;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFI2_EN;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable double_tap_inc6_fail !\n");
			return result;
		}
		if ((kionix_Gsensor_data->hands_flip_state == FUNSTAT_OFF) && (kionix_Gsensor_data->flick_state == FUNSTAT_OFF))
		{//disable event irq
			if(kionix_Gsensor_data->event_irq_status == 1){
				disable_irq(kionix_Gsensor_data->event_irq);
				printk("[Gsensor] alp : kx022_disable event irq (%d)\n", kionix_Gsensor_data->event_irq);
				kionix_Gsensor_data->event_irq_status = 0;
			}
		}
		kionix_Gsensor_data->tap_state = FUNSTAT_OFF;
	}

	// enable changes ie. double tap
	init_data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	init_data |= PC1_ON;
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data); 

	if (result < 0){
		printk("[Gsensor] alp : init double tap enable fail (%d)\n",result);
		return result;
	}

	printk("[Gsensor] alp : init gsensor double tap ---\n");	
	return count;
}

ssize_t init_gsensor_flip(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	//u8 wufe_data[2]={0,0};
	int result = 0;
	int enable = simple_strtoul(buf, NULL, 10);
	//read CNTL1 data and set KX022 as standby-mode
	u8 data = 0;
	u8 init_data = 0;

	printk("[Gsensor] alp : init gsensor flip feature +++\n");
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);
	data &= PC1_OFF;
	// set KX022 into standby mode
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, data);
	if (result < 0){
		printk("[Gsensor] alp : flip_power_off_fail !\n");
		return result;
	}
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);

	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);
	if(enable){//do Flip init procedure here
		//all function
		//init ODCNTL register, acceleration ODR is 100Hz
		init_data = 0b00000011;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, DATA_CTRL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_cntl1_fail !\n");
			return result;
		}
		//init CNTL1 register
		init_data = 0b01000001;
		//check if other two states are ON
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTE_ON;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFE_ON;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_cntl1_fail !\n");
			return result;
		}
		//kionix_Gsensor_data->ctrl_reg1 = init_data;
		//init CNTL2 register, enable TFDM&TFUM(Z), TLEM&TRIM(X) for tilt axis mask
		init_data = 0b00110011;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG2, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_cntl2_fail !\n");
			return result;
		}
		//init CNTL3 register, tilt ODR is 12.5Hz, tap is 400Hz, wakeup is 50Hz
		init_data = 0b10011110;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG3, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_cntl3_fail !\n");
			return result;
		}
		//init TILT_TIMER register
		init_data = 0b00000001; //count * 1/Tilt ODR
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TILT_TIMER, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_ODCNTL_fail !\n");
			return result;
		}
		
		//INC5: init register setup for INT2
		init_data = 0x30; // irq handler is active high. done in probe.
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL5, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_inc1_fail !\n");
			return result;
		}
		//INC6: enable Tilt position interrupt setup for INT2
		init_data = 0b00000001;
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTI2_EN;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFI2_EN;
		
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_inc3_fail !\n");
			return result;
		}
		
		//TILT_ANGLE_LL, Set Angle Limitation_LSB
		init_data = 0b00001100;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TILT_ANGLE_LL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_TILT_ANGLE_LL_fail !\n");
			return result;
		}

		//TILT_ANGLE_HL, Set Angle Limitation_MSB 
		init_data = 0x2A;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, TILT_ANGLE_HL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_TILT_ANGLE_HL_fail !\n");
			return result;
		}
		//HYST_SET,
		init_data = 0x14;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, HYST_SET, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flip_HYST_SET_fail !\n");
			return result;
		}

		/* Clean interrupt bit */
		i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);

		if(kionix_Gsensor_data->event_irq_status == 0){
			enable_irq(kionix_Gsensor_data->event_irq);
			printk("[Gsensor] alp : kx022_enable event irq (%d)\n", kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 1;
		}
		kionix_Gsensor_data->hands_flip_state = FUNSTAT_ON;
	}	
	else	{
		printk("[Gsensor] alp : Hands_flip_disable function !\n");
		//change CNTL1 register
		init_data = 0b01101000; //RES, DRDYE and 4g
		//check if other two states are ON
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTE_ON;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFE_ON;
		if ((kionix_Gsensor_data->tap_state == FUNSTAT_OFF) && (kionix_Gsensor_data->flick_state == FUNSTAT_OFF))
			init_data |= DRDYE;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable flip_cntl1_fail !\n");
			return result;
		}
		//change INC6: init register setup for INT2
		init_data = 0b00000000;
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTI2_EN;
		if (kionix_Gsensor_data->flick_state == FUNSTAT_ON)
			init_data |= WUFI2_EN;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable flip_inc6_fail !\n");
			return result;
		}

		if ((kionix_Gsensor_data->tap_state == FUNSTAT_OFF) && (kionix_Gsensor_data->flick_state == FUNSTAT_OFF))
		{//disable event irq
		if(kionix_Gsensor_data->event_irq_status == 1){
			disable_irq(kionix_Gsensor_data->event_irq);
			printk("[Gsensor] alp : kx022_disable event irq (%d)\n", kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 0;
		}
	}
		kionix_Gsensor_data->hands_flip_state = FUNSTAT_OFF;
	}

	// enable changes ie. flip
	init_data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	init_data |= PC1_ON;
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data); 

	if (result < 0){
		printk("[Gsensor] alp : init flip enable fail (%d)\n",result);
		return result;
	}

	printk("[Gsensor] alp : init gsensor flip ---\n");	
	return count;
}

ssize_t init_gsensor_flick(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	//u8 wufe_data[2]={0,0};
	int result = 0;
	int enable = simple_strtoul(buf, NULL, 10);
	//read CNTL1 data and set KX022 as standby-mode
	u8 data = 0;
	u8 init_data = 0;

	printk("[Gsensor] alp : init gsensor flick feature +++\n");
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);
	data &= PC1_OFF;
	// set KX022 into standby mode
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, data);
	if (result < 0){
		printk("[Gsensor] alp : flip_power_off_fail !\n");
		return result;
	}
	data = i2c_smbus_read_byte_data(client, CTRL_REG1);

	printk("[Gsensor] KX022 - CNTL1 reg = %x\n",data);
	if(enable){//do Flick init procedure here
		//all function
		//init ODCNTL register, acceleration ODR is 100Hz
		init_data = 0b00000011;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, DATA_CTRL, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_ODCNTL_fail !\n");
			return result;
		}
		//init CNTL1 register
		init_data = 0b01000010;
		//check if other two states are ON
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTE_ON;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPE_ON;
		
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_cntl1_fail !\n");
			return result;
		}
		
		//init CNTL3 register, tilt ODR is 12.5Hz, tap is 400Hz, wakeup is 50Hz
		init_data = 0b10011110;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG3, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_cntl3_fail !\n");
			return result;
		}
				
		//INC5: init register setup for INT2
		init_data = 0x30; // irq handler is active high. done in probe.
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL5, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_inc1_fail !\n");
			return result;
		}
		//INC2: enable wakeup direction interrupt setup for INT2
		init_data = 0b00111111;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL2, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_inc2_fail !\n");
			return result;
		}
		//INC6: control interrupt reporting for INT2
		init_data = 0b00000010;
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTI2_EN;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPI2_EN;
		
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_inc4_fail !\n");
			return result;
		}
		//WUFC: initial count register for the motion detection timer
		init_data = 0b00000111;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, WUFC, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_wufc_fail !\n");
			return result;
		}
		//ATH: the threshold for wake-up (motion detect) interrupt
		init_data = 0b00000110;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, ATH, init_data);
		if (result < 0){
			printk("[Gsensor] alp : flick_ATH_fail !\n");
			return result;
		}
		
		/* Clean interrupt bit */
		i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);

		if(kionix_Gsensor_data->event_irq_status == 0){
			enable_irq(kionix_Gsensor_data->event_irq);
			printk("[Gsensor] alp : kx022_enable event irq (%d)\n", kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 1;
		}
		kionix_Gsensor_data->flick_state = FUNSTAT_ON;
	}	
	else	{
		printk("[Gsensor] alp : flick_disable function !\n");
		//change CNTL1 register
		init_data = 0b01101000; //RES, DRDYE and 4g
		//check if other two states are ON
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTE_ON;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPE_ON;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable flick_cntl1_fail !\n");
			return result;
		}
		//change INC6: init register setup for INT2
		init_data = 0b00000000;
		if (kionix_Gsensor_data->tap_state == FUNSTAT_ON)
			init_data |= TDTI2_EN;
		if (kionix_Gsensor_data->hands_flip_state == FUNSTAT_ON)
			init_data |= TPI2_EN;
		if ((kionix_Gsensor_data->hands_flip_state == FUNSTAT_OFF) && (kionix_Gsensor_data->tap_state == FUNSTAT_OFF))
			init_data |= DRDYE;
		result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, INT_CTRL6, init_data);
		if (result < 0){
			printk("[Gsensor] alp : disable flick_inc6_fail !\n");
			return result;
		}
		
		if ((kionix_Gsensor_data->hands_flip_state == FUNSTAT_OFF) && (kionix_Gsensor_data->tap_state == FUNSTAT_OFF))
		{//disable event irq
		if(kionix_Gsensor_data->event_irq_status == 1){
			disable_irq(kionix_Gsensor_data->event_irq);
			printk("[Gsensor] alp : kx022_disable event irq (%d)\n", kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 0;
		}
	}
		kionix_Gsensor_data->flick_state = FUNSTAT_OFF;
	}

	// enable changes ie. flip
	init_data = i2c_smbus_read_byte_data(client, CTRL_REG1);
	init_data |= PC1_ON;
	result = kx022_i2c_write_byte_and_trace(kionix_Gsensor_data->client, CTRL_REG1, init_data); 

	if (result < 0){
		printk("[Gsensor] alp : init flick enable fail (%d)\n",result);
		return result;
	}

	printk("[Gsensor] alp : init gsensor flick ---\n");	
	return count;
}


#ifdef CONFIG_INPUT_KXTJ9_POLLED_MODE
static void kx022_poll(struct input_polled_dev *dev)
{
	unsigned int poll_interval = dev->poll_interval;

	kx022_report_acceleration_data();

	if (poll_interval != kionix_Gsensor_data->last_poll_interval) {
		kx022_update_odr(poll_interval);
		kionix_Gsensor_data->last_poll_interval = poll_interval;
	}
	if(KX022_DEBUG_MESSAGE) 
		printk("[Gsensor] alp : kx022_poll poll_interval(%d)\n",kionix_Gsensor_data->last_poll_interval);
}

static void kx022_polled_input_open(struct input_polled_dev *dev)
{
	kx022_enable();
}

static void kx022_polled_input_close(struct input_polled_dev *dev)
{
	kx022_disable();
}

static int kx022_setup_polled_device(void)
{
	int err;
	struct input_polled_dev *poll_dev;
	poll_dev = input_allocate_polled_device();

	if (!poll_dev) {
		dev_err(&kionix_Gsensor_data->client->dev,
			"Failed to allocate polled device\n");
		return -ENOMEM;
	}

	kionix_Gsensor_data->poll_dev = poll_dev;
	kionix_Gsensor_data->input_dev = poll_dev->input;

	poll_dev->private = kionix_Gsensor_data;
	poll_dev->poll = kx022_poll;
	poll_dev->open = kx022_polled_input_open;
	poll_dev->close = kx022_polled_input_close;

	kx022_setup_input_device();

	err = input_register_polled_device(poll_dev);
	if (err) {
		dev_err(&kionix_Gsensor_data->client->dev,
			"Unable to register polled device, err=%d\n", err);
		input_free_polled_device(poll_dev);
		return err;
	}

	return 0;
}

static void __devexit kx022_teardown_polled_device(void)
{
	input_unregister_polled_device(kionix_Gsensor_data->poll_dev);
	input_free_polled_device(kionix_Gsensor_data->poll_dev);
}

#else

static inline int kx022_setup_polled_device(void)
{
	return -ENOSYS;
}

static inline void kx022_teardown_polled_device(void)
{
}

#endif

static int KIONIX_HW_vertify(void)
{
	int retval=0;
	if(KX022_DEBUG_MESSAGE) printk("[Gsensor] KIONIX_HW_vertify ++\n");

	retval = i2c_smbus_read_byte_data(kionix_Gsensor_data->client, WHO_AM_I);
	printk("[Gsensor] KIONIX_HW_vertify ret = %d\n",retval);
	if (retval < 0) {
		printk("[Gsensor] read fail ret = %d\n", retval);
		dev_err(&kionix_Gsensor_data->client->dev, "read err int source\n");
		goto out;
	}
	if(KX022_DEBUG_MESSAGE) printk("[Gsensor] alp : read retval = %d\n", retval);

	if(retval == WHOAMI_VALUE_FOR_KX022)
		printk("[Gsensor] Use kx022!!\n");
	else if(retval == WHOAMI_VALUE_FOR_KXTJ2)
		printk("[Gsensor] Use kxtj2!!\n");
	else if(retval == WHOAMI_VALUE_FOR_KXTJ9)
		printk("[Gsensor] Use kxtj9!!\n");
	else
		printk("[Gsensor] Use other!!\n");
out:
	kx022_device_power_off();
	if(KX022_DEBUG_MESSAGE) printk("[Gsensor] alp : KIONIX_HW_vertify --\n");
	return retval;
}

/*
static struct file_operations kx022_fops = {
	.owner			= 	THIS_MODULE,
//	.poll 			= 	kx022_poll,
//	.read 			= 	kxtj9_read,
	.unlocked_ioctl	=	kxtj9_ioctl,
//	.compat_ioctl		=	kxtj9_ioctl,
	.open			=	kxtj9_open,
	.release			=	kxtj9_release,
};
*/

static struct miscdevice kx022_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "kx022_dev",
//	.fops = &kx022_fops,
};

#ifdef CONFIG_HAS_EARLYSUSPEND
static void kx022_early_suspend(struct early_suspend *h)
{
	int status=0;

	mutex_lock(&kionix_Gsensor_data->lock);
	kionix_Gsensor_data->suspend_resume_state = 1;

	status = 	kionix_Gsensor_data->ctrl_reg1 >> 7;
	printk("[Gsensor] alp : kx022_early_suspend enable(%d)\n", status);
	if(status == KX022_RESUME_ENABLE){
		printk("[Gsensor] alp : kxtj, need to enable after resume!\n");
		kionix_Gsensor_data->resume_enable = KX022_RESUME_ENABLE;
	} else	{
		kionix_Gsensor_data->resume_enable = KX022_RESUME_DISABLE;
	}

	kx022_disable();
	mutex_unlock(&kionix_Gsensor_data->lock);
	printk("[Gsensor] alp : kx022_early_suspend irq(%d)\n", kionix_Gsensor_data->irq);
}

static void kx022_late_resume(struct early_suspend *h)
{
	mutex_lock(&kionix_Gsensor_data->lock);
	kionix_Gsensor_data->suspend_resume_state = 0;
	if ( (kionix_Gsensor_data->resume_enable == KX022_RESUME_ENABLE) ||
			(kionix_Gsensor_data->resume_enable==KX022_RESUME_MISSENABLE) )	{
		kx022_enable();
		printk("[Gsensor] alp : kx022_late_resume enable\n");
	}else
		printk("[Gsensor] alp : kx022_late_resume pass enable\n");
	mutex_unlock(&kionix_Gsensor_data->lock);
	printk("[Gsensor] alp : kx022_late_resume irq(%d)\n", kionix_Gsensor_data->irq);
}
#endif

#ifdef CONFIG_PM_SLEEP
static int kx022_suspend(struct device *dev)
{
	mutex_lock(&kionix_Gsensor_data->lock);
	if (atomic_read(&kionix_Gsensor_data->enabled) != KX022_DEVICE_DISABLE) {
		kx022_device_power_off();
		if (kionix_Gsensor_data->irq_status==1)	{
			disable_irq(kionix_Gsensor_data->irq);
			kionix_Gsensor_data->irq_status = 0;
		}
	}
	mutex_unlock(&kionix_Gsensor_data->lock);
	printk("[Gsensor] alp : kx022_suspend irq status(%d)\n", kionix_Gsensor_data->irq_status );
	return 0;
}

static int kx022_resume(struct device *dev)
{
	printk("[Gsensor] alp : kx022_resume\n");
	mutex_lock(&kionix_Gsensor_data->lock);
	if (atomic_read(&kionix_Gsensor_data->enabled) != KX022_DEVICE_DISABLE)	{
		switch(atomic_read(&kionix_Gsensor_data->enabled))	{
		case KX022_ACC_ENABLE:
			kionix_Gsensor_data->data_report_count = 0;
			kx022_enable();
			break;
		case KX022_ORI_ENABLE:
			kionix_Gsensor_data->data_report_count = 0;
			kx022_enable_by_orientation();
			break;
		case KX022_BOTH_ENABLE:
			kionix_Gsensor_data->data_report_count = 0;
			kx022_enable();
			break;
		}
	}
	mutex_unlock(&kionix_Gsensor_data->lock);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(kx022_pm_ops, kx022_suspend, kx022_resume);

static int kx022_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct device_node *np = dev->of_node;
	int gpio=0, iloop=0;
	int err = 0;
	printk("[Gsensor] Probe KX022 Gsensor i2c driver\n");

	/* Setting i2c client and device_data */
	if (!i2c_check_functionality(client->adapter,
				I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "client is not i2c capable\n");
		return -ENXIO;
	}

	kionix_Gsensor_data = kzalloc(sizeof(struct ASUS_Gsensor_data), GFP_KERNEL);
	if (!kionix_Gsensor_data) {
		dev_err(&client->dev,
			"failed to allocate memory for module data\n");
		return -ENOMEM;
	}
	memset(kionix_Gsensor_data, 0, sizeof(struct ASUS_Gsensor_data));
	kionix_Gsensor_data->client = client;
	i2c_set_clientdata(client, kionix_Gsensor_data);
	mutex_init(&kionix_Gsensor_data->lock);

	/* Init calibration data */
	for (iloop = 0; iloop < 6; iloop++)	{
		kionix_Gsensor_data->accel_cal_data[iloop] = 0;
	}
	for (iloop = 0; iloop < 3; iloop++)	{
		kionix_Gsensor_data->accel_cal_offset[iloop] = 0;
		kionix_Gsensor_data->accel_cal_sensitivity[iloop] = GSENSOR_MAX_RESOLUTION;
	}
	kionix_Gsensor_data->x_gain_neg_data = 1;
	kionix_Gsensor_data->y_gain_neg_data = 1;
	kionix_Gsensor_data->z_gain_neg_data = 1;
	kionix_Gsensor_data->x_gain_pos_data = 1;
	kionix_Gsensor_data->y_gain_pos_data = 1;
	kionix_Gsensor_data->z_gain_pos_data = 1;
	kionix_Gsensor_data->data_report_count = 0;
	init_cali_data (&kionix_Gsensor_data->asus_gsensor_cali_data);
	kionix_Gsensor_data->tap_state = FUNSTAT_OFF;
	kionix_Gsensor_data->hands_flip_state = FUNSTAT_OFF;
	kionix_Gsensor_data->flick_state = FUNSTAT_OFF;

	/* Setting input event & device */
	err = kx022_setup_input_device();
	if (err)
		printk("[Gsensor] Unable to request gsensor input device\n");

	/* Setting gpio & irq */
	/* Interrupt1 */
	gpio = of_get_named_gpio(np, "kionix,gpio-int", 0);
	
	printk("[Gsensor] Get Gsensor interrupt : %d\n", gpio);
	err = gpio_request(gpio,"accel_kx022_interrupt");
	if (err)
		printk("[Gsensor] Unable to request gpio %d (kx022-irq)\n", gpio);
	err = gpio_direction_input(gpio);
	if (err)
		printk("[Gsensor] Unable to set the direction of gpio %d (kx022-irq)\n", gpio);
	
	kionix_Gsensor_data->irq = gpio_to_irq(gpio);
	printk("[Gsensor] Setting Final irq=%d\n", kionix_Gsensor_data->irq);

	/* Set data ready workqueue */
	Gsensor_data_ready_workqueue = create_singlethread_workqueue("kx022_drdy_wq");
	INIT_WORK(&Gsensor_data_ready_isr_work, kx022_asseleration_ist);

	/* Interrupt2 */
	gpio = 0;
	gpio = of_get_named_gpio(np, "kionix,gpio-event_int", 0);
	
	printk("[Gsensor] Get Gsensor event interrupt : %d\n", gpio);
	err = gpio_request(gpio,"accel_kx022_event_interrupt");
	if (err)
		printk("[Gsensor] Unable to request gpio %d (kx022-event_irq)\n", gpio);
	err = gpio_direction_input(gpio);
	if (err)
		printk("[Gsensor] Unable to set the direction of gpio %d (kx022-event_irq)\n", gpio);
	
	kionix_Gsensor_data->event_irq = gpio_to_irq(gpio);
	printk("[Gsensor] Setting Final event irq=%d\n", kionix_Gsensor_data->event_irq);
	/*.............................*/

	err = KIONIX_HW_vertify();
	if (err == 0)
		Gsensor_status = 1;

	if (err < 0) {
		dev_err(&client->dev, "[Gsensor] device not recognized\n");
	}
	kionix_Gsensor_data->ctrl_reg1 |= (RES_16bit | GRP4_G_4G);
	kionix_Gsensor_data->last_poll_interval = 0;
	printk("[Gsensor] Setting init reg1 = 0x%X \n",kionix_Gsensor_data->ctrl_reg1);

	/* Reguest data ready Interrupt1 */
	if (kionix_Gsensor_data->irq) {
		/* If in irq mode, populate INT_CTRL_REG1 and enable DRDY. */
		kionix_Gsensor_data->int_ctrl |= (KX022_IEN | KX022_IEA);

		kionix_Gsensor_data->ctrl_reg1 |= DRDYE;

		err = request_threaded_irq(kionix_Gsensor_data->irq, NULL,
					kx022_isr,IRQF_TRIGGER_RISING | IRQF_ONESHOT, "kx022-irq", kionix_Gsensor_data);
		if (err < 0)
			printk("[Gsensor] Gsensor request_irq() error %d.\n", err);
		else		{
			printk("[Gsensor] Gsensor request_irq ok.(0x%x)\n", kionix_Gsensor_data->int_ctrl);
			/* Clean interrupt bit */
			i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
			disable_irq(kionix_Gsensor_data->irq);
			kionix_Gsensor_data->irq_status = 0;
		}
		if (err)
			dev_err(&client->dev, "request irq failed: %d\n", err);

		/* Setting system file */
		err = sysfs_create_group(&client->dev.kobj, &kx022_attribute_group);
		if (err) {
			dev_err(&client->dev, "[Gsensor] sysfs create failed: %d\n", err);
		}
		err = misc_register(&kx022_device);
		if (err) {
			printk("[Gsensor] alp :  kx022_misc_register failed\n");
		}
	} else {
		err = kx022_setup_polled_device();
	}
	printk("[Gsensor] Final reg1 = 0x%X \n", kionix_Gsensor_data->ctrl_reg1);
	atomic_set(&kionix_Gsensor_data->enabled, KX022_DEVICE_DISABLE);

	/* Reguest event Interrupt2 */
	if (kionix_Gsensor_data->event_irq) {

		err = request_threaded_irq(kionix_Gsensor_data->event_irq, NULL,
					kx022_event_isr,IRQF_TRIGGER_RISING | IRQF_ONESHOT, "kx022-event_irq", kionix_Gsensor_data);
		if (err < 0)
			printk("[Gsensor] Gsensor request_event_irq() error %d.\n", err);
		else		{
			printk("[Gsensor] Gsensor request_event_irq ok.(0x%x)\n", kionix_Gsensor_data->event_irq);
			/* Clean interrupt bit */
			i2c_smbus_read_byte_data(kionix_Gsensor_data->client, INT_REL);
			disable_irq(kionix_Gsensor_data->event_irq);
			kionix_Gsensor_data->event_irq_status = 0;
		}
		if (err)
			dev_err(&client->dev, "[Gsensor] Request event irq failed: %d\n", err);
	} else {
		dev_err(&client->dev, "[Gsensor] Event irq failed: %d\n", err);
	}

	g_ilocation = KX022_CHIP_LOCATION_SR_ZC500KL;
	
// for enable motion detect , added by cheng_kao 2014.02.12 ++
	if(build_version==1){	// 1:eng ; 2:user ; 3:userdebug 
		bypass_for_eng_mode = true;
		printk("[Gsensor] alp.D. : kx022 G-sensor for eng mode !!\n");
	}
	kionix_Gsensor_data->motion_detect_threshold_x=0;
	kionix_Gsensor_data->motion_detect_threshold_y=0;
	kionix_Gsensor_data->motion_detect_threshold_z=0;
	kionix_Gsensor_data->motion_detect_timer=-1;	// init counter
	kionix_Gsensor_data->chip_interrupt_mode=INT_MODE_DRDY;
	kionix_Gsensor_data->wufe_rate=WUFE25F;		// 	rate
	kionix_Gsensor_data->wufe_timer=0x01;			//	1 / rate = delay (sec)
	kionix_Gsensor_data->wufe_thres=0x01;			//	counts 16 = 1g
// for enable motion detect , added by cheng_kao 2014.02.12 --

#ifdef CONFIG_HAS_EARLYSUSPEND
	kionix_Gsensor_data->gsensor_early_suspendresume.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN - 1;
	kionix_Gsensor_data->gsensor_early_suspendresume.suspend = kx022_early_suspend;
	kionix_Gsensor_data->gsensor_early_suspendresume.resume = kx022_late_resume;
	register_early_suspend(&kionix_Gsensor_data->gsensor_early_suspendresume);
#endif

	kionix_Gsensor_data->suspend_resume_state = 0;
	kionix_Gsensor_data->resume_enable = 0;

	printk("[Gsensor] kx022_probe (%d) --\n", g_ilocation);

	return err;
}

static int kx022_remove(struct i2c_client *client)
{
	if (kionix_Gsensor_data->irq) {
		sysfs_remove_group(&client->dev.kobj, &kx022_attribute_group);
		free_irq(kionix_Gsensor_data->irq, NULL);
		input_unregister_device(kionix_Gsensor_data->input_dev);
	} else {
		kx022_teardown_polled_device();
	}

	destroy_workqueue(Gsensor_data_ready_workqueue);
	mutex_destroy(&kionix_Gsensor_data->lock);
	kfree(kionix_Gsensor_data);

	return 0;
}

static struct of_device_id kionix_gsensor_match_table[] = {
	{ .compatible = "kionix, kx022",},
	{ },
};

static const struct i2c_device_id kx022_id[] = {
	{ ASUS_GSENSOR_NAME, 0 },
	{ },
};

MODULE_DEVICE_TABLE(i2c, kx022_id);

static struct i2c_driver kx022_driver = {
	.driver = {
		.name			= ASUS_GSENSOR_NAME,
		.owner			= THIS_MODULE,
		.of_match_table	= kionix_gsensor_match_table,
		.pm				= &kx022_pm_ops,
	},
	.probe	= kx022_probe,
	.remove	= kx022_remove,
	.id_table	= kx022_id,
};

static int __init kx022_init(void)
{
	int res = 0;
	pr_info("[Gsensor] Gsensor driver: initialize.\n");
	res = i2c_add_driver(&kx022_driver);
	if (res != 0)
		printk("[Gsensor] I2c_add_driver fail, Error : %d\n", res);
	return res;
}
module_init(kx022_init);

static void __exit kx022_exit(void)
{
	i2c_del_driver(&kx022_driver);
}
module_exit(kx022_exit);

MODULE_DESCRIPTION("KX022 accelerometer driver");
MODULE_LICENSE("GPL");
