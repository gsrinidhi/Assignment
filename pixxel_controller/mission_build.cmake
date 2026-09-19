###########################################################
#
# SAMPLE_APP mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SAMPLE_APP configuration
set(PIXXEL_CONTROLLER_MISSION_CONFIG_FILE_LIST
  pixxel_controller_fcncode_values.h
  pixxel_controller_interface_cfg_values.h
  pixxel_controller_mission_cfg.h
  pixxel_controller_perfids.h
  pixxel_controller_msg.h
  pixxel_controller_msgdefs.h
  pixxel_controller_msgstruct.h
  pixxel_controller_tbl.h
  pixxel_controller_tbldefs.h
  pixxel_controller_tblstruct.h
  pixxel_controller_topicid_values.h
)

generate_configfile_set(${PIXXEL_CONTROLLER_MISSION_CONFIG_FILE_LIST})

