# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigAnalysisTest )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist RIO )

# Extra sources and libraries, based on the build environment:
set( extra_srcs )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_srcs Root/*.cxx )
   set( extra_libs AsgTools xAODRootAccess TrigConfxAODLib )
endif()

# Component(s) in the package:
atlas_add_library( TrigAnalysisTestLib
   TrigAnalysisTest/*.h
   Root/BasicTriggerFired.cxx
   Root/Run1BStoxAODTrigger.cxx
   Root/TestFactory.cxx
   ${extra_srcs}
   PUBLIC_HEADERS TrigAnalysisTest
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES TrigDecisionToolLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODMuon xAODTrigBphys
   xAODTrigMissingET ${extra_libs} )

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_triggers_fired
      SOURCES test/ut_triggers_fired.cxx
      LINK_LIBRARIES TrigAnalysisTestLib )

   atlas_add_test( ut_validate_run1_bs_to_xaod
      SOURCES test/ut_validate_run1_bs_to_xaod.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigAnalysisTestLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.py test/test*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/exec*.sh test/test*.sh )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

# Unit tests:
atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
