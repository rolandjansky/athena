################################################################################
# Package: VP1CaloClusterSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1CaloClusterSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Calorimeter/CaloEvent
                          graphics/VP1/VP1HEPVis
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1CaloClusterSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1CaloClusterSystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent VP1HEPVis VP1Utils )

