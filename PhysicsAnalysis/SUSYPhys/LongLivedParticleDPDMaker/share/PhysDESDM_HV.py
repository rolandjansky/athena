
### configure trigger filters

triggerFilterNames=[]

if len(primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.HV_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.HV_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.HV_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.HV_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.HV_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.HV_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames))

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "HV_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "HV_CombinedTriggerFilter"
topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
triggerFilterCounter = 0
cmdstring = ""
for triggerFilterName in triggerFilterNames :
    if triggerFilterCounter > 0 :
        cmdstring += " or "
        pass
    cmdstring += triggerFilterName
    triggerFilterCounter += 1
    pass
topSequence.HV_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["HV_CombinedTriggerFilter"]

offlineObjectFilterNames=[]

## configure a muon filter

#from PrimaryDPDMaker.MuonFilter import MuonFilter
#muonFilterName = 'HV_MuonFilter'
#topSequence += MuonFilter(muonFilterName)
#offlineObjectFilterNames.append( muonFilterName )
#topSequence.HV_MuonFilter.cutPtMinMu = primRPVLLDESDM.HV_muonFilterFlags.cutEtMin
#topSequence.HV_MuonFilter.cutContainerMu=primRPVLLDESDM.HV_muonFilterFlags.cutContainerMu
#topSequence.HV_MuonFilter.usingAOD=primRPVLLDESDM.HV_muonFilterFlags.usingAOD

########### combine the offline filters

#combinedOfflineFilterName = "HV_CombinedOfflineFilter"
#topSequence += LogicalFilterCombiner( combinedOfflineFilterName )

#offlineFilterCounter = 0
#cmdstring = ""
#for offlineFilterName in offlineObjectFilterNames :
#    if offlineFilterCounter > 0 :
#        cmdstring += " or "
#        pass
#    cmdstring += offlineFilterName
#    offlineFilterCounter += 1
#    pass
#topSequence.HV_CombinedOfflineFilter.cmdstring=cmdstring
#filtersToBookkeep+=["HV_CombinedOfflineFilter"]

########### combine the trigger and offline filters

HVCombinedFilter=LogicalFilterCombiner("HVCombinedFilter")
topSequence+=HVCombinedFilter

topSequence.HVCombinedFilter.cmdstring="HV_CombinedTriggerFilter" #and HV_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="HVCombinedFilter"

filtersToBookkeep+=["HVCombinedFilter"]
