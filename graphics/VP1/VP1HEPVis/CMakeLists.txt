################################################################################
# Package: VP1HEPVis
################################################################################
# Author: Joe Boudreau (Importer)
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VP1HEPVis )

message( "CMAKE_PREFIX_PATH = $ENV{CMAKE_PREFIX_PATH}" )

# External dependencies:
find_package( Coin3D  )
find_package( OpenGL )

message("VP1HEPVis: '${COIN3D_FOUND}' '${COIN3D_INCLUDE_DIRS}'
'${COIN3D_LIBRARIES}' '${COIN3D_LIBRARY}' '${COIN3D_INCLUDE_DIR}' ")

# Component(s) in the package:
atlas_add_library( VP1HEPVis src/*.cxx
   PUBLIC_HEADERS VP1HEPVis
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} ${OPENGL_LIBRARIES} GL )
