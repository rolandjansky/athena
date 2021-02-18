# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class Stopped_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = [
        "L1_RD0_EMPTY",
        "L1_RD0_FIRSTEMPTY",
        "L1_RD1_EMPTY",
        "L1_RD1_FIRSTEMPTY",
        "L1_J10_FIRSTEMPTY",
        "L1_J10_EMPTY",
        "L1_J10_UNPAIRED",
        "L1_J10_UNPAIRED_ISO",
        "L1_J10_UNPAIRED_NONISO",
        "L1_J30_FIRSTEMPTY",
        "L1_J30_EMPTY",
        "L1_J30_UNPAIRED",
        "L1_J30_UNPAIRED_ISO",
        "L1_J30_UNPAIRED_NONISO"
        ]
    MuonsTriggerNames = []
    signalPrescale      = 1
    backgroundPrescale  = 1000
primRPVLLDESDM.add_JobProperty(Stopped_triggerFilterFlags)

class Stopped_jetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 40*Units.GeV
    cutEtMax = 7000*Units.GeV
    cutEtaMin = 0
    cutEtaMax = 1.3
    cutPSFracMax = 0.95
    jetCollectionName='AntiKt4EMTopoJets'
primRPVLLDESDM.add_JobProperty(Stopped_jetFilterFlags)

class Stopped_muonSegmentFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    cutNsegMin = -1
    cutNsegMax = 2
    addPrescaledLooseFilter = True
    cutNsegMinLoose = 0
    cutNsegMaxLoose = 20
    prescaleFactorForLooseFilter = 20
    muonSegmentCollectionName = "MuonSegments" #"ConvertedMBoySegments"
primRPVLLDESDM.add_JobProperty(Stopped_muonSegmentFilterFlags)

