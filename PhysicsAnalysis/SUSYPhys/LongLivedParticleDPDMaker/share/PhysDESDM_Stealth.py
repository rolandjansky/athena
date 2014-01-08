### configure trigger filters
if len(primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stealth_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stealth_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stealth_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stealth_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Stealth_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Stealth_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.Stealth_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "Stealth_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass


# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "Stealth_CombinedTriggerFilter"
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
topSequence.Stealth_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["Stealth_CombinedTriggerFilter"]

offlineObjectFilterNames=[]

## configure a track filter

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import StealthFilterAlg
stealthFilterName = 'Stealth_StealthFilter'
topSequence += StealthFilterAlg(stealthFilterName)
offlineObjectFilterNames.append( stealthFilterName )

topSequence.Stealth_StealthFilter.cutMinTrackSd0 = primRPVLLDESDM.Stealth_stealthFilterFlags.cutMinTrackSd0
topSequence.Stealth_StealthFilter.cutMinNTracks = primRPVLLDESDM.Stealth_stealthFilterFlags.cutMinNTracks
topSequence.Stealth_StealthFilter.cutMinTrackpT = primRPVLLDESDM.Stealth_stealthFilterFlags.cutMinTrackpT
topSequence.Stealth_StealthFilter.cutMinTrackEta = primRPVLLDESDM.Stealth_stealthFilterFlags.cutMinTrackEta
topSequence.Stealth_StealthFilter.passAll = False

########### combine the offline filters

combinedOfflineFilterName = "Stealth_CombinedOfflineFilter"
topSequence += LogicalFilterCombiner( combinedOfflineFilterName )

offlineFilterCounter = 0
cmdstring = ""
for offlineFilterName in offlineObjectFilterNames :
    if offlineFilterCounter > 0 :
        cmdstring += " or "
        pass
    cmdstring += offlineFilterName
    offlineFilterCounter += 1
    pass
topSequence.Stealth_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["Stealth_CombinedOfflineFilter"]

########### combine the trigger and offline filters

StealthCombinedFilter=LogicalFilterCombiner("StealthCombinedFilter")
topSequence+=StealthCombinedFilter

topSequence.StealthCombinedFilter.cmdstring="(Stealth_CombinedTriggerFilter and Stealth_CombinedOfflineFilter)"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="StealthCombinedFilter"
