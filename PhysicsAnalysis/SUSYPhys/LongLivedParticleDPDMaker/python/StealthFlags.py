# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class Stealth_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = [
	        "EF_4j80_a4tchad_L2FS"
	]
    MuonsTriggerNames = []
    pass
primRPVLLDESDM.add_JobProperty(Stealth_triggerFilterFlags)

class Stealth_stealthFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutMinTrackSd0 = 15.0
    cutMinTrackpT = 2000.0
    cutMinTrackEta = 2.5
    cutMinNTracks = 30
    pass
primRPVLLDESDM.add_JobProperty(Stealth_stealthFilterFlags)
