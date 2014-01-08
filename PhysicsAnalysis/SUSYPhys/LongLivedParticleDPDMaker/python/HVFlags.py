# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class HV_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
        #"EF_2mu6_MSonly_g10_loose"
    ]
    JetTauEtmissTriggerNames = [
        #"EF_j35_a4tc_EFFS_L1TAU_HVtrk",
        #"EF_j35_a4tc_EFFS_L1TAU_HV",
        #"EF_j35_L1TAU_HV",
        #"EF_l2j30_Trackless_HV",
        #"EF_l2j30_Trackless_HV_L1MU10",
        #"EF_j35_a4tc_EFFS_L1TAU_HVtrk_LOF",
        "EF_l2j25_a4tcem_Trackless_HV",
        "EF_j35_a4tcem_L1TAU_LOF_HV",
        "EF_l2j30_a4tcem_2L1MU6_HV",
        "EF_l2j30_a4tcem_2L1MU10_ExtendedEta_HV",
        "EF_l2j30_a4tcem_2L1MU10_HV",
        "EF_j35_a4tcem_L1TAU_LOF_HV_EMPTY",
        "EF_j35_a4tcem_L1TAU_LOF_HV_UNPAIRED_ISO",
        "EF_l2j25_a4tcem_Trackless_HV_EMPTY",
        "EF_l2j25_a4tcem_Trackless_HV_UNPAIRED_ISO",
        "EF_l2j30_a4tcem_2L1MU6_HV_EMPTY",
        "EF_l2j30_a4tcem_2L1MU4_HV_EMPTY",
        "EF_l2j30_a4tcem_2L1MU6_HV_UNPAIRED_ISO"
    ]
    MuonsTriggerNames = [
        #"EF_2MUL1_l2j30_HV",
        #"EF_2mu6_MSonly_g10_loose",
        #"EF_3mu6_MSonly"
        #"EF_l2j30_a4tcem_2L1MU6_HV"

    ]
    pass
primRPVLLDESDM.add_JobProperty(HV_triggerFilterFlags)


class HV_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 25*Units.GeV
    cutContainerMu='all'
    usingAOD=False
    pass
primRPVLLDESDM.add_JobProperty(HV_muonFilterFlags)
