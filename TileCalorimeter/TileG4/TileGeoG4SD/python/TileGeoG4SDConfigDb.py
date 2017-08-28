# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService
addTool("TileGeoG4SD.TileGeoG4SDConfig.getTileGeoG4SD"         , "TileGeoG4SD" )
addTool("TileGeoG4SD.TileGeoG4SDConfig.getTileCTBGeoG4SD"      , "TileCTBGeoG4SD" )
addService("TileGeoG4SD.TileGeoG4SDConfig.getTileGeoG4SDCalc"  , "TileGeoG4SDCalc")
addService("TileGeoG4SD.TileGeoG4SDConfig.getTileCTBGeoG4SDCalc"  , "TileCTBGeoG4SDCalc")
