# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

class PrimaryDPDFlags_HIPsStream(JobPropertyContainer):
    """ The PrimaryDPD HIPs flag/job property  container."""

jobproperties.add_Container(PrimaryDPDFlags_HIPsStream)

primHIPsDESD=jobproperties.PrimaryDPDFlags_HIPsStream


primHIPsDESD=jobproperties.PrimaryDPDFlags_HIPsStream

class HipsTriggerFilterExpression(JobProperty):
    statusOn = True
    allowedTypes = ['string']
    StoredValue  = 'HLT_g0_hiptrt_L1EM18VH || HLT_g0_hiptrt_L1EM20VH || HLT_g0_hiptrt_L1EM20VHI || HLT_g0_hiptrt_L1EM22VHI || HLT_g0_hiptrt_L1EM24VHI || HLT_g0_hiptrt_L1EM24VHIM' 
    pass
primHIPsDESD.add_JobProperty(HipsTriggerFilterExpression)
