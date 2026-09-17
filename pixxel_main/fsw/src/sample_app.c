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
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "sample_app.h"
#include "sample_app_cmds.h"
#include "sample_app_utils.h"
#include "sample_app_eventids.h"
#include "sample_app_dispatch.h"
#include "sample_app_tbl.h"
#include "sample_app_version.h"

/*
** global data
*/
PIXXEL_MAIN_Data_t PIXXEL_MAIN_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void PIXXEL_MAIN_Main(void)
{
    CFE_Status_t     status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(PIXXEL_CONTROLLER_PERF_ID);

    /*
    ** Perform application-specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = PIXXEL_MAIN_Init();
    if (status != CFE_SUCCESS)
    {
        PIXXEL_MAIN_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Sample App Runloop
    */

    PIXXEL_Write_Test();

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(PIXXEL_CONTROLLER_PERF_ID);

    /* Pend on receipt of command packet */
    status = CFE_SB_ReceiveBuffer(&SBBufPtr, PIXXEL_MAIN_Data.CommandPipe, CFE_SB_PEND_FOREVER);

    /*
    ** Performance Log Entry Stamp
    */
    CFE_ES_PerfLogEntry(PIXXEL_CONTROLLER_PERF_ID);

    if (status == CFE_SUCCESS)
    {
        PIXXEL_MAIN_TaskPipe(SBBufPtr);
    }
    else
    {
        CFE_EVS_SendEvent(PIXXEL_CONTROLLER_PIPE_ERR_EID,
                            CFE_EVS_EventType_ERROR,
                            "PIXXEL CONTROLLER: SB Pipe Read Error, App Will Exit");

        PIXXEL_MAIN_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(PIXXEL_CONTROLLER_PERF_ID);

    CFE_ES_ExitApp(PIXXEL_MAIN_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t PIXXEL_MAIN_Init(void)
{
    CFE_Status_t status;
    // char         VersionString[PIXXEL_CONTROLLER_CFG_MAX_VERSION_STR_LEN];

    /* Zero out the global data structure */
    memset(&PIXXEL_MAIN_Data, 0, sizeof(PIXXEL_MAIN_Data));

    PIXXEL_MAIN_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Pixxel Controller: Error Rsegistering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        /*
         ** Initialize housekeeping packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(PIXXEL_MAIN_Data.devRead.CommandHeader),
                     CFE_SB_ValueToMsgId(PIXXEL_CONTROLLER_DEV_READ_MID),
                     sizeof(PIXXEL_MAIN_Data.devRead));

        CFE_MSG_Init(CFE_MSG_PTR(PIXXEL_MAIN_Data.devWrite.CommandHeader),
                     CFE_SB_ValueToMsgId(PIXXEL_CONTROLLER_DEV_WRITE_MID),
                     sizeof(PIXXEL_MAIN_Data.devWrite));

        /*
         ** Create Software Bus message pipe.
         */
        status = CFE_SB_CreatePipe(&PIXXEL_MAIN_Data.CommandPipe,
                                   SAMPLE_APP_PLATFORM_PIPE_DEPTH,
                                   SAMPLE_APP_PLATFORM_PIPE_NAME);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(PIXXEL_CONTROLLER_CR_PIPE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Pixxel Controller: Error creating SB Command Pipe, RC = 0x%08lX",
                              (unsigned long)status);
        }
    }

    // if (status == CFE_SUCCESS)
    // {
    //     /*
    //     ** Subscribe to Housekeeping request commands
    //     */
    //     status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SAMPLE_APP_SEND_HK_MID), PIXXEL_MAIN_Data.CommandPipe);
    //     if (status != CFE_SUCCESS)
    //     {
    //         CFE_EVS_SendEvent(SAMPLE_APP_SUB_HK_ERR_EID,
    //                           CFE_EVS_EventType_ERROR,
    //                           "Sample App: Error Subscribing to HK request, RC = 0x%08lX",
    //                           (unsigned long)status);
    //     }
    // }

    // if (status == CFE_SUCCESS)
    // {
    //     /*
    //     ** Subscribe to ground command packets
    //     */
    //     status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SAMPLE_APP_CMD_MID), PIXXEL_MAIN_Data.CommandPipe);
    //     if (status != CFE_SUCCESS)
    //     {
    //         CFE_EVS_SendEvent(SAMPLE_APP_SUB_CMD_ERR_EID,
    //                           CFE_EVS_EventType_ERROR,
    //                           "Sample App: Error Subscribing to Commands, RC = 0x%08lX",
    //                           (unsigned long)status);
    //     }
    // }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to pixxel device read data
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(PIXXEL_CONTROLLER_DEV_DATA_MID), PIXXEL_MAIN_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SAMPLE_APP_SUB_CMD_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Sample App: Error Subscribing to Commands, RC = 0x%08lX",
                              (unsigned long)status);
        }
    }

    // if (status == CFE_SUCCESS)
    // {
    //     /*
    //     ** Register Example Table(s)
    //     */
    //     status = CFE_TBL_Register(&PIXXEL_MAIN_Data.TblHandles[0],
    //                               "ExampleTable",
    //                               sizeof(SAMPLE_APP_ExampleTable_t),
    //                               CFE_TBL_OPT_DEFAULT,
    //                               SAMPLE_APP_TblValidationFunc);
    //     if (status != CFE_SUCCESS)
    //     {
    //         CFE_EVS_SendEvent(SAMPLE_APP_TABLE_REG_ERR_EID,
    //                           CFE_EVS_EventType_ERROR,
    //                           "Sample App: Error Registering Example Table, RC = 0x%08lX",
    //                           (unsigned long)status);
    //     }
    //     else
    //     {
    //         status = CFE_TBL_Load(PIXXEL_MAIN_Data.TblHandles[0], CFE_TBL_SRC_FILE, PIXXEL_CONTROLLER_PLATFORM_TABLE_FILE);
    //     }

    //     CFE_Config_GetVersionString(VersionString,
    //                                 PIXXEL_CONTROLLER_CFG_MAX_VERSION_STR_LEN,
    //                                 "Sample App",
    //                                 SAMPLE_APP_VERSION,
    //                                 SAMPLE_APP_BUILD_CODENAME,
    //                                 SAMPLE_APP_LAST_OFFICIAL);

    //     CFE_EVS_SendEvent(SAMPLE_APP_INIT_INF_EID,
    //                       CFE_EVS_EventType_INFORMATION,
    //                       "Sample App Initialized.%s",
    //                       VersionString);
    // }

    return status;
}
