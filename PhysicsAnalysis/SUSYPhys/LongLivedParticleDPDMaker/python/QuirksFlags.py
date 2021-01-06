# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class Quirks_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = ["EF_xe80_tclcw_loose", "EF_xe80T_tclcw_loose"]
    MuonsTriggerNames = []
primRPVLLDESDM.add_JobProperty(Quirks_triggerFilterFlags)

class Quirks_offlineFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutMetMin = 130.0*Units.GeV
primRPVLLDESDM.add_JobProperty(Quirks_offlineFilterFlags)
