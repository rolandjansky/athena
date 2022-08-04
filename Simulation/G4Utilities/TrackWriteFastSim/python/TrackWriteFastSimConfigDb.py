# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("TrackWriteFastSim.TrackWriteFastSimConfigLegacy.getTrackFastSimSD",   "TrackFastSimSD")
addTool("TrackWriteFastSim.TrackWriteFastSimConfigLegacy.getNeutronFastSim",   "NeutronFastSim")
addTool("TrackWriteFastSim.TrackWriteFastSimConfigLegacy.getCosmicTRSD",       "CosmicRecord")
addTool("TrackWriteFastSim.TrackWriteFastSimConfigLegacy.getMuonEntryRecordSD","MuonEntryRecord")
