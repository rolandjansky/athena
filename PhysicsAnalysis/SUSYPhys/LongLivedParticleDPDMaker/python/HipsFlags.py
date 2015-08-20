# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class Hips_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
        #"EF_g20_loose",
        #"EF_g40_loose",
        #"EF_g60_loose",
        #"EF_g80_loose",
        #"EF_e20_loose",
        #"EF_e60_loose",
        #"EF_g40_loose_EFxe40_noMu",
        #"EF_g40_loose_xe45_medium_noMu",
        #"EF_g40_loose_xe55_medium_noMu",
        #"EF_2g20_loose"
        #"EF_e60_medium1",
        "EF_g120_loose",
        "EF_2g40_loose"
    ]
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = []
    pass
primRPVLLDESDM.add_JobProperty(Hips_triggerFilterFlags)

class Hips_triggerFilterFlags_hiptrt(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = [
        "EF_g_nocut_hiptrtL2"
    ]
    JetTauEtmissTriggerNames = []
    MuonsTriggerNames = []
    pass
primRPVLLDESDM.add_JobProperty(Hips_triggerFilterFlags_hiptrt)

class Hips_electronFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True  
#    cutMinE = 15.0*Units.GeV
#    cutEtaMax = 2.2
    cutMinE = 60.0*Units.GeV
    cutEtaMax = 1.4
#    passAll = True
    pass
primRPVLLDESDM.add_JobProperty(Hips_electronFilterFlags)


class Hips_hipsFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutMinE = 40.0*Units.GeV
    cutEtaMax = 2.2
    cutMaxDeltaPhi = 0.05
    cutMinTRTHTHits = 20
    cutMinTRTHTFraction = 0.2
    cutCaloCluster='CaloCalTopoCluster'
## FOR TESTING ON R16
#    cutCaloCluster='EMTopoCluster430'
    pass
primRPVLLDESDM.add_JobProperty(Hips_hipsFilterFlags)
