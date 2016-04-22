# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class HV_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
        "HLT_j30_muvtx",
        # "HLT_j30_muvtx_noiso", # Now in the prescaled list
        "HLT_j30_jes_PS_llp_L1TAU30",
        "HLT_j30_jes_PS_llp_L1TAU40",
        "HLT_j30_jes_PS_llp_L1TAU60",
        "HLT_j30_jes_PS_llp_L1LLP-NOMATCH",
        "HLT_j30_jes_PS_llp_noiso_L1TAU60",
        "HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH",
        #"HLT_j30_jes_PS_llp_L1TAU8_EMPTY",
        #"HLT_j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO",
        #"HLT_j30_muvtx_L1MU4_EMPTY",
        #"HLT_j30_muvtx_L1MU4_UNPAIRED_ISO",
        "HLT_j30_jes_cleanLLP_PS_llp_L1TAU30", # adding cleanLLP chains for the future 
        "HLT_j30_jes_cleanLLP_PS_llp_L1TAU40",
        "HLT_j30_jes_cleanLLP_PS_llp_L1TAU60",
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60",
        "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH",
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH",
        #"HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY",
        #"HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY",
        #"HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO",
        #"HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO"
        #EMPTY and UNPAIRED_ISO triggers are in physics_Late, and thus not part of DRAW_RPVLL    
    ]

    pass
primRPVLLDESDM.add_JobProperty(HV_triggerFilterFlags)

class HV_prescaledTriggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
        "HLT_j30_muvtx_noiso",
    ]
    Prescale = 1
    pass
primRPVLLDESDM.add_JobProperty(HV_prescaledTriggerFilterFlags)

class HV_jetMETFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
  #      "HLT_j80_xe80",
	"HLT_j120_xe80"
    ]
    cutMetMin = 120.0*Units.GeV  # MET cut, currently placed on MET_Reference_AntiKt4TopoEM
    pass
primRPVLLDESDM.add_JobProperty(HV_jetMETFilterFlags)
