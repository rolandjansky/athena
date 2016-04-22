# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

class DiLep_FilterFlags(JobProperty):
    statusOn      = True
    allowedTypes  = ['bool']
    StoredValue   = True
    
    SiPhTriggers  = ["HLT_g140_loose"]
    DiPhTriggers  = ["HLT_2g50_loose"]
    SiMuTriggers  = ["HLT_mu60_0eta105_msonly"]
    
    ElEtaMax      = 2.5
    PhEtaMax      = 2.5
    MuEtaMax      = 2.5
    MuBaEtaMax    = 1.07
    
    ElD0Min       = 2.0
    MuD0Min       = 1.5
    
    SiElPtMin     = 140.
    SiPhPtMin     = 150.
    SiPhXPtMin    =  10.
    SiMuPtMin     =  60.
    DiElPtMin     =  50.
    DiPhPtMin     =  50.
    DiElPhPtMin   =  50.
    DiLoElPhPtMin =  50.

primRPVLLDESDM.add_JobProperty(DiLep_FilterFlags)
