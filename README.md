Pixxel Assignment

Implemented device drivers, cFS Application and created yocto layer having recipes for the driver and application

Current status: Getting EC -104 when loading pixxel_controller in cFS Startup

The yocto layer contains the device driver source code

NOTE: the cFS zip file from recipes-apps/cFS/files has been removed due to github limitations. It contains the complete cFS along with the two apps compressed into a zip folder

To-do:
1. Solve pixxel_controller startup issue
2. Add 50ms timing behaviour in both device emulation and user space pixxel_main application
3. Will have to ensure data consistency in driver by adding locks
4. Have to rename files in cFS from sample_app to pixxel_controller / pixxel_main and update includes
