# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class DiLep_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
        #"EF_g80_loose",
	"EF_g120_loose",
        "EF_g40_loose_L2EFxe60",
        "EF_2g40_loose",
        "EF_g35_loose_g25_loose",
        ]
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = []

    pass
primRPVLLDESDM.add_JobProperty(DiLep_triggerFilterFlags)


class DiLep_photonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 130*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Photon'
    cutIsEM = 'Loose'#what isEm is this?? tight?? loose??
    photonCollectionName='PhotonCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_photonFilterFlags)

class DiLep_electronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 150*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Electron'
    cutIsEM = 'Medium'#what isEm is this?? tight?? loose??
    electronCollectionName='ElectronCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_electronFilterFlags)

class DiLep_mediumphotonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 48*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Photon'
    cutIsEM = 'Loose'#what isEm is this?? tight?? loose??
    photonCollectionName='PhotonCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_mediumphotonFilterFlags)

class DiLep_mediumelectronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 48*Units.GeV
    cutEtaMax = 2.5
    cutIsEM = 'Loose'#what isEm is this?? tight?? loose??
    cutAuthor = 'Electron'
    electronCollectionName='ElectronCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_mediumelectronFilterFlags)

class DiLep_softphotonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 39*Units.GeV
    cutEtaMax = 2.5
    cutIsEM = 'Loose'#what isEm is this?? tight?? loose??
    cutAuthor = 'Photon'
    photonCollectionName='PhotonCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_softphotonFilterFlags)

class DiLep_softelectronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 39*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Electron'
    cutIsEM = 'Loose'#what isEm is this?? tight?? loose??
    electronCollectionName='ElectronCollection'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_softelectronFilterFlags)

class DiLep_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 15*Units.GeV
    cutEtaMax = 2.5
    cutContainerMu='all'
    usingAOD=False
    pass
primRPVLLDESDM.add_JobProperty(DiLep_muonFilterFlags)
class DiLep_diphotonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False
    cutEtMin1    = 48.0*Units.GeV
    cutEtMin2    = 48.0*Units.GeV
    cutIsEM1     = 'Loose'    # Possible values: 'None', 'Photon'
    cutIsEM2     = 'Loose'    # Possible values: 'None', 'Photon'
    cutAuthor1   = 'either'  # Possible values: 'Photon'
    cutAuthor2   = 'either'  # Possible values: 'Photon'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52

    pass
primRPVLLDESDM.add_JobProperty(DiLep_diphotonFilterFlags)

class DiLep_dielectronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False
    cutEtMin1    = 49.0*Units.GeV
    cutEtMin2    = 49.0*Units.GeV
    cutIsEM1     = 'Loose'    # Possible values: 'None', 'Photon'
    cutIsEM2     = 'Medium'    # Possible values: 'None', 'Photon'
    cutAuthor1   = 'Electron'  # Possible values: 'Photon'
    cutAuthor2   = 'Electron'  # Possible values: 'Photon'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52

    pass
primRPVLLDESDM.add_JobProperty(DiLep_dielectronFilterFlags)

class DiLep_softd0FilterFlagsEgamma(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    electronD0Cut=2.0
    electronPtCut=38.0*Units.GeV
    electronEtaCut=2.5
    pass
primRPVLLDESDM.add_JobProperty(DiLep_softd0FilterFlagsEgamma)

class DiLep_d0FilterFlagsEgamma(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    electronD0Cut=2.0
    electronPtCut=120.0*Units.GeV
    electronEtaCut=2.5
    pass
primRPVLLDESDM.add_JobProperty(DiLep_d0FilterFlagsEgamma)



class DiLep_d0FilterFlagsDiElectron(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    dielectronD0Cut=2.5
    dielectronPtCut=43.0*Units.GeV
    dielectronEtaCut=2.5
    pass
primRPVLLDESDM.add_JobProperty(DiLep_d0FilterFlagsDiElectron)


class DiLep_d0FilterFlagsMuon(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    muD0Cut = 1.5
    muPtCut=15.0*Units.GeV
    muEtaCut=2.5
    pass
primRPVLLDESDM.add_JobProperty(DiLep_d0FilterFlagsMuon)

#class DiLep_diemclusterFilterFlags(JobProperty):
#    statusOn = False
#    allowedTypes = ['bool']
#    StoredValue  = False    
#    cutEtMinEMCl1    = 49.0*Units.GeV
#    cutEtMinEMCl2    = 49.0*Units.GeV
#    cutEtaMax      = 2.5
#    removeEtaCrack = False
#    crackEtaMin    = 1.37
#    crackEtaMax    = 1.52
#    pass
#primRPVLLDESDM.add_JobProperty(DiLep_diemclusterFilterFlags)
#
#class DiLep_diemcluster2FilterFlags(JobProperty):
#    statusOn = False
#    allowedTypes = ['bool']
#    StoredValue  = False    
#    cutEtMinEMCl1    = 140.0*Units.GeV
#    cutEtMinEMCl2    = 25.0*Units.GeV
#    cutEtaMax      = 2.5
#    removeEtaCrack = False
#    crackEtaMin    = 1.37
#    crackEtaMax    = 1.52
#    pass
#primRPVLLDESDM.add_JobProperty(DiLep_diemcluster2FilterFlags)
#
class DiLep_missingetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutMetMin = 85.0*Units.GeV
    jetContainerTypeList = []
    jetContainerNameList = []
    useUncalibratedJets = True
    useLeadingJet= False
    minDeltaPhi = 0.0
    requireMet = True
    pass
primRPVLLDESDM.add_JobProperty(DiLep_missingetFilterFlags)

#class DiLep_jetFilterFlags(JobProperty):
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue  = False
#    jetCollectionName= "AntiKt4TopoEMJets"
#    cutEtMin = 150*Units.TeV
#    cutEtMax = 14*Units.TeV
#    cutEtaMax= 2.5
#    removeOverlap= False
#    deltaR= 0.2
#    leptonContainerTypeList= []
#    leptonContainerNameList= []
#    minNumberPassed= 1
#    pass
#primRPVLLDESDM.add_JobProperty(DiLep_jetFilterFlags)
