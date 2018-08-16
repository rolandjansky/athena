# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("InDetOverlay.InDetOverlayConfig.getPixelOverlay", "PixelOverlay")
addAlgorithm("InDetOverlay.InDetOverlayConfig.getSCTOverlay", "SCTOverlay")
addAlgorithm("InDetOverlay.InDetOverlayConfig.getTRTOverlay", "TRTOverlay")

addAlgorithm("InDetOverlay.InDetOverlayConfig.getInDetSDOOverlay", "InDetSDOOverlay")
