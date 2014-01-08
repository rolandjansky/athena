# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class NonPointing_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
#        "EF_g40_loose_EFxe40_noMu",
        "EF_g40_loose_xe45_medium_noMu",
        "EF_g40_loose_xe55_medium_noMu",
        "EF_g40_loose_EFxe40_noMu",
#        "EF_g100_etcut_g50_etcut",
#        "EF_g40_loose",
#        "EF_2g20_loose",
#        "EF_g20_etcut_xe30_nuMu",
#        "EF_g60_loose",
        "EF_g80_loose",
#        "EF_g150_etcut"
        "EF_g40_loose_L2xe60",
        "EF_2g40_loose",
    ]
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = []

    pass
primRPVLLDESDM.add_JobProperty(NonPointing_triggerFilterFlags)


class NonPointing_photonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 60*Units.TeV
    cutEtaMax = 1.81
#    cutIsEM = 'None'#what isEm is this?? tight?? loose??
    cutIsEM = 'PhotonTightAR'#what isEm is this?? tight?? loose??
    photonCollectionName='PhotonCollection'
    pass
primRPVLLDESDM.add_JobProperty(NonPointing_photonFilterFlags)

class NonPointing_diphotonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False
    cutEtMin1    = 20.0*Units.TeV
    cutEtMin2    = 20.0*Units.TeV
    cutIsEM1     = 'PhotonLooseAR'    # Possible values: 'None', 'Photon'
    cutIsEM2     = 'None'    # Possible values: 'None', 'Photon'
    cutAuthor1   = 'either'  # Possible values: 'Photon'
    cutAuthor2   = 'either'  # Possible values: 'Photon'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52

    pass
primRPVLLDESDM.add_JobProperty(NonPointing_diphotonFilterFlags)

class NonPointing_diemclusterFilterFlags(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMinEMCl1    = 60.0*Units.TeV
    cutEtMinEMCl2    = 40.0*Units.TeV
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52
    pass
primRPVLLDESDM.add_JobProperty(NonPointing_diemclusterFilterFlags)

class NonPointing_missingetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutMetMin = 60.0*Units.GeV
    jetContainerTypeList = []
    jetContainerNameList = []
    useUncalibratedJets = True
    useLeadingJet= False
    minDeltaPhi = 0.0
    requireMet = True
    pass
primRPVLLDESDM.add_JobProperty(NonPointing_missingetFilterFlags)

class NonPointing_jetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False
    jetCollectionName= "AntiKt4TopoEMJets"
    cutEtMin = 150*Units.TeV
    cutEtMax = 14*Units.TeV
    cutEtaMax= 2.5
    removeOverlap= False
    deltaR= 0.2
    leptonContainerTypeList= []
    leptonContainerNameList= []
    minNumberPassed= 1
    pass
primRPVLLDESDM.add_JobProperty(NonPointing_jetFilterFlags)
