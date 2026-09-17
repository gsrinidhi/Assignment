SUMMARY = "My device kernel driver"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=28c5624a47c606291f2df6e8c86a1f19"

SRC_URI = " \
    file://pixxeldriver.c \
    file://pixxeldriver.h \
    file://Makefile \
    file://LICENSE \
"

S = "${UNPACKDIR}"

inherit module

EXTRA_OEMAKE += "KERNEL_SRC=${STAGING_KERNEL_DIR}"
