################################################################################
# Package: TRT_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( TRT_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetG4/TRT_G4Utilities
                          InnerDetector/InDetSimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                           )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TRT_G4_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel CxxUtils StoreGateLib SGtests GaudiKernel TRT_G4Utilities InDetSimEvent G4AtlasToolsLib MCTruth )


atlas_add_test( TRT_G4_SDToolConfig_test
                SCRIPT test/TRT_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )


# Install files from the package:
atlas_install_python_modules( python/*.py )

