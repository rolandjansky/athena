################################################################################
# Package: G4HitFilter
################################################################################

# Declare the package name:
atlas_subdir( G4HitFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4HitFilter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CaloSimEvent AthenaBaseComps CxxUtils StoreGateLib SGtests LUCID_SimEvent InDetSimEvent LArSimEvent MuonSimEvent G4AtlasInterfaces G4AtlasToolsLib TileSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

