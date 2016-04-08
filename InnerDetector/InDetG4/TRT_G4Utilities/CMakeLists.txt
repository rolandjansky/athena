################################################################################
# Package: TRT_G4Utilities
################################################################################

# Declare the package name:
atlas_subdir( TRT_G4Utilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          PRIVATE
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TRT_G4Utilities
                   src/*.cc
                   PUBLIC_HEADERS TRT_G4Utilities
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} PathResolver )

# Install files from the package:
atlas_install_runtime( share/TRT_G4Utilities_management.txt share/TRT_G4Utilities_geometry.txt )

