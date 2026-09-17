/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the Sample App Ground Command-handling functions
 */

/*
** Include Files:
*/
#include "sample_app.h"
#include "sample_app_cmds.h"
#include "sample_app_msgids.h"
#include "sample_app_eventids.h"
#include "sample_app_version.h"
#include "sample_app_tbl.h"
#include "sample_app_utils.h"
#include "sample_app_msg.h"

/* The sample_lib module provides the SAMPLE_Function() prototype */
#include "sample_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t SAMPLE_APP_SendHkCmd(const SAMPLE_APP_SendHkCmd_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    SAMPLE_APP_Data.HkTlm.Payload.CommandErrorCounter = SAMPLE_APP_Data.CommandErrorCounter;
    SAMPLE_APP_Data.HkTlm.Payload.CommandCounter      = SAMPLE_APP_Data.CommandCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(SAMPLE_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(SAMPLE_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < SAMPLE_APP_PLATFORM_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(SAMPLE_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t SAMPLE_APP_NoopCmd(const SAMPLE_APP_NoopCmd_t *Msg)
{
    SAMPLE_APP_Data.CommandCounter++;

    CFE_EVS_SendEvent(SAMPLE_APP_NOOP_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "SAMPLE: NOOP command %s",
                      SAMPLE_APP_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t SAMPLE_APP_ResetCountersCmd(const SAMPLE_APP_ResetCountersCmd_t *Msg)
{
    SAMPLE_APP_Data.CommandCounter      = 0;
    SAMPLE_APP_Data.CommandErrorCounter = 0;

    CFE_EVS_SendEvent(SAMPLE_APP_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t SAMPLE_APP_ProcessCmd(const SAMPLE_APP_ProcessCmd_t *Msg)
{
    CFE_Status_t               Status;
    void                      *TblAddr;
    SAMPLE_APP_ExampleTable_t *TblPtr;
    const char                *TableName = "SAMPLE_APP.ExampleTable";

    /* Sample Use of Example Table */
    SAMPLE_APP_Data.CommandCounter++;
    Status = CFE_TBL_GetAddress(&TblAddr, SAMPLE_APP_Data.TblHandles[0]);
    if (Status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Fail to get table address: 0x%08lx", (unsigned long)Status);
    }
    else
    {
        TblPtr = TblAddr;
        CFE_ES_WriteToSysLog("Sample App: Example Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

        SAMPLE_APP_GetCrc(TableName);

        Status = CFE_TBL_ReleaseAddress(SAMPLE_APP_Data.TblHandles[0]);
        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Fail to release table address: 0x%08lx", (unsigned long)Status);
        }
        else
        {
            /* Invoke a function provided by SAMPLE_APP_LIB */
            SAMPLE_LIB_Function();
        }
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* A simple example command that displays a passed-in value                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t SAMPLE_APP_DisplayParamCmd(const SAMPLE_APP_DisplayParamCmd_t *Msg)
{
    SAMPLE_APP_Data.CommandCounter++;
    CFE_EVS_SendEvent(SAMPLE_APP_VALUE_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "SAMPLE_APP: ValU32=%lu, ValI16=%d, ValStr=%s",
                      (unsigned long)Msg->Payload.ValU32,
                      (int)Msg->Payload.ValI16,
                      Msg->Payload.ValStr);

    return CFE_SUCCESS;
}

//command to read from /dev/pixxeldriver0 file and send the data to ground station
CFE_Status_t PIXXEL_CONTROLLER_ReadPixxelDriverCmd(const PIXXEL_CONTROLLER_DEV_READ_t *Msg)
{
    PIXXEL_CONTROLLER_Data.CommandCounter++;

    //open the pixxel driver file using the OSAL API

    osal_id_t fd;
    int32 retStatus = OS_OpenCreate(&fd,"/dev/pixxeldriver0", OS_FILE_FLAG_NONE, OS_READ_ONLY);
    if (retStatus != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(PIXXEL_CONTROLLER_OPEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "PIXXEL_CONTROLLER: Error opening pixxel driver file, RC = %d",
                          fd);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;  
    }

    //read data from the pixxel driver file
    char buffer[4];
    int32 bytes_read = OS_read(fd, buffer, sizeof(buffer));
    if (bytes_read < 0)
    {
        CFE_EVS_SendEvent(PIXXEL_CONTROLLER_READ_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "PIXXEL_CONTROLLER: Error reading pixxel driver file, RC = %d",
                          bytes_read);
        OS_close(fd);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    OS_close(fd);

    PIXXEL_CONTROLLER_Data.DevReadBuf.Payload.seqNo = PIXXEL_CONTROLLER_Data.DevReadBuf.Payload.seqNo + 1;
    memcpy(PIXXEL_CONTROLLER_Data.DevReadBuf.Payload.RegValue, buffer, sizeof(buffer));

    //send the data over software bus
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(PIXXEL_CONTROLLER_Data.DevReadBuf.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(PIXXEL_CONTROLLER_Data.DevReadBuf.TelemetryHeader), true);
    return CFE_SUCCESS;
}

//command to read from /dev/pixxeldriver0 file and send the data to software bus
CFE_Status_t PIXXEL_CONTROLLER_WritePixxelDriverCmd(const PIXXEL_CONTROLLER_DEV_WRITE_t *Msg)
{
    PIXXEL_CONTROLLER_Data.CommandCounter++;

    //open the pixxel driver file using the OSAL API
    osal_id_t fd;
    int32 retStatus = OS_OpenCreate(&fd,"/dev/pixxeldriver0", OS_FILE_FLAG_NONE, OS_WRITE_ONLY);
    if (retStatus != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(PIXXEL_CONTROLLER_OPEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "PIXXEL_CONTROLLER: Error opening pixxel driver file, RC = %d",
                          fd);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;  
    }

    //write data to device file
    // char buffer[4];

    int32 bytes_written = OS_write(fd, Msg->Payload.RegValue, 4 * sizeof(char));
    if (bytes_written < 0)
    {
        CFE_EVS_SendEvent(PIXXEL_CONTROLLER_READ_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "PIXXEL_CONTROLLER: Error writing pixxel driver file, RC = %d",
                          bytes_written);
        OS_close(fd);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    OS_close(fd);
    return CFE_SUCCESS;
}
