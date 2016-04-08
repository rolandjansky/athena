################################################################################
# Package: TileSimEvent
################################################################################

# Declare the package name:
atlas_subdir( TileSimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          Simulation/HitManagement
                          Tools/AtlasDoxygen
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          TileCalorimeter/TileConditions )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TileSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS TileSimEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools Identifier HitManagement TileConditionsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier )

atlas_add_dictionary( TileSimEventDict
                      TileSimEvent/TileSimEventDict.h
                      TileSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools Identifier HitManagement CaloIdentifier TileConditionsLib TileSimEvent )

