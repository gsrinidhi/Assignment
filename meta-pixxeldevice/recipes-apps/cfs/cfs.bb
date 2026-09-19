SUMMARY = "NASA Core Flight System"
DESCRIPTION = "Core Flight System with Pixxel applications"
LICENSE = "CLOSED"

SRC_URI = " \
    file://cFS.zip \
"

S = "${UNPACKDIR}/cFS"

DEPENDS += "cmake-native"

OEQA_BUILDPATHS_SKIP = "/home/srinidhi"

INSANE_SKIP:${PN} += "buildpaths"

do_configure() {
    :
}

do_compile() {
    oe_runmake arm64_linux.prep
    oe_runmake VERBOSE=1 arm64_linux.compile
}



do_install() {
    oe_runmake arm64_linux.install DESTDIR="${D}"
    rm -rf "${D}/exe/host"
    
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/cFS_startup \
        ${D}${sysconfdir}/init.d/cfs
}

FILES:${PN} += " \
    /exe/cpu1/ \
    ${sysconfdir}/init.d/cfs \
"

inherit update-rc.d

INITSCRIPT_NAME = "cfs"
INITSCRIPT_PARAMS = "defaults 50"
