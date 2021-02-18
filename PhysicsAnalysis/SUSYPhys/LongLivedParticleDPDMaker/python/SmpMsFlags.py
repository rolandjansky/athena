# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class SmpMs_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = [ 
          "EF_mu40_slow",
          "EF_mu40_slow_empty",
          "EF_mu40_slow_outOfTime",
          "EF_mu40_slow_tight",
          "EF_mu40_slow_outOfTime_tight",
          "EF_mu60_slow_tight",
          "EF_mu60_slow_outOfTime_tight"
          ]
primRPVLLDESDM.add_JobProperty(SmpMs_triggerFilterFlags)


class SmpMs_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 40*Units.GeV
    cutContainerMu='all'
    usingAOD=False
primRPVLLDESDM.add_JobProperty(SmpMs_muonFilterFlags)
