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
	"HLT_j30_muvtx_noiso",
	"HLT_j30_jes_PS_llp_L1TAU30",
	"HLT_j30_jes_PS_llp_L1TAU40",
	"HLT_j30_jes_PS_llp_L1TAU60",
	"HLT_j30_jes_PS_llp_L1LLP-NOMATCH",
	"HLT_j30_jes_PS_llp_noiso_L1TAU60",
 	"HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH",
#	"HLT_j100_xe80",
	"HLT_j30_jes_PS_llp_L1TAU8_EMPTY",
	"HLT_j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO",
	"HLT_j30_muvtx_L1MU4_EMPTY",
	"HLT_j30_muvtx_L1MU4_UNPAIRED_ISO"
    ]


    pass
primRPVLLDESDM.add_JobProperty(HV_triggerFilterFlags)

class HV_jetMETFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [
  #      "HLT_j80_xe80",
	"HLT_j100_xe80"
    ]
    cutMetMin = 120.0*Units.GeV  # MET cut, currently placed on MET_Reference_AntiKt4TopoEM
    pass
primRPVLLDESDM.add_JobProperty(HV_jetMETFilterFlags)
