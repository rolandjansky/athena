### configure trigger filters

if len(primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpCaloId_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpCaloId_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpCaloId_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "HadDelayed":
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpCaloId_triggerFilterFlags.HadDelayedTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = primRPVLLDESDM.SmpCaloId_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames += primRPVLLDESDM.SmpCaloId_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames += primRPVLLDESDM.SmpCaloId_triggerFilterFlags.MuonsTriggerNames
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames += primRPVLLDESDM.SmpCaloId_triggerFilterFlags.HadDelayedTriggerNames
    else:
        primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.SmpCaloId_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "SmpCaloId_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName)
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "SmpCaloId_CombinedTriggerFilter"
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
topSequence.SmpCaloId_CombinedTriggerFilter.cmdstring = cmdstring
filtersToBookkeep+=["SmpCaloId_CombinedTriggerFilter"]

offlineObjectFilterNames=[]
#=#=
#=#=## configure muon filters
#=#=
#=#=#from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
#=#=#
#=#=#muonCollNames=["CaloESDMuonCollection", "CaloMuonCollection", "MuGirlLowBetaCollection", "MuidESDMuonCollection", "MuidMuonCollection",
#=#=#               "StacoESDMuonCollection", "StacoMuonCollection"]
#=#=#
#=#=#for mucoll in muonCollNames:
#=#=#    muonFilterName = 'SmpCaloId_MuFilter_'+mucoll
#=#=#    muonFilter = D2PDMuonSelector(muonFilterName)
#=#=#    offlineObjectFilterNames.append( muonFilterName )
#=#=#    muonFilter.ptMin = primRPVLLDESDM.SmpCaloId_muonFilterFlags.cutEtMin
#=#=#    muonFilter.inputCollection = mucoll
#=#=#    muonFilter.minNumberPassed=1
#=#=#    topSequence+=muonFilter
#=#=
#=#=## configure a track particle filter
#=#=
#=#=#from EventUtils.EventUtilsConf import ParticleSelectionAlg
#=#=#trackParticleFilterName = 'SmpCaloId_TrackParticleFilter'
#=#=#topSequence += ParticleSelectionAlg(trackParticleFilterName,
#=#=#                                   InputContainer="InDetTrackParticles",
#=#=#                                    OutputContainer="SmpCaloId_SelectedTrackParticles",
#=#=#                                    Selection="pt>10000.")
#=#=
#=#=
#=#from EventUtils.EventUtilsConf import CutAlg
#=#cutString="count( "
#=#cutString+= primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutInputCollection
#=#cutString+=".pt > "
#=#cutString+= str(primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutPtMin)
#=#cutString+= " && "
#=#cutString+= primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutInputCollection
#=#cutString+=".numberOfInnermostPixelLayerHits >="
#=#cutString+=str(primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutBLayerHitsMin)
#=#cutString+=" ) >= 1 "
#=#print "NICK SmpCaloId cutString is ",cutString

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import SmpTrackFilterAlg

trackParticleFilterName = 'SmpCaloId_TrackParticleFilter'
topSequence+=SmpTrackFilterAlg(trackParticleFilterName,
                               ptMin                = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutPtMin,
                               numberBLayerHitsMin  = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutBLayerHitsMin,
                               numberPixelHitsMin   = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutPixelHitsMin,
                               numberSCTHitsMin     = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutSCTHitsMin, 
                               numberSiliconHitsMin = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutSiliconHitsMin,
                               inputCollection      = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutInputCollection,
                               minNumberPassed      = 1)

##topSequence += CutAlg(trackParticleFilterName,
##                      Cut = cutString)

offlineObjectFilterNames.append( trackParticleFilterName )

#=#=
#=#=from D2PDMaker.D2PDMakerConf import D2PDTrackParticleSelector
#=#=
#=#=topSequence += D2PDTrackParticleSelector( trackParticleFilterName,
#=#=                                          ptMin                = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutPtMin,
#=#=                                          numberBLayerHitsMin  = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutBLayerHitsMin,
#=#=                                          numberPixelHitsMin   = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutPixelHitsMin,
#=#=                                          numberSCTHitsMin     = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutSCTHitsMin, 
#=#=                                          numberSiliconHitsMin = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutSiliconHitsMin,
#=#=                                          inputCollection      = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutInputCollection,
#=#=                                          outputLinkCollection = primRPVLLDESDM.SmpCaloId_trackParticleFilterFlags.cutOutputLinkCollection,
#=#=                                          minNumberPassed      = 1 )
#=#=offlineObjectFilterNames.append( trackParticleFilterName )
#=#=
#=#=########### combine the offline filters
#=#=
combinedOfflineFilterName = "SmpCaloId_CombinedOfflineFilter"
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
print("NICK cmdstring of OfflineObjectFilter is ",cmdstring)
topSequence.SmpCaloId_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["SmpCaloId_CombinedOfflineFilter"]
#=#=#=#=

########### combine the trigger and offline filters

SmpCaloIdCombinedFilter=LogicalFilterCombiner("SmpCaloIdCombinedFilter")
topSequence+=SmpCaloIdCombinedFilter

topSequence.SmpCaloIdCombinedFilter.cmdstring="SmpCaloId_CombinedTriggerFilter and SmpCaloId_CombinedOfflineFilter"
##topSequence.SmpCaloIdCombinedFilter.cmdstring="SmpCaloId_CombinedTriggerFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="SmpCaloIdCombinedFilter"

filtersToBookkeep+=["SmpCaloIdCombinedFilter"]
