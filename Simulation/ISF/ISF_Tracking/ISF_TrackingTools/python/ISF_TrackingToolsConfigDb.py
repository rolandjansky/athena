# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm
## Lorentz Vector Generators
addTool("ISF_TrackingTools.ISF_TrackingToolsConfig.getCaloEntryTrkExtrapolator",  "ISF_CaloEntryTrkExtrapolator")

