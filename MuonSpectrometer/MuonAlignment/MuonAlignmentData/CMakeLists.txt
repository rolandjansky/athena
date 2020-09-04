################################################################################
# Package: MuonAlignmentData
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignmentData )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( MuonAlignmentData
                   src/*.cxx
                   PUBLIC_HEADERS MuonAlignmentData
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers Identifier MuonIdHelpersLib )

