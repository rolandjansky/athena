#!/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script that is used by the build_externals.sh scripts of those individual
# projects that do not require an "externals project". As such, this script just
# takes care of cloning the right version of atlas/atlasexternals into the
# appropriate directory.
#
# The includer script must set (default) values for the following variable(s)
# before sourcing this script:
#  - ATLAS_PROJECT_DIR: Directory of the project that this script is being used
#    from.
#  - ATLAS_PROJECT_NAME: The name of the ATLAS project being built.
#

# Function printing the usage information for the script.
usage() {
   echo "Script getting the cmake code for ${ATLAS_PROJECT_NAME}"
   echo ""
   echo "Usage: build_externals.sh [options]"
   echo "Options:"
   echo " -b: 'Main build directory' to use for the code checkout and build"
   echo " -f: Force the re-download of the externals repository"
   echo " -c: Get the externals for the continuous integration (CI) system"
   echo " -t: Build type (IGNORED)"
   echo " -x: Extra CMake arguments (IGNORED)"
   echo " -k: Extra 'make tool' arguments (IGNORED)"
   echo ""
   echo "If a build directory is not given, the default is '../build'"
   echo "relative to the athena source directory."
}

# Parse the command line arguments.
ATLAS_BUILD_DIR=""
ATLAS_FORCE_DOWNLOAD=""
ATLAS_CI_BUILD=""
while getopts ":t:b:x:k:fch" opt; do
   case $opt in
      b)
         ATLAS_BUILD_DIR=$OPTARG
         ;;
      f)
         ATLAS_FORCE_DOWNLOAD="1"
         ;;
      c)
         ATLAS_CI_BUILD="1"
         ;;
      t|x|k)
         echo "Argument \"-${opt}\" ignored for" \
              "${ATLAS_PROJECT_NAME}/build_externals.sh"
         ;;
      h)
         usage
         exit 0
         ;;
      :)
         echo "Argument -$OPTARG requires a parameter!"
         usage
         exit 1
         ;;
      ?)
         echo "Unknown argument: -$OPTARG"
         usage
         exit 1
         ;;
   esac
done

# Only stop on errors if we are executing a CI build. Otherwise just count them.
if [ "${ATLAS_CI_BUILD}" = "1" ]; then
   set -e
fi
set -o pipefail
ERROR_COUNT=0

# Get the absolute path of this script.
ATLAS_BUILD_SCRIPTS_DIR=$(dirname ${BASH_SOURCE[0]})
ATLAS_BUILD_SCRIPTS_DIR=$(cd ${ATLAS_BUILD_SCRIPTS_DIR} && pwd)

# Check if the user specified any source/build directories.
if [ "${ATLAS_BUILD_DIR}" = "" ]; then
   ATLAS_BUILD_DIR="${ATLAS_BUILD_SCRIPTS_DIR}/../../../build"
fi
cmake -E make_directory "${ATLAS_BUILD_DIR}" "${ATLAS_BUILD_DIR}/src"
ATLAS_BUILD_DIR=$(cd "${ATLAS_BUILD_DIR}" && pwd)

# Greet the user.
echo "Downloading CMake code for ${ATLAS_PROJECT_NAME} in: ${ATLAS_BUILD_DIR}"

# Clean the build directory, if necessary.
if [ "${ATLAS_FORCE_DOWNLOAD}" = "1" ]; then
   echo "Force deleting existing CMake sources area..."
   rm -rf "${ATLAS_BUILD_DIR}/src/atlasexternals"
fi

# Figure out what tag/branch to use for the externals repository.
ATLAS_EXT_PROJECT_TAG=$(grep "AtlasExternals"                                  \
                        ${ATLAS_PROJECT_DIR}/externals.txt                     |
                        sed "s/AtlasExternalsVersion = \(.*\)/\1/g" )

# Include the timing function.
source "${ATLAS_BUILD_SCRIPTS_DIR}/atlas_build_time.sh"

# Check out atlasexternals from the right branch/tag:
{ atlas_build_time EXTERNALS_CHECKOUT                                          \
   ${ATLAS_BUILD_SCRIPTS_DIR}/checkout_atlasexternals.sh                       \
   -t "${ATLAS_EXT_PROJECT_TAG}"                                               \
   -s "${ATLAS_BUILD_DIR}/src/atlasexternals"; }                               \
   2>&1 | tee "${ATLAS_BUILD_DIR}/src/checkout.atlasexternals.log"             \
   || ((ERROR_COUNT++))

# Exit with the error count taken into account.
if [ ${ERROR_COUNT} -ne 0 ]; then
    echo "${ATLAS_PROJECT_NAME} external setup encountered ${ERROR_COUNT} error(s)"
fi
exit ${ERROR_COUNT}
