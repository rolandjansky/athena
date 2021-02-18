# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class PrimaryDPDFlags_HIPsStream(JobPropertyContainer):
    """ The PrimaryDPD HIPs flag/job property  container."""

jobproperties.add_Container(PrimaryDPDFlags_HIPsStream)

primHIPsDESD=jobproperties.PrimaryDPDFlags_HIPsStream

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
from LongLivedParticleDPDMaker.RPVLLTriggers import rpvllTrig


class HipsTriggerFilterExpression(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True
    triggers = ['HLT_g0_hiptrt_L1EM18VH', 'HLT_g0_hiptrt_L1EM20VH', 'HLT_g0_hiptrt_L1EM20VHI', 'HLT_g0_hiptrt_L1EM22VHI', 'HLT_g0_hiptrt_L1EM24VHI', 'HLT_g0_hiptrt_L1EM24VHIM']
    if rpvllTrig.doRPVLLTriggerAPI:
        triggers += apitriggers.getHIPsTriggers() # TriggerAPI
primHIPsDESD.add_JobProperty(HipsTriggerFilterExpression)
