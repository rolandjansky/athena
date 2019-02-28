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
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
                          Tracking/TrkEvent/TrkTrack
                          )

#External dependencies:
find_package( CLHEP )
find_package( GTest )

atlas_add_test( PixelOverlay_test
                SOURCES src/PixelOverlay.cxx test/PixelOverlay_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES  ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack ${GTEST_LIBRARIES}
                EXTRA_PATTERNS "[0-9]+ ms")


# Component(s) in the package:
atlas_add_component( InDetOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests GeneratorObjects InDetIdentifier InDetSimData TrkTrack TRT_ConditionsServicesLib)

# Install files from the package:
atlas_install_headers( InDetOverlay )
atlas_install_python_modules( python/*.py )
