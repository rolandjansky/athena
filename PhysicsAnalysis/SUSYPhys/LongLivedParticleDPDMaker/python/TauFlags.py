# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
        #'HLT_tau160_medium1_tracktwoMVA_L1TAU100',
        'HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100',
        #'HLT_tau160_medium1_tracktwo_L1TAU100',
        #'HLT_tau160_medium1_tracktwoEF_L1TAU100',
        'HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100',
        #'HLT_tau200_medium1_tracktwo_L1TAU100',
        #'HLT_tau200_medium1_tracktwoEF_L1TAU100',
        #'HLT_tau200_medium1_tracktwoMVA_L1TAU100'

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
    #'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
    #'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',
    #'HLT_tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I', # This Topo trigger causes crash in code and needs special attention (https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DerivationFramework)
    'HLT_tau80_mediumRNN_tracktwoMVA_L1TAU60_tau60_mediumRNN_tracktwoMVA_L1TAU40',
    #'HLT_tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA',
    #'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
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
        #'HLT_tau35_medium1_tracktwoEF_xe70_L1XE45',
        #'HLT_tau35_medium1_tracktwoMVA_xe70_L1XE45',
        'HLT_tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',
        #'HLT_tau35_medium1_tracktwo_xe70_L1XE45',
        'HLT_tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50',
        #'HLT_tau60_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_xe50',
        #'HLT_tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
        #'HLT_tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50'
    ] # Primary 2018 RNN trigger
    # I preffer to not rely on the trigger API at the moment, while R&D in progress
    #if rpvllTrig.doRPVLLTriggerAPI:
    #     triggers += apitriggers.getTauMETTriggers() ## TriggerAPI
    pass
primRPVLLDESDM.add_JobProperty(Taus_TauMETFilterFlags)

# Do we have to define a prescale?
class Taus_PrescalerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    prescale=1
    pass
primRPVLLDESDM.add_JobProperty(Taus_PrescalerFlags)
