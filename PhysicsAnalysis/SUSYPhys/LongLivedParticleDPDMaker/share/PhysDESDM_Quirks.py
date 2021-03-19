# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

### configure trigger filters
if len(primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Quirks_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Quirks_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Quirks_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Quirks_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Quirks_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Quirks_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.Quirks_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "Quirks_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "Quirks_CombinedTriggerFilter"
topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
triggerFilterCounter = 0
cmdstring = ""
for triggerFilterName in triggerFilterNames :
    if triggerFilterCounter > 0 :
        cmdstring += " or "
    cmdstring += triggerFilterName
    triggerFilterCounter += 1
topSequence.Quirks_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["Quirks_CombinedTriggerFilter"]

# BEGIN QUIRKS-SPECIFIC CODE

from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
metFilterName = "Quirks_CombinedOfflineFilter"
topSequence += MissingEtFilter(metFilterName, cutMetMin=primRPVLLDESDM.Quirks_offlineFilterFlags.cutMetMin)
filtersToBookkeep.append(metFilterName)

# END QUIRKS-SPECIFIC CODE

########### combine the trigger and offline filters

QuirksCombinedFilter=LogicalFilterCombiner("QuirksCombinedFilter")
topSequence+=QuirksCombinedFilter

topSequence.QuirksCombinedFilter.cmdstring="Quirks_CombinedTriggerFilter and Quirks_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
topCmdString+="QuirksCombinedFilter"

filtersToBookkeep+=["QuirksCombinedFilter"]
