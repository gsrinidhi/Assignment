###########################################################
#
# SAMPLE_APP platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SAMPLE_APP configuration
set(PIXXEL_CONTROLLER_PLATFORM_CONFIG_FILE_LIST
  pixxel_controller_internal_cfg_values.h
  pixxel_controller_platform_cfg.h
  pixxel_controller_perfids.h
  pixxel_controller_msgids.h
  pixxel_controller_msgid_values.h
)

generate_configfile_set(${PIXXEL_CONTROLLER_PLATFORM_CONFIG_FILE_LIST})

