# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: project.py 724502 2016-02-17 11:07:33Z krasznaa $

## Function generating a top level CMake project file
#
# This code can generate a simple main project file, which relies heavily
# on the CMake code in this package.
#
# @param fname The name of the file to write
# @param pname The name of the project to set up
# @param pvers The version of the project to set up
#
def generateProjectFile( fname, pname, pvers ):

    # Open the main output file:
    cmakeFile = open( fname, "w" )
    cmakeFile.write(
    """# The minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# If there's a directory called AtlasCMake in the source
# tree, and the user didn't specify AtlasProject_DIR yet, then let's give
# a default value for it.
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/AtlasCMake AND
   NOT AtlasCMake_DIR AND NOT ENV{AtlasCMake_DIR} )
   set( AtlasCMake_DIR ${CMAKE_SOURCE_DIR}/Build/AtlasCMake )
endif()
# The same for the AnalysisBase project:
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/AnalysisBase AND
   NOT AnalysisBaseRelease_DIR AND NOT ENV{AnalysisBaseRelease_DIR} )
   set( AnalysisBaseRelease_DIR ${CMAKE_SOURCE_DIR}/AnalysisBase )
endif()

# Find the ATLAS CMake code:
find_package( AtlasCMake )
find_package( AnalysisBaseRelease )

# Declare the project:
atlas_project( %s %s )

# Generate the RootCore/Packages.h header:
analysisbase_generate_release_header()

# Generate the environment configuration file(s):
analysisbase_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
analysisbase_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}/env_setup_install.sh
   REPLACE \"${CMAKE_INSTALL_PREFIX}\" \"\\${%s_DIR}\"
           \".*${CMAKE_BINARY_DIR}/.*\" \"\" )
install( FILES ${CMAKE_BINARY_DIR}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Set up the release packaging:
atlas_cpack_setup()
"""
    % ( pname, pvers, pname )
    )
    cmakeFile.close()

    # Return gracefully:
    return

## Function generating a CTest/CDash configuration file
#
# This code generates a CTestConfig.cmake file that is configured to upload
# information to the test CDash instance on pcadp02.cern.ch.
#
# @param fname The name of the file to write
# @param pname The name of the project to set up
#
def generateCTestFile( fname, pname ):

    # Open the main output file:
    cmakeFile = open( fname, "w" )
    cmakeFile.write(
    """
set(CTEST_PROJECT_NAME "%s")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "pcadp02.cern.ch")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=%s")
set(CTEST_DROP_SITE_CDASH TRUE)
    """
    % ( pname, pname )
    )
    cmakeFile.close()

    # Return gracefully:
    return
