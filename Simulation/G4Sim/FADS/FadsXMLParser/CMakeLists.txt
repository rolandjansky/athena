################################################################################
# Package: FadsXMLParser
################################################################################

# Declare the package name:
atlas_subdir( FadsXMLParser )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Simulation/G4Sim/FADS/FadsMaterial
                          Simulation/G4Sim/FADS/FadsUtilities )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( FadsXMLParser
                   src/*.cxx
                   PUBLIC_HEADERS FadsXMLParser
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} FadsMaterial FadsUtilities )

