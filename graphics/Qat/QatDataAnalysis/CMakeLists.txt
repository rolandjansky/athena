################################################################################
# Package: QatDataAnalysis
################################################################################

# Declare the package name:
atlas_subdir( QatDataAnalysis )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( QatDataAnalysis
                   src/*.cpp
                   PUBLIC_HEADERS QatDataAnalysis
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} )

