/*
 * Copyright (c) 2015, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header: //depot/release/Embedded/Components/Qorvo/HAL_PLATFORM/v2.10.2.1/comps/halCortexM4/k8e/src/hal_timer.c#1 $
 * $Change: 189026 $
 * $DateTime: 2022/01/18 14:46:53 $
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "gpHal_reg.h"
#include "gpLog.h"
#include "hal_timer.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/* Workaround for missing macro */
#define GP_WB_CHECK_OFFSET_TIMER(offset) do{}while(0)
/*****************************************************************************
 *                    Static Data
 *****************************************************************************/
static UInt8 halTimer_timerInUse;
static halTimer_cbTimerWrapInterruptHandler_t halTimer_cbIntHandler[MAX_NOF_TIMER];
static void halTimer_clrTimerWrapInterrupt(halTimer_timerId_t timerId);
static INLINE UInt32 halTimer_getBaseAddress(halTimer_timerId_t timerId);
/*****************************************************************************
 *                    Interrupt handler
 *****************************************************************************/
void timer_handler_impl(void)
{
    UInt8 i = 0;
    UInt8 maskedTimerInt;

    HAL_DISABLE_GLOBAL_INT();

    /*
     * Since there is a single interrupt triggered for all the
     * timers, we have to loop through the masked interrupts from
     * all timers to see which triggered.
     */
    maskedTimerInt = GP_WB_READ_INT_CTRL_MASKED_TIMERS_INTERRUPTS();
    while (maskedTimerInt != 0)
    {
        if((maskedTimerInt & (1)) != 0)
        {
            /* Make sure this timer is in use */
            GP_ASSERT_DEV_INT((halTimer_timerInUse & (1 << i)) != 0);
            halTimer_stopTimer(i);

            /* Call its callback handler */
            /* Only timerId with registered callbacks have interrupts enabled */
            GP_ASSERT_DEV_INT(halTimer_cbIntHandler[i] != NULL);
            halTimer_cbIntHandler[i]();

            halTimer_clrTimerWrapInterrupt(i);
        }
        maskedTimerInt >>= 1;
        i++;
    }
    HAL_ENABLE_GLOBAL_INT();
}

/*****************************************************************************
 *                    Private Function Definitions
 *****************************************************************************/

static INLINE UInt32 halTimer_getBaseAddress(halTimer_timerId_t timerId)
{
    return GP_WB_TIMERS_TMR0_PRESCALER_DIV_ADDRESS +
              (UInt32)timerId * (GP_WB_TIMERS_TMR1_PRESCALER_DIV_ADDRESS - GP_WB_TIMERS_TMR0_PRESCALER_DIV_ADDRESS);
}

static void halTimer_clrTimerWrapInterrupt(halTimer_timerId_t timerId)
{
    GP_WB_TIMER_CLR_WRAP_INTERRUPT(halTimer_getBaseAddress(timerId));
}

static void halTimer_setMaskTimerWrapInterrupt(halTimer_timerId_t timerId, UInt8 val)
{
    GP_WB_RANGE_CHECK(val, 0x01);
    GP_WB_MWRITE_U1(GP_WB_INT_CTRL_MASK_TIMERS_TMR0_WRAP_INTERRUPT_ADDRESS,
            timerId, (val));
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
UInt16 halTimer_getTimerValue(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    return GP_WB_READ_TIMER_VALUE(halTimer_getBaseAddress(timerId));
}

void halTimer_setPrescalerDiv(halTimer_timerId_t timerId, UInt16 val)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    GP_WB_WRITE_TIMER_PRESCALER_DIV(halTimer_getBaseAddress(timerId), val);
}

void halTimer_setClkSel(halTimer_timerId_t timerId, halTimer_clkSel_t val)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    GP_WB_WRITE_TIMER_CLK_SEL(halTimer_getBaseAddress(timerId), val);
}

void halTimer_setThreshold(halTimer_timerId_t timerId, UInt16 threshold)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    GP_WB_WRITE_TIMER_THRESHOLD(halTimer_getBaseAddress(timerId), threshold);
}

UInt16 halTimer_getThreshold(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    return GP_WB_READ_TIMER_THRESHOLD(halTimer_getBaseAddress(timerId));
}

void halTimer_setTimerPreset(halTimer_timerId_t timerId, UInt16 val)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    GP_WB_WRITE_TIMERS_TMR_PRESET_VALUE(val);
    GP_WB_TIMER_PRESET(halTimer_getBaseAddress(timerId));
}

void halTimer_enableTimer(halTimer_timerId_t timerId, UInt8 val)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    GP_WB_RANGE_CHECK(val, 0x01);
    GP_WB_MWRITE_U1(GP_WB_TIMERS_TMR0_ENABLE_ADDRESS, timerId, val);
}

void halTimer_startTimer(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    halTimer_enableTimer(timerId, 1);
}

void halTimer_stopTimer(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    halTimer_enableTimer(timerId, 0);
}

void halTimer_resetTimer(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    halTimer_setTimerPreset(timerId, 0);
}

void halTimer_Init()
{
    /* clear global variables */
    halTimer_timerInUse = 0;

    /* Enable timer interrupts */
    NVIC_EnableIRQ(TIMERS_IRQn);
    GP_WB_WRITE_INT_CTRL_MASK_INT_TIMERS_INTERRUPT(1);
}

void halTimer_initTimer(halTimer_timerId_t timerId,
        UInt8 prescalerDiv,
        halTimer_clkSel_t clkSel,
        UInt16 threshold,
        halTimer_cbTimerWrapInterruptHandler_t Inthandler)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    /* reserve timer */
    /* assert timer is free */
    GP_ASSERT_DEV_INT((halTimer_timerInUse & (1<<timerId)) == 0);
    halTimer_timerInUse |= (1<<timerId);

    /* Reset the timer */
    halTimer_stopTimer(timerId);
    halTimer_resetTimer(timerId);
    halTimer_clrTimerWrapInterrupt(timerId);

    /* configure timer */
    halTimer_setPrescalerDiv(timerId, prescalerDiv);
    /* timer 0 can have as clkSel only intclk
     * timer 1 can have as clkSel intclk and timer0
     * and so on*/
    GP_ASSERT_DEV_INT(clkSel <= timerId);
    halTimer_setClkSel(timerId, clkSel);
    halTimer_setThreshold(timerId, threshold);

    if(Inthandler == NULL) {
        /* No callback registered, mask the interrupt */
        halTimer_setMaskTimerWrapInterrupt(timerId, 0);
    } else {
        /* store callback handler */
        halTimer_setMaskTimerWrapInterrupt(timerId, 1);
        halTimer_cbIntHandler[timerId] = Inthandler;
    }
}

void halTimer_freeTimer(halTimer_timerId_t timerId)
{
    GP_ASSERT_DEV_INT(timerId < MAX_NOF_TIMER);
    /* assert timer is in use */
    GP_ASSERT_DEV_INT((halTimer_timerInUse & (1<<timerId)) != 0);
    halTimer_timerInUse &= ~(1 << timerId);

    halTimer_resetTimer(timerId);
    halTimer_setMaskTimerWrapInterrupt(timerId, 0);
}
