# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This script sets up the build enironment for an DetCommon
# build.
#
# This script is kept separate from the build.sh
# wrapper so it can be sourced separately from it when
# clients want to manage their own build and just want
# to setup the build environment

# Function printing the usage information for the script.
atlas_env_usage() {
   echo "Script setting up the build environment for DetCommon."
   echo ""
   echo "Usage: build_env.sh [-b build dir]"
   echo ""
   echo "If a build directory is not given, the default is '../build'"
   echo "relative to the athena source directory."
}

# This function actually sets up the environment for us
# (factorise it here in case it needs skipped)
env_setup() {

    # As this script can be sourced we need to support zsh and
    # possibly other Bourne shells
    if [ "x${BASH_SOURCE[0]}" = "x" ]; then
    # This trick should do the right thing under ZSH:
        DetCommonSrcDir=$(dirname `print -P %x`)
        if [ $? != 0 ]; then
            echo "ERROR: This script must be sourced from BASH or ZSH"
            return 1
        fi
    else
        # The BASH solution is a bit more straight forward:
        DetCommonSrcDir=$(dirname ${BASH_SOURCE[0]})
    fi
    DetCommonSrcDir=$(cd ${DetCommonSrcDir};pwd)

    # The directory holding the helper scripts:
    scriptsdir=${DetCommonSrcDir}/../../Build/AtlasBuildScripts

    # Go to the main directory of the repository:
    cd ${DetCommonSrcDir}/../..

    # Check if the user specified any source/build directories:
    if [ "${ATLAS_BUILD_DIR}" = "" ]; then
        ATLAS_BUILD_DIR=${DetCommonSrcDir}/../../../build
    fi

    # Set up the environment variables for finding LCG and the TDAQ externals:
    source "${scriptsdir}/LCG_RELEASE_BASE.sh"
    source "${scriptsdir}/TDAQ_RELEASE_BASE.sh"

    # Set up the environment variables necessary to find the CMake code from
    # atlasexternals:
    ATLAS_EXT_DIR="${ATLAS_BUILD_DIR}/src/atlasexternals"
    if [ ! -d ${ATLAS_EXT_DIR} ]; then
        echo "Warning: Didn't find atlasexternals under ${ATLAS_EXT_DIR}!"
    fi
    export AtlasCMake_DIR="${ATLAS_EXT_DIR}/Build/AtlasCMake"
    export LCG_DIR="${ATLAS_EXT_DIR}/Build/AtlasLCG"
    echo "Picking up AtlasCMake from: ${AtlasCMake_DIR}"
    echo "Picking up AtlasLCG from  : ${LCG_DIR}"

    # Return gracefully.
    return 0
}

# we need to reset the option index as we are sourcing this script
# http://stackoverflow.com/questions/23581368/bug-in-parsing-args-with-getopts-in-bash
OPTIND=1

# Parse the command line arguments:
ATLAS_BUILD_DIR=""
while getopts "b:h" opt; do
    case $opt in
        b)
            ATLAS_BUILD_DIR=$OPTARG
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
