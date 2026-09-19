# Command to start the test application
#  CFE_ES_START_APP_CC = 4
#
# typedef struct CFE_ES_StartAppCmd_Payload
# {
#      char Application[CFE_MISSION_MAX_API_LEN]; //CFE_MISSION_MAX_API_LEN = 20
#      char AppEntryPoint[CFE_MISSION_MAX_API_LEN];
#      char AppFileName[CFE_MISSION_MAX_PATH_LEN]; //CFE_MISSION_MAX_PATH_LEN = 64
#      CFE_ES_MemOffset_t StackSize; //uint32
#      CFE_ES_ExceptionAction_Enum_t ExceptionAction; //uint8
#      undocumented SPARE uint8
#      CFE_ES_TaskPriority_Atom_t Priority; //uint16
# } CFE_ES_StartAppCmd_Payload_t;

# typedef struct CFE_ES_StartAppCmd_Payload
# {
#     char Application[CFE_MISSION_MAX_API_LEN];   /**< \brief Name of Application to be started */
#     char AppEntryPoint[CFE_MISSION_MAX_API_LEN]; /**< \brief Symbolic name of Application's entry point */

#     char AppFileName[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Full path and filename of Application's
#                                                    executable image */

#     CFE_ES_MemOffset_t            StackSize;       /**< \brief Desired stack size for the new application */
#     CFE_ES_ExceptionAction_Enum_t ExceptionAction; /**< \brief #CFE_ES_ExceptionAction_RESTART_APP=On exception,
#                                                        restart Application,
#                                                        #CFE_ES_ExceptionAction_PROC_RESTART=On exception,
#                                                        perform a Processor Reset */

#     uint8                      Padding1;    /** \brief Padding for alignment */
#     CFE_ES_TaskPriority_Atom_t Priority;    /**< \brief The new Applications runtime priority. */
#     uint8                      Padding2[4]; /** \brief Padding for 64-bit alignment */
# } CFE_ES_StartAppCmd_Payload_t;

./cmdUtil --host=127.0.0.1 --port=1234 --pktid=0x1806 --cmdcode=4 --endian=LE --string="20:PIXXEL_MAIN_APP" --string="20:PIXXEL_MAIN_Main" \
          --string="64:pixxel_main.so" --uint64=16384 --uint8=0 --uint8=0 --uint16=100 --uint32=0

