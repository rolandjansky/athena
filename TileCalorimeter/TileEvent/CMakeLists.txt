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
                          DetectorDescription/IdDictParser
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TileEvent
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent AthContainers Identifier GaudiKernel TileIdentifier TileSimEvent StoreGateLib SGtests TileConditionsLib CaloDetDescrLib EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel TileCalibBlobObjs )

atlas_add_dictionary( TileEventDict
                      TileEvent/TileEventDict.h
                      TileEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthContainers StoreGateLib SGtests Identifier GaudiKernel TileConditionsLib TileIdentifier TileSimEvent CaloDetDescrLib CaloIdentifier AthenaKernel TileEvent EventContainers )


atlas_add_test( TileRawChannelCollection_test
                SOURCES
                test/TileRawChannelCollection_test.cxx
                LINK_LIBRARIES TileEvent )


atlas_add_test( TileDigitsCollection_test
                SOURCES
                test/TileDigitsCollection_test.cxx
                LINK_LIBRARIES TileEvent )

atlas_add_test( TileHitCollection_test
                SOURCES
                test/TileHitCollection_test.cxx
                LINK_LIBRARIES TileEvent )

atlas_add_test( TileBeamElemCollection_test
                SOURCES
                test/TileBeamElemCollection_test.cxx
                LINK_LIBRARIES TileEvent )


atlas_add_test( TileMutableDataContainer_test
                SOURCES
                test/TileMutableDataContainer_test.cxx
                LINK_LIBRARIES TileEvent IdDictParser )


atlas_add_test( TileMutableRawChannelContainer_test
                SOURCES
                test/TileMutableRawChannelContainer_test.cxx
                LINK_LIBRARIES TileEvent IdDictParser )

atlas_add_test( TileMutableDigitsContainer_test
                SOURCES
                test/TileMutableDigitsContainer_test.cxx
                LINK_LIBRARIES TileEvent IdDictParser )

atlas_add_test( TileMutableHitContainer_test
                SOURCES
                test/TileMutableHitContainer_test.cxx
                LINK_LIBRARIES TileEvent IdDictParser )

atlas_add_test( TileMutableBeamElemContainer_test
                SOURCES
                test/TileMutableBeamElemContainer_test.cxx
                LINK_LIBRARIES TileEvent IdDictParser )
