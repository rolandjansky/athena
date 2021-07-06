# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

### configure trigger filters
if len(primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpMs_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpMs_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpMs_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpMs_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames += primRPVLLDESDM.SmpMs_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames += primRPVLLDESDM.SmpMs_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.SmpMs_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "SmpMs_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "SmpMs_CombinedTriggerFilter"
topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
triggerFilterCounter = 0
cmdstring = ""
for triggerFilterName in triggerFilterNames :
    if triggerFilterCounter > 0 :
        cmdstring += " or "
    cmdstring += triggerFilterName
    triggerFilterCounter += 1
topSequence.SmpMs_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["SmpMs_CombinedTriggerFilter"]

offlineObjectFilterNames=[]

## configure muon filters

from EventUtils.EventUtilsConf import CutAlg


muonFilterName = 'SmpMs_MuonFilter'
cutString="count( Muons.pt > "
cutString+=str(primRPVLLDESDM.DV_muonFilterFlags.cutEtMin)
cutString+=" ) > 1"


muonFilter = CutAlg(muonFilterName,
                    Cut=cutString)
offlineObjectFilterNames.append( muonFilterName )
topSequence+=muonFilter


########### combine the offline filters

combinedOfflineFilterName = "SmpMs_CombinedOfflineFilter"
topSequence += LogicalFilterCombiner( combinedOfflineFilterName )

offlineFilterCounter = 0
cmdstring = ""
for offlineFilterName in offlineObjectFilterNames :
    if offlineFilterCounter > 0 :
        cmdstring += " or "
    cmdstring += offlineFilterName
    offlineFilterCounter += 1
topSequence.SmpMs_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["SmpMs_CombinedOfflineFilter"]

########### combine the trigger and offline filters

SmpMsCombinedFilter=LogicalFilterCombiner("SmpMsCombinedFilter")
topSequence+=SmpMsCombinedFilter

topSequence.SmpMsCombinedFilter.cmdstring="SmpMs_CombinedTriggerFilter and SmpMs_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
topCmdString+="SmpMsCombinedFilter"

filtersToBookkeep+=["SmpMsCombinedFilter"]
