SUMMARY = "program to test the driver"
DESCRIPTION = "program to test the driver"
LICENSE = "CLOSED"

SRC_URI = " \
    file://testdriver.c \
    file://testdriver.h \
    file://Makefile \
"

S = "${UNPACKDIR}"

do_configure() {
    :
}

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/testdriver ${D}${bindir}/testdriver
}

FILES:${PN} += "${bindir}/testdriver"
