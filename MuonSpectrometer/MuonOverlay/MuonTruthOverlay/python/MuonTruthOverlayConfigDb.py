# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("MuonTruthOverlay.MuonTruthOverlayConfig.getMdtTruthOverlay", "MdtTruthOverlay")
addAlgorithm("MuonTruthOverlay.MuonTruthOverlayConfig.getRpcTruthOverlay", "RpcTruthOverlay")
addAlgorithm("MuonTruthOverlay.MuonTruthOverlayConfig.getTgcTruthOverlay", "TgcTruthOverlay")
