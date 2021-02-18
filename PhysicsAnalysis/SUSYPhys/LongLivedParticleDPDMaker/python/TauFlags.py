# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
from LongLivedParticleDPDMaker.RPVLLTriggers import rpvllTrig


class Tau_containerFlags(JobProperty):
    statusOn = True
    photonCollectionName='Photons'
    electronCollectionName='Electrons'
    muonCollectionName='Muons'
    jetCollectionName="AntiKt4EMTopoJets"
    METCollectionName="MET_LocHadTopo"
    pass
primRPVLLDESDM.add_JobProperty(Tau_containerFlags)


class Taus_SingleTauFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    triggers=[
        'HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100',
        'HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100'
        ] # Primary 2018 RNN trigger
    # I preffer to not rely on the trigger API at the moment, while R&D in progress
    #if rpvllTrig.doRPVLLTriggerAPI:
    #     triggers += apitriggers.getTauSingleTriggers() ## TriggerAPI
    pass
primRPVLLDESDM.add_JobProperty(Taus_SingleTauFilterFlags)

class Taus_DiTauFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    triggers=[
    'HLT_tau80_mediumRNN_tracktwoMVA_L1TAU60_tau60_mediumRNN_tracktwoMVA_L1TAU40'
    ]
    # I preffer to not rely on the trigger API at the moment, while R&D in progress
    #if rpvllTrig.doRPVLLTriggerAPI:
    #     triggers += apitriggers.getTauDiTriggers() ## TriggerAPI
    pass
primRPVLLDESDM.add_JobProperty(Taus_DiTauFilterFlags)

class Taus_TauMETFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    triggers=[
        'HLT_tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',
        'HLT_tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50'
    ] # Primary 2018 RNN trigger
    # I preffer to not rely on the trigger API at the moment, while R&D in progress
    #if rpvllTrig.doRPVLLTriggerAPI:
    #     triggers += apitriggers.getTauMETTriggers() ## TriggerAPI
    pass
primRPVLLDESDM.add_JobProperty(Taus_TauMETFilterFlags)

class Taus_PrescalerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    prescale=1
    pass
primRPVLLDESDM.add_JobProperty(Taus_PrescalerFlags)
