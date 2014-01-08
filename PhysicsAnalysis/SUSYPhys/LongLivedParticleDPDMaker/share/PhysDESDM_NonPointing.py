
### configure trigger filters
if len(primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = primRPVLLDESDM.NonPointing_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = primRPVLLDESDM.NonPointing_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = primRPVLLDESDM.NonPointing_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = primRPVLLDESDM.NonPointing_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames += primRPVLLDESDM.NonPointing_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames += primRPVLLDESDM.NonPointing_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.NonPointing_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "NonPointing_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "NonPointing_CombinedTriggerFilter"
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
topSequence.NonPointing_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["NonPointing_CombinedTriggerFilter"]

offlineObjectFilterNames=[]

## configure a photon filter

from PrimaryDPDMaker.PhotonFilter import PhotonFilter
photonFilterName = 'NonPointing_PhotonFilter'
topSequence += PhotonFilter(photonFilterName)
offlineObjectFilterNames.append( photonFilterName )
topSequence.NonPointing_PhotonFilter.cutEtMin = primRPVLLDESDM.NonPointing_photonFilterFlags.cutEtMin
topSequence.NonPointing_PhotonFilter.cutEtaMax = primRPVLLDESDM.NonPointing_photonFilterFlags.cutEtaMax
topSequence.NonPointing_PhotonFilter.cutIsEM=primRPVLLDESDM.NonPointing_photonFilterFlags.cutIsEM
topSequence.NonPointing_PhotonFilter.photonCollectionName=primRPVLLDESDM.NonPointing_photonFilterFlags.photonCollectionName

## configure a diphoton filter
from PrimaryDPDMaker.DiPhotonFilter import DiPhotonFilter
diphotonFilterName = 'NonPointing_DiPhotonFilter'
topSequence += DiPhotonFilter(diphotonFilterName)
offlineObjectFilterNames.append( diphotonFilterName )
topSequence.NonPointing_DiPhotonFilter.cutEtMin1 = primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutEtMin1
topSequence.NonPointing_DiPhotonFilter.cutEtMin2 = primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutEtMin2
topSequence.NonPointing_DiPhotonFilter.cutIsEM1=primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutIsEM1
topSequence.NonPointing_DiPhotonFilter.cutIsEM2=primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutIsEM2
topSequence.NonPointing_DiPhotonFilter.cutAuthor1=primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutAuthor1
topSequence.NonPointing_DiPhotonFilter.cutAuthor2=primRPVLLDESDM.NonPointing_diphotonFilterFlags.cutAuthor2

## configure a EMCluster filter

from PrimaryDPDMaker.DiEMClusterFilter import DiEMClusterFilter
diemclusterFilterName = 'NonPointing_DiEMClusterFilter'
topSequence += DiEMClusterFilter(diemclusterFilterName)
offlineObjectFilterNames.append( diemclusterFilterName )

topSequence.NonPointing_DiEMClusterFilter.cutEtMinEMCl1 = primRPVLLDESDM.NonPointing_diemclusterFilterFlags.cutEtMinEMCl1
topSequence.NonPointing_DiEMClusterFilter.cutEtMinEMCl2 = primRPVLLDESDM.NonPointing_diemclusterFilterFlags.cutEtMinEMCl2 
topSequence.NonPointing_DiEMClusterFilter.cutEtaMax   = primRPVLLDESDM.NonPointing_diemclusterFilterFlags.cutEtaMax
topSequence.NonPointing_DiEMClusterFilter.removeEtaCrack= primRPVLLDESDM.NonPointing_diemclusterFilterFlags.removeEtaCrack
topSequence.NonPointing_DiEMClusterFilter.crackEtaMin = primRPVLLDESDM.NonPointing_diemclusterFilterFlags.crackEtaMin
topSequence.NonPointing_DiEMClusterFilter.crackEtaMax = primRPVLLDESDM.NonPointing_diemclusterFilterFlags.crackEtaMax
## configure a MissingEt filter

from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
missingetFilterName = 'NonPointing_MissingEtFilter'
topSequence += MissingEtFilter(missingetFilterName)
offlineObjectFilterNames.append( missingetFilterName )

topSequence.NonPointing_MissingEtFilter.cutMetMin = primRPVLLDESDM.NonPointing_missingetFilterFlags.cutMetMin
topSequence.NonPointing_MissingEtFilter.jetContainerTypeList = primRPVLLDESDM.NonPointing_missingetFilterFlags.jetContainerTypeList
topSequence.NonPointing_MissingEtFilter.jetContainerNameList = primRPVLLDESDM.NonPointing_missingetFilterFlags.jetContainerNameList
topSequence.NonPointing_MissingEtFilter.useUncalibratedJets = primRPVLLDESDM.NonPointing_missingetFilterFlags.useUncalibratedJets
topSequence.NonPointing_MissingEtFilter.useLeadingJet = primRPVLLDESDM.NonPointing_missingetFilterFlags.useLeadingJet
topSequence.NonPointing_MissingEtFilter.minDeltaPhi = primRPVLLDESDM.NonPointing_missingetFilterFlags.minDeltaPhi
topSequence.NonPointing_MissingEtFilter.requireMet = primRPVLLDESDM.NonPointing_missingetFilterFlags.requireMet

## configure a Jet filter

from PrimaryDPDMaker.JetFilter import JetFilter
jetFilterName = 'NonPointing_JetFilter'
topSequence += JetFilter(jetFilterName)
offlineObjectFilterNames.append( jetFilterName )
topSequence.NonPointing_JetFilter.jetCollectionName = primRPVLLDESDM.NonPointing_jetFilterFlags.jetCollectionName
topSequence.NonPointing_JetFilter.cutEtMin = primRPVLLDESDM.NonPointing_jetFilterFlags.cutEtMin
topSequence.NonPointing_JetFilter.cutEtMax = primRPVLLDESDM.NonPointing_jetFilterFlags.cutEtMax
topSequence.NonPointing_JetFilter.cutEtaMax = primRPVLLDESDM.NonPointing_jetFilterFlags.cutEtaMax
topSequence.NonPointing_JetFilter.removeOverlap = primRPVLLDESDM.NonPointing_jetFilterFlags.removeOverlap
topSequence.NonPointing_JetFilter.deltaR = primRPVLLDESDM.NonPointing_jetFilterFlags.deltaR
topSequence.NonPointing_JetFilter.leptonContainerTypeList = primRPVLLDESDM.NonPointing_jetFilterFlags.leptonContainerTypeList
topSequence.NonPointing_JetFilter.leptonContainerNameList = primRPVLLDESDM.NonPointing_jetFilterFlags.leptonContainerNameList
topSequence.NonPointing_JetFilter.minNumberPassed = primRPVLLDESDM.NonPointing_jetFilterFlags.minNumberPassed

########### combine the offline filters

combinedOfflineFilterName = "NonPointing_CombinedOfflineFilter"
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
topSequence.NonPointing_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["NonPointing_CombinedOfflineFilter"]

########### combine the trigger and offline filters

NonPointingCombinedFilter=LogicalFilterCombiner("NonPointingCombinedFilter")
topSequence+=NonPointingCombinedFilter

topSequence.NonPointingCombinedFilter.cmdstring="NonPointing_CombinedTriggerFilter and NonPointing_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="NonPointingCombinedFilter"
