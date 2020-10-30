# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileEvent )

# Component(s) in the package:
atlas_add_library( TileEvent
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileEvent
                   LINK_LIBRARIES CaloEvent AthContainers AthenaKernel Identifier GaudiKernel TileIdentifier TileSimEvent StoreGateLib TileConditionsLib EventContainers
                   PRIVATE_LINK_LIBRARIES CaloDetDescrLib CaloIdentifier TileCalibBlobObjs )

atlas_add_dictionary( TileEventDict
                      TileEvent/TileEventDict.h
                      TileEvent/selection.xml
                      LINK_LIBRARIES TileEvent )

# Tests in the package:
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
                LINK_LIBRARIES IdDictParser TileConditionsLib TileEvent TileIdentifier )

atlas_add_test( TileMutableRawChannelContainer_test
                SOURCES
                test/TileMutableRawChannelContainer_test.cxx
                LINK_LIBRARIES IdDictParser TileConditionsLib TileEvent TileIdentifier )

atlas_add_test( TileMutableDigitsContainer_test
                SOURCES
                test/TileMutableDigitsContainer_test.cxx
                LINK_LIBRARIES IdDictParser TileConditionsLib TileEvent TileIdentifier )

atlas_add_test( TileMutableHitContainer_test
                SOURCES
                test/TileMutableHitContainer_test.cxx
                LINK_LIBRARIES IdDictParser TileConditionsLib TileEvent TileIdentifier )

atlas_add_test( TileMutableBeamElemContainer_test
                SOURCES
                test/TileMutableBeamElemContainer_test.cxx
                LINK_LIBRARIES IdDictParser TileConditionsLib TileEvent TileIdentifier )
