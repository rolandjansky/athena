# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
from LongLivedParticleDPDMaker.RPVLLTriggers import rpvllTrig


class DiLep_FilterFlags(JobProperty):
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    
    SiPhTriggers    = ["HLT_g140_loose", "HLT_g200_loose", "HLT_g200_loose_L1EM24VHIM"]
    if rpvllTrig.doRPVLLTriggerAPI:
        SiPhTriggers   += apitriggers.getDiLepSiPhTriggers() ## TriggerAPI
    DiPhTriggers    = ["HLT_2g50_loose", "HLT_2g50_loose_L12EM20VH", "HLT_2g60_loose_L12EM20VH"]
    if rpvllTrig.doRPVLLTriggerAPI:
        DiPhTriggers   += apitriggers.getDiLepDiPhTriggers() ## TriggerAPI
    SiMuTriggers    = ["HLT_mu80_msonly_3layersEC"]
    if rpvllTrig.doRPVLLTriggerAPI:
        SiMuTriggers   += apitriggers.getDiLepSiMuTriggers() ## TriggerAPI
    SiMuBaTriggers  = ["HLT_mu60_0eta105_msonly"]
    if rpvllTrig.doRPVLLTriggerAPI:
        SiMuBaTriggers += apitriggers.getDiLepSiMuBaTriggers() ## TriggerAPI
    
    ElEtaMax       = 2.5
    PhEtaMax       = 2.5
    MuEtaMax       = 2.5
    MuBaEtaMax     = 1.07
    
    ElD0Min        = 2.0
    MuD0Min        = 2.0
    
    SiElPtMin      = 160.
    SiPhPtMin      = 160.
    SiPhXPtMin     =  10.
    SiMuPtMin      =  80.
    SiMuBaPtMin    =  60.
    DiElPtMin      =  60.
    DiPhPtMin      =  60.
    DiElPhPtMin    =  60.
    DiLoElPhPtMin  =  60.

primRPVLLDESDM.add_JobProperty(DiLep_FilterFlags)
