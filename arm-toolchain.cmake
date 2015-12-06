# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)
# specify qmake
SET(QT_QMAKE_EXECUTABLE /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/bin/qmake)
SET(QT_UIC_EXECUTABLE /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/bin/uic)
SET(QT_MOC_EXECUTABLE /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/bin/moc)
SET(QT_RCC_EXECUTABLE /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/bin/rcc)
# specify pkg-config
SET(PKG_CONFIG_EXECUTABLE /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/bin/pkg-config)
# specify the cross compiler
SET(CMAKE_C_COMPILER   /home/nasser/project/arm-cortexa8_neon-linux-gnueabihf-4.8.1/bin/arm-cortexa8_neon-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /home/nasser/project/arm-cortexa8_neon-linux-gnueabihf-4.8.1/bin/arm-cortexa8_neon-linux-gnueabihf-g++)
# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /home/nasser/project/buildroot/buildroot-2015.05/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot)
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
