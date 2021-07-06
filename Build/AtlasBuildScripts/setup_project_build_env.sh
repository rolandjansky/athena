# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script that is used by the build_env.sh scripts of the individual projects.
# It takes care of setting up a functional build environment for an ATLAS
# project.
#
# The includer script must set (default) values for the following variables
# before sourcing this script:
#  - ATLAS_PROJECT_NAME: The name of the main project, for which this script
#    is building an "externals project".
#  - ATLAS_PROJECT_DIR: Directory of the project that this script is being used
#    from.
#  - ATLAS_EXT_PROJECT_NAME: The name of the "externals project" needed by the
#    ATLAS project being built. If not specified, an "externals project" is not
#    looked for. Instead environment variables are set up to find the AtlasCMake
#    and AtlasLCG modules in the atlasexternals checkout.
#

# Function printing the usage information for the script.
atlas_env_usage() {
   echo "Script setting up the build environment for ${ATLAS_PROJECT_NAME}"
   echo ""
   echo "Usage: build_env.sh [-b build dir]"
   echo ""
   echo "If a build directory is not given, the default is '../build'"
   echo "relative to the athena source directory."
}

# Function setting up the build environment. Factorised into a function, in case
# it needs to be skipped.
atlas_env_setup() {

   # Get the absolute path of this script.
   if [ "x${BASH_SOURCE[0]}" = "x" ]; then
      # Hopefully we are in ZSH.
      ATLAS_BUILD_SCRIPTS_DIR=$(dirname `print -P %x`)
      if [ $? != 0 ]; then
         echo "ERROR: This script must be sourced from BASH or ZSH"
         return 1
      fi
   else
      # We are in BASH.
      ATLAS_BUILD_SCRIPTS_DIR=$(dirname ${BASH_SOURCE[0]})
   fi
   ATLAS_BUILD_SCRIPTS_DIR=$(cd ${ATLAS_BUILD_SCRIPTS_DIR} && pwd)

   # Check if the user specified any source/build directories.
   if [ "${ATLAS_BUILD_DIR}" = "" ]; then
      ATLAS_BUILD_DIR="${ATLAS_BUILD_SCRIPTS_DIR}/../../../build"
   fi
   ATLAS_BUILD_DIR=$(cd "${ATLAS_BUILD_DIR}" && pwd)
   # Export the build directory's value, as it is used by the CMake code to make
   # the built project relocatable.
   export ATLAS_BUILD_DIR

   # Get the version of the project to be built.
   ATLAS_PROJECT_VERSION=$(cat ${ATLAS_PROJECT_DIR}/version.txt)

   # Set up the environment variables for finding LCG and the TDAQ externals.
   source "${ATLAS_BUILD_SCRIPTS_DIR}/LCG_RELEASE_BASE.sh"
   source "${ATLAS_BUILD_SCRIPTS_DIR}/TDAQ_RELEASE_BASE.sh"

   # Set up the environment of the "externals project".
   if [ -z "${ATLAS_EXT_PROJECT_NAME}" ]; then
      ATLAS_EXT_DIR="${ATLAS_BUILD_DIR}/src/atlasexternals"
      if [ ! -d ${ATLAS_EXT_DIR} ]; then
         echo "Warning: Didn't find atlasexternals under ${ATLAS_EXT_DIR}!"
      fi
      export AtlasCMake_DIR="${ATLAS_EXT_DIR}/Build/AtlasCMake"
      export LCG_DIR="${ATLAS_EXT_DIR}/Build/AtlasLCG"
      echo "Picking up AtlasCMake from: ${AtlasCMake_DIR}"
      echo "Picking up AtlasLCG from  : ${LCG_DIR}"
   else
      ATLAS_EXT_DIR="${ATLAS_BUILD_DIR}/install/${ATLAS_EXT_PROJECT_NAME}/${ATLAS_PROJECT_VERSION}/InstallArea"
      if [ ! -d ${ATLAS_EXT_DIR} ]; then
	      echo "Warning: Didn't find ${ATLAS_EXT_PROJECT_NAME} under ${ATLAS_EXT_DIR}"
         echo "         (Hopefully this is intentional and you have done e.g. asetup AthenaExternals,master,latest)"
      else
         echo "Setting up ${ATLAS_EXT_PROJECT_NAME} from: ${ATLAS_EXT_DIR}"
         source ${ATLAS_EXT_DIR}/*/setup.sh
      fi
   fi

   # Return gracefully.
   return 0
}

# We need to reset the option index as we are sourcing this script, and the
# arguments may have already been parsed by the "parent" script. See:
# http://stackoverflow.com/questions/23581368/bug-in-parsing-args-with-getopts-in-bash
OPTIND=1

# Parse the command line arguments.
ATLAS_BUILD_DIR=""
while getopts "b:h" opt; do
   case $opt in
      b)
         ATLAS_BUILD_DIR=$OPTARG
         ;;
      h)
         atlas_env_usage
         ABORT=1
         ;;
      :)
         echo "Argument -$OPTARG requires a parameter!"
         atlas_env_usage
         ABORT=1
         ;;
      ?)
         echo "Unknown argument: -$OPTARG"
         atlas_env_usage
         ABORT=1
         ;;
   esac
done

# Don't do the environment setup if the argument parsin was not successful.
if [ -z "$ABORT" ]; then
   atlas_env_setup
fi
