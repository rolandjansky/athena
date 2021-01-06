# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class VH_DV_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = ["EF_e24vh_medium1_EFxe30","EF_g40_loose_L2EFxe60_tclcw","EF_2e12Tvh_loose1"] #"EF_e24vhi_medium1"
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = ["EF_mu40_MSonly","EF_mu36_tight"]
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_triggerFilterFlags)

class VH_DV_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 30*Units.GeV
    cutEtaMax = 2.5
    cutContainerMu='all'
    usingAOD=False
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_muonFilterFlags)

class VH_DV_electronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True    
    cutEtMin = 30*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Electron'
    cutIsEM = 'Medium'
    electronCollectionName='Electrons'
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_electronFilterFlags)

class VH_DV_DielectronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True    
    cutEtMin = 20*Units.GeV
    cutEtaMax = 2.5
    cutAuthor = 'Electron'
    cutIsEM = 'Medium'
    electronCollectionName='Electrons'
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_DielectronFilterFlags)

class VH_DV_jetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    jetPtCut=40.0*Units.GeV
    nJetPassed=1
    MaxSumPtTrk=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_jetFilterFlags)

class VH_DV_missingetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutMetMin=30.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(VH_DV_missingetFilterFlags)

