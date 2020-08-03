# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/post.sh share/nopost.sh )

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
