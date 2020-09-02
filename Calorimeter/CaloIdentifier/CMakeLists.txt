################################################################################
# Package: CaloIdentifier
################################################################################

# Declare the package name:
atlas_subdir( CaloIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/IdDictParser
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS CaloIdentifier
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils IdDictParser PathResolver )

atlas_add_dictionary( CaloIdentifierDict
                      CaloIdentifier/CaloIdentifierDict.h
                      CaloIdentifier/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

atlas_add_executable( test_lar_id
                      test/test_lar_id.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

atlas_add_executable( test_calo_dm_id
                      test/test_calo_dm_id.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

atlas_add_executable( test_cell_id
                      test/test_cell_id.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

atlas_add_executable( test_lvl1_id
                      test/test_lvl1_id.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

atlas_add_executable( caloCellLookup
                      utils/caloCellLookup.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier )

#atlas_add_test( GTower_ID_test
#                SOURCES
#                test/GTower_ID_test.cxx
#                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
#                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
#                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

#atlas_add_test( JTower_ID_test
#                SOURCES
#                test/JTower_ID_test.cxx
#                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
#                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
#                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArEM_ID_test
                SOURCES
                test/LArEM_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArEM_SuperCell_ID_test
                SOURCES
                test/LArEM_SuperCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArHEC_ID_test
                SOURCES
                test/LArHEC_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArHEC_SuperCell_ID_test
                SOURCES
                test/LArHEC_SuperCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArFCAL_ID_test
                SOURCES
                test/LArFCAL_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArFCAL_SuperCell_ID_test
                SOURCES
                test/LArFCAL_SuperCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( LArMiniFCAL_ID_test
                SOURCES
                test/LArMiniFCAL_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( TileID_test
                SOURCES
                test/TileID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( Tile_SuperCell_ID_test
                SOURCES
                test/Tile_SuperCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( CaloCell_ID_test
                SOURCES
                test/CaloCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( CaloCell_SuperCell_ID_test
                SOURCES
                test/CaloCell_SuperCell_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( CaloIDHelper_test
                SOURCES
                test/CaloIDHelper_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel AtlasDetDescr IdDict Identifier GaudiKernel TestTools CxxUtils StoreGateLib SGtests IdDictParser PathResolver CaloIdentifier
                LOG_IGNORE_PATTERN "mask/zero|Reading file|^AtlasDetectorID(Helper)?::" )

atlas_add_test( CaloHelpersTest_test
                SOURCES
                test/CaloHelpersTest_test.cxx
                LINK_LIBRARIES CaloIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/SuperCaloNeighborsSuperCells-April2014.dat share/FCalSuperCells2DNeighborsNew-April2014.txt share/FCalSuperCells3DNeighborsPrevNew-April2014.txt share/FCalSuperCells3DNeighborsNextNew-April2014.txt share/TileSuperCellNeighbour.txt share/TileNeighbour.txt share/TileNeighbour_reduced.txt share/FCal2DNeighbors.txt share/FCal3DNeighborsPrev.txt share/FCal3DNeighborsNext.txt share/FCal2DNeighbors_H6.txt share/FCal3DNeighborsPrev_H6.txt share/FCal3DNeighborsNext_H6.txt share/SuperCaloNeighbours.dat share/SuperCaloNeighbours_H6.dat share/SuperCaloNeighbours_H8.dat share/FCal2DNeighbors-DC3-05-Comm-01.txt share/FCal3DNeighborsNext-DC3-05-Comm-01.txt share/FCal3DNeighborsPrev-DC3-05-Comm-01.txt share/SuperCaloNeighbours-DC3-05-Comm-01.dat share/FCal2DNeighbors-April2011.txt share/FCal3DNeighborsNext-April2011.txt share/FCal3DNeighborsPrev-April2011.txt share/SuperCaloNeighbours-April2011.dat share/sFCal2DNeighborsNew-Dec2014.txt share/sFCal3DNeighborsNextNew-Dec2014.txt share/sFCal3DNeighborsPrevNew-Dec2014.txt share/SuperCaloNeighbours-sFCal-Dec2014.dat )

