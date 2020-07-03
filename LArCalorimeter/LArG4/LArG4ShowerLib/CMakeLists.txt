################################################################################
# Package: LArG4ShowerLib
################################################################################

# Declare the package name:
atlas_subdir( LArG4ShowerLib )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( LArG4ShowerLib
                   src/*.cxx
                   PUBLIC_HEADERS LArG4ShowerLib
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers LArG4Code AtlasHepMCLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} )

