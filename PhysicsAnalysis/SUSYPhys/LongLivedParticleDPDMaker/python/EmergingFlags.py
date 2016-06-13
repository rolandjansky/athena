# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

class Emerging_FilterFlags(JobProperty):
    statusOn      = True
    allowedTypes  = ['bool']
    StoredValue   = True

primRPVLLDESDM.add_JobProperty(Emerging_FilterFlags)

