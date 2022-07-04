# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( Asg_Test )

# Read in the settings of the package from the files under cmt/:
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/ReleaseSeries.txt
   ROOTCORE_RELEASE_SERIES )
string( STRIP "${ROOTCORE_RELEASE_SERIES}" ROOTCORE_RELEASE_SERIES )

file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileData.txt
   ASG_TEST_FILE_DATA )
string( STRIP "${ASG_TEST_FILE_DATA}" ASG_TEST_FILE_DATA )
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileMC.txt
   ASG_TEST_FILE_MC )
string( STRIP "${ASG_TEST_FILE_MC}" ASG_TEST_FILE_MC )
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileMCAFII.txt
   ASG_TEST_FILE_MC_AFII )
string( STRIP "${ASG_TEST_FILE_MC_AFII}" ASG_TEST_FILE_MC_AFII )

file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileLiteData.txt
   ASG_TEST_FILE_LITE_DATA )
string( STRIP "${ASG_TEST_FILE_LITE_DATA}" ASG_TEST_FILE_LITE_DATA )
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileLiteMC.txt
   ASG_TEST_FILE_LITE_MC )
string( STRIP "${ASG_TEST_FILE_LITE_MC}" ASG_TEST_FILE_LITE_MC )
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileLiteMCAFII.txt
   ASG_TEST_FILE_LITE_MC_AFII )
string( STRIP "${ASG_TEST_FILE_LITE_MC_AFII}" ASG_TEST_FILE_LITE_MC_AFII )

# currently don't have a full set of Run 3 test input files
#file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileR3Data.txt
#   ASG_TEST_FILE_RUN3_DATA )
#string( STRIP "${ASG_TEST_FILE_RUN3_DATA}" ASG_TEST_FILE_RUN3_DATA )
file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileR3MC.txt
   ASG_TEST_FILE_RUN3_MC )
string( STRIP "${ASG_TEST_FILE_RUN3_MC}" ASG_TEST_FILE_RUN3_MC )
#file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/TestFileR3MCAFII.txt
#   ASG_TEST_FILE_RUN3_MC_AFII )
#string( STRIP "${ASG_TEST_FILE_RUN3_MC_AFII}" ASG_TEST_FILE_RUN3_MC_AFII )

set( ROOTCORE_TEST_FILE ${ASG_TEST_FILE_MC} )

get_filename_component( ROOTCORE_TEST_DATA ${ROOTCORE_TEST_FILE} PATH )

# Create the CMake file that will be used to generate the environment config:
configure_file( 
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Asg_TestEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Asg_TestEnvironmentConfig.cmake
   @ONLY )
set( Asg_TestEnvironment_DIR
   ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}
   CACHE PATH "Location of Asg_TestEnvironmentConfig.cmake" FORCE )

# Now find it:
find_package( Asg_TestEnvironment )
