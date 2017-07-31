# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream


class HV_MuvtxTriggerFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
        "HLT_j30_muvtx",
        #"HLT_j30_muvtx_noiso", #Move to prescaled filter
        #"HLT_j30_muvtx_L1MU6_EMPTY",
        #"HLT_j30_muvtx_L1MU4_UNPAIRED_ISO",
        #EMPTY and UNPAIRED_ISO triggers are currently in physics_Late, and thus not part of DRAW_RPVLL    
    ]

    pass
primRPVLLDESDM.add_JobProperty(HV_MuvtxTriggerFlags)

class HV_prescaledMuvtxTriggerFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
        "HLT_j30_muvtx_noiso",
    ]
    Prescale = 1
    pass
primRPVLLDESDM.add_JobProperty(HV_prescaledMuvtxTriggerFlags)

class HV_JetMETFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
  #      "HLT_j80_xe80",
	"HLT_j120_xe80"
    ]
    cutMetMin = 120.0*Units.GeV  # MET cut, currently placed on MET_Reference_AntiKt4TopoEM
    pass
primRPVLLDESDM.add_JobProperty(HV_JetMETFilterFlags)

class HV_CalRatioTriggerFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
        "HLT_j30_jes_cleanLLP_PS_llp_L1TAU60",
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60",
        "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH", #L1Topo triggers are not running yet but should be "soon"
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH",
        "HLT_j30_jes_cleanLLP_PS_llp_L1TAU100", #will replace L1TAU60 soon
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100",
        "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO", #new L1Topo item; rate should be small
        "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO",
        #"HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY",
        #"HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY",
        #"HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO",
        #"HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO"
        #EMPTY and UNPAIRED_ISO triggers are currently in physics_Late, and thus not part of DRAW_RPVLL    
    ]

    pass
primRPVLLDESDM.add_JobProperty(HV_CalRatioTriggerFlags)
