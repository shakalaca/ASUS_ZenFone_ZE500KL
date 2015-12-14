 /* Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef MSM8X16_WCD_REGISTERS_H
#define MSM8X16_WCD_REGISTERS_H

#define MSM8X16_WCD_A_DIGITAL_REVISION1			(0x000)
#define MSM8X16_WCD_A_DIGITAL_REVISION1__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_REVISION2			(0x001)
#define MSM8X16_WCD_A_DIGITAL_REVISION2__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_PERPH_TYPE			(0x004)
#define MSM8X16_WCD_A_DIGITAL_PERPH_TYPE__POR				(0x23)
#define MSM8X16_WCD_A_DIGITAL_PERPH_SUBTYPE			(0x005)
#define MSM8X16_WCD_A_DIGITAL_PERPH_SUBTYPE__POR			(0x01)
#define MSM8X16_WCD_A_DIGITAL_INT_RT_STS			(0x010)
#define MSM8X16_WCD_A_DIGITAL_INT_RT_STS__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_SET_TYPE			(0x011)
#define MSM8X16_WCD_A_DIGITAL_INT_SET_TYPE__POR				(0xFF)
#define MSM8X16_WCD_A_DIGITAL_INT_POLARITY_HIGH			(0x012)
#define MSM8X16_WCD_A_DIGITAL_INT_POLARITY_HIGH__POR			(0xFF)
#define MSM8X16_WCD_A_DIGITAL_INT_POLARITY_LOW			(0x013)
#define MSM8X16_WCD_A_DIGITAL_INT_POLARITY_LOW__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_LATCHED_CLR			(0x014)
#define MSM8X16_WCD_A_DIGITAL_INT_LATCHED_CLR__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_EN_SET			(0x015)
#define MSM8X16_WCD_A_DIGITAL_INT_EN_SET__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_EN_CLR			(0x016)
#define MSM8X16_WCD_A_DIGITAL_INT_EN_CLR__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_LATCHED_STS			(0x018)
#define MSM8X16_WCD_A_DIGITAL_INT_LATCHED_STS__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_PENDING_STS			(0x019)
#define MSM8X16_WCD_A_DIGITAL_INT_PENDING_STS__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_MID_SEL			(0x01A)
#define MSM8X16_WCD_A_DIGITAL_INT_MID_SEL__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_PRIORITY			(0x01B)
#define MSM8X16_WCD_A_DIGITAL_INT_PRIORITY__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_GPIO_MODE			(0x040)
#define MSM8X16_WCD_A_DIGITAL_GPIO_MODE__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_PIN_CTL_OE			(0x041)
#define MSM8X16_WCD_A_DIGITAL_PIN_CTL_OE__POR				(0x01)
#define MSM8X16_WCD_A_DIGITAL_PIN_CTL_DATA			(0x042)
#define MSM8X16_WCD_A_DIGITAL_PIN_CTL_DATA__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_PIN_STATUS			(0x043)
#define MSM8X16_WCD_A_DIGITAL_PIN_STATUS__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_HDRIVE_CTL			(0x044)
#define MSM8X16_WCD_A_DIGITAL_HDRIVE_CTL__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_RST_CTL			(0x046)
#define MSM8X16_WCD_A_DIGITAL_CDC_RST_CTL__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_TOP_CLK_CTL			(0x048)
#define MSM8X16_WCD_A_DIGITAL_CDC_TOP_CLK_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_ANA_CLK_CTL			(0x049)
#define MSM8X16_WCD_A_DIGITAL_CDC_ANA_CLK_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_DIG_CLK_CTL			(0x04A)
#define MSM8X16_WCD_A_DIGITAL_CDC_DIG_CLK_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_TX1_CTL			(0x050)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_TX1_CTL__POR			(0x02)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_TX2_CTL			(0x051)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_TX2_CTL__POR			(0x02)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_HPHR_DAC_CTL			(0x052)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_HPHR_DAC_CTL__POR		(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX1_CTL			(0x053)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX2_CTL			(0x054)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX2_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX3_CTL			(0x055)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX3_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX_LB_CTL			(0x056)
#define MSM8X16_WCD_A_DIGITAL_CDC_CONN_RX_LB_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL1			(0x058)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL1__POR				(0x7C)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL2			(0x059)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL2__POR				(0x7C)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL3			(0x05A)
#define MSM8X16_WCD_A_DIGITAL_CDC_RX_CTL3__POR				(0x7C)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA0			(0x05B)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA0__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA1			(0x05C)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA1__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA2			(0x05D)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA2__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA3			(0x05E)
#define MSM8X16_WCD_A_DIGITAL_DEM_BYPASS_DATA3__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_DIG_DEBUG_CTL			(0x068)
#define MSM8X16_WCD_A_DIGITAL_DIG_DEBUG_CTL__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_DIG_DEBUG_EN			(0x069)
#define MSM8X16_WCD_A_DIGITAL_DIG_DEBUG_EN__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_SPARE_0				(0x070)
#define MSM8X16_WCD_A_DIGITAL_SPARE_0__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_SPARE_1				(0x071)
#define MSM8X16_WCD_A_DIGITAL_SPARE_1__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_SPARE_2				(0x072)
#define MSM8X16_WCD_A_DIGITAL_SPARE_2__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_SEC_ACCESS			(0x0D0)
#define MSM8X16_WCD_A_DIGITAL_SEC_ACCESS__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL1			(0x0D8)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL1__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL2			(0x0D9)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL2__POR			(0x01)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL3			(0x0DA)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL3__POR			(0x05)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL4			(0x0DB)
#define MSM8X16_WCD_A_DIGITAL_PERPH_RESET_CTL4__POR			(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_TEST1				(0x0E0)
#define MSM8X16_WCD_A_DIGITAL_INT_TEST1__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_INT_TEST_VAL			(0x0E1)
#define MSM8X16_WCD_A_DIGITAL_INT_TEST_VAL__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_TRIM_NUM				(0x0F0)
#define MSM8X16_WCD_A_DIGITAL_TRIM_NUM__POR				(0x00)
#define MSM8X16_WCD_A_DIGITAL_TRIM_CTRL				(0x0F1)
#define MSM8X16_WCD_A_DIGITAL_TRIM_CTRL__POR				(0x00)

#define MSM8X16_WCD_A_ANALOG_REVISION1			(0x100)
#define MSM8X16_WCD_A_ANALOG_REVISION1__POR				(0x00)
#define MSM8X16_WCD_A_ANALOG_REVISION2			(0x101)
#define MSM8X16_WCD_A_ANALOG_REVISION2__POR				(0x00)
#define MSM8X16_WCD_A_ANALOG_REVISION3			(0x102)
#define MSM8X16_WCD_A_ANALOG_REVISION3__POR				(0x00)
#define MSM8X16_WCD_A_ANALOG_REVISION4			(0x103)
#define MSM8X16_WCD_A_ANALOG_REVISION4__POR				(0x00)
#define MSM8X16_WCD_A_ANALOG_PERPH_TYPE			(0x104)
#define MSM8X16_WCD_A_ANALOG_PERPH_TYPE__POR			(0x23)
#define MSM8X16_WCD_A_ANALOG_PERPH_SUBTYPE		(0x105)
#define MSM8X16_WCD_A_ANALOG_PERPH_SUBTYPE__POR			(0x09)
#define MSM8X16_WCD_A_ANALOG_INT_RT_STS			(0x110)
#define MSM8X16_WCD_A_ANALOG_INT_RT_STS__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_SET_TYPE		(0x111)
#define MSM8X16_WCD_A_ANALOG_INT_SET_TYPE__POR			(0x3F)
#define MSM8X16_WCD_A_ANALOG_INT_POLARITY_HIGH		(0x112)
#define MSM8X16_WCD_A_ANALOG_INT_POLARITY_HIGH__POR		(0x3F)
#define MSM8X16_WCD_A_ANALOG_INT_POLARITY_LOW		(0x113)
#define MSM8X16_WCD_A_ANALOG_INT_POLARITY_LOW__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_LATCHED_CLR		(0x114)
#define MSM8X16_WCD_A_ANALOG_INT_LATCHED_CLR__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_EN_SET			(0x115)
#define MSM8X16_WCD_A_ANALOG_INT_EN_SET__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_EN_CLR			(0x116)
#define MSM8X16_WCD_A_ANALOG_INT_EN_CLR__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_LATCHED_STS		(0x118)
#define MSM8X16_WCD_A_ANALOG_INT_LATCHED_STS__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_PENDING_STS		(0x119)
#define MSM8X16_WCD_A_ANALOG_INT_PENDING_STS__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_MID_SEL		(0x11A)
#define MSM8X16_WCD_A_ANALOG_INT_MID_SEL__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_PRIORITY		(0x11B)
#define MSM8X16_WCD_A_ANALOG_INT_PRIORITY__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_MICB_1_EN			(0x140)
#define MSM8X16_WCD_A_ANALOG_MICB_1_EN__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_MICB_1_VAL			(0x141)
//+++ASUS_BSP : Eric
//#define MSM8X16_WCD_A_ANALOG_MICB_1_VAL__POR			(0x20)
#define MSM8X16_WCD_A_ANALOG_MICB_1_VAL__POR			(0xC0)
//---ASUS_BSP : Eric
#define MSM8X16_WCD_A_ANALOG_MICB_1_CTL			(0x142)
#define MSM8X16_WCD_A_ANALOG_MICB_1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_MICB_1_INT_RBIAS			(0x143)
//#define MSM8X16_WCD_A_ANALOG_MICB_1_INT_RBIAS__POR		(0x49)
#define MSM8X16_WCD_A_ANALOG_MICB_1_INT_RBIAS__POR		(0x48)
#define MSM8X16_WCD_A_ANALOG_MICB_2_EN			(0x144)
#define MSM8X16_WCD_A_ANALOG_MICB_2_EN__POR			(0x20)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_ATEST_CTL_2			(0x145)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_ATEST_CTL_2__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_MASTER_BIAS_CTL			(0x146)
#define MSM8X16_WCD_A_ANALOG_MASTER_BIAS_CTL__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_MBHC_DET_CTL_1		(0x147)
#define MSM8X16_WCD_A_ANALOG_MBHC_DET_CTL_1__POR		(0x35)
#define MSM8X16_WCD_A_ANALOG_MBHC_DET_CTL_2		(0x150)
#define MSM8X16_WCD_A_ANALOG_MBHC_DET_CTL_2__POR		(0x08)
#define MSM8X16_WCD_A_ANALOG_MBHC_FSM_CTL		(0x151)
#define MSM8X16_WCD_A_ANALOG_MBHC_FSM_CTL__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_MBHC_DBNC_TIMER		(0x152)
#define MSM8X16_WCD_A_ANALOG_MBHC_DBNC_TIMER__POR		(0x98)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN0_ZDETL_CTL	(0x153)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN0_ZDETL_CTL__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN1_ZDETM_CTL	(0x154)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN1_ZDETM_CTL__POR		(0x20)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN2_ZDETH_CTL	(0x155)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN2_ZDETH_CTL__POR		(0x40)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN3_CTL		(0x156)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN3_CTL__POR			(0x61)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN4_CTL		(0x157)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN4_CTL__POR			(0x80)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN_RESULT		(0x158)
#define MSM8X16_WCD_A_ANALOG_MBHC_BTN_RESULT__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_MBHC_ZDET_ELECT_RESULT	(0x159)
#define MSM8X16_WCD_A_ANALOG_MBHC_ZDET_ELECT_RESULT__POR	(0x00)
#define MSM8X16_WCD_A_ANALOG_TX_1_EN			(0x160)
#define MSM8X16_WCD_A_ANALOG_TX_1_EN__POR			(0x43)
#define MSM8X16_WCD_A_ANALOG_TX_2_EN			(0x161)
#define MSM8X16_WCD_A_ANALOG_TX_2_EN__POR			(0x43)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TEST_CTL_1		(0x162)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TEST_CTL_1__POR		(0xBF)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TEST_CTL_2		(0x163)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TEST_CTL_2__POR		(0x8C)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_ATEST_CTL		(0x164)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_ATEST_CTL__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_OPAMP_BIAS		(0x165)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_OPAMP_BIAS__POR		(0x6B)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TXFE_CLKDIV		(0x166)
#define MSM8X16_WCD_A_ANALOG_TX_1_2_TXFE_CLKDIV__POR		(0x51)
#define MSM8X16_WCD_A_ANALOG_TX_3_EN			(0x167)
#define MSM8X16_WCD_A_ANALOG_TX_3_EN__POR			(0x42)
#define MSM8X16_WCD_A_ANALOG_NCP_EN			(0x180)
#define MSM8X16_WCD_A_ANALOG_NCP_EN__POR			(0x26)
#define MSM8X16_WCD_A_ANALOG_NCP_CLK			(0x181)
#define MSM8X16_WCD_A_ANALOG_NCP_CLK__POR			(0x23)
#define MSM8X16_WCD_A_ANALOG_NCP_DEGLITCH		(0x182)
#define MSM8X16_WCD_A_ANALOG_NCP_DEGLITCH__POR			(0x5B)
#define MSM8X16_WCD_A_ANALOG_NCP_FBCTRL			(0x183)
#define MSM8X16_WCD_A_ANALOG_NCP_FBCTRL__POR			(0x08)
#define MSM8X16_WCD_A_ANALOG_NCP_BIAS			(0x184)
#define MSM8X16_WCD_A_ANALOG_NCP_BIAS__POR			(0x29)
#define MSM8X16_WCD_A_ANALOG_NCP_VCTRL			(0x185)
#define MSM8X16_WCD_A_ANALOG_NCP_VCTRL__POR			(0x24)
#define MSM8X16_WCD_A_ANALOG_NCP_TEST			(0x186)
#define MSM8X16_WCD_A_ANALOG_NCP_TEST__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_NCP_CLIM_ADDR		(0x187)
#define MSM8X16_WCD_A_ANALOG_NCP_CLIM_ADDR__POR			(0xD5)
#define MSM8X16_WCD_A_ANALOG_RX_CLOCK_DIVIDER		(0x190)
#define MSM8X16_WCD_A_ANALOG_RX_CLOCK_DIVIDER__POR		(0xE8)
#define MSM8X16_WCD_A_ANALOG_RX_COM_OCP_CTL		(0x191)
#define MSM8X16_WCD_A_ANALOG_RX_COM_OCP_CTL__POR		(0xCF)
#define MSM8X16_WCD_A_ANALOG_RX_COM_OCP_COUNT		(0x192)
#define MSM8X16_WCD_A_ANALOG_RX_COM_OCP_COUNT__POR		(0x6E)
#define MSM8X16_WCD_A_ANALOG_RX_COM_BIAS_DAC		(0x193)
#define MSM8X16_WCD_A_ANALOG_RX_COM_BIAS_DAC__POR		(0x10)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_PA		(0x194)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_PA__POR		(0x5A)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_LDO_OCP	(0x195)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_LDO_OCP__POR		(0x69)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_CNP		(0x196)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_BIAS_CNP__POR		(0x29)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_EN		(0x197)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_EN__POR			(0x80)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_WG_CTL		(0x198)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_WG_CTL__POR		(0xDA)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_WG_TIME		(0x199)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_CNP_WG_TIME__POR		(0x16)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_L_TEST		(0x19A)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_L_TEST__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_L_PA_DAC_CTL	(0x19B)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_L_PA_DAC_CTL__POR		(0x20)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_R_TEST		(0x19C)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_R_TEST__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_R_PA_DAC_CTL	(0x19D)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_R_PA_DAC_CTL__POR		(0x20)
#define MSM8X16_WCD_A_ANALOG_RX_EAR_CTL			(0x19E)
#define MSM8X16_WCD_A_ANALOG_RX_EAR_CTL___POR			(0x32)
#define MSM8X16_WCD_A_ANALOG_RX_ATEST			(0x19F)
#define MSM8X16_WCD_A_ANALOG_RX_ATEST__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_STATUS		(0x1A0)
#define MSM8X16_WCD_A_ANALOG_RX_HPH_STATUS__POR			(0x0C)
#define MSM8X16_WCD_A_ANALOG_RX_EAR_STATUS		(0x1A1)
#define MSM8X16_WCD_A_ANALOG_RX_EAR_STATUS__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_SPKR_DAC_CTL		(0x1B0)
#define MSM8X16_WCD_A_ANALOG_SPKR_DAC_CTL__POR			(0x83)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_CLIP_DET		(0x1B1)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_CLIP_DET__POR		(0x91)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_CTL		(0x1B2)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_CTL__POR			(0x29)
#define MSM8X16_WCD_A_ANALOG_SPKR_ANA_BIAS_SET		(0x1B3)
#define MSM8X16_WCD_A_ANALOG_SPKR_ANA_BIAS_SET__POR		(0x4D)
#define MSM8X16_WCD_A_ANALOG_SPKR_OCP_CTL		(0x1B4)
#define MSM8X16_WCD_A_ANALOG_SPKR_OCP_CTL__POR			(0xE1)
#define MSM8X16_WCD_A_ANALOG_SPKR_PWRSTG_CTL		(0x1B5)
#define MSM8X16_WCD_A_ANALOG_SPKR_PWRSTG_CTL__POR		(0x1E)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_MISC		(0x1B6)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_MISC__POR			(0xCB)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_DBG		(0x1B7)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_DBG__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_CURRENT_LIMIT		(0x1C0)
#define MSM8X16_WCD_A_ANALOG_CURRENT_LIMIT__POR			(0x02)
#define MSM8X16_WCD_A_ANALOG_OUTPUT_VOLTAGE		(0x1C1)
#define MSM8X16_WCD_A_ANALOG_OUTPUT_VOLTAGE__POR		(0x14)
#define MSM8X16_WCD_A_ANALOG_BYPASS_MODE		(0x1C2)
#define MSM8X16_WCD_A_ANALOG_BYPASS_MODE__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_BOOST_EN_CTL		(0x1C3)
#define MSM8X16_WCD_A_ANALOG_BOOST_EN_CTL__POR			(0x1F)
#define MSM8X16_WCD_A_ANALOG_SLOPE_COMP_IP_ZERO		(0x1C4)
#define MSM8X16_WCD_A_ANALOG_SLOPE_COMP_IP_ZERO__POR		(0x8C)
#define MSM8X16_WCD_A_ANALOG_RDSON_MAX_DUTY_CYCLE	(0x1C5)
#define MSM8X16_WCD_A_ANALOG_RDSON_MAX_DUTY_CYCLE__POR		(0xC0)
#define MSM8X16_WCD_A_ANALOG_BOOST_TEST1_1		(0x1C6)
#define MSM8X16_WCD_A_ANALOG_BOOST_TEST1_1__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_BOOST_TEST_2		(0x1C7)
#define MSM8X16_WCD_A_ANALOG_BOOST_TEST_2__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_SPKR_SAR_STATUS		(0x1C8)
#define MSM8X16_WCD_A_ANALOG_SPKR_SAR_STATUS__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_STATUS		(0x1C9)
#define MSM8X16_WCD_A_ANALOG_SPKR_DRV_STATUS__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_PBUS_ADD_CSR		(0x1CE)
#define MSM8X16_WCD_A_ANALOG_PBUS_ADD_CSR__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_PBUS_ADD_SEL		(0x1CF)
#define MSM8X16_WCD_A_ANALOG_PBUS_ADD_SEL__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_SEC_ACCESS			(0x1D0)
#define MSM8X16_WCD_A_ANALOG_SEC_ACCESS__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL1		(0x1D8)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL1__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL2		(0x1D9)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL2__POR		(0x01)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL3		(0x1DA)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL3__POR		(0x05)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL4		(0x1DB)
#define MSM8X16_WCD_A_ANALOG_PERPH_RESET_CTL4__POR		(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_TEST1			(0x1E0)
#define MSM8X16_WCD_A_ANALOG_INT_TEST1__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_INT_TEST_VAL		(0x1E1)
#define MSM8X16_WCD_A_ANALOG_INT_TEST_VAL__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_TRIM_NUM			(0x1F0)
#define MSM8X16_WCD_A_ANALOG_TRIM_NUM__POR			(0x04)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL1			(0x1F1)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL1__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL2			(0x1F2)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL2__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL3			(0x1F3)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL3__POR			(0x00)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL4			(0x1F4)
#define MSM8X16_WCD_A_ANALOG_TRIM_CTRL4__POR			(0x00)

#define MSM8X16_WCD_A_CDC_CLK_RX_RESET_CTL			(0x200)
#define MSM8X16_WCD_A_CDC_CLK_RX_RESET_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CLK_TX_RESET_B1_CTL			(0x204)
#define MSM8X16_WCD_A_CDC_CLK_TX_RESET_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_CLK_DMIC_B1_CTL			(0x208)
#define MSM8X16_WCD_A_CDC_CLK_DMIC_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CLK_RX_I2S_CTL			(0x20C)
#define MSM8X16_WCD_A_CDC_CLK_RX_I2S_CTL__POR				(0x13)
#define MSM8X16_WCD_A_CDC_CLK_TX_I2S_CTL			(0x210)
#define MSM8X16_WCD_A_CDC_CLK_TX_I2S_CTL__POR				(0x13)
#define MSM8X16_WCD_A_CDC_CLK_OTHR_RESET_B1_CTL			(0x214)
#define MSM8X16_WCD_A_CDC_CLK_OTHR_RESET_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_CLK_TX_CLK_EN_B1_CTL			(0x218)
#define MSM8X16_WCD_A_CDC_CLK_TX_CLK_EN_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_CLK_OTHR_CTL			(0x21C)
#define MSM8X16_WCD_A_CDC_CLK_OTHR_CTL__POR				(0x04)
#define MSM8X16_WCD_A_CDC_CLK_RX_B1_CTL			(0x220)
#define MSM8X16_WCD_A_CDC_CLK_RX_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CLK_MCLK_CTL			(0x224)
#define MSM8X16_WCD_A_CDC_CLK_MCLK_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CLK_PDM_CTL			(0x228)
#define MSM8X16_WCD_A_CDC_CLK_PDM_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CLK_SD_CTL			(0x22C)
#define MSM8X16_WCD_A_CDC_CLK_SD_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_B1_CTL			(0x240)
#define MSM8X16_WCD_A_CDC_RX1_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX2_B1_CTL			(0x260)
#define MSM8X16_WCD_A_CDC_RX2_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX3_B1_CTL			(0x280)
#define MSM8X16_WCD_A_CDC_RX3_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_B2_CTL			(0x244)
#define MSM8X16_WCD_A_CDC_RX1_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX2_B2_CTL			(0x264)
#define MSM8X16_WCD_A_CDC_RX2_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX3_B2_CTL			(0x284)
#define MSM8X16_WCD_A_CDC_RX3_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_B3_CTL			(0x248)
#define MSM8X16_WCD_A_CDC_RX1_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX2_B3_CTL			(0x268)
#define MSM8X16_WCD_A_CDC_RX2_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX3_B3_CTL			(0x288)
#define MSM8X16_WCD_A_CDC_RX3_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_B4_CTL			(0x24C)
#define MSM8X16_WCD_A_CDC_RX1_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX2_B4_CTL			(0x26C)
#define MSM8X16_WCD_A_CDC_RX2_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX3_B4_CTL			(0x28C)
#define MSM8X16_WCD_A_CDC_RX3_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_B5_CTL			(0x250)
#define MSM8X16_WCD_A_CDC_RX1_B5_CTL__POR				(0x68)
#define MSM8X16_WCD_A_CDC_RX2_B5_CTL			(0x270)
#define MSM8X16_WCD_A_CDC_RX2_B5_CTL__POR				(0x68)
#define MSM8X16_WCD_A_CDC_RX3_B5_CTL			(0x290)
#define MSM8X16_WCD_A_CDC_RX3_B5_CTL__POR				(0x68)
#define MSM8X16_WCD_A_CDC_RX1_B6_CTL			(0x254)
#define MSM8X16_WCD_A_CDC_RX1_B6_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX2_B6_CTL			(0x274)
#define MSM8X16_WCD_A_CDC_RX2_B6_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX3_B6_CTL			(0x294)
#define MSM8X16_WCD_A_CDC_RX3_B6_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B1_CTL			(0x258)
#define MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B1_CTL			(0x278)
#define MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B1_CTL			(0x298)
#define MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL			(0x25C)
#define MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL			(0x27C)
#define MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL			(0x29C)
#define MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_TOP_GAIN_UPDATE			(0x2A0)
#define MSM8X16_WCD_A_CDC_TOP_GAIN_UPDATE__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TOP_CTL				(0x2A4)
#define MSM8X16_WCD_A_CDC_TOP_CTL__POR					(0x01)
#define MSM8X16_WCD_A_CDC_DEBUG_DESER1_CTL			(0x2E0)
#define MSM8X16_WCD_A_CDC_DEBUG_DESER1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_DEBUG_DESER2_CTL			(0x2E4)
#define MSM8X16_WCD_A_CDC_DEBUG_DESER2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_DEBUG_B1_CTL_CFG			(0x2E8)
#define MSM8X16_WCD_A_CDC_DEBUG_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_DEBUG_B2_CTL_CFG			(0x2EC)
#define MSM8X16_WCD_A_CDC_DEBUG_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_DEBUG_B3_CTL_CFG			(0x2F0)
#define MSM8X16_WCD_A_CDC_DEBUG_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B1_CTL			(0x300)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B1_CTL			(0x340)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B2_CTL			(0x304)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B2_CTL			(0x344)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B3_CTL			(0x308)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B3_CTL			(0x348)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B4_CTL			(0x30C)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B4_CTL			(0x34C)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B5_CTL			(0x310)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B5_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B5_CTL			(0x350)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B5_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B6_CTL			(0x314)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B6_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B6_CTL			(0x354)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B6_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B7_CTL			(0x318)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B7_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B7_CTL			(0x358)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B7_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B8_CTL			(0x31C)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_B8_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B8_CTL			(0x35C)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_B8_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_CTL			(0x320)
#define MSM8X16_WCD_A_CDC_IIR1_CTL__POR				(0x40)
#define MSM8X16_WCD_A_CDC_IIR2_CTL			(0x360)
#define MSM8X16_WCD_A_CDC_IIR2_CTL__POR				(0x40)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_TIMER_CTL			(0x324)
#define MSM8X16_WCD_A_CDC_IIR1_GAIN_TIMER_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_TIMER_CTL			(0x364)
#define MSM8X16_WCD_A_CDC_IIR2_GAIN_TIMER_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_COEF_B1_CTL			(0x328)
#define MSM8X16_WCD_A_CDC_IIR1_COEF_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_COEF_B1_CTL			(0x368)
#define MSM8X16_WCD_A_CDC_IIR2_COEF_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR1_COEF_B2_CTL			(0x32C)
#define MSM8X16_WCD_A_CDC_IIR1_COEF_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_IIR2_COEF_B2_CTL			(0x36C)
#define MSM8X16_WCD_A_CDC_IIR2_COEF_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B1_CTL			(0x380)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B2_CTL			(0x384)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B3_CTL			(0x388)
#define MSM8X16_WCD_A_CDC_CONN_RX1_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B1_CTL			(0x38C)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B2_CTL			(0x390)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B3_CTL			(0x394)
#define MSM8X16_WCD_A_CDC_CONN_RX2_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX3_B1_CTL			(0x398)
#define MSM8X16_WCD_A_CDC_CONN_RX3_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_RX3_B2_CTL			(0x39C)
#define MSM8X16_WCD_A_CDC_CONN_RX3_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_TX_B1_CTL			(0x3A0)
#define MSM8X16_WCD_A_CDC_CONN_TX_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B1_CTL			(0x3A8)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B2_CTL			(0x3AC)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B3_CTL			(0x3B0)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B4_CTL			(0x3B4)
#define MSM8X16_WCD_A_CDC_CONN_EQ1_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B1_CTL			(0x3B8)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B1_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B2_CTL			(0x3BC)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B2_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B3_CTL			(0x3C0)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B3_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B4_CTL			(0x3C4)
#define MSM8X16_WCD_A_CDC_CONN_EQ2_B4_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_CONN_TX_I2S_SD1_CTL			(0x3C8)
#define MSM8X16_WCD_A_CDC_CONN_TX_I2S_SD1_CTL__POR			(0x00)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_TIMER			(0x480)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_TIMER__POR			(0x00)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_TIMER			(0x4A0)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_TIMER__POR			(0x00)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN			(0x484)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN			(0x4A4)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_CFG			(0x488)
#define MSM8X16_WCD_A_CDC_TX1_VOL_CTL_CFG__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_CFG			(0x4A8)
#define MSM8X16_WCD_A_CDC_TX2_VOL_CTL_CFG__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX1_MUX_CTL			(0x48C)
#define MSM8X16_WCD_A_CDC_TX1_MUX_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX2_MUX_CTL			(0x4AC)
#define MSM8X16_WCD_A_CDC_TX2_MUX_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX1_CLK_FS_CTL			(0x490)
#define MSM8X16_WCD_A_CDC_TX1_CLK_FS_CTL__POR				(0x03)
#define MSM8X16_WCD_A_CDC_TX2_CLK_FS_CTL			(0x4B0)
#define MSM8X16_WCD_A_CDC_TX2_CLK_FS_CTL__POR				(0x03)
#define MSM8X16_WCD_A_CDC_TX1_DMIC_CTL			(0x494)
#define MSM8X16_WCD_A_CDC_TX1_DMIC_CTL__POR				(0x00)
#define MSM8X16_WCD_A_CDC_TX2_DMIC_CTL			(0x4B4)
#define MSM8X16_WCD_A_CDC_TX2_DMIC_CTL__POR				(0x00)
#endif
