################################################################################
# Package: TRT_G4Utilities
################################################################################

# Declare the package name:
atlas_subdir( TRT_G4Utilities )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( TRT_G4Utilities
                   src/*.cc
                   PUBLIC_HEADERS TRT_G4Utilities
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel CxxUtils
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} PathResolver )

# Install files from the package:
atlas_install_runtime( share/TRT_G4Utilities_management.txt share/TRT_G4Utilities_geometry.txt )

atlas_add_test( ut_TRT_G4UtilitiesTest
   SOURCES test/ut_TRTParametersTest.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TRT_G4Utilities )

