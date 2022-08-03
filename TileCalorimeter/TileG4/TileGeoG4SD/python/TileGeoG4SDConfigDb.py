# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService
addTool("TileGeoG4SD.TileGeoG4SDConfigLegacy.getTileGeoG4SD"         , "TileGeoG4SD" )
addTool("TileGeoG4SD.TileGeoG4SDConfigLegacy.getTileCTBGeoG4SD"      , "TileCTBGeoG4SD" )
addService("TileGeoG4SD.TileGeoG4SDConfigLegacy.getTileGeoG4SDCalc"  , "TileGeoG4SDCalc")
addService("TileGeoG4SD.TileGeoG4SDConfigLegacy.getTileCTBGeoG4SDCalc"  , "TileCTBGeoG4SDCalc")
