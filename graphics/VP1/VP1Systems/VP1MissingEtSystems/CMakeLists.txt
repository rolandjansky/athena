################################################################################
# Package: VP1MissingEtSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1MissingEtSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Reconstruction/MissingETEvent
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1MissingEtSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1MissingEtSystems
                   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${COIN3D_LIBRARIES} VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} MissingETEvent VP1Utils )

