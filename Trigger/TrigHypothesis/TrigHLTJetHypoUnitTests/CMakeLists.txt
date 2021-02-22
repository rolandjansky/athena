# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetHypoUnitTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )
find_package( GMock )

atlas_add_library( TrigHLTJetHypoUnitTestsLib
                   src/*.cxx
                   exerciser/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES TrigHLTJetHypoLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel xAODJet )

atlas_add_component( TrigHLTJetHypoUnitTests
                     exerciser/components/*.cxx
                     LINK_LIBRARIES TrigHLTJetHypoUnitTestsLib)
		     
atlas_add_test( TrigHLTJetHypoUnitTestsg
   SOURCES tests/all_tests.cxx
   tests/CombinationsGenTest.cxx
   tests/ProductGenTest.cxx
   tests/JetGroupProductTest.cxx
   tests/JetGroupUnionTest.cxx
   tests/TLorentzVectorFactoryTest.cxx
   tests/xAODJetCollectorTest.cxx
   tests/JetTrigTimerTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GoogleTestTools ${GMOCK_LIBRARIES} TrigHLTJetHypoLib TrigHLTJetHypoUnitTestsLib )
   
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
