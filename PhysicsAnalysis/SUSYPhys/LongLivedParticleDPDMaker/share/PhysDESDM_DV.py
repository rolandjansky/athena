
### configure trigger filters
if len(primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma" or primRPVLLDESDM.doDV.ForceToRunEgammaSel :
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DV_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss" or primRPVLLDESDM.doDV.ForceToRunJetTauEtmissSel :
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DV_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons" or primRPVLLDESDM.doDV.ForceToRunMuonsSel :
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DV_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DV_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.DV_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.DV_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.DV_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "DV_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "DV_CombinedTriggerFilter"
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
topSequence.DV_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["DV_CombinedTriggerFilter"]

#################################################################################################
####### Muons stream selection
#################################################################################################

if rec.triggerStream() == "Muons" or primRPVLLDESDM.doDV.ForceToRunMuonsSel:
    
    offlineMuonFilterNames=[]
    
    ## configure muon filters
    
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    
    muonCollNames=["MuidESDMuonCollection", "MuidMuonCollection", "StacoESDMuonCollection", "StacoMuonCollection"]
    
    for mucoll in muonCollNames:
        muonFilterName = 'DV_MuFilter_'+mucoll
        muonFilter = D2PDMuonSelector(muonFilterName)
        offlineMuonFilterNames.append( muonFilterName )
        muonFilter.ptMin = primRPVLLDESDM.DV_muonFilterFlags.cutEtMin
        muonFilter.absEtaMax = primRPVLLDESDM.DV_muonFilterFlags.cutEtaMax
        muonFilter.inputCollection = mucoll
        muonFilter.minNumberPassed=1
        topSequence+=muonFilter
        pass
    
##  a real jet filter
    
    from PrimaryDPDMaker.JetFilter import JetFilter
    jetFilterName = "DV_JetFilter_muonChannel"
    topSequence += JetFilter(jetFilterName)
    topSequence.DV_JetFilter.cutEtMin=primRPVLLDESDM.DV_jetFilterFlagsMuon.jetPtCut
    topSequence.DV_JetFilter.cutEtMax=14.0*Units.TeV
    topSequence.DV_JetFilter.minNumberPassed=primRPVLLDESDM.DV_jetFilterFlagsMuon.nJetPassed
    filtersToBookkeep+=["DV_JetFilter_muonChannel"]
    
    ########## d0 filter
    from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlg
    d0FilterName="DV_mud0Filter"
    d0Filter=DVFilterAlg(d0FilterName)
    topSequence+=d0Filter
    topSequence.DV_mud0Filter.MuonD0Cut=primRPVLLDESDM.DV_d0FilterFlags.muD0Cut
    topSequence.DV_mud0Filter.MuonPtCut=primRPVLLDESDM.DV_d0FilterFlags.muPtCut
    topSequence.DV_mud0Filter.MuonEtaCut=primRPVLLDESDM.DV_d0FilterFlags.muEtaCut
    filtersToBookkeep+=["DV_mud0Filter"]
    
    jetOrD0FilterName = "DV_JetOrD0Filter_muonChannel"
    topSequence += LogicalFilterCombiner( jetOrD0FilterName)
    topSequence.DV_JetOrD0Filter.cmdstring = "DV_JetFilter_muonChannel or DV_mud0Filter"
    filtersToBookkeep+=["DV_JetOrD0Filter_muonChannel"]
    
    ########### combine the offline filters
    
    muonFilterName = "DV_MuonFilter"
    topSequence += LogicalFilterCombiner( muonFilterName )
    muonFilterCounter = 0
    cmdstring = ""
    for muFilterName in offlineMuonFilterNames :
        if muonFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += muFilterName
        muonFilterCounter += 1
        pass
    topSequence.DV_MuonFilter.cmdstring=cmdstring ##+ " and DV_JetFilter2"
    filtersToBookkeep+=["DV_MuonFilter"]
    
    
    combinedOfflineFilterName = "DV_CombinedOfflineFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
    cmdstring = "DV_MuonFilter and DV_JetOrD0Filter_muonChannel"
    topSequence.DV_CombinedOfflineFilter.cmdstring=cmdstring 
    filtersToBookkeep+=["DV_CombinedOfflineFilter"]
    pass

#################################################################################################
############### Egamma stream selection
#################################################################################################

if rec.triggerStream() == "Egamma" or primRPVLLDESDM.doDV.ForceToRunEgammaSel:
    offlineEgammaFilterNames=[]
    
    from D2PDMaker.D2PDMakerConf import D2PDElectronSelector	
    electronCollNames=["ElectronCollection"]
    
    from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlgEgamma
    d0FilterEgammaName="DV_egammad0Filter"
    d0FilterEgamma=DVFilterAlgEgamma(d0FilterEgammaName)
    offlineEgammaFilterNames.append( d0FilterEgammaName )
    topSequence+=d0FilterEgamma
    topSequence.DV_egammad0Filter.electronD0Cut=primRPVLLDESDM.DV_d0FilterFlagsEgamma.electronD0Cut
    topSequence.DV_egammad0Filter.electronPtCut=primRPVLLDESDM.DV_d0FilterFlagsEgamma.electronPtCut
    topSequence.DV_egammad0Filter.electronEtaCut=primRPVLLDESDM.DV_d0FilterFlagsEgamma.electronEtaCut
    filtersToBookkeep+=["DV_egammad0Filter"]
## Di-electron filter
    from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlgDiElectron
    d0FilterDiElectronName="DV_diElectrond0Filter"
    d0FilterEgamma=DVFilterAlgDiElectron(d0FilterDiElectronName)
    offlineEgammaFilterNames.append( d0FilterDiElectronName )
    topSequence+=d0FilterEgamma
    topSequence.DV_diElectrond0Filter.dielectronD0Cut=primRPVLLDESDM.DV_d0FilterFlagsDiElectron.dielectronD0Cut
    topSequence.DV_diElectrond0Filter.dielectronPtCut=primRPVLLDESDM.DV_d0FilterFlagsDiElectron.dielectronPtCut
    topSequence.DV_diElectrond0Filter.dielectronEtaCut=primRPVLLDESDM.DV_d0FilterFlagsDiElectron.dielectronEtaCut
    filtersToBookkeep+=["DV_diElectrond0Filter"]
    
    
    from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
    photonCollNames=["PhotonCollection"]
    for phcoll in photonCollNames:
        photonFilterName = 'DV_PhFilter'
        photonFilter = D2PDPhotonSelector(photonFilterName)        
        photonFilter.ptMin = primRPVLLDESDM.DV_photonFilterFlags.photonPtCut
        photonFilter.inputCollection = phcoll
        photonFilter.minNumberPassed=primRPVLLDESDM.DV_photonFilterFlags.minPhotonPassed
        topSequence+=photonFilter
        photonFilter2Name = 'DV_PhFilter2'
        photonFilter2 = D2PDPhotonSelector(photonFilter2Name)
        photonFilter2.ptMin = primRPVLLDESDM.DV_photonFilterFlags2.photonPtCut        
        photonFilter2.inputCollection = phcoll
        photonFilter2.minNumberPassed=primRPVLLDESDM.DV_photonFilterFlags2.minPhotonPassed
        topSequence+=photonFilter2
        pass

    from LongLivedParticleDPDMaker.FancyJetFilter import FancyJetFilter
    jetFilterName = "DV_JetFilter"
    topSequence += FancyJetFilter(jetFilterName)
    topSequence.DV_JetFilter.jetCollectionName=primRPVLLDESDM.DV_jetFilterFlagsEgamma.jetCollectionName
    topSequence.DV_JetFilter.cutEtMin=primRPVLLDESDM.DV_jetFilterFlagsEgamma.jetPtCut
    topSequence.DV_JetFilter.cutEtMax=14.0*Units.TeV
    topSequence.DV_JetFilter.minNumberPassed=primRPVLLDESDM.DV_jetFilterFlagsEgamma.nJetPassed
    topSequence.DV_JetFilter.cutSumPtTrkMax=primRPVLLDESDM.DV_jetFilterFlagsEgamma.MaxSumPtTrk
    filtersToBookkeep+=["DV_JetFilter"]
        
    jetFilterName2 = "DV_JetFilter2"
    topSequence += FancyJetFilter(jetFilterName2)
    topSequence.DV_JetFilter2.jetCollectionName=primRPVLLDESDM.DV_jetFilterFlagsEgamma.jetCollectionName
    topSequence.DV_JetFilter2.cutEtMin=primRPVLLDESDM.DV_jetFilterFlagsEgamma.jetPtCut
    topSequence.DV_JetFilter2.cutEtMax=14.0*Units.TeV
    topSequence.DV_JetFilter2.minNumberPassed=primRPVLLDESDM.DV_jetFilterFlagsEgamma.nJetPassed
    topSequence.DV_JetFilter2.cutSumPtTrkMax=primRPVLLDESDM.DV_jetFilterFlagsEgamma.MaxSumPtTrk
    filtersToBookkeep+=["DV_JetFilter2"]

    jetAndPhFilterName = "DV_JetAndPhFilter"
    topSequence += LogicalFilterCombiner( jetAndPhFilterName)
    topSequence.DV_JetAndPhFilter.cmdstring = "DV_JetFilter and DV_PhFilter"
    offlineEgammaFilterNames.append( jetAndPhFilterName )
    filtersToBookkeep+=["DV_JetAndPhFilter"]
    jetAndPhFilter2Name = "DV_JetAndPhFilter2"
    topSequence += LogicalFilterCombiner( jetAndPhFilter2Name)
    topSequence.DV_JetAndPhFilter2.cmdstring = "DV_JetFilter2 and DV_PhFilter2"
    offlineEgammaFilterNames.append( jetAndPhFilter2Name )
    filtersToBookkeep+=["DV_JetAndPhFilter2"]

    ########### combine the offline filters

    egammaFilterName = "DV_EgammaFilter"
    topSequence += LogicalFilterCombiner( egammaFilterName )
    egammaFilterCounter = 0
    cmdstring = ""
    for elFilterName in offlineEgammaFilterNames :
        if egammaFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += elFilterName
        egammaFilterCounter += 1
        pass
    topSequence.DV_EgammaFilter.cmdstring=cmdstring
    filtersToBookkeep+=["DV_EgammaFilter"]
    
    
    combinedOfflineFilterName = "DV_CombinedOfflineFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
    cmdstring = "DV_EgammaFilter"
    topSequence.DV_CombinedOfflineFilter.cmdstring=cmdstring 
    filtersToBookkeep+=["DV_CombinedOfflineFilter"]
    pass


#################################################################################################
### JetTauEtmiss stream selection
##################################################################################################



if rec.triggerStream() == "JetTauEtmiss" or primRPVLLDESDM.doDV.ForceToRunJetTauEtmissSel:
    offlineJetFilterNames=[]

## logic - 3 possible selections:
## 1) ISR type (high-pt jet back-to-back with MET)
## 2) multi-jet (either 4,5,6 jets, pT cuts reflecting trigger cuts) AND trackless jet
## 3) MET AND trackless jet

    
########   ISR selection first #############################
## JET+MET filter for ISR
    from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
    ISR_METFilterName = 'DV_ISR_METFilter'
    topSequence += MissingEtFilter(ISR_METFilterName)
    topSequence.DV_ISR_METFilter.useLeadingJet=True
    topSequence.DV_ISR_METFilter.cutMetMin = primRPVLLDESDM.DV_ISRFilterFlags.cutMetMin
    topSequence.DV_ISR_METFilter.minDeltaPhi=primRPVLLDESDM.DV_ISRFilterFlags.deltaPhiCut
    topSequence.DV_ISR_METFilter.requireMet = True
    filtersToBookkeep+=["DV_ISR_METFilter"]

    from PrimaryDPDMaker.JetFilter import JetFilter
    jetFilterName = "DV_ISR_JetFilter"
    topSequence += JetFilter(jetFilterName)
    topSequence.DV_ISR_JetFilter.jetCollectionName=primRPVLLDESDM.DV_ISRFilterFlags.jetCollectionName
    topSequence.DV_ISR_JetFilter.cutEtMin=primRPVLLDESDM.DV_ISRFilterFlags.jetPtCut
    topSequence.DV_ISR_JetFilter.cutEtMax=14.0*Units.TeV
    topSequence.DV_ISR_JetFilter.minNumberPassed=1
    filtersToBookkeep+=["DV_ISR_JetFilter"]
## combine the jet and the MET
    combinedISRFilterName = "DV_ISRFilter"
    topSequence += LogicalFilterCombiner( combinedISRFilterName )
    cmdstring = "DV_ISR_METFilter and DV_ISR_JetFilter"
    topSequence.DV_ISRFilter.cmdstring=cmdstring
    filtersToBookkeep+=["DV_ISRFilter"]

############################################################
##  a not-very-track-y jet filter
    from LongLivedParticleDPDMaker.FancyJetFilter import FancyJetFilter
    jetFilterName = "DV_TracklessJetFilter"
    topSequence += FancyJetFilter(jetFilterName)
    topSequence.DV_TracklessJetFilter.cutEtMin=primRPVLLDESDM.DV_fancyJetFilterFlags.jetPtCut
    topSequence.DV_TracklessJetFilter.cutEtMax=14.0*Units.TeV
    topSequence.DV_TracklessJetFilter.minNumberPassed=primRPVLLDESDM.DV_fancyJetFilterFlags.nJetPassed
    topSequence.DV_TracklessJetFilter.cutSumPtTrkMax=primRPVLLDESDM.DV_fancyJetFilterFlags.MaxSumPtTrk
    topSequence.DV_TracklessJetFilter.jetCollectionName = "AntiKt4TopoEMJets"
    filtersToBookkeep+=["DV_TracklessJetFilter"]

    jetFilterName = "DV_LooseTracklessJetFilter"
    topSequence += FancyJetFilter(jetFilterName)
    topSequence.DV_LooseTracklessJetFilter.cutEtMin=primRPVLLDESDM.DV_looseFancyJetFilterFlags.jetPtCut
    topSequence.DV_LooseTracklessJetFilter.cutEtMax=14.0*Units.TeV
    topSequence.DV_LooseTracklessJetFilter.minNumberPassed=primRPVLLDESDM.DV_looseFancyJetFilterFlags.nJetPassed
    topSequence.DV_LooseTracklessJetFilter.cutSumPtTrkMax=primRPVLLDESDM.DV_looseFancyJetFilterFlags.MaxSumPtTrk
    topSequence.DV_LooseTracklessJetFilter.jetCollectionName = "AntiKt4TopoEMJets"
    filtersToBookkeep+=["DV_LooseTracklessJetFilter"]    


## METfilter

    missingetFilterName = 'DV_MissingEtFilter'
    topSequence += MissingEtFilter(missingetFilterName)
    topSequence.DV_MissingEtFilter.cutMetMin = primRPVLLDESDM.DV_METFilterFlags.cutMetMin
    topSequence.DV_MissingEtFilter.requireMet = True
    topSequence.DV_MissingEtFilter.useLeadingJet=True
    topSequence.DV_MissingEtFilter.minDeltaPhi=primRPVLLDESDM.DV_METFilterFlags.deltaPhiCut
    filtersToBookkeep+=["DV_MissingEtFilter"]

## put them together.. 
    combinedMETFilterName = "DV_CombinedMETFilter"
    topSequence += LogicalFilterCombiner( combinedMETFilterName )
    cmdstring = "DV_TracklessJetFilter and DV_MissingEtFilter"
    topSequence.DV_CombinedMETFilter.cmdstring=cmdstring
    filtersToBookkeep+=["DV_CombinedMETFilter"]


## Multijet filter
    cmdstring=""
    for i in range(3):
        multiJetFilterName = "DV_multiJetFilter_"+str(i+4)+"trk"
        multiJetFilter = JetFilter(multiJetFilterName)
        multiJetFilter.jetCollectionName=primRPVLLDESDM.DV_multiJetFilterFlags.jetCollectionName
        multiJetFilter.minNumberPassed=primRPVLLDESDM.DV_multiJetFilterFlags.nJetPassed[i]
        multiJetFilter.cutEtMin=primRPVLLDESDM.DV_multiJetFilterFlags.jetPtCut[i]        
        multiJetFilter.cutEtMax=14.0*Units.TeV
        topSequence+=multiJetFilter
        filtersToBookkeep+=[ multiJetFilterName ]
        if i>0:
            cmdstring+=" or "
            pass
        cmdstring+=multiJetFilterName
        pass
    combinedMultiJetFilterName="DV_MultiJetFilter"
    topSequence+= LogicalFilterCombiner( combinedMultiJetFilterName )
    print "NICK cmdstring is ",cmdstring
    topSequence.DV_MultiJetFilter.cmdstring=cmdstring
    filtersToBookkeep+=[ combinedMultiJetFilterName ]

## put them together.. 
    combinedMultiJetFilterName = "DV_CombinedMultiJetFilter"
    topSequence += LogicalFilterCombiner( combinedMultiJetFilterName )
    cmdstring = "DV_LooseTracklessJetFilter and DV_MultiJetFilter"
    topSequence.DV_CombinedMultiJetFilter.cmdstring=cmdstring
    filtersToBookkeep+=["DV_CombinedMultiJetFilter"]
##
    
## just for fun, configure a track filter

    from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import StealthFilterAlg
    stealthFilterName = 'DV_StealthFilter'
    topSequence += StealthFilterAlg(stealthFilterName)
    offlineObjectFilterNames.append( stealthFilterName )

    topSequence.DV_StealthFilter.cutMinTrackSd0 = primRPVLLDESDM.DV_stealthFilterFlags.cutMinTrackSd0
    topSequence.DV_StealthFilter.cutMinNTracks = primRPVLLDESDM.DV_stealthFilterFlags.cutMinNTracks
    topSequence.DV_StealthFilter.cutMinTrackpT = primRPVLLDESDM.DV_stealthFilterFlags.cutMinTrackpT
    topSequence.DV_StealthFilter.cutMinTrackEta = primRPVLLDESDM.DV_stealthFilterFlags.cutMinTrackEta
    topSequence.DV_StealthFilter.passAll = False
    filtersToBookkeep+=["DV_StealthFilter"]

    combinedOfflineFilterName = "DV_CombinedOfflineFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
    cmdstring = "DV_CombinedMETFilter or DV_CombinedMultiJetFilter or DV_ISRFilter"
    topSequence.DV_CombinedOfflineFilter.cmdstring=cmdstring 
    filtersToBookkeep+=["DV_CombinedOfflineFilter"]

    pass

#################################################################################################
###########  End of stream-specific section.
########### combine the trigger and offline filters

DVCombinedFilter=LogicalFilterCombiner("DVCombinedFilter")
topSequence+=DVCombinedFilter

topSequence.DVCombinedFilter.cmdstring="DV_CombinedTriggerFilter and DV_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="DVCombinedFilter"

filtersToBookkeep+=["DVCombinedFilter"]
