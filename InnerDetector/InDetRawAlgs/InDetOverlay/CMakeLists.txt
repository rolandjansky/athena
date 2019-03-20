################################################################################
# Package: InDetOverlay
################################################################################

# Declare the package name:
atlas_subdir( InDetOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/EventOverlay/IDC_OverlayBase
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/IdDictParser
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
                          Tracking/TrkEvent/TrkTrack
                          )

#External dependencies:
find_package( CLHEP )
find_package( GTest )

# Helper variable(s):
set( _jobOPath
    "${CMAKE_CURRENT_SOURCE_DIR}/share:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Unit Tests
atlas_add_test( PixelOverlay_test
                SOURCES test/PixelOverlay_test.cxx src/PixelOverlay.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack ${GTEST_LIBRARIES}
                EXTRA_PATTERNS "[0-9]+ ms")

atlas_add_test( SCTOverlay_test
                SOURCES src/SCTOverlay.cxx test/SCTOverlay_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack IdDictParser ${GTEST_LIBRARIES}
                EXTRA_PATTERNS "[0-9]+ ms"
                ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )


# Component(s) in the package:
atlas_add_component( InDetOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack TRT_ConditionsServicesLib)

# Install files from the package:
atlas_install_headers( InDetOverlay )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/StoreGateTestCommon.txt )
