# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

class Emerging_FilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin     = 100.0*Units.GeV
    cutEtaMax    = 2.5
    nPassed      = 4
    Triggers     = ["HLT_4j90",
                    "HLT_4j100",
                    "HLT_4j110",
                    "HLT_4j120",
                    "HLT_4j130",
                    "HLT_4j140",
                    "HLT_4j150"
                    ]
primRPVLLDESDM.add_JobProperty(Emerging_FilterFlags)

class Emerging_containerFlags(JobProperty):
    statusOn          = True
    jetCollectionName = "AntiKt4EMTopoJets"
    hltJetCollectionName = "HLT_xAOD__JetContainer_a4tcemsubjesFS"
    pass
primRPVLLDESDM.add_JobProperty(Emerging_containerFlags)


# ----- PRESCALED DI-JET TRIGGER TEST ----- #
class Emerging_DiJet110FilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin     = 100.0*Units.GeV
    cutEtaMax    = 2.5
    nPassed      = 2
    Triggers     = ["HLT_j110"]
primRPVLLDESDM.add_JobProperty(Emerging_DiJet110FilterFlags)

class Emerging_DiJet175FilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin     = 100.0*Units.GeV
    cutEtaMax    = 2.5
    nPassed      = 2
    Triggers     = ["HLT_j175"]
primRPVLLDESDM.add_JobProperty(Emerging_DiJet175FilterFlags)

class Emerging_DiJet260FilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin     = 100.0*Units.GeV
    cutEtaMax    = 2.5
    nPassed      = 2
    Triggers     = ["HLT_j260"]
primRPVLLDESDM.add_JobProperty(Emerging_DiJet260FilterFlags)
