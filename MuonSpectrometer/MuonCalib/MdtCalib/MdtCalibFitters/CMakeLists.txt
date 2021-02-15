################################################################################
# Package: MdtCalibFitters
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibFitters )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( MdtCalibFitters
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibFitters
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeoPrimitives EventPrimitives MuonCalibEventBase MuonCalibMath MdtCalibInterfacesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

