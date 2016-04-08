################################################################################
# Package: QatDataModeling
################################################################################

# Declare the package name:
atlas_subdir( QatDataModeling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatDataAnalysis )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( QatDataModeling
                   src/*.cpp
                   src/*.cc
                   src/*.F
                   PUBLIC_HEADERS QatDataModeling
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} QatDataAnalysis )

