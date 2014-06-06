# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm,addTool

# add tools and algorithms to the ConfiguredFactory, see:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory

addTool("xAODTrigMuonCnv.xAODTrigMuonCnvConfig.TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks","TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks")

addAlgorithm("xAODTrigMuonCnv.xAODTrigMuonCnvConfig.TrigMuonEFInfoToMuonCnvAlgTMEF","TrigMuonEFInfoToMuonCnvAlgTMEF")
addAlgorithm("xAODTrigMuonCnv.xAODTrigMuonCnvConfig.TrigMuonEFInfoToMuonCnvAlgTMG","TrigMuonEFInfoToMuonCnvAlgTMG")

addAlgorithm("xAODTrigMuonCnv.xAODTrigMuonCnvConfig.TrigMuonEFInfoCnvTestAlgTMEF","TrigMuonEFInfoCnvTestAlgTMEF")
