# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( AssociationUtils )

# External(s):
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS program_options )
find_package( GTest )

# Libraries in the package:
atlas_add_library( AssociationUtilsLib
   AssociationUtils/*.h Root/*.cxx
   PUBLIC_HEADERS AssociationUtils
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AnaAlgorithmLib AsgMessagingLib AsgTools AthContainers AthLinks
   xAODBase xAODEgamma xAODJet xAODMuon xAODTau xAODTracking
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} FourMomUtils )

atlas_add_dictionary( AssociationUtilsDict
    AssociationUtils/AssociationUtilsDict.h
    AssociationUtils/selection.xml
    LINK_LIBRARIES AssociationUtilsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( AssociationUtils
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps xAODBase xAODBTagging GaudiKernel AssociationUtilsLib )
endif()

# Executables in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( OverlapRemovalTester
      util/OverlapRemovalTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
      xAODEventInfo xAODEgamma xAODMuon xAODJet xAODBTagging xAODTau xAODEgamma
      AssociationUtilsLib )
endif()

# Tests in the package:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs xAODRootAccess )
endif()

# Helper function for defining google tests
macro( _add_gtest name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgTestingLib AssociationUtilsLib
      ${extra_libs}
      POST_EXEC_SCRIPT nopost.sh )
endmacro( _add_gtest )

# Define the google unit tests.
# Disabling some which don't work in Athena.
foreach( test gt_AnaToolHandle_test ) # gt_OverlapRemovalInit_test gt_toolbox_test
   _add_gtest( ${test} )
endforeach()

if( XAOD_STANDALONE )
   atlas_add_test( ut_assocUtilsDict
      SCRIPT test/ut_assocUtilsDict.py
      POST_EXEC_SCRIPT nopost.sh )
   atlas_add_test( ut_ort_tester
      SCRIPT test/ut_ort_tester.sh
      POST_EXEC_SCRIPT nopost.sh )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
