/******************************************************************************
 * Copyright (c) 2022 Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <los_task.h>
#include <los_arch_interrupt.h>

#include <hiview_log.h>

#include <ohos_init.h>
#include <ohos_types.h>

#include <b91_irq.h>
#include <riscv_hal.h>

#include <B91/gpio.h>

#include <stack/ble/ble.h>

#include "app_config.h"
#include "app.h"

#define RTOS_SLEEP_MS(__MS__) LOS_Msleep(__MS__)
#define RTOS_TASK_YIELD() LOS_TaskYield()

#define LED_TASK_PRIORITY LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO
#define PROTO_TASK_PRIORITY (OS_TASK_PRIORITY_LOWEST-1)

static void BleTask(const char *pvParameters)
{
    UNUSED(pvParameters);

    /*
     * Enable STimer IRQ trigger if value in level register is below tick.
     * Without this line STimer interrupt will be missed if interrupts were disabled when 
     * COUNT register reached value of LEVEL register.
     */
    reg_system_irq_mask |= FLD_SYSTEM_TRIG_PAST_EN;

    HILOG_INFO(HILOG_MODULE_APP, "%s:%d", __func__, __LINE__);

    while(1) {
        MainLoop();
    }
}

/**
 * @brief		BLE RF interrupt handler.
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void RfIrqHandler(void)
{
    blc_sdk_irq_handler ();
}

/**
 * @brief		System timer interrupt handler.
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void StimerIrqHandler(void)
{
    blc_sdk_irq_handler ();
}

void BleSampleInit(void)
{
    rf_drv_ble_init();

    /* load customized freq_offset cap value. */
    blc_app_loadCustomizedParameters();
    UserInitNormal();

    UINT32 ret;
    UINT32 taskId = 0;
    TSK_INIT_PARAM_S taskParam = {0};
    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)BleTask;
    taskParam.uwArg = 0;
    taskParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.pcName = "BleTask";
    taskParam.usTaskPrio = PROTO_TASK_PRIORITY;
    ret = LOS_TaskCreate(&taskId, &taskParam);
    if (ret != LOS_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "ret of LOS_TaskCreate(BleTask) = %#x", ret);
    }

    B91IrqRegister(IRQ15_ZB_RT, (HWI_PROC_FUNC)RfIrqHandler, 0);
    B91IrqRegister(IRQ1_SYSTIMER, (HWI_PROC_FUNC)StimerIrqHandler, 0);
}

SYS_RUN(BleSampleInit);
