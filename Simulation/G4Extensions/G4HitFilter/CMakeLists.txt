################################################################################
# Package: G4HitFilter
################################################################################

# Declare the package name:
atlas_subdir( G4HitFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          PRIVATE
                          Calorimeter/CaloSimEvent
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonSimEvent
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
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4AtlasToolsLib CaloSimEvent LUCID_SimEvent InDetSimEvent LArSimEvent MuonSimEvent TileSimEvent )

# Install files from the package:
atlas_install_headers( G4HitFilter )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

