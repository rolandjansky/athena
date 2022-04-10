# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("xAODEventInfoCnv.xAODEventInfoCnvConfigLegacy.getEventInfoOverlay", "EventInfoOverlay")
addAlgorithm("xAODEventInfoCnv.xAODEventInfoCnvConfigLegacy.getEventInfoUpdateFromContextAlg", "EventInfoUpdateFromContextAlg")
