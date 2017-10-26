################################################################################
# Package: TileEvent
################################################################################

# Declare the package name:
atlas_subdir( TileEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventContainers
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier
                          TileCalorimeter/TileSimEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TileEvent
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent AthContainers Identifier GaudiKernel TileIdentifier TileSimEvent StoreGateLib SGtests TileConditionsLib CaloDetDescrLib EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier SGTools )

atlas_add_dictionary( TileEventDict
                      TileEvent/TileEventDict.h
                      TileEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthContainers StoreGateLib SGtests Identifier GaudiKernel TileConditionsLib TileIdentifier TileSimEvent CaloDetDescrLib CaloIdentifier SGTools TileEvent EventContainers )

