################################################################################
# Package: Quirks
################################################################################

# Declare the package name:
atlas_subdir( Quirks )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( Quirks
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps G4AtlasInterfaces G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

