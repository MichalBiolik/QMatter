/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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
 * $Header: //depot/release/Embedded/Components/Qorvo/BleController/v2.10.2.0/comps/gpBleLlcp/src/gpBleLlcp_Vsd.c#1 $
 * $Change: 187624 $
 * $DateTime: 2021/12/20 10:58:50 $
 *
 */

//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLELLCP

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle.h"
#include "gpSched.h"
#include "gpBleComps.h"
#include "gpBleConfig.h"
#include "gpBle_defs.h"
#include "gpBleLlcp.h"
#include "gpBleLlcpProcedures.h"
#include "gpLog.h"
#include "gpHal.h"
#include "hal.h"

#ifdef GP_DIVERSITY_BLE_SLAVE
#include "gpBleAdvertiser.h"
#endif //GP_DIVERSITY_BLE_SLAVE

#ifdef GP_DIVERSITY_BLE_MASTER
#include "gpBleScanner.h"
#endif //GP_DIVERSITY_BLE_MASTER

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// VSD helpers
#ifdef GP_DIVERSITY_DEVELOPMENT
#ifdef GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdWinoffsetHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdDataRxFlushtEndOfEventHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t Ble_SetVsdAuthenticatedPayloadTimeoutEnableHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t Ble_SetVsdGeneratePacketWithCorruptedMIC_OnConnIdHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#ifdef GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdConnReqWinSizeHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdMasterConnEstabFirstMToSSignedOffsetHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#ifdef GP_DIVERSITY_DEVELOPMENT
static gpHci_Result_t Ble_SetVsdAutomaticFeatureExchangeEnableHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif
#ifdef GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdSetSleepClockAccuracyHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t Ble_SetVsdArtificialDriftAsSignedNbrMicrosecHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t Ble_SetVsdFeatureSetUsedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t Ble_SetVsdTriggerForAnchorMoveHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif /* GP_DIVERSITY_DEVELOPMENT */
static gpHci_Result_t gpBle_SetVsdDualModeTimeFor15Dot4Helper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t gpBle_SetVsdProcessorClockSpeedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

#ifdef GP_DIVERSITY_DEVELOPMENT
static gpHci_Result_t gpBle_SetVsdConnectionEventPriorityHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#ifdef GP_DIVERSITY_BLE_MASTER
static gpHci_Result_t gpBle_SetVsdScanEventPriorityHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif // GP_DIVERSITY_BLE_MASTER
#ifdef GP_DIVERSITY_BLE_SLAVE
static gpHci_Result_t gpBle_SetVsdAdvEventPriorityHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif // GP_DIVERSITY_BLE_SLAVE
static void Ble_SetVsdLLCPFeatureSetUsed(UInt8 connId, UInt8 LE_NewFeatureSetUsed);
static gpHci_Result_t gpBle_SetVsdDataChannelRxQueueLatencyHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif //GP_DIVERSITY_DEVELOPMENT

static gpHci_Result_t Ble_SetSleepAction(gpHci_SleepMode_t hciSleepMode, Bool enable);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_DEVELOPMENT


#ifdef GP_DIVERSITY_BLE_MASTER
gpHci_Result_t Ble_SetVsdWinoffsetHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 winOffset;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&winOffset, pParams->SetVsdTestParams.value, sizeof(Int16));
    return Ble_SetVsdWinoffset(winOffset);
}
#endif //GP_DIVERSITY_BLE_MASTER

gpHci_Result_t Ble_SetVsdDataRxFlushtEndOfEventHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdDataRxFlushAtEndOfEvent((UInt8)*pParams->SetVsdTestParams.value);

    return gpHci_ResultSuccess;
}

gpHci_Result_t Ble_SetVsdAuthenticatedPayloadTimeoutEnableHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdAuthenticatedPayloadTimeoutEnable((UInt8)*pParams->SetVsdTestParams.value);

    return gpHci_ResultSuccess;
}

gpHci_Result_t Ble_SetVsdGeneratePacketWithCorruptedMIC_OnConnIdHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 HciHandle;
    Ble_IntConnId_t connId;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&HciHandle, pParams->SetVsdTestParams.value, sizeof(UInt16));
    connId = gpBleLlcp_HciHandleToIntHandle(HciHandle);
    Ble_SetVsdGeneratePacketWithCorruptedMIC_OnConnId(connId);

    return gpHci_ResultSuccess;
}

#ifdef GP_DIVERSITY_BLE_MASTER
gpHci_Result_t Ble_SetVsdConnReqWinSizeHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdConnReqWinSize((UInt8)*pParams->SetVsdTestParams.value);

    return gpHci_ResultSuccess;
}
#endif //GP_DIVERSITY_BLE_MASTER

gpHci_Result_t Ble_SetVsdMasterConnEstabFirstMToSSignedOffsetHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    Int16 SignedOffset;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&SignedOffset, pParams->SetVsdTestParams.value, sizeof(Int16));
    Ble_SetVsdMasterConnEstabFirstMToSSignedOffset(SignedOffset);

    return gpHci_ResultSuccess;
}

#ifdef GP_DIVERSITY_DEVELOPMENT
gpHci_Result_t Ble_SetVsdAutomaticFeatureExchangeEnableHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdAutomaticFeatureExchangeEnable((UInt8)*pParams->SetVsdTestParams.value);

    return gpHci_ResultSuccess;
}
#endif

#ifdef GP_DIVERSITY_BLE_MASTER
gpHci_Result_t Ble_SetVsdSetSleepClockAccuracyHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdSetSleepClockAccuracy((UInt8)*pParams->SetVsdTestParams.value);

    return gpHci_ResultSuccess;
}
#endif //GP_DIVERSITY_BLE_MASTER

gpHci_Result_t Ble_SetVsdArtificialDriftAsSignedNbrMicrosecHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    Int16 SignedDrift;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&SignedDrift, pParams->SetVsdTestParams.value, sizeof(Int16));
    Ble_SetVsdArtificialDriftAsSignedNbrMicrosec(SignedDrift);

    return gpHci_ResultSuccess;
}

gpHci_Result_t Ble_SetVsdFeatureSetUsedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_ConnectionHandle_t HciHandle;

    gpHci_Result_t result;

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&HciHandle, pParams->SetVsdTestParams.value, sizeof(gpHci_ConnectionHandle_t));

    result = gpBleLlcp_IsHostConnectionHandleValid(HciHandle);
    if(result == gpHci_ResultSuccess)
    {
        UInt8 NewLeFeatureSet;

        MEMCPY(&NewLeFeatureSet, &pParams->SetVsdTestParams.value[sizeof(gpHci_ConnectionHandle_t)], sizeof(UInt8));
        Ble_SetVsdLLCPFeatureSetUsed(gpBleLlcp_HciHandleToIntHandle(HciHandle), NewLeFeatureSet);
    }

    return gpHci_ResultSuccess;
}

gpHci_Result_t Ble_SetVsdTriggerForAnchorMoveHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_ConnectionHandle_t hciConnHandle;
    UInt16 offset;
    Ble_LlcpLinkContext_t* pContext;

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&hciConnHandle, pParams->SetVsdTestParams.value, sizeof(gpHci_ConnectionHandle_t));

    pContext = Ble_GetLinkContext(gpBleLlcp_HciHandleToIntHandle(hciConnHandle));

    if(pContext == NULL)
    {
        return gpHci_ResultUnknownConnectionIdentifier;
    }

    MEMCPY(&offset, &pParams->SetVsdTestParams.value[sizeof(gpHci_ConnectionHandle_t)], sizeof(UInt16));

    return gpBleLlcpProcedures_TriggerAnchorMove(pContext->connId, offset);
}
#endif /* GP_DIVERSITY_DEVELOPMENT */


gpHci_Result_t gpBle_SetVsdDualModeTimeFor15Dot4Helper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt32 timeFor15Dot4;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&timeFor15Dot4, pParams->SetVsdTestParams.value, sizeof(UInt32));
    Ble_SetTimeFor15Dot4(timeFor15Dot4);

    return gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_SetVsdProcessorClockSpeedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt8 clockSpeed;

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    clockSpeed = pParams->SetVsdTestParams.value[0];

    GP_LOG_SYSTEM_PRINTF("setting clock speed to setting %x",0,clockSpeed);

#ifdef GP_DIVERSITY_CORTEXM4
    HAL_SET_MCU_CLOCK_SPEED(clockSpeed);
#endif

    return gpHci_ResultSuccess;
}

#ifdef GP_DIVERSITY_DEVELOPMENT
gpHci_Result_t gpBle_SetVsdDataChannelRxQueueLatencyHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt32 latency = 0;

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    latency |= (UInt32)pParams->SetVsdTestParams.value[0] << 0;
    latency |= (UInt32)pParams->SetVsdTestParams.value[1] << 8;
    latency |= (UInt32)pParams->SetVsdTestParams.value[2] << 16;
    latency |= (UInt32)pParams->SetVsdTestParams.value[3] << 24;

    gpBle_SetVsdDataChannelRxQueueLatency(latency);

    return gpHci_ResultSuccess;
}

void Ble_SetVsdLLCPFeatureSetUsed(UInt8 connId, UInt8 LE_NewFeatureSetUsed)
{
    Ble_LlcpLinkContext_t* pContext;

    pContext = Ble_GetLinkContext(connId);

    if (pContext == NULL)
    {
        GP_LOG_PRINTF("Invalid context %u",0,connId);
    }

    MEMCPY(&pContext->featureSetLink, &LE_NewFeatureSetUsed, sizeof(LE_NewFeatureSetUsed));
}
#endif //GP_DIVERSITY_DEVELOPMENT

gpHci_Result_t Ble_SetSleepAction(gpHci_SleepMode_t hciSleepMode, Bool enable)
{
    gpHal_SleepMode_t sleepMode = 0xFF;

    if(hciSleepMode == gpHci_SleepMode_RC)
    {
        GP_LOG_SYSTEM_PRINTF("RC",0);
        sleepMode = gpHal_SleepModeRC;
    }
    else if(hciSleepMode == gpHci_SleepMode_32kHzXtal)
    {
        GP_LOG_SYSTEM_PRINTF("32 Xtal",0);
        sleepMode = gpHal_SleepMode32kHz;
    }
    else if(hciSleepMode == gpHci_SleepMode_16Mhz)
    {
        GP_LOG_SYSTEM_PRINTF("16 Xtal",0);
        sleepMode = gpHal_SleepMode16MHz;
    }
    else
    {
        // Sleep mode disabled
        GP_LOG_SYSTEM_PRINTF("Set sleep disable",0);
        gpSched_SetGotoSleepEnable(false);
        return gpHci_ResultSuccess;
    }

    GP_LOG_SYSTEM_PRINTF("set sleep mode HCI %x hal: %x",0,hciSleepMode,sleepMode);

    gpHal_SetSleepMode(sleepMode);

    if(enable)
    {
        gpSched_SetGotoSleepEnable(true);
    }

    return gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_SetVsdfixedRxWindowThresholdHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 threshold;
    if (pParams->SetVsdTestParams.length != sizeof(threshold) )
    {
        return gpHci_ResultInvalidHCICommandParameters;
    }

    MEMCPY(&threshold, pParams->SetVsdTestParams.value, sizeof(threshold));
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    gpBle_SetVsdfixedRxWindowThresholdParam( threshold );

    return gpHci_ResultSuccess;
}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/



/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/

gpHci_Result_t gpBle_SetVsdTestParams(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_Result_t result = gpHci_ResultUnsupportedFeatureOrParameterValue;
    switch(pParams->SetVsdTestParams.type)
    {
#ifdef GP_DIVERSITY_DEVELOPMENT
#ifdef GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdWinoffsetType: return Ble_SetVsdWinoffsetHelper( pParams, pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdDataRxFlushAtEndOfEventType: return Ble_SetVsdDataRxFlushtEndOfEventHelper( pParams, pEventBuf);
        case gpBle_SetVsdAuthenticatedPayloadTimeoutEnableType: return Ble_SetVsdAuthenticatedPayloadTimeoutEnableHelper( pParams, pEventBuf);
        case gpBle_SetVsdGeneratePacketWithCorruptedMIC_OnConnIdType: return Ble_SetVsdGeneratePacketWithCorruptedMIC_OnConnIdHelper( pParams, pEventBuf);
#ifdef GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdConnReqWinSizeType: return Ble_SetVsdConnReqWinSizeHelper( pParams, pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdMasterConnEstabFirstMToSSignedOffsetType: return Ble_SetVsdMasterConnEstabFirstMToSSignedOffsetHelper( pParams, pEventBuf);
        case gpBle_SetVsdAutomaticFeatureExchangeEnableType: return Ble_SetVsdAutomaticFeatureExchangeEnableHelper( pParams, pEventBuf);
#ifdef GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdSleepClockAccuracyType: return Ble_SetVsdSetSleepClockAccuracyHelper( pParams, pEventBuf);
#endif //GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdArtificialDriftAsSignedNbrMicrosecType: return Ble_SetVsdArtificialDriftAsSignedNbrMicrosecHelper( pParams, pEventBuf);
        case gpBle_SetVsdFeatureSetUsedType: return Ble_SetVsdFeatureSetUsedHelper( pParams, pEventBuf);
        case gpBle_SetVsdTriggerForAnchorMoveType: return Ble_SetVsdTriggerForAnchorMoveHelper( pParams, pEventBuf);
        case gpBle_SetVsdOverruleLocalSupportedFeatures: return gpBle_SetVsdOverruleLocalSupportedFeaturesHelper( pParams, pEventBuf);
        case gpBle_SetVsdConnectionEventPriority: return gpBle_SetVsdConnectionEventPriorityHelper( pParams, pEventBuf);
#ifdef GP_DIVERSITY_BLE_MASTER
        case gpBle_SetVsdScanEventPriority: return gpBle_SetVsdScanEventPriorityHelper( pParams, pEventBuf);
#endif
#ifdef GP_DIVERSITY_BLE_SLAVE
        case gpBle_SetVsdAdvEventPriority:  return gpBle_SetVsdAdvEventPriorityHelper( pParams, pEventBuf);
#endif // #ifdef GP_DIVERSITY_BLE_SLAVE
#endif /* GP_DIVERSITY_DEVELOPMENT */
        case gpBle_SetVsdDualModeTimeFor15Dot4Type: return gpBle_SetVsdDualModeTimeFor15Dot4Helper(pParams, pEventBuf);
        case gpBle_SetVsdProcessorClockSpeed: return gpBle_SetVsdProcessorClockSpeedHelper(pParams, pEventBuf);
#ifdef GP_DIVERSITY_DEVELOPMENT
        case gpBle_SetVsdDataChannelRxQueueLatencyType: return gpBle_SetVsdDataChannelRxQueueLatencyHelper(pParams, pEventBuf);
#endif /* GP_DIVERSITY_DEVELOPMENT */
        case gpBle_SetVsdfixedRxWindowThreshold: return gpBle_SetVsdfixedRxWindowThresholdHelper(pParams, pEventBuf);
        default:
        {
            BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);
            result=gpHci_ResultUnsupportedFeatureOrParameterValue;
        }
    }
    return result;
}

gpHci_Result_t gpBle_VsdSetSleep( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    if(pParams->VsdSetSleep.mode >= gpHci_SleepMode_Invalid)
    {
        GP_LOG_PRINTF("sleep mode %x invalid", 0, pParams->VsdSetSleep.mode);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return Ble_SetSleepAction(pParams->VsdSetSleep.mode, pParams->VsdSetSleep.enable);
}
#ifdef GP_DIVERSITY_DEVELOPMENT
gpHci_Result_t gpBle_SetVsdConnectionEventPriorityHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);
    UInt8 connId = pParams->SetVsdTestParams.value[0];
    UInt16 priority = 0;
    MEMCPY(&priority, &pParams->SetVsdTestParams.value[1], sizeof(UInt16));
    GP_LOG_PRINTF("SetVsdConnectionEventPriority connId %d priority %d ", 0, connId, priority);
    if(priority == 0xFFFF)
    {
        priority = BLE_CONN_PRIORITY;
    }
    else if(priority > 0xFF)
    {
        return gpHci_ResultInvalidHCICommandParameters;
    }
    else
    {
        priority = priority;
    }
    if(gpHal_SetConnectionPriority(connId, priority))
    {
        GP_LOG_PRINTF("SetVsdConnectionEventPriority connId %d invalid ", 0,.connId);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return gpHci_ResultSuccess;
}
#ifdef GP_DIVERSITY_BLE_MASTER
gpHci_Result_t gpBle_SetVsdScanEventPriorityHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 priority = 0;
    MEMCPY(&priority, pParams->SetVsdTestParams.value, sizeof(UInt16));
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    if(gpBleScanner_SetVsdScanEventPriority(priority))
    {
        GP_LOG_PRINTF("SetVsdScanEventPriority unsuccessful ", 0);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return gpHci_ResultSuccess;
}
#endif // GP_DIVERSITY_BLE_MASTER
#ifdef GP_DIVERSITY_BLE_SLAVE
gpHci_Result_t gpBle_SetVsdAdvEventPriorityHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 priority = 0;
    MEMCPY(&priority, pParams->SetVsdTestParams.value, sizeof(UInt16));
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);
    if(gpBleAdvertiser_SetVsdAdvEventPriority(priority))
    {
        GP_LOG_PRINTF("SetVsdAdvEventPriority unsuccessful ", 0);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return gpHci_ResultSuccess;
}
#endif // GP_DIVERSITY_BLE_SLAVE
#endif // #ifdef GP_DIVERSITY_DEVELOPMENT

#ifdef GP_DIVERSITY_DEVELOPMENT
gpHci_Result_t gpBle_VsdSetAccessCode(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    Ble_SetVsdAccessCode(true, pParams->VsdSetAccessCode.AccessCode);

    return gpHci_ResultSuccess;
}
#endif //GP_DIVERSITY_DEVELOPMENT


