################################################################################
# Package: VP1LegoSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1LegoSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArIdentifier
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1LegoSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1LegoSystems
                   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COIN3D_LIBRARIES} VP1Base GL CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES CaloEvent LArReadoutGeometry LArIdentifier VP1Utils )

