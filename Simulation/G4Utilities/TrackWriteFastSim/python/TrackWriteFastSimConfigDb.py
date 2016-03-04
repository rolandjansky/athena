# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("TrackWriteFastSim.TrackWriteFastSimConfig.getTrackFastSimSD",   "TrackFastSimSD")
addTool("TrackWriteFastSim.TrackWriteFastSimConfig.getNeutronFastSim",   "NeutronFastSim")
addTool("TrackWriteFastSim.TrackWriteFastSimConfig.getCosmicTRSD",       "CosmicRecord")
addTool("TrackWriteFastSim.TrackWriteFastSimConfig.getMuonEntryRecordSD","MuonEntryRecord")
