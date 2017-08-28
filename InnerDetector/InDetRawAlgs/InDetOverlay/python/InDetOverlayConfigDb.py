# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm
addAlgorithm("InDetOverlay.InDetOverlayConfig.getInDetOverlay", "InDetOverlay")
addAlgorithm("InDetOverlay.InDetOverlayConfig.getInDetSDOOverlay", "InDetSDOOverlay")
