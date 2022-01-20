/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Example GATT and GAP service implementations.
 *
 *  Copyright (c) 2009-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2021 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * Copyright (c) 2021, Qorvo Inc
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
 */
/*************************************************************************************************/

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "app_db.h"
#include "app_api.h"
#include "att_api.h"
#include "att_uuid.h"
#include "util/bstream.h"
#include "svc_core.h"
#include "svc_ch.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef CORE_SEC_PERMIT_READ
#define CORE_SEC_PERMIT_READ SVC_SEC_PERMIT_READ
#endif

/*! Characteristic write permissions */
#ifndef CORE_SEC_PERMIT_WRITE
#define CORE_SEC_PERMIT_WRITE SVC_SEC_PERMIT_WRITE
#endif

#ifndef CORE_DEFAULT_DEV_NAME
#define CORE_DEFAULT_DEV_NAME       "Packetcraft"
#endif //CORE_DEFAULT_DEV_NAME
/*! Length of default device name */
#ifndef CORE_DEFAULT_DEV_NAME_LEN
#define CORE_DEFAULT_DEV_NAME_LEN   6
#endif //CORE_DEFAULT_DEV_NAME_LEN

/**************************************************************************************************
 GAP group
**************************************************************************************************/

/* service */
static const uint8_t gapValSvc[] = {UINT16_TO_BYTES(ATT_UUID_GAP_SERVICE)};
static const uint16_t gapLenSvc = sizeof(gapValSvc);

/* device name characteristic */
static const uint8_t gapValDnCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GAP_DN_HDL), UINT16_TO_BYTES(ATT_UUID_DEVICE_NAME)};
static const uint16_t gapLenDnCh = sizeof(gapValDnCh);

/* device name */
static uint8_t gapValDn[ATT_DEFAULT_PAYLOAD_LEN] = CORE_DEFAULT_DEV_NAME;
static uint16_t gapLenDn = CORE_DEFAULT_DEV_NAME_LEN;

/* appearance characteristic */
static const uint8_t gapValApCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GAP_AP_HDL), UINT16_TO_BYTES(ATT_UUID_APPEARANCE)};
static const uint16_t gapLenApCh = sizeof(gapValApCh);

/* appearance */
static uint8_t gapValAp[] = {UINT16_TO_BYTES(CH_APPEAR_UNKNOWN)};
static const uint16_t gapLenAp = sizeof(gapValAp);

/* central address resolution characteristic */
static const uint8_t gapValCarCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GAP_CAR_HDL), UINT16_TO_BYTES(ATT_UUID_CAR)};
static const uint16_t gapLenCarCh = sizeof(gapValCarCh);

/* central address resolution */
static uint8_t gapValCar[] = {FALSE};
static const uint16_t gapLenCar = sizeof(gapValCar);

/* resolvable private address only characteristic */
static const uint8_t gapValRpaoCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GAP_RPAO_HDL), UINT16_TO_BYTES(ATT_UUID_RPAO)};
static const uint16_t gapLenRpaoCh = sizeof(gapValRpaoCh);

/* resolvable private address only */
static uint8_t gapValRpao[] = {0};
static const uint16_t gapLenRpao = sizeof(gapValRpao);

/* Attribute list for GAP group */
static const attsAttr_t gapList[] =
{
  {
    attPrimSvcUuid,
    (uint8_t *) gapValSvc,
    (uint16_t *) &gapLenSvc,
    sizeof(gapValSvc),
    0,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) gapValDnCh,
    (uint16_t *) &gapLenDnCh,
    sizeof(gapValDnCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attDnChUuid,
    gapValDn,
    &gapLenDn,
    sizeof(gapValDn),
    (ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
    (ATTS_PERMIT_READ | CORE_SEC_PERMIT_WRITE)
  },
  {
    attChUuid,
    (uint8_t *) gapValApCh,
    (uint16_t *) &gapLenApCh,
    sizeof(gapValApCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attApChUuid,
    gapValAp,
    (uint16_t *) &gapLenAp,
    sizeof(gapValAp),
    0,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) gapValCarCh,
    (uint16_t *) &gapLenCarCh,
    sizeof(gapValCarCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attCarChUuid,
    gapValCar,
    (uint16_t *) &gapLenCar,
    sizeof(gapValCar),
    0,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) gapValRpaoCh,
    (uint16_t *) &gapLenRpaoCh,
    sizeof(gapValRpaoCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attRpaoChUuid,
    gapValRpao,
    (uint16_t *) &gapLenRpao,
    sizeof(gapValRpao),
    0,
    ATTS_PERMIT_READ
  },
};

/* GAP group structure */
static attsGroup_t svcGapGroup =
{
  NULL,
  (attsAttr_t *) gapList,
  NULL,
  NULL,
  GAP_START_HDL,
  GAP_END_HDL
};

/**************************************************************************************************
 GATT group
**************************************************************************************************/

/* service */
static const uint8_t gattValSvc[] = {UINT16_TO_BYTES(ATT_UUID_GATT_SERVICE)};
static const uint16_t gattLenSvc = sizeof(gattValSvc);

/* service changed characteristic */
static const uint8_t gattValScCh[] = {ATT_PROP_INDICATE, UINT16_TO_BYTES(GATT_SC_HDL), UINT16_TO_BYTES(ATT_UUID_SERVICE_CHANGED)};
static const uint16_t gattLenScCh = sizeof(gattValScCh);

/* service changed */
static const uint8_t gattValSc[] = {UINT16_TO_BYTES(0x0001), UINT16_TO_BYTES(0xFFFF)};
static const uint16_t gattLenSc = sizeof(gattValSc);

/* service changed client characteristic configuration */
static uint8_t gattValScChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t gattLenScChCcc = sizeof(gattValScChCcc);

/* client supported features characteristic */
static const uint8_t gattValCsfCh[] = {ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(GATT_CSF_HDL), UINT16_TO_BYTES(ATT_UUID_CLIENT_SUPPORTED_FEATURES)};
static const uint16_t gattLenCsfCh = sizeof(gattValCsfCh);

/* client supported features */
static uint8_t gattValCsf[ATT_CSF_LEN] = { 0 };
static const uint16_t gattLenCsf = sizeof(gattValCsf);

/* database hash characteristic */
static const uint8_t gattValDbhCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GATT_DBH_HDL), UINT16_TO_BYTES(ATT_UUID_DATABASE_HASH)};
static const uint16_t gattLenDbhCh = sizeof(gattValDbhCh);

/* database hash */
static uint8_t gattValDbh[ATT_DATABASE_HASH_LEN] = { 0 };
static const uint16_t gattLenDbh = sizeof(gattValDbh);

/* server supported features characteristic */
static const uint8_t gattValSsfCh[] = {ATT_PROP_READ, UINT16_TO_BYTES(GATT_SSF_HDL), UINT16_TO_BYTES(ATT_UUID_SERVER_SUPPORTED_FEATURES)};
static const uint16_t gattLenSsfCh = sizeof(gattValSsfCh);

/*  server supported features value */
static uint8_t gattValSsf[] = {0};
static const uint16_t gattLenSsf = sizeof(gattValSsf);


/* Attribute list for GATT group */
static const attsAttr_t gattList[] =
{
  {
    attPrimSvcUuid,
    (uint8_t *) gattValSvc,
    (uint16_t *) &gattLenSvc,
    sizeof(gattValSvc),
    0,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) gattValScCh,
    (uint16_t *) &gattLenScCh,
    sizeof(gattValScCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attScChUuid,
    (uint8_t *) gattValSc,
    (uint16_t *) &gattLenSc,
    sizeof(gattValSc),
    0,
    0
  },
  {
    attCliChCfgUuid,
    gattValScChCcc,
    (uint16_t *) &gattLenScChCcc,
    sizeof(gattValScChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | CORE_SEC_PERMIT_WRITE)
  },
  {
    attChUuid,
    (uint8_t *) gattValCsfCh,
    (uint16_t *) &gattLenCsfCh,
    sizeof(gattValCsfCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attGattCsfChUuid,
    (uint8_t *)gattValCsf,
    (uint16_t *)&gattLenCsf,
    sizeof(gattValCsf),
    (ATTS_SET_READ_CBACK | ATTS_SET_WRITE_CBACK),
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  },
  {
    attChUuid,
    (uint8_t *) gattValDbhCh,
    (uint16_t *) &gattLenDbhCh,
    sizeof(gattValDbhCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attGattDbhChUuid,
    (uint8_t *) gattValDbh,
    (uint16_t *) &gattLenDbh,
    sizeof(gattValDbh),
    ATTS_SET_READ_CBACK,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) gattValSsfCh,
    (uint16_t *) &gattLenSsfCh,
    sizeof(gattValSsfCh),
    0,
    ATTS_PERMIT_READ
  },
  {
    attSsfChUuid,
    gattValSsf,
    (uint16_t *) &gattLenSsf,
    sizeof(gattValSsf),
    0,
    ATTS_PERMIT_READ
  }
};

/* GATT group structure */
static attsGroup_t svcGattGroup =
{
  NULL,
  (attsAttr_t *) gattList,
  NULL,
  NULL,
  GATT_START_HDL,
  GATT_END_HDL
};

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreAddGroup(void)
{
  AttsAddGroup(&svcGapGroup);
  AttsAddGroup(&svcGattGroup);
}

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreRemoveGroup(void)
{
  AttsRemoveGroup(GAP_START_HDL);
  AttsRemoveGroup(GATT_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGapCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcGapGroup.readCback = readCback;
  svcGapGroup.writeCback = writeCback;
}

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGattCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcGattGroup.readCback = readCback;
  svcGattGroup.writeCback = writeCback;
}

/*************************************************************************************************/
/*!
 *  \brief  Update the central address resolution attribute value.
 *
 *  \param  value   New value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGapCentAddrResUpdate(bool_t value)
{
  gapValCar[0] = value;
}

/*************************************************************************************************/
/*!
 *  \brief  Add the Resolvable Private Address Only (RPAO) characteristic to the GAP service.
 *          The RPAO characteristic should be added only when DM Privacy is enabled.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGapAddRpaoCh(void)
{
  /* if RPAO characteristic not already in GAP service */
  if (svcGapGroup.endHandle < GAP_RPAO_HDL)
  {
    svcGapGroup.endHandle = GAP_RPAO_HDL;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Set the Server Supported Features (SSF) bitmask.
 *
 *  \param  value   New value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGattSetSsf(uint8_t value)
{
  gattValSsf[0] = value;
}

/*************************************************************************************************/
/*!
 *  \brief  Update the Device Name attribute value.
 *
 *  \param  name   Pointer to NULL terminated char string with device name
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGapDeviceNameUpdate(const char *name)
{
    WSF_ASSERT(sizeof(gapValDn) > strlen(name));
    gapLenDn = strlen(name) + 1; //NULL terminated string
    memcpy(gapValDn, name, gapLenDn);
}

/*************************************************************************************************/
/*!
 *  \brief  Update the Appearance attribute value.
 *
 *  \param  appearance  Value of Appearance attribute (@see svc_ch.c)
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCoreGapAppearanceUpdate(uint16_t appearance)
{
    gapValAp[0] = (uint8_t)(appearance);
    gapValAp[1] = (uint8_t)((appearance) >> 8);
}
