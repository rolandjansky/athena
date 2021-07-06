# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm
addAlgorithm("TrigT1CaloSim.TTL1OverlayConfigLegacy.getLArTTL1Overlay", "LArTTL1Overlay")
addAlgorithm("TrigT1CaloSim.TTL1OverlayConfigLegacy.getTileTTL1Overlay", "TileTTL1Overlay")
