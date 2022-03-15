# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TestTools )

# In standalone mode we just use the headers from the package. While in
# offline mode we build a proper library.
if( XAOD_STANDALONE )
   atlas_add_library( TestTools
      TestTools/*.h
      INTERFACE
      PUBLIC_HEADERS TestTools )
else()
   atlas_add_library( TestTools
      TestTools/*.h src/*.cxx
      PUBLIC_HEADERS TestTools
      PRIVATE_LINK_LIBRARIES ${CMAKE_DL_LIBS} GaudiKernel )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/post.sh share/nopost.sh share/noerror.sh )

if( NOT XAOD_STANDALONE )
   find_package( TBB )
   atlas_add_test( ParallelCallTest_test
      INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
      SOURCES test/test_ParallelCallTestExample.cxx
      LINK_LIBRARIES ${TBB_LIBRARIES}  TestTools  )
endif()

# Unit tests for post.sh script:
atlas_add_test( postsh_default_test
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log" )

atlas_add_test( postsh_failure_test
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log"
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( postsh_ignore_test
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log"
   LOG_IGNORE_PATTERN "MyAlg" )

atlas_add_test( postsh_select_test
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log"
   LOG_SELECT_PATTERN "MyAlg" )

atlas_add_test( postsh_select_ignore_test
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log"
   LOG_SELECT_PATTERN "MyAlg"
   LOG_IGNORE_PATTERN "XYZ marker" )

# Unit tests for noerror.sh script:
atlas_add_test( noerror_test_success
   SCRIPT "echo Hello"
   POST_EXEC_SCRIPT noerror.sh )

atlas_add_test( noerror_test_failure
   SCRIPT "exit 1"
   POST_EXEC_SCRIPT noerror.sh
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( noerror_failure
   SCRIPT cat "${CMAKE_CURRENT_SOURCE_DIR}/test/test_postsh_example.log"
   POST_EXEC_SCRIPT noerror.sh
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( noerror_fpe
   SCRIPT "echo INFO       FPEAuditor        WARNING FPE INVALID in"
   POST_EXEC_SCRIPT noerror.sh
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( noerror_select
   SCRIPT "echo This is a failure"
   LOG_SELECT_PATTERN ".*failure.*"
   POST_EXEC_SCRIPT noerror.sh
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( noerror_ignore
   SCRIPT "echo ERROR known error"
   LOG_IGNORE_PATTERN ".*ERROR known"
   POST_EXEC_SCRIPT noerror.sh )

atlas_add_test( noerror_false_positive
   SCRIPT "echo ['WARNING', 'ERROR', 'FATAL']"
   POST_EXEC_SCRIPT noerror.sh )


# Unit test for python modules
atlas_add_test( pymodules
                SCRIPT python -m unittest discover -v -s ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )
