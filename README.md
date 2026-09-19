Pixxel Assignment

Implemented device drivers, cFS Application and created yocto layer having recipes for the driver and application

Current status: Able to simulate pixxel_main and pixxel_controller apps in qemu

The yocto layer contains the device driver source code

NOTE: the cFS zip file from recipes-apps/cFS/files has been removed due to github limitations. It contains the complete cFS along with the two apps compressed into a zip folder

Design flow:

Started with developing kernel device drivers for pixxel-virt-dev@60000000. Thought about having a linked list for all devices, but then learnt the structure having data of a particular device can be linked to its file, and then a pointer to it can be obtained during open, so no linked list was used.

Then developed the qemu device behaviour. Added pixxeldev.c in qemu/hw/misc. This implements what happens in the device when it is read. Status register reflecting enable register is implemented here. Currently status register immediately reflects enable register, yet to implement 50ms delay. Also changed qemu/hw/arm/virt.c to attach the device to address 0x6000000.

Then configured yocto for arm64 with meta-pixxeldevice layer, which had the kernel device drivers. Learnt to write a recipe to build and install the kernel device driver. Built core-image-minimal and observed device init and probe log messages in qemu

Then started implementing pixxel_controller and pixxel_main in cFS. Took sample_app as a template. 
Messages from pixxel_main to pixxel_controller:
a) write command, along with data( for enable, data = 1)
b) read command
 
Messages from pixxel_controller to pixxel_main:
a) write ack
b) read data having status register data

Added the pixxel_controller to the startup script in cFS/sample_defs/generate_startup.cmake

First did build using native_std for host system, then added a new configuration arm64_linux for cross compiling to arm64. Exported toolchain from yocto using populate_sdk, then gave path of this toolchain in cFS/sample_defs/toolchain-aarch64-poky-linux.cmake. 

Once it got cross-compiled, added a new recipe in meta-pixxeldevice to add cFS. Added startup script to start cFS after booting

Started the pixxel_main app through a ground command using the cFS-GroundSystem/Subsystem/cmdUtil

Qemu output in output/qemu_output.txt.

Issues encountered:

1. License issues during yocto build. Added MIT license with checksum, then learnt CLOSED can also be used
2. After qemu booting, was asking password for root user. Disabled root password in poky-master/build/conf/local.conf
3. Was not able to cross compile cFS from yocto. This was because yocto also did host compilation of some apps (cfeconfig_platformdata_tool), which it ran on the host pc to build the target executables. Outside yocto, default CC was host /usr/bin/gcc, so this was not observed. In yocto this was for target, so cfeconfig_platformdata_tool was being compiled for arm64. To fix this, passed CMAKE_C_COMPILER flags in target-rules.mk in cFS, then worked
4. Host compiler was failing to compile a simple program. This was because target cflags were being passed to host compiler. Added CFLAGS in target-rules.mk
5. Was not able to load pixxel_controller.so during cFS startup. This was because the max file name size was 20 characters. Changed the max limit to 25
6. cFS was not able to open /dev/pixxelDevice0. Had to map the /dev directory to a /dev virtual directory in cFS/psp/fsw/pc-linux/src/cfe_psp_start.c. The /cf directory was also mapped here, so mapped /dev just after it
7. Tried using GroundSystem.py gui interface for command, but it was causing packet length mismatch. So used cmdUtil command line utility.
