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
#   DEPENDS_SUCCESS otherTest: define test dependency on `otherTest` and only run if
#                              `otherTest` was successfull (implemented as test fixture)
#

function( atlas_add_citest testName )
   # Look for possible extra arguments:
   cmake_parse_arguments( ARG "" "POST_EXEC_SCRIPT" "DEPENDS_SUCCESS" ${ARGN} )

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
      PROPERTIES FIXTURES_SETUP CITest_${testName}_fixture
      PRE_EXEC_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_pre.sh
      POST_EXEC_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${testName}_citest_post.sh
      ${ARG_UNPARSED_ARGUMENTS} )

   # Each test defines a setup test fixture (see above). Use this fixture to define
   # test dependencies that are only run on success of the dependee(s):
   if( ARG_DEPENDS_SUCCESS )
      foreach( _test ${ARG_DEPENDS_SUCCESS} )
         set_property( TEST CITest_${testName}_ctest
            APPEND PROPERTY FIXTURES_REQUIRED "CITest_${_test}_fixture" )
      endforeach()
   endif()

endfunction( atlas_add_citest )
