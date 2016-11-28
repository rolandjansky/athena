# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

class Emerging_FilterFlags(JobProperty):
    statusOn      = True
    allowedTypes  = ['bool']
    StoredValue   = True
    cutEtMin=100.0*Units.GeV
    cutEtaMax=2.5
    cutSumPtTrkMax=5.0*Units.GeV
    cutSumJetsPtMax=800.0*Units.GeV
    nPassed=4
    Triggers  = ["HLT_4j100", "HLT_4j110"]
primRPVLLDESDM.add_JobProperty(Emerging_FilterFlags)

class Emerging_containerFlags(JobProperty):
    statusOn = True
    jetCollectionName="AntiKt4EMTopoJets"
    bjetCollectionName="BTagging_AntiKt4EMTopo"
    trackCollectionName="InDetTrackParticles"
    pass
primRPVLLDESDM.add_JobProperty(Emerging_containerFlags)
