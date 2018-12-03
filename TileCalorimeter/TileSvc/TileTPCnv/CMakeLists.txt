################################################################################
# Package: TileTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TileTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventContainers
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSvc/TileSimEventTPCnv
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          DetectorDescription/IdDictParser
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TileTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TileTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier )

atlas_add_dictionary( TileTPCnvDict
                      TileTPCnv/TileTPCnvDict.h
                      TileTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_dictionary( OLD_TileTPCnvDict
                      TileTPCnv/TileTPCnvDict.h
                      TileTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileBeamElemCnv_p1_test
                SOURCES
                test/TileBeamElemCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileCosmicMuonCnv_p1_test
                SOURCES
                test/TileCosmicMuonCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileCosmicMuonCnv_p2_test
                SOURCES
                test/TileCosmicMuonCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsCnv_p1_test
                SOURCES
                test/TileDigitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsCnv_p2_test
                SOURCES
                test/TileDigitsCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsCnv_p3_test
                SOURCES
                test/TileDigitsCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileRawChannelCnv_p1_test
                SOURCES
                test/TileRawChannelCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileTTL1CellCnv_p1_test
                SOURCES
                test/TileTTL1CellCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileTTL1Cnv_p1_test
                SOURCES
                test/TileTTL1Cnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileEvent TileSimEventTPCnv TestTools AthenaKernel IdDictParser GaudiKernel TileConditionsLib TileIdentifier TileTPCnv )

