# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CfgGetter import addAlgorithm,addTool,addService

# RoI based (Currently only on flavour of TaP algorithm)
addAlgorithm("TrigMuonEFTagandProbe.TrigMuonEFTagandProbeConfig.TrigMuonEFTagandProbe","TrigMuonEFTagandProbe")

# tools for Calotag  
                                                                  
#addTool("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_MuonCaloTagTool", "TrigMuSuperEF_MuonCaloTagTool")
#addTool("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_TrackDepositInCaloTool", "TrigMuSuperEF_TrackDepositInCaloTool")
#addTool("TrigMuSuperEF.TrigMuSuperEFConfig.TrigMuSuperEF_CaloTrkSelectorTool", "TrigMuSuperEF_CaloTrkSelectorTool")
