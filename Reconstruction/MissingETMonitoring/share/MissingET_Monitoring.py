from AthenaCommon.AlgSequence import AlgSequence
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
from MissingETMonitoring.MissingETMonitoringConf import *
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
monbadlb = GetLArBadLBFilterTool()



topSequence = AlgSequence()
monManETmiss = AthenaMonManager(
                   name                = "ETmissMonManager",
                   FileKey             = DQMonFlags.monManFileKey(),
                   Environment         = DQMonFlags.monManEnvironment(),
                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                   DataType            = DQMonFlags.monManDataType()
               )
topSequence += monManETmiss

# Monitoring of all events and XE trigger
if DQMonFlags.useTrigger():
    triggerList     = [""]   # All triggers
#     triggerList    += ["EF_xe30_FEB_L1EM5"]   # updated on March, 2012 (Heavy Ion runs)
#     triggerList    += ["EF_xe25_FEB_L1TE90"]   # updated on March, 2012
    triggerList    += ["EF_xe30"]   # updated on June, 2013
    triggerList    += ["EF_xe80"]   # updated on June, 2013
else:
    triggerList = [""]

# If trigger is on then run twice the same tools:
#   - for all events
#   - for triggered events
# Otherwise run for all events only
for trigger in triggerList:
    
    if trigger != "":
        triggerName = trigger   # this makes sure we have for each trigger a separate folder!
    else:
        triggerName = ""
    
    # Standard Monitoring
    metMonTool = METMonTool(name = "METMonTool_"+triggerName)
    metMonTool.NameSuffix   = triggerName
    metMonTool.TriggerChain = trigger
   
    #New AthenaMonitoring filter tool to be added to filter out events in non-filled BCIDs
    metMonTool.FilterTools.append(monFilledBunchFilterTool)
    metMonTool.FilterTools.append(monbadlb)

    if trigger != "":
        metMonTool.TrigDecisionTool = monTrigDecTool
        
    #Uncomment when switching to AOD    
    #metMonTool.eleColKey = "ElectronAODCollection"
    #Uncomment to avoid the jet cleaning    
    #metMonTool.doJetcleaning = False


    ToolSvc += metMonTool
    monManETmiss.AthenaMonTools += [ metMonTool ]

  
    # Bad Jets Monitoring
    metMonTool = METMonTool(name = "METMonTool_BadJets"+triggerName)
    metMonTool.NameSuffix   = "BadJets"
    if triggerName != "":
        metMonTool.NameSuffix += "_"+triggerName
    metMonTool.metKeys      = ["MET_RefFinal"]
    metMonTool.metCalKey    = ""
    metMonTool.eleColKey    = ""
    metMonTool.muoColKey    = ""
    metMonTool.jetColKey    = "AntiKt4LCTopoJets"
    metMonTool.TriggerChain = trigger
    metMonTool.FilterTools.append(monFilledBunchFilterTool)
    metMonTool.FilterTools.append(monbadlb)
    metMonTool.badJets = True
    if trigger != "":
        metMonTool.TrigDecisionTool = monTrigDecTool
    #Uncomment when switching to AOD    
    #metMonTool.eleColKey = "ElectronAODCollection"
    #Uncomment to avoid the jet cleaning    
    #metMonTool.doJetcleaning = False
    ToolSvc += metMonTool
    monManETmiss.AthenaMonTools += [ metMonTool ]

    
    # Tool for calorimetric MET monitoring
    metMonTool = METMonTool(name = "METMonTool_Calo_"+triggerName)
    metMonTool.NameSuffix   = "Calo"
    if triggerName != "":
        metMonTool.NameSuffix += "_"+triggerName
    metMonTool.metKeys      = ["MET_Base0","MET_Base","MET_Topo","MET_LocHadTopo"]
    metMonTool.metFinKey    = ""
    metMonTool.metCalKey    = ""
    metMonTool.jetColKey    = "AntiKt4LCTopoJets"
    metMonTool.eleColKey    = ""
    metMonTool.muoColKey    = ""
    metMonTool.TriggerChain = trigger
    metMonTool.FilterTools.append(monFilledBunchFilterTool)
    metMonTool.FilterTools.append(monbadlb)
    if trigger != "":
        metMonTool.TrigDecisionTool = monTrigDecTool
    ToolSvc += metMonTool
    monManETmiss.AthenaMonTools += [ metMonTool ]
    
    # Tool for refined MET monitoring
    metMonTool = METMonTool(name = "METMonTool_Refined_"+triggerName)
    metMonTool.NameSuffix   = "Refined"
    if triggerName != "":
        metMonTool.NameSuffix += "_"+triggerName
    metMonTool.metKeys      = ["MET_RefEle", "MET_RefGamma", "MET_RefTau", "MET_RefJet", "MET_Muon", "MET_RefFinal"]
    metMonTool.metFinKey    = ""
    metMonTool.metCalKey    = ""
    metMonTool.jetColKey    = "AntiKt4LCTopoJets"
    metMonTool.eleColKey    = ""
    metMonTool.muoColKey    = ""
    metMonTool.TriggerChain = trigger
    metMonTool.FilterTools.append(monFilledBunchFilterTool)
    metMonTool.FilterTools.append(monbadlb)
    if trigger != "":
        metMonTool.TrigDecisionTool = monTrigDecTool
    ToolSvc += metMonTool
    monManETmiss.AthenaMonTools += [ metMonTool ]

# Monitoring for Random trigger
# If trigger is off, run anyway without trigger (useful for Random stream)
if DQMonFlags.useTrigger():
    trigger = "L1_RD0_EMPTY"
else:
    trigger = ""

# Tool for calorimeter term monitoring (TopoClusters)
metMonTool = METMonTool(name = "METMonTool_Topo")
metMonTool.NameSuffix        = "Topo"
metMonTool.metKeys           = []
metMonTool.metFinKey         = "MET_Topo"
metMonTool.metCalKey         = "MET_Topo"
metMonTool.jetColKey         = "AntiKt4LCTopoJets"
metMonTool.eleColKey         = ""
metMonTool.muoColKey         = ""
metMonTool.nEtBins           = 100
metMonTool.EtRange           = 10.0
metMonTool.SumEtRangeFactor  = 2.0
metMonTool.FillNegativeSumEt = True
metMonTool.TriggerChain      = trigger
if trigger != "":
    metMonTool.TrigDecisionTool = monTrigDecTool
ToolSvc += metMonTool
monManETmiss.AthenaMonTools += [ metMonTool ]

# Tool for calorimeter term monitoring (2 sigma noise suppression)
metMonTool = METMonTool(name = "METMonTool_Base")
metMonTool.NameSuffix        = "Base"
metMonTool.metKeys           = []
metMonTool.metFinKey         = "MET_Base"
metMonTool.metCalKey         = "MET_Base"
metMonTool.jetColKey         = "AntiKt4LCTopoJets"
metMonTool.eleColKey         = ""
metMonTool.muoColKey         = ""
metMonTool.nEtBins           = 100
metMonTool.EtRange           = 50.0
metMonTool.SumEtRangeFactor  = 2.0
metMonTool.FillNegativeSumEt = True
metMonTool.TriggerChain      = trigger
if trigger != "":
    metMonTool.TrigDecisionTool = monTrigDecTool
ToolSvc += metMonTool
monManETmiss.AthenaMonTools += [ metMonTool ]
