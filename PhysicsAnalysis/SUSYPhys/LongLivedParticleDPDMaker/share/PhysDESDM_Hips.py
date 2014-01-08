
### configure trigger filters
if len(primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Hips_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Hips_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

### configure hiptrt trigger filter 
if len(primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.EgammaTriggerNames
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames += primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.JetTauEtmissTriggerNames
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames += primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.MuonsTriggerNames
    else:
        primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames))

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR  ## hiptrt
primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames = list(set(primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames))

triggerFilterNames=[]
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.Hips_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "Hips_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

### hiptrt
triggerFilterNames_hiptrt=[]
for trigName_hiptrt in primRPVLLDESDM.Hips_triggerFilterFlags_hiptrt.TriggerNames:
    TriggerFilterName_hiptrt = "Hips_TriggerFilter_"+trigName_hiptrt
    topSequence += TriggerFilter( TriggerFilterName_hiptrt,
                                  trigger = trigName_hiptrt )
    triggerFilterNames_hiptrt.append( TriggerFilterName_hiptrt )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "Hips_CombinedTriggerFilter"
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
topSequence.Hips_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["Hips_CombinedTriggerFilter"]

# Create a combined filter by ORing together all the trigger filters -- hiptrt
combinedTriggerFilterName_hiptrt = "Hips_CombinedTriggerFilter_hiptrt"
topSequence += LogicalFilterCombiner( combinedTriggerFilterName_hiptrt )
triggerFilterCounter_hiptrt = 0
cmdstring = ""
for triggerFilterName_hiptrt in triggerFilterNames_hiptrt :
    if triggerFilterCounter_hiptrt > 0 :
        cmdstring += " or "
        pass
    cmdstring += triggerFilterName_hiptrt
    triggerFilterCounter_hiptrt += 1
    pass
topSequence.Hips_CombinedTriggerFilter_hiptrt.cmdstring = cmdstring    
filtersToBookkeep+=["Hips_CombinedTriggerFilter_hiptrt"]


offlineObjectFilterNames=[]

## configure an electron filter

## from PrimaryDPDMaker.ElectronFilter import ElectronFilter
## electronFilterName = 'Hips_ElectronFilter'
## topSequence += ElectronFilter(electronFilterName)
## offlineObjectFilterNames.append( electronFilterName )
## topSequence.Hips_ElectronFilter.cutPtMin = primRPVLLDESDM.Hips_electronFilterFlags.cutMinE
## topSequence.Hips_ElectronFilter.cutEtaMax = primRPVLLDESDM.Hips_electronFilterFlags.cutEtaMax
## topSequence.Hips_ElectronFilter.passAll = True

## configure a HIPs filter

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import HIPsFilterAlg
hipsFilterName = 'Hips_HIPsFilter'
topSequence += HIPsFilterAlg(hipsFilterName)
offlineObjectFilterNames.append( hipsFilterName )

topSequence.Hips_HIPsFilter.cutMinE  = primRPVLLDESDM.Hips_hipsFilterFlags.cutMinE
topSequence.Hips_HIPsFilter.cutEtaMax = primRPVLLDESDM.Hips_hipsFilterFlags.cutEtaMax
topSequence.Hips_HIPsFilter.cutMaxDeltaPhi = primRPVLLDESDM.Hips_hipsFilterFlags.cutMaxDeltaPhi
topSequence.Hips_HIPsFilter.cutMinTRTHTHits = primRPVLLDESDM.Hips_hipsFilterFlags.cutMinTRTHTHits
topSequence.Hips_HIPsFilter.cutMinTRTHTFraction = primRPVLLDESDM.Hips_hipsFilterFlags.cutMinTRTHTFraction
topSequence.Hips_HIPsFilter.caloCluster = primRPVLLDESDM.Hips_hipsFilterFlags.cutCaloCluster
topSequence.Hips_HIPsFilter.passAll = False

########### combine the offline filters

combinedOfflineFilterName = "Hips_CombinedOfflineFilter"
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
topSequence.Hips_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["Hips_CombinedOfflineFilter"]

########### combine the trigger and offline filters

HipsCombinedFilter=LogicalFilterCombiner("HipsCombinedFilter")
topSequence+=HipsCombinedFilter

topSequence.HipsCombinedFilter.cmdstring="(Hips_CombinedTriggerFilter and Hips_CombinedOfflineFilter) or Hips_CombinedTriggerFilter_hiptrt"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="HipsCombinedFilter"

