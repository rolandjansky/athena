# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Script that is used by the build.sh scripts of the individual projects.
# It builds and packages the project in a uniform way.
#
# The includer script must set (default) values for the following variables
# before sourcing this script:
#  - ATLAS_PROJECT_NAME: The name of the ATLAS project being built.
#  - ATLAS_PROJECT_DIR: Directory of the project that this script is being used
#    from.
#  - ATLAS_BUILDTYPE: The default build type to build the project with.
#  - ATLAS_EXTRA_CMAKE_ARGS: Extra CMake arguments to provide for the build of
#    the project.
#  - ATLAS_EXTRA_MAKE_ARGS: Extra 'make tool' arguments to provide for the build
#    of the project.
#

# Function printing the usage information for the script.
usage() {
   echo "Script building ${ATLAS_PROJECT_NAME}."
   echo ""
   echo "Usage: build.sh [options]"
   echo "Options:"
   echo "  General flags:"
   echo "    -t: The (optional) CMake build type to use. (default: ${ATLAS_BUILDTYPE})"
   echo "    -b: The (optional) build directory to use."
   echo "    -x: Custom argument(s) to pass to the CMake configuration."
   echo "    -k: Custom argument(s) to pass to the 'make tool'."
   echo "    -a: Abort on error."
   echo "  Build step selection:"
   echo "    -c: Execute the CMake step."
   echo "    -m: Execute the make/build step."
   echo "    -i: Execute the install step."
   echo "    -p: Execute the CPack step."
   echo ""
   echo "If none of the c, m, i or p options are set then the script will do"
   echo "all steps necessary for a nightly build. Otherwise only the enabled"
   echo "steps are run - it's your reponsibility to ensure that precusors are"
   echo "in a good shape."
   echo ""
   echo "If a build directory is not given, the default is '../build'"
   echo "relative to the athena/ source directory."
}

# Parse the command line arguments.
ATLAS_BUILD_DIR=""
ATLAS_EXE_CMAKE=""
ATLAS_EXE_MAKE=""
ATLAS_EXE_INSTALL=""
ATLAS_EXE_CPACK=""
ATLAS_CI_BUILD=""
while getopts ":t:b:x:k:acmiph" opt; do
   case $opt in
      t)
         ATLAS_BUILDTYPE=$OPTARG
         ;;
      b)
         ATLAS_BUILD_DIR=$OPTARG
         ;;
      x)
         ATLAS_EXTRA_CMAKE_ARGS+=($OPTARG)
         ;;
      k)
         ATLAS_EXTRA_MAKE_ARGS+=($OPTARG)
         ;;
      a)
         ATLAS_CI_BUILD="1"
         ;;
      c)
         ATLAS_EXE_CMAKE="1"
         ;;
      m)
         ATLAS_EXE_MAKE="1"
         ;;
      i)
         ATLAS_EXE_INSTALL="1"
         ;;
      p)
         ATLAS_EXE_CPACK="1"
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

# If no step was explicitly specified, turn them all on:
if [ -z "${ATLAS_EXE_CMAKE}" -a -z "${ATLAS_EXE_MAKE}" -a                      \
     -z "${ATLAS_EXE_INSTALL}" -a -z "${ATLAS_EXE_CPACK}" ]; then
   ATLAS_EXE_CMAKE="1"
   ATLAS_EXE_MAKE="1"
   ATLAS_EXE_CPACK="1"
fi

# If the caller has not specified any particular build tool arguments, then
# choose one according to the build tool being used, in order to make the build
# continue through any possible errors. Note that the CI build also doesn't stop
# on build errors. To be able to see all failures at once.
if [[ "${ATLAS_EXTRA_MAKE_ARGS[@]}" = "" ]]; then
   if [[ "${ATLAS_EXTRA_CMAKE_ARGS[@]}" =~ "Ninja" ]]; then
      ATLAS_EXTRA_MAKE_ARGS+=(-k0)
   else
      ATLAS_EXTRA_MAKE_ARGS+=(-k)
   fi
fi

# Try to get past CMake configuration errors as much as possible in a nightly
# build. (A failed CMake build configuration generation may still allow for a
# partially successful build...)
if [ "${ATLAS_CI_BUILD}" = "1" ]; then
   set -e
   # Enable build of CITest package
   ATLAS_EXTRA_CMAKE_ARGS+=(-DATLAS_ENABLE_CI_TESTS:BOOL=TRUE)
else
   set +e
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
cmake -E make_directory "${ATLAS_BUILD_DIR}" || ((ERROR_COUNT++))
ATLAS_BUILD_DIR=$(cd "${ATLAS_BUILD_DIR}" && pwd)

# Create the actual build directory for the project..
cmake -E make_directory "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}"       \
   || ((ERROR_COUNT++))

# Include the timing function.
source "${ATLAS_BUILD_SCRIPTS_DIR}/atlas_build_time.sh"

# Set up the build environment for the project.
source "${ATLAS_PROJECT_DIR}/build_env.sh" -b "${ATLAS_BUILD_DIR}"             \
   >& "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/build_env.log"
cat "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/build_env.log"

# Run the CMake configuration for the project.
if [ -n "${ATLAS_EXE_CMAKE}" ]; then
   # Remove the CMakeCache.txt file, to force CMake to find externals
   # from scratch in an incremental build.
   cmake -E remove -f                                                          \
      "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/CMakeCache.txt"
   # Now run the actual CMake configuration.
   { atlas_build_time PROJECT_CONFIG cmake                                     \
      --graphviz="${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/packages.dot" \
      -DCMAKE_BUILD_TYPE:STRING=${ATLAS_BUILDTYPE}                             \
      -DCTEST_USE_LAUNCHERS:BOOL=TRUE                                          \
      ${ATLAS_EXTRA_CMAKE_ARGS[@]}                                             \
      -B "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/"                     \
      -S "${ATLAS_PROJECT_DIR}/"; }                                            \
   2>&1 | tee "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/cmake_config.log" \
   || ((ERROR_COUNT++))
fi

# Run the build of the project.
if [ -n "${ATLAS_EXE_MAKE}" ]; then
   # Forcibly remove the merged CLID file from the previous build, to
   # avoid issues with some library possibly changing the name/CLID
   # of something during the build.
   cmake -E remove -f                                                          \
      "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/*/share/clid.db"

   # Delete all package CLID files if there are any conflicts (ATEAM-809)
   clid_conflicts=$(find "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}"      \
                         -name '*_clid.db' -exec cat {} + |                    \
                        sort -u | awk 'seen[$1]++')
   if [ -n "${clid_conflicts}" ]; then
       echo "Deleting clid.db files due to this conflict: ${clid_conflicts}"
       find "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}" -name '*_clid.db' -delete
   fi

   # Build the project.
   { atlas_build_time PROJECT_BUILD cmake                                      \
      --build "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}" --              \
      ${ATLAS_EXTRA_MAKE_ARGS[@]}; }                                           \
   2>&1 | tee "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/cmake_build.log" \
   || ((ERROR_COUNT++))
fi

# Install the results.
if [ -n "${ATLAS_EXE_INSTALL}" ]; then
   { DESTDIR="${ATLAS_BUILD_DIR}/install" atlas_build_time PROJECT_INSTALL     \
      cmake --install "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}"; }      \
	2>&1 | tee "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/cmake_install.log" \
   || ((ERROR_COUNT++)) &
fi

# Package up the release.
if [ -n "${ATLAS_EXE_CPACK}" ]; then
   # Build the "main" package for the project.
   { atlas_build_time PROJECT_PACKAGE cpack                                    \
      -B "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}"                      \
      --config "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/CPackConfig.cmake"; } \
   2>&1 | tee "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/cmake_cpack.log" \
   || ((ERROR_COUNT++)) &
   # If an "opt-dbg" package can be built for the project, build it.
   if [ -f "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/CPackDbgRPMConfig.cmake" ]; then
      { atlas_build_time PROJECT_PACKAGE_OPTDBG cpack                          \
         -B "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}"                   \
         --config "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/CPackDbgRPMConfig.cmake"; } \
      2>&1 | tee -a "${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/cmake_cpack.log" \
      || ((ERROR_COUNT++)) &
   fi
   # Wait for the packaging to finish.
   wait
   # Copy the file(s) in place. The redirection of stderr to /dev/null is needed
   # not to see complaints about missing files. Since it's okay not to have all
   # the types available that are listed here.
   FILES=$(ls ${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/${ATLAS_PROJECT_NAME}*.rpm \
              ${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/${ATLAS_PROJECT_NAME}*.dmg \
              ${ATLAS_BUILD_DIR}/build/${ATLAS_PROJECT_NAME}/${ATLAS_PROJECT_NAME}*.tar.gz \
              2>/dev/null)
   cmake -E copy ${FILES} "${ATLAS_BUILD_DIR}/" || ((ERROR_COUNT++))
fi

# Wait for all the backgrounded tasks to finish.
wait

# Exit with an error code, even if we didn't stop on those errors.
if [ ${ERROR_COUNT} -ne 0 ]; then
    echo "${ATLAS_PROJECT_NAME} build encountered ${ERROR_COUNT} error(s)"
fi
exit ${ERROR_COUNT}
