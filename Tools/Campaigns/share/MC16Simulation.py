# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from G4AtlasApps.SimFlags import simFlags
simFlags.PhysicsList = "FTFP_BERT_ATL"
simFlags.TruthStrategy = "MC15aPlus"

simFlags.RunNumber = 284500

simFlags.TRTRangeCut = 30.0
simFlags.TightMuonStepping = True

from AthenaCommon.Resilience import protectedInclude
protectedInclude("SimulationJobOptions/preInclude.BeamPipeKill.py")
protectedInclude("SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py")
