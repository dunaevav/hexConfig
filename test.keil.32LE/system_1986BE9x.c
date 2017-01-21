/**
  ******************************************************************************
  * @file    system_1986BE9x.c
  * @author  Phyton Application Team
  * @version V1.0.0
  * @date    11/06/2010
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  ******************************************************************************
  * <br><br>
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, PHYTON SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 Phyton</center></h2>
  ******************************************************************************
  * FILE system_1986BE9x.c
  */


/** @addtogroup __CMSIS CMSIS
  * @{
  */

/** @addtogroup __1986BE9X 1986BE9X
  * @{
  */

/** @addtogroup System_Private_Includes System Private Includes
  * @{
  */

#include "1986BE91.h"

/* Target system parameters */
/* RST_CLK generators frequencies in HZ */
#define HSI_Value       ((uint32_t)8000000)
#define HSE_Value       ((uint32_t)8000000)
#define LSI_Value       ((uint32_t)40000)
#define LSE_Value       ((uint32_t)32768)


#define uint32_t unsigned int
//#include "1986BE9x_config.h"

/** @} */ /* End of group System_Private_Includes */

/** @addtogroup __1986BE9X_System_Private_Variables 1986BE9X System Private Variables
  * @{
  */

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
uint32_t SystemCoreClock = (uint32_t)8000000;         /*!< System Clock Frequency (Core Clock)
                                                         *   default value */

/** @} */ /* End of group __1986BE9X_System_Private_Variables */

/** @addtogroup __1986BE9X_System_Private_Functions 1986BE9X System Private Functions
  * @{
  */

/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  * @note   None
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
    uint32_t cpu_c1_freq, cpu_c2_freq, cpu_c3_freq;
    uint32_t pll_mul;

    /* Compute CPU_CLK frequency */

    /* Determine CPU_C1 frequency */
    if ((RST_CLK->CPU_CLOCK & (uint32_t)0x00000002) == (uint32_t)0x00000002)
        cpu_c1_freq = HSE_Value;
    else
        cpu_c1_freq = HSI_Value;

    if ((RST_CLK->CPU_CLOCK & (uint32_t)0x00000001) == (uint32_t)0x00000001)
        cpu_c1_freq /= 2;

    /* Determine CPU_C2 frequency */
    cpu_c2_freq = cpu_c1_freq;

    if ((RST_CLK->CPU_CLOCK & (uint32_t)0x00000004) == (uint32_t)0x00000004)
    {
        /* Determine CPU PLL output frequency */
        pll_mul = ((RST_CLK->PLL_CONTROL >> 4) & (uint32_t)0x0F) + 1;
        cpu_c2_freq *= pll_mul;
    }

    /*Select CPU_CLK from HSI, CPU_C3, LSE, LSI cases */
    switch ((RST_CLK->PLL_CONTROL >> 8) & (uint32_t)0x03)
    {
    case 0 :
        /* HSI */
        SystemCoreClock = HSI_Value;
        break;
    case 1 :
        /* CPU_C3 */
        /* Determine CPU_C3 frequency */
        cpu_c3_freq = cpu_c2_freq / ((RST_CLK->CPU_CLOCK >> 4 & (uint32_t)0x0F) + 1);
        SystemCoreClock = cpu_c3_freq;
        break;
    case 2 :
        /* LSE */
        SystemCoreClock = LSE_Value;
        break;
    default : /* case 3 */
        /* LSI */
        SystemCoreClock = LSI_Value;
        break;
    }
}

/**
  * @brief  Setup the microcontroller system
  *         RST clock configuration to the default reset state
  *         Setup SystemCoreClock variable.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
    /* Reset the RST clock configuration to the default reset state */

    /* Reset all clock but RST_CLK & BKP_CLC bits */
    RST_CLK->PER_CLOCK   = (uint32_t)0x8000010;

    /* Reset REG_0F bits to zero but HSION & LSION bits */
    BKP->REG_0F |= (uint32_t)((1 << 22) | (1 << 15)); /* HSION & LSION */
    BKP->REG_0F &= (uint32_t)((1 << 22) | (1 << 15));

    /* Reset CPU_CLOCK bits */
    RST_CLK->CPU_CLOCK   &= (uint32_t)0x00000000;

    /* Reset PLL_CONTROL bits */
    RST_CLK->PLL_CONTROL &= (uint32_t)0x00000000;

    /* Reset HSEON and HSEBYP bits */
    RST_CLK->HS_CONTROL  &= (uint32_t)0x00000000;

    /* Reset USB_CLOCK bits */
    RST_CLK->USB_CLOCK   &= (uint32_t)0x00000000;

    /* Reset ADC_MCO_CLOCK bits */
    RST_CLK->ADC_MCO_CLOCK   &= (uint32_t)0x00000000;

    /* Reset RTC_CLOCK bits */
    RST_CLK->RTC_CLOCK   &= (uint32_t)0x00000000;

    SystemCoreClockUpdate();
}

/** @} */ /* End of group __1986BE9X_System_Private_Functions */

/** @} */ /* End of group __1986BE9X */

/** @} */ /* End of group __CMSIS */

/******************* (C) COPYRIGHT 2010 Phyton *********************************
*
* END OF FILE system_1986BE9x.c */
