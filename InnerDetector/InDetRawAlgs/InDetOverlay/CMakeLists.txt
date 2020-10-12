# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetOverlay )

# External dependencies:
find_package( CLHEP )
find_package( GTest )
find_package( GMock )

# Helper variable(s):
set( _jobOPath
    "${CMAKE_CURRENT_SOURCE_DIR}/share:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Unit Tests
atlas_add_test( BCMOverlay_test
                SOURCES test/BCMOverlay_test.cxx src/BCMOverlay.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES AthenaBaseComps GaudiKernel CxxUtils StoreGateLib SGtests GeneratorObjects InDetBCM_RawData InDetSimData ${GTEST_LIBRARIES} )

atlas_add_test( PixelOverlay_test
                SOURCES test/PixelOverlay_test.cxx src/PixelOverlay.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData CxxUtils StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack ${GTEST_LIBRARIES} )

atlas_add_test( SCTOverlay_test
                SOURCES test/SCTOverlay_test.cxx src/SCTOverlay.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData CxxUtils StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack IdDictParser ${GTEST_LIBRARIES}
                ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( TRTOverlay_test
                SOURCES test/TRTOverlay_test.cxx src/TRTOverlay.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack TRT_ConditionsServicesLib TRT_ElectronPidToolsLib IdDictParser ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
                ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( InDetOverlay_TRTOverlay_test PROPERTIES ENABLE_EXPORTS True )

# Component(s) in the package:
atlas_add_component( InDetOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetBCM_RawData InDetIdentifier InDetSimData TrkTrack TRT_ConditionsServicesLib TRT_ElectronPidToolsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/StoreGateTestCommon.txt )

# Configuration tests
atlas_add_test( BCMOverlayConfig_test
                SCRIPT test/BCMOverlayConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( PixelOverlayConfig_test
                SCRIPT test/PixelOverlayConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( SCTOverlayConfig_test
                SCRIPT test/SCTOverlayConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( TRTOverlayConfig_test
                SCRIPT test/TRTOverlayConfig_test.py
                PROPERTIES TIMEOUT 300 )
