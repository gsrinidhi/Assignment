SUMMARY = "NASA Core Flight System"
DESCRIPTION = "Core Flight System with Pixxel applications"
LICENSE = "CLOSED"

SRC_URI = "file://cFS.zip"

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

    echo "===== SAMPLE APP COMPILE COMMAND ====="
    grep -n -A2 -B2 'sample_app.c' \
        build-arm64_linux/compile_commands.json || true
}

do_install() {
    oe_runmake arm64_linux.install DESTDIR="${D}"
    rm -rf "${D}/exe/host"
}

FILES:${PN} += "/exe/cpu1/"
