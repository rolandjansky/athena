################################################################################
# Package: LArG4FastSimSvc
################################################################################

# Declare the package name:
atlas_subdir( LArG4FastSimSvc )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4FastSimSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps )

# Install files from the package:
atlas_install_headers( LArG4FastSimSvc )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

