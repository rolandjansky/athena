# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class DV_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
		# "EF_g40_loose",
		# "EF_g60_loose",
		# "EF_e60_loose",
		# "EF_g80_loose",
		# "EF_g100_loose",
        "EF_g120_loose",
        "EF_2g40_loose"
	]
    JetTauEtmissTriggerNames = ["EF_xe80_tclcw",
                                "EF_j80_a4tchad_xe100_tclcw_loose",
                                "EF_4j80_a4tchad_L2FS",
                                "EF_5j55_a4tchad_L2FS",
                                "EF_6j45_a4tchad_L2FS_5L2j15"]
    MuonsTriggerNames = [
#        "EF_mu40_MSonly",
#        "EF_mu40_MSonly_barrel",
#        "EF_mu40_MSonly_barrel_medium",
#        "EF_mu40_MSonly_barrel_tight",        
        "EF_mu50_MSonly_barrel_tight"
    ]
    pass
primRPVLLDESDM.add_JobProperty(DV_triggerFilterFlags)

### multi-jet filter just to verify trigger - 4j80 OR 5j55 OR 6j45
class DV_multiJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    jetCollectionName = "AntiKt6TopoEMJets"
    jetPtCut=[80.0*Units.GeV, 55.0*Units.GeV, 45.0*Units.GeV]
    nJetPassed=[4,5,6]
    pass
primRPVLLDESDM.add_JobProperty(DV_multiJetFilterFlags)

class DV_looseFancyJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    jetPtCut=45.0*Units.GeV
    nJetPassed=1
    MaxSumPtTrk=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_looseFancyJetFilterFlags)


class DV_fancyJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    jetPtCut=50.0*Units.GeV
    nJetPassed=2
    MaxSumPtTrk=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_fancyJetFilterFlags)

class DV_ISRFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    jetCollectionName = "AntiKt6TopoEMJets"    
    jetPtCut=110.0*Units.GeV
    deltaPhiCut=2.0
    cutMetMin=100.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_ISRFilterFlags)


class DV_METFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutMetMin=100.0*Units.GeV
    deltaPhiCut=0.1
    pass
primRPVLLDESDM.add_JobProperty(DV_METFilterFlags)

class DV_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 50*Units.GeV
    cutEtaMax = 1.1
    cutContainerMu='all'
    usingAOD=False
    pass
primRPVLLDESDM.add_JobProperty(DV_muonFilterFlags)

class DV_d0FilterFlagsMuon(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    muD0Cut = 1.5
    muPtCut=50.0*Units.GeV
    muEtaCut=1.1
    pass
primRPVLLDESDM.add_JobProperty(DV_d0FilterFlagsMuon)

class DV_d0FilterFlagsEgamma(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    electronD0Cut=1.5
    electronPtCut=110.0*Units.GeV
    electronEtaCut=5.0
    pass
primRPVLLDESDM.add_JobProperty(DV_d0FilterFlagsEgamma)

class DV_d0FilterFlagsDiElectron(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    dielectronD0Cut=1.5
    dielectronPtCut=40.0*Units.GeV
    dielectronEtaCut=5.0
    pass
primRPVLLDESDM.add_JobProperty(DV_d0FilterFlagsDiElectron)

class DV_photonFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue= True
    photonPtCut=110.0*Units.GeV
    minPhotonPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_photonFilterFlags)

class DV_photonFilterFlags2(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue= True
    photonPtCut=40.0*Units.GeV
    minPhotonPassed=2
    pass
primRPVLLDESDM.add_JobProperty(DV_photonFilterFlags2)

class DV_jetFilterFlagsMuon(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    jetPtCut=50.0*Units.GeV
    nJetPassed=2   
    pass
primRPVLLDESDM.add_JobProperty(DV_jetFilterFlagsMuon)

class DV_jetFilterFlagsEgamma(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    jetCollectionName = "AntiKt4TopoEMJets"
    jetPtCut=40.0*Units.GeV
    MaxSumPtTrk=5.0*Units.GeV
    nJetPassed=2
    pass
primRPVLLDESDM.add_JobProperty(DV_jetFilterFlagsEgamma)

class DV_stealthFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutMinTrackSd0 = 15.0
    cutMinTrackpT = 2000.0
    cutMinTrackEta = 2.5
    cutMinNTracks = 30
    pass
primRPVLLDESDM.add_JobProperty(DV_stealthFilterFlags)
