# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("MuonWall.MuonWallConfig.getMuonWallSD"         , "MuonWallSD" )
addTool("MuonWall.MuonWallConfig.getMuonWallTool"       , "MuonWall" )
addTool("MuonWall.MuonWallConfig.getMuonWallTileTB"     , "MuonWallTileTB" )
