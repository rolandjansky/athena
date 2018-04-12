# This script sets up the build enironment for an AthDataQuality
# build.
#
# This script is kept separate from the build.sh
# wrapper so it can be sourced separately from it when
# clients want to manage their own build and just want
# to setup the build environment

env_usage() {
    echo "Usage: build_env.sh [-b build dir]"
}

# This function actually sets up the environment for us
# (factorise it here in case it needs skipped)
env_setup() {
    startdir=$(pwd)
    # As this script can be sourced we need to support zsh and
    # possibly other Bourne shells
    if [ "x${BASH_SOURCE[0]}" = "x" ]; then
    # This trick should do the right thing under ZSH:
        thisdir=$(dirname `print -P %x`)
        if [ $? != 0 ]; then
            echo "ERROR: This script must be sourced from BASH or ZSH"
            return 1
        fi
    else
        # The BASH solution is a bit more straight forward:
        thisdir=$(dirname ${BASH_SOURCE[0]})
    fi
    AthDataQualitySrcDir=$(cd ${thisdir};pwd)

    # The directory holding the helper scripts:
    scriptsdir=${AthDataQualitySrcDir}/../../Build/AtlasBuildScripts

    # Go to the main directory of the repository:
    cd ${AthDataQualitySrcDir}/../..

    # Check if the user specified any source/build directories:
    if [ "$BUILDDIR" = "" ]; then
        BUILDDIR=${AthDataQualitySrcDir}/../../../build
    fi

    # Set up the environment for the build:
    export NICOS_PROJECT_VERSION=`cat ${AthDataQualitySrcDir}/version.txt`
    export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
    export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}
    export NICOS_PROJECT_HOME=${BUILDDIR}/install/AthDataQuality

    # Set up the environment variables for finding LCG and the TDAQ externals:
    source ${scriptsdir}/LCG_RELEASE_BASE.sh
    source ${scriptsdir}/TDAQ_RELEASE_BASE.sh

    # Set up the environment variables necessary to find the CMake code from
    # atlasexternals:
    extDir=${BUILDDIR}/src/atlasexternals
    if [ ! -d ${extDir} ]; then
        echo "Didn't find atlasexternals under ${extDir}!"
    fi
    export AtlasCMake_DIR=${extDir}/Build/AtlasCMake
    export LCG_DIR=${extDir}/Build/AtlasLCG
    echo "Picking up AtlasCMake from: ${AtlasCMake_DIR}"
    echo "Picking up AtlasLCG from  : ${LCG_DIR}"

    # Return to the original directory:
    cd $startdir
}

# we need to reset the option index as we are sourcing this script
# http://stackoverflow.com/questions/23581368/bug-in-parsing-args-with-getopts-in-bash
OPTIND=1

# Parse the command line arguments:
BUILDDIR=""
while getopts "b:h" opt; do
    case $opt in
        b)
            BUILDDIR=$OPTARG
            ;;
        h)
            env_usage
            ABORT=1
            ;;
        :)
            echo "Argument -$OPTARG requires a parameter!"
            env_usage
            ABORT=1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            env_usage
            ABORT=1
            ;;
    esac
done

# Put a big wrapper around bad argument case, because
# a sourced script should not call "exit". This is quite
# annoying...
if [ -z "$ABORT" ]; then
    env_setup
fi
