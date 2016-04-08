################################################################################
# Package: VP1CaloReadoutSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1CaloReadoutSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1CaloReadoutSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1CaloReadoutSystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} LArHV LArReadoutGeometry VP1Utils )

