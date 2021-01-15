# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script that is used by the build_externals.sh scripts of the individual
# projects.
#
# The includer script must set (default) values for the following variables
# before sourcing this script:
#  - ATLAS_PROJECT_DIR: Directory of the project that this script is being used
#    from.
#  - ATLAS_EXT_PROJECT_NAME: The name of the "externals project" needed by the
#    ATLAS project being built.
#  - ATLAS_BUILDTYPE: The default build type to build the "externals project"
#    with.
#  - ATLAS_EXTRA_CMAKE_ARGS: Extra CMake arguments to provide for the build of
#    the "externals project".
#  - ATLAS_EXTRA_MAKE_ARGS: Extra 'make tool' arguments to provide for the build
#    of the "externals project".
#

# Function printing the usage information for the script.
usage() {
   echo "Script building ${ATLAS_EXT_PROJECT_NAME}."
   echo ""
   echo "Usage: build_externals.sh [options]"
   echo "Options:"
   echo " -t: Build type, by default set to '${ATLAS_BUILDTYPE}'"
   echo " -b: 'Main build directory' to use for the code checkout and build"
   echo " -f: Force rebuild of externals from scratch, otherwise if script"
   echo "     finds an external build present it will only do an incremental"
   echo "     build"
   echo " -c: Build the externals for the continuous integration (CI) system,"
   echo "     skipping the build of the externals RPMs."
   echo " -x: Extra cmake argument(s) to provide for the build(configuration)"
   echo "     of all externals needed by Athena."
   echo " -k: Extra 'make tool' argument(s) to provide to the build tool."
   echo ""
   echo "If a build directory is not given, the default is '../build'"
   echo "relative to the athena source directory."
}

# Parse the command line arguments.
ATLAS_BUILD_DIR=""
ATLAS_FORCE_REBUILD=""
ATLAS_CI_BUILD=""
while getopts ":t:b:x:k:fch" opt; do
   case ${opt} in
      t)
         ATLAS_BUILDTYPE=${OPTARG}
         ;;
      b)
         ATLAS_BUILD_DIR=${OPTARG}
         ;;
      f)
         ATLAS_FORCE_REBUILD="1"
         ;;
      c)
         ATLAS_CI_BUILD="1"
         ;;
      x)
         ATLAS_EXTRA_CMAKE_ARGS+=(${OPTARG})
            ;;
      k)
         ATLAS_EXTRA_MAKE_ARGS+=(${OPTARG})
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
   set -o pipefail
fi
ERROR_COUNT=0

# If we are *not* executing a CI build, and have not specified any particular
# build tool arguments, then choose one according to the build tool being used.
# (In order to make the build continue through any possible errors.)
if [[ "${ATLAS_CI_BUILD}" = "" && "${ATLAS_EXTRA_MAKE_ARGS[@]}" = "" ]]; then
   if [[ "${ATLAS_EXTRA_CMAKE_ARGS[@]}" =~ "Ninja" ]]; then
      ATLAS_EXTRA_MAKE_ARGS+=(-- -k0)
   else
      ATLAS_EXTRA_MAKE_ARGS+=(-- -k)
   fi
fi

# Get the absolute path of this script.
ATLAS_BUILD_SCRIPTS_DIR=$(dirname ${BASH_SOURCE[0]})
ATLAS_BUILD_SCRIPTS_DIR=$(cd ${ATLAS_BUILD_SCRIPTS_DIR} && pwd)

# Check if the user specified any source/build directories.
if [ "${ATLAS_BUILD_DIR}" = "" ]; then
   ATLAS_BUILD_DIR="${ATLAS_BUILD_SCRIPTS_DIR}/../../../build"
fi
mkdir -p "${ATLAS_BUILD_DIR}"
ATLAS_BUILD_DIR=$(cd "${ATLAS_BUILD_DIR}" && pwd)

# Greet the user.
echo "Building ${ATLAS_EXT_PROJECT_NAME} in: ${ATLAS_BUILD_DIR}"

# Clean the build directory, if necessary.
if [ "$ATLAS_FORCE_REBUILD" = "1" ]; then
   echo "Force deleting existing build area..."
   rm -rf "${ATLAS_BUILD_DIR}/install/${ATLAS_EXT_PROJECT_NAME}"
   rm -rf "${ATLAS_BUILD_DIR}/src/${ATLAS_EXT_PROJECT_NAME}"
   rm -rf "${ATLAS_BUILD_DIR}/build/${ATLAS_EXT_PROJECT_NAME}"
fi

# Figure out the version of the main project.
ATLAS_PROJECT_VERSION=$(cat ${ATLAS_PROJECT_DIR}/version.txt)
# Figure out what tag/branch to use for the "externals project".
ATLAS_EXT_PROJECT_TAG=$(grep "${ATLAS_EXT_PROJECT_NAME}"                       \
                        ${ATLAS_PROJECT_DIR}/externals.txt                     |
                        sed "s/${ATLAS_EXT_PROJECT_NAME}Version = \(.*\)/\1/g" )

# Generate hash of any extra (c)make arguments.
cmakehash=$(echo -n "${ATLAS_EXTRA_CMAKE_ARGS[@]};${ATLAS_EXTRA_MAKE_ARGS[@]}" |
            openssl md5 | awk '{print $2}')

# Check if previous externals build can be reused.
externals_stamp="${ATLAS_BUILD_DIR}/build/${ATLAS_EXT_PROJECT_NAME}/externals-${ATLAS_PROJECT_VERSION}-${cmakehash}.stamp"
if [ -f "${externals_stamp}" ]; then
   if diff -q "${externals_stamp}" "${ATLAS_PROJECT_DIR}/externals.txt"; then
      echo "Correct version of externals already available in ${ATLAS_BUILD_DIR}"
      exit 0
   else
      rm ${externals_stamp}
   fi
fi

# Create some directories.
mkdir -p ${ATLAS_BUILD_DIR}/{src,install}

# Set the environment variables for finding LCG and TDAQ releases.
source "${ATLAS_BUILD_SCRIPTS_DIR}/LCG_RELEASE_BASE.sh"
source "${ATLAS_BUILD_SCRIPTS_DIR}/TDAQ_RELEASE_BASE.sh"

# Include the timing function.
source "${ATLAS_BUILD_SCRIPTS_DIR}/atlas_build_time.sh"

# Flag for triggering the build of RPMs for the externals.
RPMOPTIONS="-r ${ATLAS_BUILD_DIR}"
if [ "$ATLAS_CI_BUILD" = "1" ]; then
    RPMOPTIONS=
fi

# Check out the externals project from the right branch/tag.
{ atlas_build_time EXTERNALS_CHECKOUT                                          \
   ${ATLAS_BUILD_SCRIPTS_DIR}/checkout_atlasexternals.sh                       \
   -t "${ATLAS_EXT_PROJECT_TAG}"                                               \
   -s "${ATLAS_BUILD_DIR}/src/${ATLAS_EXT_PROJECT_NAME}"; }                    \
   2>&1 | tee "${ATLAS_BUILD_DIR}/src/checkout.${ATLAS_EXT_PROJECT_NAME}.log"  \
   || ((ERROR_COUNT++))

# Build the externals project.
{ atlas_build_time EXTERNALS_BUILD                                             \
   ${ATLAS_BUILD_SCRIPTS_DIR}/build_atlasexternals.sh                          \
   -s "${ATLAS_BUILD_DIR}/src/${ATLAS_EXT_PROJECT_NAME}"                       \
   -b "${ATLAS_BUILD_DIR}/build/${ATLAS_EXT_PROJECT_NAME}"                     \
   -i "${ATLAS_BUILD_DIR}/install"                                             \
   -p "${ATLAS_EXT_PROJECT_NAME}" ${RPMOPTIONS} -t "${ATLAS_BUILDTYPE}"        \
   -v "${ATLAS_PROJECT_VERSION}"                                               \
   ${ATLAS_EXTRA_CMAKE_ARGS[@]/#/-x }                                          \
   ${ATLAS_EXTRA_MAKE_ARGS[@]/#/-m }; } || ((ERROR_COUNT++))

# Exit with the error count taken into account.
if [ ${ERROR_COUNT} -ne 0 ]; then
    echo "${ATLAS_EXT_PROJECT_NAME} build encountered ${ERROR_COUNT} error(s)"
else
    cp "${ATLAS_PROJECT_DIR}/externals.txt" "${externals_stamp}"
fi
exit ${ERROR_COUNT}
