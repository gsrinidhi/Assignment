Pixxel Assignment

Implemented device drivers, cFS Application and created yocto layer having recipes for the driver and application

Current status: Getting EC -104 when loading pixxel_controller in cFS Startup

The yocto layer contains the device driver source code

To-do:
1. Solve pixxel_controller startup issue
2. Add 50ms timing behaviour in both device emulation and user space pixxel_main application
3. Will have to ensure data consistency in driver by adding locks
