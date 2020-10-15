# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TileTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TileTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib EventContainers TileEvent TileSimEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( TileTPCnvDict
                      TileTPCnv/TileTPCnvDict.h
                      TileTPCnv/selection.xml
                      LINK_LIBRARIES TileTPCnv )

atlas_add_dictionary( OLD_TileTPCnvDict
                      TileTPCnv/TileTPCnvDict.h
                      TileTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TileTPCnv )

# Tests in the package:
atlas_add_test( TileBeamElemCnv_p1_test
                SOURCES
                test/TileBeamElemCnv_p1_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileBeamElemContainerCnv_p1_test
                SOURCES
                test/TileBeamElemContainerCnv_p1_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileCosmicMuonCnv_p1_test
                SOURCES
                test/TileCosmicMuonCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools TileTPCnv )

atlas_add_test( TileCosmicMuonCnv_p2_test
                SOURCES
                test/TileCosmicMuonCnv_p2_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel TestTools TileTPCnv )

atlas_add_test( TileDigitsCnv_p1_test
                SOURCES
                test/TileDigitsCnv_p1_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsCnv_p2_test
                SOURCES
                test/TileDigitsCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsCnv_p3_test
                SOURCES
                test/TileDigitsCnv_p3_test.cxx
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileDigitsContainerCnv_p1_test
                SOURCES
                test/TileDigitsContainerCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileDigitsContainerCnv_p2_test
                SOURCES
                test/TileDigitsContainerCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileDigitsContainerCnv_p3_test
                SOURCES
                test/TileDigitsContainerCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileHitContainerCnv_p1_test
                SOURCES
                test/TileHitContainerCnv_p1_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileRawChannelCnv_p1_test
                SOURCES
                test/TileRawChannelCnv_p1_test.cxx
                LINK_LIBRARIES LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv )

atlas_add_test( TileRawChannelContainerCnv_p1_test
                SOURCES
                test/TileRawChannelContainerCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel IdDictParser TestTools TileConditionsLib TileIdentifier TileTPCnv
                LOG_IGNORE_PATTERN "initialize_from_dictionary" )

atlas_add_test( TileTTL1CellCnv_p1_test
                SOURCES
                test/TileTTL1CellCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools TileTPCnv )

atlas_add_test( TileTTL1Cnv_p1_test
                SOURCES
                test/TileTTL1Cnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools TileTPCnv )
