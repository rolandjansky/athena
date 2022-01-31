# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Function to setup CI tests. Acccepts the same arguments as
# atlas_add_test and sets some suitable defaults:
#
# - tests are run within their own working directory (cleaned before each run)
# - default (long) timeout
# - use default post-processing script for log files
#
# Additonal arguments:
#
#   DEPENDS otherTest: define test dependency on `otherTest`
#
function( atlas_add_citest testName )
   # Look for possible extra arguments:
   cmake_parse_arguments( ARG "" "DEPENDS;POST_EXEC_SCRIPT" "" ${ARGN} )

   # define private working directory (cleaned in PRE_EXEC_SCRIPT)
   set( CI_WORKDIR "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ciTestRun/${testName}" )
   file( MAKE_DIRECTORY "${CI_WORKDIR}" )

   # configure post-processing:
   if( ARG_POST_EXEC_SCRIPT )
      set( CI_POST_EXEC_SCRIPT ${ARG_POST_EXEC_SCRIPT} )
   else()
      set( CI_POST_EXEC_SCRIPT "true" )  # shell builtin always returning success
   endif()

   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/citest_pre.sh.in
      ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_pre.sh @ONLY )

   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/citest_post.sh.in
      ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_post.sh @ONLY )

   atlas_add_test( ${testName}
      PROPERTIES WORKING_DIRECTORY ${CI_WORKDIR}
      PROPERTIES TIMEOUT 3600
      PRE_EXEC_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_pre.sh
      POST_EXEC_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_post.sh
      ${ARG_UNPARSED_ARGUMENTS} )

   if( ARG_DEPENDS )
      set_property( TEST CITest_${testName}_ctest
         PROPERTY DEPENDS "CITest_${ARG_DEPENDS}_ctest" )
   endif()

endfunction( atlas_add_citest )
