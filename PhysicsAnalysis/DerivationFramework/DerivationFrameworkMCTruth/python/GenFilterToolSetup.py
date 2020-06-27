# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Common code for setting up the gen filter tools

# Set up the MCTruthClassifier
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc,'DFCommonTruthClassifier'):
    from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
    DFCommonTruthClassifier = MCTruthClassifier(name = "DFCommonTruthClassifier",
                                        ParticleCaloExtensionTool = "") 
    ToolSvc += DFCommonTruthClassifier

#Save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo)
#from AthenaCommon import CfgMgr
#DFCommonTruthGenFilter = CfgMgr.DerivationFramework__GenFilterTool(
#  "DFCommonTruthGenFilt",
#  )
#ToolSvc += DFCommonTruthGenFilter
