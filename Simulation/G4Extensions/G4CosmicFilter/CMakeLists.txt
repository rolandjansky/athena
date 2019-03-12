################################################################################
# Package: G4CosmicFilter
################################################################################

# Declare the package name:
atlas_subdir( G4CosmicFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          PRIVATE
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4CosmicFilter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel G4AtlasInterfaces G4AtlasToolsLib MCTruth )

# Install files from the package:
atlas_install_headers( G4CosmicFilter )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

