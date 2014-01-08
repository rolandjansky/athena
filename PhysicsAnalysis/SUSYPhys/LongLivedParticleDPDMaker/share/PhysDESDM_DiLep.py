
### configure trigger filters
if len(primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames) == 0:
    if rec.triggerStream() == "Egamma":
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DiLep_triggerFilterFlags.EgammaTriggerNames
    elif rec.triggerStream() == "JetTauEtmiss":
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DiLep_triggerFilterFlags.JetTauEtmissTriggerNames
    elif rec.triggerStream() == "Muons":
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DiLep_triggerFilterFlags.MuonsTriggerNames
    elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = primRPVLLDESDM.DiLep_triggerFilterFlags.EgammaTriggerNames
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames += primRPVLLDESDM.DiLep_triggerFilterFlags.JetTauEtmissTriggerNames
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames += primRPVLLDESDM.DiLep_triggerFilterFlags.MuonsTriggerNames
    else:
        primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

# get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames = list(set(primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames))

triggerFilterNames=[]

from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primRPVLLDESDM.DiLep_triggerFilterFlags.TriggerNames:
    TriggerFilterName = "DiLep_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    triggerFilterNames.append( TriggerFilterName )
    pass

# Create a combined filter by ORing together all the trigger filters
combinedTriggerFilterName = "DiLep_CombinedTriggerFilter"
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
topSequence.DiLep_CombinedTriggerFilter.cmdstring = cmdstring    
filtersToBookkeep+=["DiLep_CombinedTriggerFilter"]

offlineObjectFilterNames=[]

## configure a photon filter

from PrimaryDPDMaker.PhotonFilter import PhotonFilter
photonFilterName = 'DiLep_PhotonFilter'
topSequence += PhotonFilter(photonFilterName)
offlineObjectFilterNames.append( photonFilterName )
topSequence.DiLep_PhotonFilter.cutEtMin = primRPVLLDESDM.DiLep_photonFilterFlags.cutEtMin
topSequence.DiLep_PhotonFilter.cutEtaMax = primRPVLLDESDM.DiLep_photonFilterFlags.cutEtaMax
topSequence.DiLep_PhotonFilter.cutIsEM=primRPVLLDESDM.DiLep_photonFilterFlags.cutIsEM
topSequence.DiLep_PhotonFilter.cutAuthor=primRPVLLDESDM.DiLep_photonFilterFlags.cutAuthor
topSequence.DiLep_PhotonFilter.photonCollectionName=primRPVLLDESDM.DiLep_photonFilterFlags.photonCollectionName

## configure a electron filter

from PrimaryDPDMaker.ElectronFilter import ElectronFilter
electronFilterName = 'DiLep_ElectronFilter'
topSequence += ElectronFilter(electronFilterName)
##offlineObjectFilterNames.append( electronFilterName )
topSequence.DiLep_ElectronFilter.cutEtMin = primRPVLLDESDM.DiLep_electronFilterFlags.cutEtMin
topSequence.DiLep_ElectronFilter.cutEtaMax = primRPVLLDESDM.DiLep_electronFilterFlags.cutEtaMax
topSequence.DiLep_ElectronFilter.cutIsEM=primRPVLLDESDM.DiLep_electronFilterFlags.cutIsEM
topSequence.DiLep_ElectronFilter.cutAuthor=primRPVLLDESDM.DiLep_electronFilterFlags.cutAuthor
topSequence.DiLep_ElectronFilter.electronCollectionName=primRPVLLDESDM.DiLep_electronFilterFlags.electronCollectionName



## configure a medium photon filter

from PrimaryDPDMaker.PhotonFilter import PhotonFilter
sphotonFilterName = 'DiLep_MediumPhotonFilter'
topSequence += PhotonFilter(sphotonFilterName)
#offlineObjectFilterNames.append( sphotonFilterName )
topSequence.DiLep_MediumPhotonFilter.cutEtMin = primRPVLLDESDM.DiLep_mediumphotonFilterFlags.cutEtMin
topSequence.DiLep_MediumPhotonFilter.cutEtaMax = primRPVLLDESDM.DiLep_mediumphotonFilterFlags.cutEtaMax
topSequence.DiLep_MediumPhotonFilter.cutIsEM=primRPVLLDESDM.DiLep_mediumphotonFilterFlags.cutIsEM
topSequence.DiLep_MediumPhotonFilter.cutAuthor=primRPVLLDESDM.DiLep_mediumphotonFilterFlags.cutAuthor
topSequence.DiLep_MediumPhotonFilter.photonCollectionName=primRPVLLDESDM.DiLep_mediumphotonFilterFlags.photonCollectionName

## configure a medium electron filter

from PrimaryDPDMaker.ElectronFilter import ElectronFilter
selectronFilterName = 'DiLep_MediumElectronFilter'
topSequence += ElectronFilter(selectronFilterName)
#offlineObjectFilterNames.append( selectronFilterName )
topSequence.DiLep_MediumElectronFilter.cutEtMin = primRPVLLDESDM.DiLep_mediumelectronFilterFlags.cutEtMin
topSequence.DiLep_MediumElectronFilter.cutEtaMax = primRPVLLDESDM.DiLep_mediumelectronFilterFlags.cutEtaMax
topSequence.DiLep_MediumElectronFilter.cutIsEM=primRPVLLDESDM.DiLep_mediumelectronFilterFlags.cutIsEM
topSequence.DiLep_MediumElectronFilter.cutAuthor=primRPVLLDESDM.DiLep_mediumelectronFilterFlags.cutAuthor
topSequence.DiLep_MediumElectronFilter.electronCollectionName=primRPVLLDESDM.DiLep_mediumelectronFilterFlags.electronCollectionName

## configure a soft photon filter

from PrimaryDPDMaker.PhotonFilter import PhotonFilter
sphotonFilterName = 'DiLep_SoftPhotonFilter'
topSequence += PhotonFilter(sphotonFilterName)
#offlineObjectFilterNames.append( sphotonFilterName )
topSequence.DiLep_SoftPhotonFilter.cutEtMin = primRPVLLDESDM.DiLep_softphotonFilterFlags.cutEtMin
topSequence.DiLep_SoftPhotonFilter.cutEtaMax = primRPVLLDESDM.DiLep_softphotonFilterFlags.cutEtaMax
topSequence.DiLep_SoftPhotonFilter.cutIsEM=primRPVLLDESDM.DiLep_softphotonFilterFlags.cutIsEM
topSequence.DiLep_SoftPhotonFilter.cutAuthor=primRPVLLDESDM.DiLep_softphotonFilterFlags.cutAuthor
topSequence.DiLep_SoftPhotonFilter.photonCollectionName=primRPVLLDESDM.DiLep_softphotonFilterFlags.photonCollectionName

## configure a soft electron filter

from PrimaryDPDMaker.ElectronFilter import ElectronFilter
selectronFilterName = 'DiLep_SoftElectronFilter'
topSequence += ElectronFilter(selectronFilterName)
#offlineObjectFilterNames.append( selectronFilterName )
topSequence.DiLep_SoftElectronFilter.cutEtMin = primRPVLLDESDM.DiLep_softelectronFilterFlags.cutEtMin
topSequence.DiLep_SoftElectronFilter.cutEtaMax = primRPVLLDESDM.DiLep_softelectronFilterFlags.cutEtaMax
topSequence.DiLep_SoftElectronFilter.cutIsEM=primRPVLLDESDM.DiLep_softelectronFilterFlags.cutIsEM
topSequence.DiLep_SoftElectronFilter.cutAuthor=primRPVLLDESDM.DiLep_softelectronFilterFlags.cutAuthor
topSequence.DiLep_SoftElectronFilter.electronCollectionName=primRPVLLDESDM.DiLep_softelectronFilterFlags.electronCollectionName

## configure a soft muon filter

#from PrimaryDPDMaker.MuonFilter import MuonFilter
#muonFilterName = 'DiLep_MuonFilter'
#topSequence += MuonFilter(muonFilterName)
##offlineObjectFilterNames.append( smuonFilterName )
#topSequence.DiLep_MuonFilter.cutPtMinMu = primRPVLLDESDM.DiLep_muonFilterFlags.cutEtMin
#topSequence.DiLep_MuonFilter.usingAOD = False 
#topSequence.DiLep_MuonFilter.cutEtaMax = primRPVLLDESDM.DiLep_muonFilterFlags.cutEtaMax
#topSequence.DiLep_MuonFilter.cutContainerMu=primRPVLLDESDM.DiLep_muonFilterFlags.cutContainerMu
#topSequence.DiLep_MuonFilter.muonAuthors="all"
#topSequence.DiLep_MuonFilter.muonContainers="all"
#topSequence.DiLep_MuonFilter.minNumberPassed=1



offlineMuonFilterNames=[]

	## configure muon filters

from D2PDMaker.D2PDMakerConf import D2PDMuonSelector

muonCollNames=["MuidESDMuonCollection", "MuidMuonCollection", "StacoESDMuonCollection", "StacoMuonCollection"]

for mucoll in muonCollNames:
	muonFilterName = 'DiLep_MuFilter_'+mucoll
	muonFilter = D2PDMuonSelector(muonFilterName)
	offlineMuonFilterNames.append( muonFilterName )
	muonFilter.ptMin = primRPVLLDESDM.DiLep_muonFilterFlags.cutEtMin
	muonFilter.absEtaMax =primRPVLLDESDM.DiLep_muonFilterFlags.cutEtaMax
	muonFilter.inputCollection = mucoll
	muonFilter.minNumberPassed=1
	topSequence+=muonFilter

muonFilterName = "DiLep_MuonFilter"
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
      
topSequence.DiLep_MuonFilter.cmdstring=cmdstring ##+ " and DV_JetFilter2"

## configure a diphoton filter
from PrimaryDPDMaker.DiPhotonFilter import DiPhotonFilter
diphotonFilterName = 'DiLep_DiPhotonFilter'
topSequence += DiPhotonFilter(diphotonFilterName)
offlineObjectFilterNames.append( diphotonFilterName )
topSequence.DiLep_DiPhotonFilter.cutEtMin1 = primRPVLLDESDM.DiLep_diphotonFilterFlags.cutEtMin1
topSequence.DiLep_DiPhotonFilter.cutEtMin2 = primRPVLLDESDM.DiLep_diphotonFilterFlags.cutEtMin2
topSequence.DiLep_DiPhotonFilter.cutIsEM1=primRPVLLDESDM.DiLep_diphotonFilterFlags.cutIsEM1
topSequence.DiLep_DiPhotonFilter.cutIsEM2=primRPVLLDESDM.DiLep_diphotonFilterFlags.cutIsEM2
topSequence.DiLep_DiPhotonFilter.cutAuthor1=primRPVLLDESDM.DiLep_diphotonFilterFlags.cutAuthor1
topSequence.DiLep_DiPhotonFilter.cutAuthor2=primRPVLLDESDM.DiLep_diphotonFilterFlags.cutAuthor2

## configure a dielectron filter
from PrimaryDPDMaker.DiElectronFilter import DiElectronFilter
dielectronFilterName = 'DiLep_DiElectronFilter'
topSequence += DiElectronFilter(dielectronFilterName)
##offlineObjectFilterNames.append( dielectronFilterName )
topSequence.DiLep_DiElectronFilter.cutEtMin1 = primRPVLLDESDM.DiLep_dielectronFilterFlags.cutEtMin1
topSequence.DiLep_DiElectronFilter.cutEtMin2 = primRPVLLDESDM.DiLep_dielectronFilterFlags.cutEtMin2
topSequence.DiLep_DiElectronFilter.cutIsEM1=primRPVLLDESDM.DiLep_dielectronFilterFlags.cutIsEM1
topSequence.DiLep_DiElectronFilter.cutIsEM2=primRPVLLDESDM.DiLep_dielectronFilterFlags.cutIsEM2
topSequence.DiLep_DiElectronFilter.cutAuthor1=primRPVLLDESDM.DiLep_dielectronFilterFlags.cutAuthor1
topSequence.DiLep_DiElectronFilter.cutAuthor2=primRPVLLDESDM.DiLep_dielectronFilterFlags.cutAuthor2

## configure a EMCluster filter

#from PrimaryDPDMaker.DiEMClusterFilter import DiEMClusterFilter
#diemclusterFilterName = 'DiLep_DiEMClusterFilter'
#topSequence += DiEMClusterFilter(diemclusterFilterName)
#offlineObjectFilterNames.append( diemclusterFilterName )
#
#topSequence.DiLep_DiEMClusterFilter.cutEtMinEMCl1 = primRPVLLDESDM.DiLep_diemclusterFilterFlags.cutEtMinEMCl1
#topSequence.DiLep_DiEMClusterFilter.cutEtMinEMCl2 = primRPVLLDESDM.DiLep_diemclusterFilterFlags.cutEtMinEMCl2 
#topSequence.DiLep_DiEMClusterFilter.cutEtaMax   = primRPVLLDESDM.DiLep_diemclusterFilterFlags.cutEtaMax
#topSequence.DiLep_DiEMClusterFilter.removeEtaCrack= primRPVLLDESDM.DiLep_diemclusterFilterFlags.removeEtaCrack
#topSequence.DiLep_DiEMClusterFilter.crackEtaMin = primRPVLLDESDM.DiLep_diemclusterFilterFlags.crackEtaMin
#topSequence.DiLep_DiEMClusterFilter.crackEtaMax = primRPVLLDESDM.DiLep_diemclusterFilterFlags.crackEtaMax
#
#### configure a EMCluster filter
#
#from PrimaryDPDMaker.DiEMClusterFilter import DiEMClusterFilter
#diemcluster2FilterName = 'DiLep_DiEMCluster2Filter'
#topSequence += DiEMClusterFilter(diemcluster2FilterName)
#offlineObjectFilterNames.append( diemcluster2FilterName )
#
#topSequence.DiLep_DiEMCluster2Filter.cutEtMinEMCl1 = primRPVLLDESDM.DiLep_diemcluster2FilterFlags.cutEtMinEMCl1
#topSequence.DiLep_DiEMCluster2Filter.cutEtMinEMCl2 = primRPVLLDESDM.DiLep_diemcluster2FilterFlags.cutEtMinEMCl2 
#topSequence.DiLep_DiEMCluster2Filter.cutEtaMax   = primRPVLLDESDM.DiLep_diemcluster2FilterFlags.cutEtaMax
#topSequence.DiLep_DiEMCluster2Filter.removeEtaCrack= primRPVLLDESDM.DiLep_diemcluster2FilterFlags.removeEtaCrack
#topSequence.DiLep_DiEMCluster2Filter.crackEtaMin = primRPVLLDESDM.DiLep_diemcluster2FilterFlags.crackEtaMin
#topSequence.DiLep_DiEMCluster2Filter.crackEtaMax = primRPVLLDESDM.DiLep_diemcluster2FilterFlags.crackEtaMax
## configure a MissingEt filter

### configure an ElectronPhoton filter
#topSequence += LogicalFilterCombiner( "DiLep_ElectronPhotonFilter" )
#topSequence.DiLep_ElectronPhotonFilter = "DiLep_PhotonFilter and DiLep_SoftElectronFilter"
#

ElectronPhotonFilterName = "DiLep_softElectronPhotonFilter"
topSequence += LogicalFilterCombiner( ElectronPhotonFilterName)
topSequence.DiLep_softElectronPhotonFilter.cmdstring = "DiLep_MediumPhotonFilter and DiLep_SoftElectronFilter"
offlineObjectFilterNames.append( "DiLep_softElectronPhotonFilter" )



MuonPhotonFilterName = "DiLep_softMuonPhotonFilter"
topSequence += LogicalFilterCombiner( MuonPhotonFilterName)
topSequence.DiLep_softMuonPhotonFilter.cmdstring = "DiLep_SoftPhotonFilter and DiLep_MuonFilter"
offlineObjectFilterNames.append( "DiLep_softMuonPhotonFilter" )




#filtersToBookkeep+=["DV_JetOrD0Filter"]
##

from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
missingetFilterName = 'DiLep_MissingEtFilter'
topSequence += MissingEtFilter(missingetFilterName)
offlineObjectFilterNames.append( missingetFilterName )

topSequence.DiLep_MissingEtFilter.cutMetMin = primRPVLLDESDM.DiLep_missingetFilterFlags.cutMetMin
topSequence.DiLep_MissingEtFilter.jetContainerTypeList = primRPVLLDESDM.DiLep_missingetFilterFlags.jetContainerTypeList
topSequence.DiLep_MissingEtFilter.jetContainerNameList = primRPVLLDESDM.DiLep_missingetFilterFlags.jetContainerNameList
topSequence.DiLep_MissingEtFilter.useUncalibratedJets = primRPVLLDESDM.DiLep_missingetFilterFlags.useUncalibratedJets
topSequence.DiLep_MissingEtFilter.useLeadingJet = primRPVLLDESDM.DiLep_missingetFilterFlags.useLeadingJet
topSequence.DiLep_MissingEtFilter.minDeltaPhi = primRPVLLDESDM.DiLep_missingetFilterFlags.minDeltaPhi
topSequence.DiLep_MissingEtFilter.requireMet = primRPVLLDESDM.DiLep_missingetFilterFlags.requireMet

##if rec.triggerStream() == "Egamma":
  ##  offlineEgammaFilterNames=[]
    
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector	
electronCollNames=["ElectronCollection"]

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlgEgamma
d0FilterEgammaName="DiLep_egammad0Filter"
d0FilterEgamma=DVFilterAlgEgamma(d0FilterEgammaName)
offlineObjectFilterNames.append(  d0FilterEgammaName )
topSequence+=d0FilterEgamma
topSequence.DiLep_egammad0Filter.electronD0Cut=primRPVLLDESDM.DiLep_d0FilterFlagsEgamma.electronD0Cut
topSequence.DiLep_egammad0Filter.electronPtCut=primRPVLLDESDM.DiLep_d0FilterFlagsEgamma.electronPtCut
topSequence.DiLep_egammad0Filter.electronEtaCut=primRPVLLDESDM.DiLep_d0FilterFlagsEgamma.electronEtaCut
filtersToBookkeep+=["DiLep_egammad0Filter"]

########## get our own filter
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlg
d0FilterName="DiLep_mud0Filter"
d0Filter=DVFilterAlg(d0FilterName)
topSequence+=d0Filter
topSequence.DiLep_mud0Filter.MuonD0Cut=primRPVLLDESDM.DiLep_d0FilterFlagsMuon.muD0Cut
topSequence.DiLep_mud0Filter.MuonPtCut=primRPVLLDESDM.DiLep_d0FilterFlagsMuon.muPtCut
topSequence.DiLep_mud0Filter.MuonEtaCut=primRPVLLDESDM.DiLep_d0FilterFlagsMuon.muEtaCut
filtersToBookkeep+=["DiLep_mud0Filter"]


from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlgEgamma
softd0FilterEgammaName="DiLep_softegammad0Filter"
softd0FilterEgamma=DVFilterAlgEgamma(softd0FilterEgammaName)
## offlineObjectFilterNames.append(  softd0FilterEgammaName )
topSequence+=softd0FilterEgamma
topSequence.DiLep_softegammad0Filter.electronD0Cut=primRPVLLDESDM.DiLep_softd0FilterFlagsEgamma.electronD0Cut
topSequence.DiLep_softegammad0Filter.electronPtCut=primRPVLLDESDM.DiLep_softd0FilterFlagsEgamma.electronPtCut
topSequence.DiLep_softegammad0Filter.electronEtaCut=primRPVLLDESDM.DiLep_softd0FilterFlagsEgamma.electronEtaCut
filtersToBookkeep+=["DiLep_softegammad0Filter"]
## 
## Di-electron filter
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DVFilterAlgDiElectron
d0FilterDiElectronName="DiLep_diElectrond0Filter"
d0FilterEgamma=DVFilterAlgDiElectron(d0FilterDiElectronName)
offlineObjectFilterNames.append( d0FilterDiElectronName )
topSequence+=d0FilterEgamma
topSequence.DiLep_diElectrond0Filter.dielectronD0Cut=primRPVLLDESDM.DiLep_d0FilterFlagsDiElectron.dielectronD0Cut
topSequence.DiLep_diElectrond0Filter.dielectronPtCut=primRPVLLDESDM.DiLep_d0FilterFlagsDiElectron.dielectronPtCut
topSequence.DiLep_diElectrond0Filter.dielectronEtaCut=primRPVLLDESDM.DiLep_d0FilterFlagsDiElectron.dielectronEtaCut
filtersToBookkeep+=["DiLep_diElectrond0Filter"]
#

MuonElectronFilterName = "DiLep_d0MuonElectronFilter"
topSequence += LogicalFilterCombiner( MuonElectronFilterName)
#topSequence.DiLep_softMuonElectronFilter.cmdstring = "DiLep_MuonFilter and DiLep_MediumElectronFilter"
topSequence.DiLep_d0MuonElectronFilter.cmdstring = "DiLep_MuonFilter and DiLep_softegammad0Filter"
offlineObjectFilterNames.append( "DiLep_d0MuonElectronFilter" )

MuonElectronFilterName2 = "DiLep_Muond0ElectronFilter"
topSequence += LogicalFilterCombiner( MuonElectronFilterName2)
#topSequence.DiLep_softMuonElectronFilter.cmdstring = "DiLep_MuonFilter and DiLep_MediumElectronFilter"
topSequence.DiLep_Muond0ElectronFilter.cmdstring = "DiLep_mud0Filter and DiLep_SoftElectronFilter"
offlineObjectFilterNames.append( "DiLep_Muond0ElectronFilter" )

PhotonElectronFilterName = "DiLep_softPhotonElectronFilter"
topSequence += LogicalFilterCombiner( PhotonElectronFilterName)
topSequence.DiLep_softPhotonElectronFilter.cmdstring = "DiLep_SoftPhotonFilter and DiLep_softegammad0Filter"
offlineObjectFilterNames.append( "DiLep_softPhotonElectronFilter" )

### configure a Jet filter
#
#from PrimaryDPDMaker.JetFilter import JetFilter
#jetFilterName = 'DiLep_JetFilter'
#topSequence += JetFilter(jetFilterName)
#offlineObjectFilterNames.append( jetFilterName )
#topSequence.DiLep_JetFilter.jetCollectionName = primRPVLLDESDM.DiLep_jetFilterFlags.jetCollectionName
#topSequence.DiLep_JetFilter.cutEtMin = primRPVLLDESDM.DiLep_jetFilterFlags.cutEtMin
#topSequence.DiLep_JetFilter.cutEtMax = primRPVLLDESDM.DiLep_jetFilterFlags.cutEtMax
#topSequence.DiLep_JetFilter.cutEtaMax = primRPVLLDESDM.DiLep_jetFilterFlags.cutEtaMax
#topSequence.DiLep_JetFilter.removeOverlap = primRPVLLDESDM.DiLep_jetFilterFlags.removeOverlap
#topSequence.DiLep_JetFilter.deltaR = primRPVLLDESDM.DiLep_jetFilterFlags.deltaR
#topSequence.DiLep_JetFilter.leptonContainerTypeList = primRPVLLDESDM.DiLep_jetFilterFlags.leptonContainerTypeList
#topSequence.DiLep_JetFilter.leptonContainerNameList = primRPVLLDESDM.DiLep_jetFilterFlags.leptonContainerNameList
#topSequence.DiLep_JetFilter.minNumberPassed = primRPVLLDESDM.DiLep_jetFilterFlags.minNumberPassed
#
############ combine the offline filters

combinedOfflineFilterName = "DiLep_CombinedOfflineFilter"
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
topSequence.DiLep_CombinedOfflineFilter.cmdstring=cmdstring
filtersToBookkeep+=["DiLep_CombinedOfflineFilter"]

########### combine the trigger and offline filters

DiLepCombinedFilter=LogicalFilterCombiner("DiLepCombinedFilter")
topSequence+=DiLepCombinedFilter

topSequence.DiLepCombinedFilter.cmdstring="DiLep_CombinedTriggerFilter and DiLep_CombinedOfflineFilter"

########### add this to the global top filter

if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="DiLepCombinedFilter"
