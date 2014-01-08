
### configure trigger filters
if len(primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.VH_DV_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.VH_DV_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.VH_DV_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = primRPVLLDESDM.VH_DV_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.VH_DV_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames += primRPVLLDESDM.VH_DV_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.VH_DV_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "VH_DV_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "VH_DV_CombinedTriggerFilter"
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
topSequence.VH_DV_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["VH_DV_CombinedTriggerFilter"]

##  a real jet filter
from LongLivedParticleDPDMaker.FancyJetFilter import FancyJetFilter
jetFilterName = "VH_DV_JetFilter"
topSequence += FancyJetFilter(jetFilterName)
topSequence.VH_DV_JetFilter.cutEtMin=primRPVLLDESDM.VH_DV_jetFilterFlags.jetPtCut
topSequence.VH_DV_JetFilter.cutEtMax=14.0*Units.TeV
topSequence.VH_DV_JetFilter.minNumberPassed=primRPVLLDESDM.VH_DV_jetFilterFlags.nJetPassed
topSequence.VH_DV_JetFilter.cutSumPtTrkMax=primRPVLLDESDM.VH_DV_jetFilterFlags.MaxSumPtTrk
topSequence.VH_DV_JetFilter.jetCollectionName = "AntiKt4TopoEMJets"
filtersToBookkeep+=["VH_DV_JetFilter"]

## METfilter
from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
missingetFilterName = 'VH_DV_MissingEtFilter'
topSequence += MissingEtFilter(missingetFilterName)
topSequence.VH_DV_MissingEtFilter.cutMetMin = primRPVLLDESDM.VH_DV_missingetFilterFlags.cutMetMin
topSequence.VH_DV_MissingEtFilter.requireMet = True
filtersToBookkeep+=["VH_DV_MissingEtFilter"]

jetMetFilterName = "VH_DV_JetMetFilter"
topSequence += LogicalFilterCombiner( jetMetFilterName)
topSequence.VH_DV_JetMetFilter.cmdstring = "VH_DV_JetFilter and VH_DV_MissingEtFilter"
filtersToBookkeep+=["VH_DV_JetMetFilter"]

if rec.triggerStream() == "Muons" or rec.triggerStream() == "":

	offlineMuonFilterNames=[]
	from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
	muonCollNames=["MuidESDMuonCollection", "MuidMuonCollection", "StacoESDMuonCollection", "StacoMuonCollection"]
	for mucoll in muonCollNames:
		muonFilterName = 'VH_DV_MuFilter_'+mucoll
		muonFilter = D2PDMuonSelector(muonFilterName)
		offlineMuonFilterNames.append( muonFilterName )
		muonFilter.ptMin = primRPVLLDESDM.VH_DV_muonFilterFlags.cutEtMin
		muonFilter.absEtaMax = primRPVLLDESDM.VH_DV_muonFilterFlags.cutEtaMax
		muonFilter.inputCollection = mucoll
		muonFilter.minNumberPassed=1
		topSequence+=muonFilter

	########### combine the offline filters
	muonFilterName = "VH_DV_MuonFilter"
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
	topSequence.VH_DV_MuonFilter.cmdstring=cmdstring ##+ " and VH_DV_JetFilter2"
	filtersToBookkeep+=["VH_DV_MuonFilter"]

	combinedOfflineFilterName = "VH_DV_CombinedMuonsOfflineFilter"
	topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
	cmdstring = "VH_DV_MuonFilter and VH_DV_JetMetFilter"
	topSequence.VH_DV_CombinedMuonsOfflineFilter.cmdstring=cmdstring 
	filtersToBookkeep+=["VH_DV_CombinedMuonsOfflineFilter"]
	
if rec.triggerStream() == "Egamma" or rec.triggerStream() == "":

	from PrimaryDPDMaker.ElectronFilter import ElectronFilter
	electronFilterName = 'VH_DV_ElectronFilter'
	topSequence += ElectronFilter(electronFilterName)
	topSequence.VH_DV_ElectronFilter.cutEtMin = primRPVLLDESDM.VH_DV_electronFilterFlags.cutEtMin
	topSequence.VH_DV_ElectronFilter.cutEtaMax = primRPVLLDESDM.VH_DV_electronFilterFlags.cutEtaMax
	topSequence.VH_DV_ElectronFilter.cutIsEM=primRPVLLDESDM.VH_DV_electronFilterFlags.cutIsEM
	topSequence.VH_DV_ElectronFilter.cutAuthor=primRPVLLDESDM.VH_DV_electronFilterFlags.cutAuthor
	topSequence.VH_DV_ElectronFilter.electronCollectionName=primRPVLLDESDM.VH_DV_electronFilterFlags.electronCollectionName
	filtersToBookkeep+=["VH_DV_ElectronFilter"]

	electronFilterName = 'VH_DV_DiElectronFilter'
	topSequence += ElectronFilter(electronFilterName)
	topSequence.VH_DV_DiElectronFilter.cutEtMin = primRPVLLDESDM.VH_DV_DielectronFilterFlags.cutEtMin
	topSequence.VH_DV_DiElectronFilter.cutEtaMax = primRPVLLDESDM.VH_DV_DielectronFilterFlags.cutEtaMax
	topSequence.VH_DV_DiElectronFilter.cutIsEM=primRPVLLDESDM.VH_DV_DielectronFilterFlags.cutIsEM
	topSequence.VH_DV_DiElectronFilter.cutAuthor=primRPVLLDESDM.VH_DV_DielectronFilterFlags.cutAuthor
	topSequence.VH_DV_DiElectronFilter.electronCollectionName=primRPVLLDESDM.VH_DV_DielectronFilterFlags.electronCollectionName
	topSequence.VH_DV_DiElectronFilter.minNumberPassed = 2
	filtersToBookkeep+=["VH_DV_DiElectronFilter"]

	combinedOfflineFilterName = "VH_DV_CombinedWOfflineFilter"
	topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
	cmdstring = "VH_DV_ElectronFilter and VH_DV_JetMetFilter"
	topSequence.VH_DV_CombinedWOfflineFilter.cmdstring=cmdstring 
	filtersToBookkeep+=[combinedOfflineFilterName]

	combinedOfflineFilterName = "VH_DV_CombinedZOfflineFilter"
	topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
	cmdstring = "VH_DV_DiElectronFilter and VH_DV_JetFilter"
	topSequence.VH_DV_CombinedZOfflineFilter.cmdstring=cmdstring 
	filtersToBookkeep+=[combinedOfflineFilterName]

	combinedOfflineFilterName = "VH_DV_CombinedEgammaOfflineFilter"
	topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
	cmdstring = "VH_DV_CombinedWOfflineFilter or VH_DV_CombinedZOfflineFilter"
	topSequence.VH_DV_CombinedEgammaOfflineFilter.cmdstring=cmdstring 
	filtersToBookkeep+=[combinedOfflineFilterName]


########### combine the trigger and offline filters
VH_DVCombinedFilter=LogicalFilterCombiner("VH_DVCombinedFilter")
if rec.triggerStream() == "Muons":
    topSequence+=VH_DVCombinedFilter
    topSequence.VH_DVCombinedFilter.cmdstring="VH_DV_CombinedTriggerFilter and VH_DV_CombinedMuonsOfflineFilter"
elif rec.triggerStream() == "Egamma":
    topSequence+=VH_DVCombinedFilter
    topSequence.VH_DVCombinedFilter.cmdstring="VH_DV_CombinedTriggerFilter and VH_DV_CombinedEgammaOfflineFilter"
elif rec.triggerStream() == "": #MC
    combinedOfflineFilterName = "VH_DV_CombinedEgammaMuonsOfflineFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
    cmdstring = "VH_DV_CombinedEgammaOfflineFilter or VH_DV_CombinedMuonsOfflineFilter"
    topSequence.VH_DV_CombinedEgammaMuonsOfflineFilter.cmdstring=cmdstring 
    filtersToBookkeep+=[combinedOfflineFilterName]
    topSequence+=VH_DVCombinedFilter
    topSequence.VH_DVCombinedFilter.cmdstring="VH_DV_CombinedTriggerFilter and VH_DV_CombinedEgammaMuonsOfflineFilter"

########### add this to the global top filter
if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="VH_DVCombinedFilter"
filtersToBookkeep+=["VH_DVCombinedFilter"]
