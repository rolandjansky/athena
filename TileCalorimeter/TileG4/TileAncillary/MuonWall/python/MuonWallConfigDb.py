# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("MuonWall.MuonWallConfigLegacy.getMuonWallSD"         , "MuonWallSD" )
addTool("MuonWall.MuonWallConfigLegacy.getMuonWallTool"       , "MuonWall" )
addTool("MuonWall.MuonWallConfigLegacy.getMuonWallTileTB"     , "MuonWallTileTB" )
