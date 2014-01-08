
### configure trigger filters
if len(primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stopped_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stopped_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stopped_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = primRPVLLDESDM.Stopped_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Stopped_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames += primRPVLLDESDM.Stopped_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.Stopped_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "Stopped_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "Stopped_CombinedTriggerFilter"
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
topSequence.Stopped_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["Stopped_CombinedTriggerFilter"]

##
#    Stopped_CombinedTriggerFilter && (jet && (muon or prescale) )
##

## configure a jet filter
flags = primRPVLLDESDM.Stopped_jetFilterFlags
jetFilterName = "Stopped_JetFilter"

from D2PDMaker.D2PDMakerConf import D2PDJetSelector
topSequence += D2PDJetSelector(jetFilterName)
topSequence.Stopped_JetFilter.etMin = flags.cutEtMin
topSequence.Stopped_JetFilter.etMax = flags.cutEtMax
topSequence.Stopped_JetFilter.absEtaMin = flags.cutEtaMin
topSequence.Stopped_JetFilter.absEtaMax = flags.cutEtaMax
topSequence.Stopped_JetFilter.jetPSFractionMax = flags.cutPSFracMax
topSequence.Stopped_JetFilter.inputCollection = flags.jetCollectionName
topSequence.Stopped_JetFilter.minNumberPassed=1


## configure a muon segment filter
from LongLivedParticleDPDMaker.MuonSegmentFilter import MuonSegmentFilter
segFilterName = "Stopped_MuonSegmentFilter"
segmentFilter = MuonSegmentFilter(segFilterName)
flags = primRPVLLDESDM.Stopped_muonSegmentFilterFlags

segmentFilter.minNumberSegments                 = flags.cutNsegMin
segmentFilter.maxNumberSegments                 = flags.cutNsegMax
segmentFilter.muonSegmentContainerName  = flags.muonSegmentCollectionName
topSequence += segmentFilter

if primRPVLLDESDM.Stopped_muonSegmentFilterFlags.addPrescaledLooseFilter:
    looseMuSegFilterName = "Stopped_looseMuSegFilter"
    looseMuSegFilter = MuonSegmentFilter(looseMuSegFilterName)
    looseMuSegFilter.maxNumberSegments=flags.cutNsegMaxLoose
    looseMuSegFilter.minNumberSegments=flags.cutNsegMinLoose
    looseMuSegFilter.muonSegmentContainerName=flags.muonSegmentCollectionName
    topSequence += looseMuSegFilter
    
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    prescaledMuSegFilterName = "Stopped_prescaledMuSegFilter"
    topSequence += PrimaryDPDPrescaler( prescaledMuSegFilterName,
                                        Prescale  = flags.prescaleFactorForLooseFilter,
                                        AcceptAlgs= [looseMuSegFilterName]
                                      )

########### combine the offline filters

combinedOfflineFilterName = "Stopped_CombinedOfflineFilter"
topSequence += LogicalFilterCombiner( combinedOfflineFilterName )

cmdstring = jetFilterName+" and "+segFilterName;
if primRPVLLDESDM.Stopped_muonSegmentFilterFlags.addPrescaledLooseFilter:
    cmdstring = jetFilterName+" and ("+segFilterName+" or "+prescaledMuSegFilterName+")"

print 'Josh\'s Stopped_CombinedOfflineFilter: %s' % cmdstring
topSequence.Stopped_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["Stopped_CombinedOfflineFilter"]

########### combine the trigger and offline filters

StoppedCombinedFilter = LogicalFilterCombiner("StoppedCombinedFilter")
topSequence+=StoppedCombinedFilter

topSequence.StoppedCombinedFilter.cmdstring="Stopped_CombinedTriggerFilter and Stopped_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="StoppedCombinedFilter"

filtersToBookkeep+=["StoppedCombinedFilter"]
