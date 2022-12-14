################################################################################
# Package: TrackWriteFastSim
################################################################################

# Declare the package name:
atlas_subdir( TrackWriteFastSim )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TrackWriteFastSimLib
                   src/*.cxx
                   OBJECT
                   PUBLIC_HEADERS TrackWriteFastSim
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} StoreGateLib G4AtlasToolsLib TrackRecordLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel G4AtlasInterfaces MCTruth )

atlas_add_library(TrackWriteFastSim
                  src/components/*.cxx
                  OBJECT
                  NO_PUBLIC_HEADERS
                  PRIVATE_LINK_LIBRARIES TrackWriteFastSimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

